#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>

std::mutex printLock;

void downloadFile(int id, const std::string &url) {
    {
        std::lock_guard<std::mutex> g(printLock);
        std::cout << "Thread " << id << " downloading " << url << "..." << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(800 + rand()%800));
    {
        std::lock_guard<std::mutex> g(printLock);
        std::cout << "Thread " << id << " finished " << url << std::endl;
    }
}

int main() {
    std::vector<std::string> urls = {
        "file1.zip", "file2.zip", "file3.zip", "file4.zip"
    };
    std::vector<std::thread> threads;
    for(size_t i=0;i<urls.size();++i){
        threads.emplace_back(downloadFile, (int)i+1, urls[i]);
    }
    for(auto &t: threads) t.join();
    std::cout << "All downloads complete." << std::endl;
    return 0;
}
