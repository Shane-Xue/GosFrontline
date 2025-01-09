#ifndef BACKEND_H
#define BACKEND_H

#include <any>
#include <array>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <iomanip>
#include <functional>
#include <map>
#include <mutex>
#include <queue>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>
#include <unordered_set>
#include <utility>

#include "Gaming.h"
#include "MCTS.h"

namespace GosFrontline
{

  class UUID
  {
  public:
    /// @brief Generates a random UUID
    UUID() : UUID(generateUUID()) {}

    /// @brief Constructor that Will give a UUID from a given array of 16 bytes
    /// @param uuid_bytes
    explicit UUID(const std::array<uint8_t, 16> &uuid_bytes) : uuid(uuid_bytes) {}

    /// @brief Returns the UUID as a string in the format "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
    /// @return The UUID as a string
    std::string str() const
    {
    }
    /// @return
    std::string str() const
    {
      std::stringstream ss;
      ss << std::hex << std::setfill('0');
      for (size_t i = 0; i < 16; ++i)
      {
        if (i == 4 || i == 6 || i == 8 || i == 10)
          ss << "-";
        ss << std::setw(2) << static_cast<int>(uuid[i]);
      }
      return ss.str();
    }

  private:
    /// @brief Underlying UUID
    std::array<uint8_t, 16> uuid;

    struct UUIDHash
    {
      size_t operator()(const std::array<uint8_t, 16> &uuid) const
      {
        std::hash<uint64_t> hasher;
        uint64_t high = *reinterpret_cast<const uint64_t *>(uuid.data());
        uint64_t low = *reinterpret_cast<const uint64_t *>(uuid.data() + 8);
        return hasher(high) ^ hasher(low);
      }
    };

    /// @brief UUID generator. Checks against redundancy.
    static std::array<uint8_t, 16> generateUUID()
    {
      static std::random_device rd;
      static std::mt19937_64 gen(rd());
      static std::uniform_int_distribution<uint8_t> dis(0, 255);
      static std::mutex mtx;
      static std::unordered_set<std::array<uint8_t, 16>, UUIDHash> used_uuids;

      std::lock_guard<std::mutex> lock(mtx); // Ensure thread safety

      std::array<uint8_t, 16> uuid;
      do
      {
        for (auto &byte : uuid)
        {
          byte = dis(gen);
        }
        // Set the version (4 - random) and variant (2 - RFC 4122)
        uuid[6] = (uuid[6] & 0x0F) | 0x40; // Version 4
        uuid[8] = (uuid[8] & 0x3F) | 0x80; // Variant 2
      } while (!used_uuids.insert(uuid).second);

      return uuid;
    }
  };

  template <typename T>
  class SafeQueue
  {
  public:
    void push(const T &item)
    {
      std::lock_guard<std::mutex> lock(mutex);
      queue.push(item);
      condition.notify_one();
    }

    T pop()
    {
      std::unique_lock<std::mutex> lock(mutex);
      condition.wait(lock, [this]
                     { return !queue.empty(); });
      T item == move(queue.front());
      queue.pop();
      return item;
    }

    bool empty() const
    {
      std::lock_guard<std::mutex> lock(mutex);
      return queue.empty();
    }

  private:
    std::queue<T> queue;
    std::mutex mutex;
    std::condition_variable condition;
  };

  class RequestBase
  {
  public:
    virtual ~RequestBase() = default;
    virtual std::any getArguments() const = 0;
  };

  template <typename... Args>
  class Requests : public RequestBase
  {
  public:
    Requests(const std::string &alias, Args &&...args)
        : functionAlias(alias), arguments(std::forward<Args>(args)...) {}

    std::string getFunctionAlias() const
    {
      return functionAlias;
    }

    template <std::size_t Index>
    auto getArgument() const
    {
      return std::get<Index>(arguments);
    }

    std::any getArguments() const override
    {
      return arguments;
    }

  private:
    std::string functionAlias;
    std::tuple<Args...> arguments;
  };

  /// @brief Package a request
  /// @tparam ...Args
  /// @param alias
  /// @param ...args
  /// @return Packaged request
  template <typename... Args>
  Requests<Args...> make_request(const std::string &alias, Args &&...args)
  {
    return Requests<Args...>(alias, std::tuple<Args...>(std::forward<Args>(args)...));
  }

  class Backend
  {
  private:
    Backend() = default;
    SafeQueue<std::unique_ptr<RequestBase>> requestQueue;
    std::mutex queueLock;
    std::condition_variable queueCondition;

  public:
    static Backend &getInstance()
    {
      static Backend instance;
      return instance;
    }

    Backend(Backend &) = delete;
    Backend(const Backend &) = delete;
    Backend operator=(Backend &) = delete;

    template<typename... Args>
    void pushRequest(const std::string &alias, Args &&...args)
    {
      std::lock_guard<std::mutex> lock(queueLock);
      requestQueue.push(std::make_unique<Requests<Args...>>(alias, std::forward<Args>(args)...));
      queueCondition.notify_one();
    }

    int operator()(){
      while(true){
        std::unique_lock<std::mutex> lock(queueLock);
        queueCondition.wait(lock, [this] { return !requestQueue.empty(); });
        auto request = requestQueue.pop();

      }
    }
  };

} // namespace GosFrontline

#endif // BACKEND_H
