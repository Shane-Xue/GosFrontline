#ifndef REQUEST_H
#define REQUEST_H

#include <any>
#include <string>
#include <tuple>
#include <utility>
#include <variant>
#include <memory>
#include <type_traits>

#include "UUID.h"

namespace GosFrontline
{

  class RequestBase
  {
  public:
    virtual ~RequestBase() = default;
    virtual std::any getArguments() const = 0;
    virtual std::string getFunctionAlias() const = 0;
    virtual const UUID getUUID() const
    {
      return identifier;
    }

  private:
    UUID identifier;
  };

  template <typename... Args>
  class Requests : public RequestBase
  {
  public:
    Requests(const std::string &alias, Args &&...args)
        : functionAlias(alias), arguments(std::forward<Args>(args)...), identifier(UUID()) {}

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
    UUID identifier;
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

  class ResponseBase
  {
  public:
    virtual ~ResponseBase() = default;
    virtual std::any getResult() const = 0;
    virtual const UUID getUUID() const
    {
      return identifier;
    }
    virtual std::string getFunctionAlias() const = 0;

  private:
    UUID identifier;
  };

  template <typename T>
  class Response : public ResponseBase
  {
  private:
    std::variant<std::reference_wrapper<T>, std::unique_ptr<T>> result;
    std::string functionAlias;
    UUID identifier;

  public:
    explicit Response(const std::string &alias, T &&result, UUID identifier)
        : functionAlias(alias), result(std::make_unique<T>(std::forward<T>(result))), identifier(identifier) {}

    explicit Response(const std::string &alias, T &result, UUID identifier)
        : functionAlias(alias), result(std::ref(result)), identifier(identifier) {}

    std::string getFunctionAlias() const override
    {
      return functionAlias;
    }

    std::variant<std::reference_wrapper<T>, std::unique_ptr<T>> getResult() const override
    {
      return result;
    }
  };
} // namespace GosFrontline

#endif // REQUEST_H