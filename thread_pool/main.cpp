#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

class ThreadPool {
    std::vector<std::thread>          workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex                        mtx_;
    std::condition_variable           cv_;
    std::condition_variable           idle_cv_;
    bool                              stop_   = false;
    size_t                            active_ = 0;   // tasks currently running

    void worker() {
        while (true) {
            std::function<void()> job;
            {
                std::unique_lock<std::mutex> lk(mtx_);
                cv_.wait(lk, [this] { return stop_ || !tasks_.empty(); });
                if (stop_ && tasks_.empty()) return;
                job = std::move(tasks_.front());
                tasks_.pop();
                ++active_;
            }
            job();
            {
                std::lock_guard<std::mutex> lk(mtx_);
                --active_;
            }
            idle_cv_.notify_all();
        }
    }

public:
    explicit ThreadPool(size_t n = std::thread::hardware_concurrency()) {
        if (n == 0) n = 4;
        workers_.reserve(n);
        for (size_t i = 0; i < n; ++i)
            workers_.emplace_back([this] { worker(); });
    }

    ~ThreadPool() {
        {
            std::lock_guard<std::mutex> lk(mtx_);
            stop_ = true;
        }
        cv_.notify_all();
        for (auto& w : workers_) w.join();
    }

    ThreadPool(const ThreadPool&)            = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    // Submit a callable and return a future to its result.
    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args)
        -> std::future<std::invoke_result_t<F, Args...>>
    {
        using R = std::invoke_result_t<F, Args...>;
        auto task = std::make_shared<std::packaged_task<R()>>(
            [f = std::forward<F>(f),
             ...captured = std::forward<Args>(args)]() mutable {
                return f(std::forward<Args>(captured)...);
            }
        );
        std::future<R> fut = task->get_future();
        {
            std::lock_guard<std::mutex> lk(mtx_);
            if (stop_) throw std::runtime_error("submit on stopped ThreadPool");
            tasks_.emplace([task] { (*task)(); });
        }
        cv_.notify_one();
        return fut;
    }

    // Block until every submitted task has finished.
    void wait_all() {
        std::unique_lock<std::mutex> lk(mtx_);
        idle_cv_.wait(lk, [this] { return tasks_.empty() && active_ == 0; });
    }

    size_t pending() const {
        std::lock_guard<std::mutex> lk(const_cast<std::mutex&>(mtx_));
        return tasks_.size() + active_;
    }

    size_t thread_count() const { return workers_.size(); }
};

int main() {
    ThreadPool pool(4);
    std::mutex print_mtx;

    auto log = [&](const std::string& msg) {
        std::lock_guard<std::mutex> g(print_mtx);
        std::cout << msg << "\n";
    };

    // Submit tasks that return values — collect futures.
    std::vector<std::future<int>> results;
    for (int i = 1; i <= 10; ++i) {
        results.push_back(pool.submit([i, &log] {
            log("Task " + std::to_string(i) + " starting");
            std::this_thread::sleep_for(std::chrono::milliseconds(100 + (i % 5) * 50));
            log("Task " + std::to_string(i) + " done");
            return i * i;
        }));
    }

    // Wait for everything, then read results.
    pool.wait_all();

    int total = 0;
    for (int i = 0; i < 10; ++i) {
        int val = results[i].get();
        total += val;
    }
    std::cout << "\nSum of squares (1..10) = " << total << "  (expected 385)\n";
    std::cout << "Thread count: " << pool.thread_count() << "\n";

    return 0;
}
