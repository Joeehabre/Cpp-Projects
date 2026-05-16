# hash_map

A header-only hash map implemented in C++17 using **open addressing with linear probing** and **tombstone deletion**.

## Features

- `insert(key, value)` — inserts or updates; returns `true` if a new key was added
- `get(key)` — returns `std::optional<V>`; `std::nullopt` if not found
- `contains(key)` — O(1) membership check
- `erase(key)` — tombstone deletion; returns `true` on success
- `operator[](key)` — inserts a default value if absent, returns `V&`
- `clear()` — resets all slots
- `size()`, `empty()`, `capacity()`
- **Range-for** support — iterates over filled slots only

## Design

| Detail | Value |
|---|---|
| Collision resolution | Linear probing |
| Load factor threshold | 0.6 — triggers rehash to 2× capacity |
| Deleted-slot reuse | Tombstone + first-deleted-slot tracking |
| Probe safety | Capped at `capacity()` probes — no infinite loops |

## Usage

```cpp
#include "HashMap.hpp"

HashMap<std::string, int> m;

m.insert("alice", 42);
m["bob"] = 7;           // operator[] inserts default if absent

if (m.contains("alice"))
    std::cout << *m.get("alice") << "\n";   // 42

m.erase("alice");

for (auto [k, v] : m)
    std::cout << k << " -> " << v << "\n";
```

## Build & Run

```bash
make
./hash_map
```

Requires C++17 or later.
