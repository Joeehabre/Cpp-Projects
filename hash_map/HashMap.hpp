#pragma once
#include <cstdint>
#include <functional>
#include <optional>
#include <stdexcept>
#include <vector>

template<typename K, typename V>
class HashMap {
    enum class State : uint8_t { EMPTY, FILLED, DELETED };
    struct Slot {
        K key;
        V val;
        State st = State::EMPTY;
    };

    std::vector<Slot> table_;
    size_t sz_ = 0;

    size_t probe_start(const K& k) const {
        return std::hash<K>{}(k) % table_.size();
    }

    // Returns the index of the key if found, or table_.size() if not.
    // Also returns the first DELETED slot seen (for insert reuse).
    struct ProbeResult { size_t found; size_t first_del; };
    ProbeResult probe(const K& k) const {
        size_t i = probe_start(k);
        size_t first_del = table_.size();
        size_t probes = 0;
        while (probes < table_.size()) {
            if (table_[i].st == State::EMPTY)
                return {table_.size(), first_del};
            if (table_[i].st == State::DELETED && first_del == table_.size())
                first_del = i;
            if (table_[i].st == State::FILLED && table_[i].key == k)
                return {i, first_del};
            i = (i + 1) % table_.size();
            ++probes;
        }
        return {table_.size(), first_del};
    }

    void rehash(size_t new_cap) {
        std::vector<Slot> old = std::move(table_);
        table_.assign(new_cap, {});
        sz_ = 0;
        for (auto& s : old)
            if (s.st == State::FILLED)
                insert(s.key, s.val);
    }

    double load_factor() const { return static_cast<double>(sz_) / table_.size(); }

public:
    explicit HashMap(size_t cap = 16) : table_(cap) {}

    // Returns true if a new key was inserted, false if an existing key was updated.
    bool insert(const K& k, const V& v) {
        if (load_factor() > 0.6)
            rehash(table_.size() * 2);

        auto [found, first_del] = probe(k);
        if (found < table_.size()) {
            table_[found].val = v;
            return false;
        }
        // Use a recycled deleted slot if one was seen during probing.
        size_t idx = (first_del < table_.size()) ? first_del : probe_start(k);
        // Walk forward from probe_start if no deleted slot and no empty was hit fast.
        if (first_del >= table_.size()) {
            idx = probe_start(k);
            while (table_[idx].st == State::FILLED)
                idx = (idx + 1) % table_.size();
        }
        table_[idx] = {k, v, State::FILLED};
        ++sz_;
        return true;
    }

    std::optional<V> get(const K& k) const {
        auto [found, _] = probe(k);
        if (found < table_.size()) return table_[found].val;
        return std::nullopt;
    }

    bool contains(const K& k) const {
        return probe(k).found < table_.size();
    }

    bool erase(const K& k) {
        auto [found, _] = probe(k);
        if (found >= table_.size()) return false;
        table_[found].st = State::DELETED;
        --sz_;
        return true;
    }

    // Inserts default-constructed value if key absent; returns reference to value.
    V& operator[](const K& k) {
        if (load_factor() > 0.6)
            rehash(table_.size() * 2);
        auto [found, first_del] = probe(k);
        if (found < table_.size()) return table_[found].val;
        size_t idx = (first_del < table_.size()) ? first_del : probe_start(k);
        if (first_del >= table_.size()) {
            idx = probe_start(k);
            while (table_[idx].st == State::FILLED)
                idx = (idx + 1) % table_.size();
        }
        table_[idx] = {k, V{}, State::FILLED};
        ++sz_;
        return table_[idx].val;
    }

    void clear() {
        table_.assign(table_.size(), {});
        sz_ = 0;
    }

    size_t size()     const { return sz_; }
    bool   empty()    const { return sz_ == 0; }
    size_t capacity() const { return table_.size(); }

    // Range-for support — iterates over FILLED slots only.
    struct Iterator {
        const std::vector<Slot>* tbl;
        size_t idx;

        void advance() {
            while (idx < tbl->size() && (*tbl)[idx].st != State::FILLED)
                ++idx;
        }
        Iterator(const std::vector<Slot>* t, size_t i) : tbl(t), idx(i) { advance(); }

        std::pair<const K&, const V&> operator*() const {
            return {(*tbl)[idx].key, (*tbl)[idx].val};
        }
        Iterator& operator++() { ++idx; advance(); return *this; }
        bool operator!=(const Iterator& o) const { return idx != o.idx; }
    };

    Iterator begin() const { return {&table_, 0}; }
    Iterator end()   const { return {&table_, table_.size()}; }
};
