#ifndef BACKEND_H
#define BACKEND_H

#include <thread>
#include <memory>
#include <mutex>
#include <sstream>
#include <condition_variable>
#include <chrono>
#include <filesystem>

#include "Gaming.h"
#include "SafeQueue.h"
#include "Logger.h"
#include "MCTS.h"

namespace GosFrontline
{
  enum class MoveReply
  {
    Success,
    Win,
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
    using BoardAndSequence = std::pair<BoardType, std::vector<std::tuple<int, int, GosFrontline::PieceType>>>;

    template <typename T>
    using PromiseWrapper = std::shared_ptr<std::promise<T>>;

    enum class Action
    {
      MoveHuman,
      MoveEngine,
      Undo,
      Quit,
      GetBoard,
      GetBoardAndSequence,
      SetEngineStatus,
      SetEngineStatusOff,
      Save,
      NewGame,
      SetSenteName,
      SetGoteName,
      SetViolationPolicy,
      CallEngine,
      ReverseSides
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
    SafeQueue<std::pair<std::filesystem::path, PromiseWrapper<void>>> todo_save;
    SafeQueue<PromiseWrapper<BoardAndSequence>> board_and_sequences;

    std::stringstream log_stream;

    Backend() {};

    // void enqueueBoard();
    std::pair<GosFrontline::MoveReply, int> registerHumanMove(int, int);
    std::pair<GosFrontline::MoveReply, int> registerEngineMove(int, int);

    void boardSaver(std::filesystem::path);

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
    std::future<void> frontendUndo();
    void engineMove(int row, int col, PromiseWrapper<void> status);
    std::future<void> callEngine();
    std::future<void> save(std::string);
    void reverseSides();
    void newGame(int row, int col);
    void quit();
    PieceType tomove();

    // Getters
    std::future<BoardAndStep> getBoard();
    // std::pair<MoveReply, int> getMoveReply();
    std::future<BoardAndSequence> getBoardAndSequence();
    bool boardUpdated();
    std::future<void> checkWin();

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

std::future<void> GosFrontline::Backend::frontendUndo()
{
  PromiseWrapper<void> ready = std::make_shared<std::promise<void>>();
  std::future<void> ready_future = ready->get_future();
  todo_actions.push(std::make_pair(Action::Undo, ready));
  todo.push(Action::Undo);
  return ready_future;
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
  logger->log("Logged in Action::GetBoard.");
  return ready_future;
}

std::future<GosFrontline::Backend::BoardAndSequence> GosFrontline::Backend::getBoardAndSequence()
{
  auto ready = std::make_shared<std::promise<BoardAndSequence>>();
  std::future<BoardAndSequence> ready_future = ready->get_future();
  board_and_sequences.push(ready);
  todo.push(Action::GetBoardAndSequence);
  logger->log("Logged in Action::GetBoardAndSequence.");
  return ready_future;
}

bool GosFrontline::Backend::boardUpdated() // Deprecated!
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
  // Game move should already have been made. Check if it is a winning move.
  if (game.checkCurrentWin(row, col) == Opposite(game.toMove()))
  {
    return std::make_pair(MoveReply::Win, game.movesMade());
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
  logger->log("Logged in Action::NewGame.");
}

std::future<void> GosFrontline::Backend::callEngine()
{
  auto ready = std::make_shared<std::promise<void>>();
  auto ready_future = ready->get_future();
  todo_actions.push(std::make_pair(Action::CallEngine, ready));
  todo.push(GosFrontline::Backend::Action::CallEngine);
  logger->log("Logged in Action::CallEngine.");
  return ready_future;
}

std::future<void> GosFrontline::Backend::save(std::string filename)
{
  auto ready = std::make_shared<std::promise<void>>();
  auto ready_future = ready->get_future();
  todo_save.push(std::make_pair(filename, ready));
  todo.push(Action::Save);
  logger->log("Logged in Action::Save.");
  return ready_future;
}

void GosFrontline::Backend::quit()
{
  todo.push(Action::Quit);
  logger->log("Logged in Action::Quit.");
}

void GosFrontline::Backend::reverseSides()
{
  todo.push(Action::ReverseSides);
  logger->log("Logged in Action::ReverseSides.");
}

void GosFrontline::Backend::boardSaver(std::filesystem::path p)
{
  std::ofstream out(p, std::ios::out | std::ios::trunc);
  if (not out.is_open())
  {
    logger->log("Failed to open file for saving. Giving up save.", MessageType::WARNING);
    return;
  } // Silent fail
  std::string sente = game.getSenteName(), gote = game.getGoteName();
  auto board_future = getBoardAndSequence();
  auto result = board_future.wait_for(std::chrono::seconds(60));
  if (result == std::future_status::timeout)
  {
    logger->log("Failed to get board from backend. Giving up save.", MessageType::WARNING);
    return;
  }
  auto board_and_sequence = board_future.get();
  auto board = board_and_sequence.first;
  auto sequence = board_and_sequence.second;
  logger->log("Acquired adequate resources. Saving board to file.", MessageType::INFO);
  out << "Sente: " << sente << "\nGote: " << gote << "\n";
  out << "Time: " << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() << "\n";
  for (auto &&row : board)
  {
    for (auto &&item : row)
    {
      switch (item)
      {
      case GosFrontline::PieceType::Sente:
        out << "@";
        break;
      case GosFrontline::PieceType::Gote:
        out << "O";
        break;
      case GosFrontline::PieceType::None:
      default:
        out << " ";
        break;
      }
    }
    out << std::endl;
  }
  for (auto &&entry : sequence)
  {
    out << std::get<0>(entry) << ' ' << std::get<1>(entry) << ' ' << ((std::get<2>(entry) == PieceType::Sente) ? ("Sente") : ("Gote")) << std::endl;
  }
  logger->log("Saved board to file. Saving thread now exiting.", MessageType::INFO);
  return;
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
      logger->log("This api is deprecated due to sequencial concerns. Control stream should not naturally come here.", MessageType::ERROR);
      break;
    }
    case Action::SetEngineStatusOff:
    {
      game.setEngineStatus(PieceType::None);
      break;
    }
    case Action::ReverseSides:
    {
      game.setEngineStatus(Opposite(game.engineSide()));
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
    case Action::GetBoardAndSequence:
    {
      std::unique_lock<std::recursive_mutex> lock(game_mutex);
      std::shared_ptr<std::promise<GosFrontline::Backend::BoardAndSequence>> board = board_and_sequences.pop();
      logger->log("Preparing the board and sequence.");
      board->set_value(std::make_pair(game.getBoard(), game.getSequence()));
      lock.unlock();
      break;
    }
    case Action::Save:
    {
      auto save_info = todo_save.pop();
      std::thread([this, save_info]()
                  { boardSaver(save_info.first); })
          .detach();
      save_info.second->set_value();
      break;
    }
    case Action::CallEngine:
    {
      logger->log("Calling Engine.", MessageType::INFO);
      auto thisPromise = todo_actions.pop();
      SafeQueue<decltype(thisPromise)> promises_cache;

      while (thisPromise.first != Action::CallEngine)
      {
        if (todo_actions.empty())
        {
          logger->log("Engine move promise lost. Throwing.", MessageType::FATAL);
          throw std::runtime_error("Engine move promise lost.");
        }
        thisPromise = todo_actions.pop();
      }
      while (not promises_cache.empty())
      {
        auto temp = promises_cache.pop();
        todo_actions.push(temp);
      }
      logger->log("Engine move promise found.", MessageType::INFO);
      auto randmove = MCTS().getRandomMove(game);
      game.makeMoveEngine(randmove.first, randmove.second);
      logger->log("Engine has decided on move.");
      break;
    }
    case Action::Undo:
    {
      logger->log("Undo Requested");
      auto status = game.undo();
      if ((not status) and (game.toMove() == game.engineSide()))
      {
          todo._push_front(Action::CallEngine);
          PromiseWrapper<void> promise;
          todo_actions.push(std::make_pair(Action::CallEngine, promise));
          logger->log("Engine must do first move. Pushed in Action::CallEngine.");
      }
      
      auto thisPromise = todo_actions.pop();
      SafeQueue<decltype(thisPromise)> promises_cache;
      while (thisPromise.first != Action::Undo)
      {
        promises_cache.push(thisPromise);
        if (todo_actions.empty())
        {
          logger->log("Undo promise lost. Throwing.", MessageType::FATAL);
          throw std::runtime_error("Engine move promise lost.");
        }
        thisPromise = todo_actions.pop();
      }
      logger->log("Undo promise found.");
      while (not promises_cache.empty())
      {
        auto temp = promises_cache.pop();
        todo_actions.push(temp);
      }

      thisPromise.second->set_value();
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