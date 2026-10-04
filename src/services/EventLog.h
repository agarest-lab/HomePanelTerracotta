#pragma once
#include <ctime>
#include <deque>
#include <mutex>
#include <string>

// Registro degli ultimi eventi di HomePanel (il piu' recente in testa).
// Thread-safe: il repository HA potrebbe scrivere da un altro thread.
class EventLog
{
public:
    struct Entry { std::time_t when; std::string text; };

    static EventLog& instance() { static EventLog l; return l; }

    void add(const std::string& text)
    {
        std::lock_guard<std::mutex> g(m);
        q.push_front({ std::time(nullptr), text });
        if(q.size() > MAX) q.pop_back();
    }

    std::deque<Entry> snapshot()
    {
        std::lock_guard<std::mutex> g(m);
        return q;
    }

private:
    static constexpr size_t MAX = 10;
    std::deque<Entry> q;
    std::mutex m;
};