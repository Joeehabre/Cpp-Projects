# Cpp-Projects

![Build](https://github.com/Joeehabre/Cpp-Projects/actions/workflows/build.yml/badge.svg)

A collection of **modern C++** projects by **Joe Habre (AUB)**.  
Each project showcases a distinct systems-programming concept: data structures, multithreading, AI, and concurrency patterns.

<p align="left">
  <img alt="MIT" src="https://img.shields.io/badge/License-MIT-green">
  <img alt="Language" src="https://img.shields.io/badge/C++-17%20%7C%2020-blue">
  <img alt="Platform" src="https://img.shields.io/badge/Platform-Linux%20%7C%20macOS-lightgrey">
</p>

---

## Projects

| Folder | Description | Key Concepts |
|---|---|---|
| [`hash_map/`](hash_map/) | Header-only hash map with open addressing and tombstone deletion | Templates, linear probing, rehashing, iterators |
| [`thread_pool/`](thread_pool/) | Thread pool where every task returns a `std::future` | `std::packaged_task`, condition variables, `wait_all()` |
| [`thread_downloader/`](thread_downloader/) | Concurrent file downloader with per-file timing and result tracking | `std::thread`, `std::mt19937`, mutex, structured results |
| [`tictactoe/`](tictactoe/) | Console Tic-Tac-Toe with an unbeatable AI | OOP, Minimax, alpha-beta pruning, depth scoring |

---

## Quick Start

Each project is self-contained with its own `MakeFile`. Clone the repo, pick a project, and build:

```bash
git clone https://github.com/Joeehabre/Cpp-Projects.git
cd Cpp-Projects
```

### hash_map
```bash
cd hash_map
make
./hash_map
```

### thread_pool
```bash
cd thread_pool
make        # requires C++20
./thread_pool
```

### thread_downloader
```bash
cd thread_downloader
make
./thread_downloader
```

### tictactoe
```bash
cd tictactoe
make
./tictactoe
```

---

## Requirements

- **Compiler:** GCC 10+ or Clang 12+ (C++17 minimum; `thread_pool` requires C++20)
- **Build:** `make` (GNU Make)
- **Threading:** projects that use threads link with `-pthread` automatically via the Makefile

---

## License

[MIT](LICENSE)
