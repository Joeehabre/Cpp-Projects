CXX=g++
CXXFLAGS=-Wall -Wextra -std=c++17 -pthread -O2

all: thread_downloader
thread_downloader: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o thread_downloader

clean:
	rm -f thread_downloader
