#ifndef BACKEND_H
#define BACKEND_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <utility>
#include <tuple>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <map>

#include "Gaming.h"
#include "Communication.h"
#include "UUID.h"
#include "MCTS.h"
#include "SafeQueue.h"

namespace GosFrontline
{
  template <typename Class, typename Func, typename Tuple>
  auto callMemberFunction(Class &obj, Func func, Tuple t)
  {
    using return_type = typename std::invoke_result<Func, Class, Tuple>::type;
    auto boundFunc = [&obj, func](auto &&...args) 
    {
      (obj.*func)(std::forward<decltype(args)>(args)...);
    };

    return std::apply(boundFunc, std::forward<Tuple>(t));
  }

  class Backend
  {
  private:
    Backend() = default;
    SafeQueue<std::unique_ptr<RequestBase>> requestQueue;
    std::mutex requestLock;
    std::condition_variable queueCondition;
    Gaming game;
    std::unordered_map<UUID, std::unique_ptr<ResponseBase>, UUIDHash> responseMap;
    std::mutex responseLock;

  public:
    static Backend &getInstance()
    {
      static Backend instance;
      return instance;
    }

    Backend(Backend &) = delete;
    Backend(const Backend &) = delete;
    Backend operator=(Backend &) = delete;

    template <typename... Args>
    UUID pushRequest(const std::string &alias, Args &&...args)
    {
      std::lock_guard<std::mutex> lock(requestLock);
      auto request = std::make_unique<Requests<Args...>>(alias, std::forward<Args>(args)...);
      UUID uuid = request->getUUID();
      requestQueue.push(request);
      queueCondition.notify_one();
      return uuid;
    }

    bool hasResponse(const UUID &uuid)
    {
      std::lock_guard<std::mutex> lock(responseLock);

      return responseMap.find(uuid) != responseMap.end();
    }

    std::unique_ptr<ResponseBase> getResponse(const UUID &uuid)
    {
      std::lock_guard<std::mutex> lock(responseLock);
      auto it = responseMap.find(uuid);
      if (it != responseMap.end())
      {
        auto response = std::move(it->second);
        responseMap.erase(it);
        return response;
      }
      return nullptr;
    }

    int run()
    {
      game = Gaming();
      while (true)
      {
        std::unique_lock<std::mutex> lock(requestLock);
        queueCondition.wait(lock, [this]
                            { return !requestQueue.empty(); });
        auto request = requestQueue.pop();
        auto type = request->getFunctionAlias();

        // Switch for request type:
        // Set Sente name
        // Set Gote name
        // Switch pvp
        // Switch pve
        // Move
        // Undo
        if (type == "makeMove")
        {
          auto rval = callMemberFunction(game, &Gaming::makeMove, std::any_cast<std::tuple<int, int>>(request->getArguments()));
          auto resp = Response(type, rval, request->getUUID());
          responseMap[request->getUUID()] = std::make_unique<decltype(resp)>(resp);
continue;
        }
        if (type == "getBoard")
        {
          auto rval = callMemberFunction(game, &Gaming::getBoard, std::any_cast<std::tuple<>>(request->getArguments()));
          auto resp = Response(type, rval, request->getUUID());
          responseMap[request->getUUID()] = std::make_unique<decltype(resp)>(resp);
          continue;
        }
      }
    }
  };

} // namespace GosFrontline

#endif // BACKEND_H
