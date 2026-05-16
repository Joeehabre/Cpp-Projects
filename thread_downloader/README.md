# thread_downloader

A simulated concurrent file downloader that spawns one thread per file, tracks per-download timing and success/failure, and prints a structured summary when all threads finish.

## Features

- One `std::thread` per download — all run in parallel
- Per-download `DownloadResult` struct: id, URL, duration in ms, success flag
- 10% random failure rate to demonstrate error tracking
- Proper random delays via `std::mt19937` seeded from `std::random_device` (thread-safe)
- Tabular summary on completion: status, duration, slowest download

## Sample Output

```
[1] Downloading https://example.com/file1.zip ...
[2] Downloading https://example.com/file2.tar.gz ...
...
[2] https://example.com/file2.tar.gz -> OK (553 ms)
[1] https://example.com/file1.zip -> FAILED (1026 ms)

--- Download Summary ---
ID  URL                                   Duration  Status
------------------------------------------------------------
1   https://example.com/file1.zip         1026 ms   FAILED
2   https://example.com/file2.tar.gz      553 ms    OK
...

  Succeeded : 5/6
  Total wall: 4800 ms (sequential sum)
  Slowest   : https://example.com/image.png (1105 ms)
```

## Build & Run

```bash
make
./thread_downloader
```

Requires C++17 and `-pthread`.
