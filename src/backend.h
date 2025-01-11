#ifndef BACKEND_H
#define BACKEND_H

#include <thread>
#include <memory>
#include <mutex>
#include <sstream>
#include <condition_variable>
#include <chrono>

#include "Gaming.h"
#include "SafeQueue.h"
#include "Logger.h"
#include "MCTS.h"

namespace GosFrontline
{
  enum class MoveReply
  {
    Success,
    Violation,
    WrongTurn,
    InvalidLocation,
    UnknownError
  };

  class Backend
  {
  private:
    using BoardType = std::vector<std::vector<GosFrontline::PieceType>>;
    using BoardAndStep = std::pair<BoardType, int>;
    using MoveAndStep = std::pair<GosFrontline::MoveReply, int>;

    template <typename T>
    using PromiseWrapper = std::shared_ptr<std::promise<T>>;

    enum class Action
    {
      MoveHuman,
      MoveEngine,
      Undo,
      Quit,
      GetBoard,
      SetEngineStatus,
      SetEngineStatusOff,
      NewGame,
      SetSenteName,
      SetGoteName,
      SetViolationPolicy,
      CallEngine
    };

    Gaming game;
    std::shared_ptr<Logger> logger;
    mutable std::recursive_mutex game_mutex;
    std::condition_variable game_cv;
    SafeQueue<Action> todo;                                                                      // Inwards Queue
    SafeQueue<std::tuple<int, int, std::shared_ptr<std::promise<MoveAndStep>>>> todo_move_human; // Inwards Queue
    SafeQueue<std::tuple<int, int, PromiseWrapper<void>>> todo_move_engine;                      // Inwards Queue
    SafeQueue<PromiseWrapper<BoardAndStep>> boards;                                              // Outwards Queue
    SafeQueue<std::pair<MoveReply, int>> move_replies;                                           // Deprecated Queue
    SafeQueue<std::pair<Action, PromiseWrapper<void>>> todo_actions;

    std::stringstream log_stream;

    Backend() {};

    // void enqueueBoard();
    std::pair<GosFrontline::MoveReply, int> registerHumanMove(int, int);
    std::pair<GosFrontline::MoveReply, int> registerEngineMove(int, int);

    static const int default_size = 15;
    int rows = default_size, cols = default_size;

  public:
    static Backend &getBackend();

    Backend(const Backend &) = delete;
    Backend &operator=(const Backend &) = delete;
    Backend(Backend &&) = delete;
    Backend &operator=(Backend &&) = delete;

    // Actions
    std::future<MoveAndStep> frontendMove(int row, int col);
    void frontendUndo();
    void engineMove(int row, int col, PromiseWrapper<void> status);
    std::future<void> callEngine();
    void newGame(int row, int col);
    void quit();
    PieceType tomove();

    // Getters
    std::future<BoardAndStep> getBoard();
    std::pair<MoveReply, int> getMoveReply();
    bool boardUpdated();

    // Logging
    void setLogger(std::shared_ptr<Logger>);

    // Running
    int run();
  };
} // namespace GosFrontline

GosFrontline::Backend &GosFrontline::Backend::getBackend()
{
  static Backend instance;
  return instance;
}

std::future<GosFrontline::Backend::MoveAndStep> GosFrontline::Backend::frontendMove(int row, int col)
{
  auto ready = std::make_shared<std::promise<GosFrontline::Backend::MoveAndStep>>();
  std::future<GosFrontline::Backend::MoveAndStep> ready_future = ready->get_future();
  todo_move_human.push(std::move(std::make_tuple(row, col, ready)));
  todo.push(Action::MoveHuman);
  return ready_future;
}

void GosFrontline::Backend::frontendUndo()
{
  todo.push(Action::Undo);
}

void GosFrontline::Backend::engineMove(int row, int col, PromiseWrapper<void> status)
{
  todo_move_engine.push(std::make_tuple(row, col, status));
  todo.push(Action::MoveEngine);
}

GosFrontline::PieceType GosFrontline::Backend::tomove()
{
  return game.toMove();
}

// void GosFrontline::Backend::enqueueBoard()
// {
//   std::lock_guard<std::recursive_mutex> lock(game_mutex);
//   boards.push(std::move(std::make_pair(game.getBoard(), game.movesMade())));
// }

std::future<std::pair<std::vector<std::vector<GosFrontline::PieceType>>, int>> GosFrontline::Backend::getBoard()
{
  auto ready = std::make_shared<std::promise<std::pair<std::vector<std::vector<GosFrontline::PieceType>>, int>>>();
  std::future<std::pair<std::vector<std::vector<GosFrontline::PieceType>>, int>> ready_future = ready->get_future();
  boards.push(ready);
  todo.push(Action::GetBoard);
  return ready_future;
}

bool GosFrontline::Backend::boardUpdated()
{
  return !boards.empty();
}

std::pair<GosFrontline::MoveReply, int> GosFrontline::Backend::registerHumanMove(int row, int col)
{
  std::lock_guard<std::recursive_mutex> lock(game_mutex);
  if (game.toMove() == game.engineSide())
  {
    return std::make_pair(MoveReply::WrongTurn, game.movesMade() + 1);
  }
  if (not game.isValidCoord(row, col))
  {
    return std::make_pair(MoveReply::InvalidLocation, game.movesMade() + 1);
  }
  if (not game.isEmpty(row, col))
  {
    return std::make_pair(MoveReply::InvalidLocation, game.movesMade() + 1);
  }
  if (game.violation(row, col))
  {
    return std::make_pair(MoveReply::Violation, game.movesMade() + 1);
  }
  if (not game.makeMove(row, col))
  {
    return std::make_pair(MoveReply::UnknownError, game.movesMade() + 1);
  }

  // enqueueBoard();
  return std::make_pair(MoveReply::Success, game.movesMade()); // Move was made, movesMade was implicitly updated
}

std::pair<GosFrontline::MoveReply, int> GosFrontline::Backend::registerEngineMove(int row, int col)
{
  std::lock_guard<std::recursive_mutex> lock(game_mutex);
  if (game.toMove() != game.engineSide())
  {
    return std::make_pair(MoveReply::WrongTurn, game.movesMade() + 1);
  }
  if (not game.isValidCoord(row, col))
  {
    return std::make_pair(MoveReply::InvalidLocation, game.movesMade() + 1);
  }
  if (not game.isEmpty(row, col))
  {
    return std::make_pair(MoveReply::InvalidLocation, game.movesMade() + 1);
  }
  if (game.violation(row, col))
  {
    return std::make_pair(MoveReply::Violation, game.movesMade() + 1);
  }
  if (not game.makeMoveEngine(row, col))
  {
    return std::make_pair(MoveReply::UnknownError, game.movesMade() + 1);
  }

  // enqueueBoard();
  return std::make_pair(MoveReply::Success, game.movesMade()); // Move was made, movesMade was implicitly updated
}

void GosFrontline::Backend::newGame(int r = default_size, int c = default_size)
{
  rows = r;
  cols = c;
  todo.push(Action::NewGame);
}

std::future<void> GosFrontline::Backend::callEngine()
{
  auto ready = std::make_shared<std::promise<void>>();
  auto ready_future = ready->get_future();
  todo_actions.push(std::make_pair(Action::CallEngine, ready));
  todo.push(GosFrontline::Backend::Action::CallEngine);
  return ready_future;
}

void GosFrontline::Backend::quit()
{
  todo.push(Action::Quit);
}

int GosFrontline::Backend::run()
{
  game.clearBoard();
  while (true)
  {
    if (todo.empty())
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
      continue;
    }

    Action action = todo.pop();
    switch (action)
    {
    case Action::MoveHuman:
    {
      std::lock_guard<std::recursive_mutex> lock(game_mutex);
      std::tuple<int, int, std::shared_ptr<std::promise<std::pair<GosFrontline::MoveReply, int>>>> move = todo_move_human.pop();
      log_stream.str("");
      log_stream << "Trying to register human move (" << std::get<0>(move) << ", " << std::get<1>(move) << ")";
      log_stream.str("");

      std::get<2>(move)->set_value(registerHumanMove(std::get<0>(move), std::get<1>(move)));
      break;
    }
    case Action::MoveEngine:
    {
      std::tuple<int, int, PromiseWrapper<void>> move = todo_move_engine.pop();
      auto reply = registerEngineMove(std::get<0>(move), std::get<1>(move));
      if (reply.first != MoveReply::Success)
      {
        logger->log("Engine Made Bad Move.", MessageType::ERROR);
      }
      else
      {
        logger->log("Engine Made Good Move.", MessageType::INFO);
        std::get<2>(move)->set_value();
      }
      break;
    }
    case Action::SetEngineStatusOff:
    {
      game.setEngineStatus(PieceType::None);
      break;
    }
    case Action::NewGame:
    {
      logger->log(std::string("New Game Requested with parameters (") + std::to_string(rows) +
                  std::string(", ") + std::to_string(cols) + std::string(")"));
      game.clearBoard(rows, cols);
      break;
    }
    case Action::GetBoard:
    {
      std::unique_lock<std::recursive_mutex> lock(game_mutex);
      std::shared_ptr<std::promise<GosFrontline::Backend::BoardAndStep>> board = boards.pop();
      logger->log("Preparing the board.");
      board->set_value(std::make_pair(game.getBoard(), game.movesMade()));
      lock.unlock();
      break;
    }
    case Action::CallEngine:
    {
      logger->log("Calling Engine.", MessageType::INFO);
      auto thisPromise = todo_actions.pop();
      while (thisPromise.first != Action::CallEngine)
      {
        if (todo_actions.empty())
        {
          logger->log("Engine move promise lost. Throwing.", MessageType::FATAL);
          throw std::runtime_error("Engine move promise lost.");
        }
        thisPromise = todo_actions.pop();
      }
      logger->log("Engine move promise found.", MessageType::INFO);
      auto randmove = MCTS().getRandomMove(game);
      engineMove(randmove.first, randmove.second, thisPromise.second);
      logger->log("Engine has decided on move.");
      break;
    }
    case Action::Quit:
    {
      logger->log("Quitting", MessageType::INFO);
      // TODO: Autosave Current game as <autosave>
      return 0;
    }
    }
  }
}

void GosFrontline::Backend::setLogger(std::shared_ptr<GosFrontline::Logger> lg)
{
  this->logger = lg;
}

#endif // BACKEND_H