#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class ThreadPool {
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex m;
    std::condition_variable cv;
    std::atomic<bool> stop{false};

    void worker(){
        while(true){
            std::function<void()> job;
            {
                std::unique_lock<std::mutex> lk(m);
                cv.wait(lk, [&]{ return stop || !tasks.empty(); });
                if (stop && tasks.empty()) return;
                job = std::move(tasks.front()); tasks.pop();
            }
            job();
        }
    }

public:
    explicit ThreadPool(size_t n=std::thread::hardware_concurrency()){
        if (n==0) n=4;
        for (size_t i=0;i<n;i++) workers.emplace_back([this]{ worker(); });
    }
    ~ThreadPool(){
        {
            std::lock_guard<std::mutex> lk(m);
            stop = true;
        }
        cv.notify_all();
        for (auto &w: workers) w.join();
    }

    template<typename F>
    void submit(F&& f){
        {
            std::lock_guard<std::mutex> lk(m);
            tasks.emplace(std::forward<F>(f));
        }
        cv.notify_one();
    }
};

int main(){
    ThreadPool pool(4);
    std::mutex print;
    for (int i=1;i<=10;i++){
        pool.submit([i,&print]{
            {
                std::lock_guard<std::mutex> g(print);
                std::cout << "Task " << i << " starting\n";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(200 + (i%5)*50));
            {
                std::lock_guard<std::mutex> g(print);
                std::cout << "Task " << i << " done\n";
            }
        });
    }
    // pool destructor waits for tasks to finish
}
