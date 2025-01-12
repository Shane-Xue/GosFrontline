#ifndef SAFEQUEUE_H
#define SAFEQUEUE_H

#include <queue>
#include <mutex>

namespace GosFrontline
{
  template <typename T>
  class SafeQueue
  {

  private:
    std::queue<T> queue;
    mutable std::mutex mutex;
    std::condition_variable condition;

  public:
    void push(const T &item)
    {
      std::lock_guard<std::mutex> lock(mutex);
      queue.push(item);
      condition.notify_one();
    }

    /// @brief Pushing an item to the front of the queue
    /// @param item
    /// @warning DO NOT USE THIS FUNCTION UNLESS YOU KNOW WHAT YOU ARE DOING.
    ///          THIS IS NOT A DEQUE!
    void _push_front(const T &item)
    {
      std::lock_guard<std::mutex> lock(mutex);
      T dump;
      std::queue<T> temp_dump;
      while (not queue.empty())
      {
        dump = queue.front();
        queue.pop();
        temp_dump.push(dump);
      }
      queue.push(item);
      while (not temp_dump.empty())
      {
        dump = temp_dump.front();
        temp_dump.pop();
        queue.push(dump);
      }
      condition.notify_one();
    }

    // void push(const T &&item)
    // {
    //   std::lock_guard<std::mutex> lock(mutex);
    //   queue.push(std::forward<T>(item));
    //   condition.notify_one();
    // }

    T pop()
    {
      std::unique_lock<std::mutex> lock(mutex);
      condition.wait(lock, [this]
                     { return !queue.empty(); });
      T item = std::move(queue.front());
      queue.pop();
      return item;
    }

    bool empty() const
    {
      const std::lock_guard<std::mutex> lock(mutex);
      return queue.empty();
    }
  };

} // namespace GosFrontline

#endif // SAFEQUEUE_H