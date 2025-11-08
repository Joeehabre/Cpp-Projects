#include <iostream>
#include <vector>
#include <optional>
#include <string>

template<typename K, typename V>
class HashMap {
    enum State { EMPTY, FILLED, DELETED };
    struct Slot { K key; V val; State st = EMPTY; };

    std::vector<Slot> table;
    size_t sz = 0;

    size_t hash(const K& k) const {
        return std::hash<K>{}(k);
    }

    void rehash(size_t newcap){
        std::vector<Slot> old = std::move(table);
        table.assign(newcap, {});
        sz = 0;
        for (auto &s : old){
            if (s.st == FILLED) insert(s.key, s.val);
        }
    }

    double load_factor() const { return (double)sz / table.size(); }

public:
    explicit HashMap(size_t cap=16): table(cap) {}

    bool insert(const K& k, const V& v){
        if (load_factor() > 0.6) rehash(table.size()*2);
        size_t i = hash(k) % table.size();
        size_t firstDel = table.size();
        for (;; i = (i+1) % table.size()){
            if (table[i].st == EMPTY) {
                size_t idx = (firstDel != table.size()) ? firstDel : i;
                table[idx].key = k; table[idx].val = v; table[idx].st = FILLED; sz++; return true;
            }
            if (table[i].st == DELETED && firstDel == table.size()) firstDel = i;
            if (table[i].st == FILLED && table[i].key == k) { table[i].val = v; return false; }
        }
    }

    std::optional<V> get(const K& k) const {
        size_t i = hash(k) % table.size();
        for (;; i = (i+1) % table.size()){
            if (table[i].st == EMPTY) return std::nullopt;
            if (table[i].st == FILLED && table[i].key == k) return table[i].val;
        }
    }

    bool erase(const K& k){
        size_t i = hash(k) % table.size();
        for (;; i = (i+1) % table.size()){
            if (table[i].st == EMPTY) return false;
            if (table[i].st == FILLED && table[i].key == k){ table[i].st = DELETED; sz--; return true; }
        }
    }

    size_t size() const { return sz; }
};

int main(){
    HashMap<std::string, int> m;
    m.insert("joe", 1);
    m.insert("habre", 2);
    m.insert("aub", 3);
    m.insert("joe", 5); // overwrite

    std::cout << "size=" << m.size() << "\n";
    for (auto key : {"joe","habre","aub","cs"}){
        auto v = m.get(key);
        std::cout << key << " => " << (v ? std::to_string(*v) : "null") << "\n";
    }
    m.erase("aub");
    std::cout << "after erase aub, size=" << m.size() << "\n";
}
