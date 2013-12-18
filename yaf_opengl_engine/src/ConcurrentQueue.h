#ifndef CONCURRENT_QUEUE_H
#define CONCURRENT_QUEUE_H

// C++11 implementation of a thread queue. Based on Juan Palacios impl. 

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

template<typename T>
class ConcurrentQueue
{
public:
    T pop()
    {
        std::unique_lock<std::mutex> mlock(_mutex);
        while (_queue.empty())
        {
            _cond.wait(mlock);
        }

        auto item = _queue.front();
        _queue.pop();
        return item;
    }

    void pop(T& item)
    {
        std::unique_lock<std::mutex> mlock(_mutex);
        while (_queue.empty())
        {
            _cond.wait(mlock);
        }

        item = _queue.front();
        _queue.pop();
    }

    void push(const T& item)
    {
        std::unique_lock<std::mutex> mlock(_mutex);
        _queue.push(item);
        mlock.unlock();
        _cond.notify_one();
    }

    void push(T&& item)
    {
        std::unique_lock<std::mutex> mlock(_mutex);
        _queue.push(std::move(item));
        mlock.unlock();
        _cond.notify_one();
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> mlock(_mutex);
        return _queue.empty();
    }

    typename std::queue<T>::size_type size() const
    {
        std::lock_guard<std::mutex> mlock(_mutex);
        return _queue.size();
    }

private:
    std::queue<T> _queue;
    mutable std::mutex _mutex;
    mutable std::condition_variable _cond;
};

#endif
