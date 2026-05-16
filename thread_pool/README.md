# thread_pool

A C++20 thread pool where every submitted task returns a **`std::future`**, letting callers retrieve results, handle exceptions, and wait for specific tasks without polling.

## Features

- `submit(f, args...)` — enqueues a callable and returns `std::future<ReturnType>`
- `wait_all()` — blocks until every queued and in-flight task has completed
- `pending()` — snapshot of queued + currently running tasks
- `thread_count()` — number of worker threads
- Clean shutdown: destructor drains the queue and joins all threads
- Non-copyable by design

## Usage

```cpp
ThreadPool pool(4);   // 4 worker threads

// Submit a task that returns a value
std::future<int> f = pool.submit([] { return 6 * 7; });
std::cout << f.get() << "\n";   // 42

// Submit multiple tasks, collect futures
std::vector<std::future<int>> results;
for (int i = 1; i <= 10; ++i)
    results.push_back(pool.submit([i] { return i * i; }));

pool.wait_all();   // block until all tasks finish

for (auto& r : results)
    std::cout << r.get() << "\n";
```

## Design

| Detail | Value |
|---|---|
| Task storage | `std::queue<std::function<void()>>` |
| Return values | `std::packaged_task` + `std::future` |
| Idle tracking | `active_` counter + `idle_cv_` condition variable |
| Shutdown policy | Drain queue, then stop workers |

## Build & Run

```bash
make        # uses C++20
./thread_pool
```

Requires GCC 10+ or Clang 12+ with `-std=c++20` and `-pthread`.
