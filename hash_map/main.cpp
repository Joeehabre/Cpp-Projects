#include "HashMap.hpp"
#include <cassert>
#include <iostream>
#include <string>

int main() {
    HashMap<std::string, int> m;

    // Basic insert and get
    m.insert("joe",   1);
    m.insert("habre", 2);
    m.insert("aub",   3);
    m.insert("joe",   5);   // update existing key

    std::cout << "size=" << m.size() << "\n";   // 3

    for (const char* key : {"joe", "habre", "aub", "cs"}) {
        auto v = m.get(key);
        std::cout << key << " => " << (v ? std::to_string(*v) : "null") << "\n";
    }

    // contains
    std::cout << "contains(\"aub\")=" << m.contains("aub") << "\n";

    // erase
    m.erase("aub");
    std::cout << "after erase \"aub\", size=" << m.size() << "\n";
    std::cout << "contains(\"aub\")=" << m.contains("aub") << "\n";

    // operator[]
    m["grade"] = 100;
    m["grade"] += 5;
    std::cout << "grade=" << m["grade"] << "\n";

    // range-for
    std::cout << "\nAll entries:\n";
    for (auto [k, v] : m)
        std::cout << "  " << k << " -> " << v << "\n";

    // Stress test: insert enough keys to trigger multiple rehashes
    HashMap<int, int> big;
    for (int i = 0; i < 1000; ++i) big.insert(i, i * i);
    for (int i = 0; i < 1000; ++i) assert(big.get(i) == i * i);
    big.clear();
    assert(big.empty());
    std::cout << "\nStress test passed (1000 inserts + gets + clear).\n";

    return 0;
}
