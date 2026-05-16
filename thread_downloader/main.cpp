#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <random>
#include <string>
#include <thread>
#include <vector>

struct DownloadResult {
    int         id;
    std::string url;
    long long   duration_ms;
    bool        success;
};

// Shared state
static std::mutex              print_mtx;
static std::mutex              rng_mtx;
static std::mt19937            rng{std::random_device{}()};
static std::uniform_int_distribution<int> delay_dist{400, 1200};

static void log(const std::string& msg) {
    std::lock_guard<std::mutex> g(print_mtx);
    std::cout << msg << "\n";
}

static DownloadResult download(int id, const std::string& url) {
    log("[" + std::to_string(id) + "] Downloading " + url + " ...");

    int delay_ms;
    {
        std::lock_guard<std::mutex> g(rng_mtx);
        delay_ms = delay_dist(rng);
    }

    auto t0 = std::chrono::steady_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    auto t1 = std::chrono::steady_clock::now();

    long long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

    // Simulate occasional failure (10% chance).
    bool ok;
    {
        std::lock_guard<std::mutex> g(rng_mtx);
        ok = std::uniform_int_distribution<int>{1, 10}(rng) > 1;
    }

    std::string status = ok ? "OK" : "FAILED";
    log("[" + std::to_string(id) + "] " + url + " -> " + status +
        " (" + std::to_string(elapsed) + " ms)");

    return {id, url, elapsed, ok};
}

int main() {
    const std::vector<std::string> urls = {
        "https://example.com/file1.zip",
        "https://example.com/file2.tar.gz",
        "https://example.com/data.csv",
        "https://example.com/image.png",
        "https://example.com/report.pdf",
        "https://example.com/archive.7z",
    };

    std::vector<std::thread>       threads;
    std::vector<DownloadResult>    results(urls.size());
    std::mutex                     results_mtx;

    threads.reserve(urls.size());
    for (size_t i = 0; i < urls.size(); ++i) {
        threads.emplace_back([i, &urls, &results] {
            results[i] = download(static_cast<int>(i + 1), urls[i]);
        });
    }
    for (auto& t : threads) t.join();

    // Summary
    std::cout << "\n--- Download Summary ---\n";
    std::cout << std::left
              << std::setw(4)  << "ID"
              << std::setw(38) << "URL"
              << std::setw(10) << "Duration"
              << "Status\n"
              << std::string(60, '-') << "\n";

    int    ok_count   = 0;
    long long total_ms = 0;
    for (const auto& r : results) {
        std::cout << std::setw(4)  << r.id
                  << std::setw(38) << r.url
                  << std::setw(10) << (std::to_string(r.duration_ms) + " ms")
                  << (r.success ? "OK" : "FAILED") << "\n";
        if (r.success) ++ok_count;
        total_ms += r.duration_ms;
    }

    auto slowest = std::max_element(results.begin(), results.end(),
        [](const DownloadResult& a, const DownloadResult& b){
            return a.duration_ms < b.duration_ms;
        });

    std::cout << "\n  Succeeded : " << ok_count << "/" << results.size() << "\n"
              << "  Total wall: " << total_ms << " ms (sequential sum)\n"
              << "  Slowest   : " << slowest->url
              << " (" << slowest->duration_ms << " ms)\n";

    return 0;
}
