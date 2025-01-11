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