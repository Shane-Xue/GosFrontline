#ifndef BACKEND_H
#define BACKEND_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include "Gaming.h"
#include "SafeQueue.h"

namespace GosFrontline
{
    class Backend
    {
    private:
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
        };

        enum class MoveReply
        {
            Success,
            Violation,
            WrongTurn,
            InvalidLocation,
            UnknownError
        };
        Gaming game;
        mutable std::recursive_mutex game_mutex;
        std::condition_variable game_cv;
        SafeQueue<Action> todo;
        SafeQueue<std::pair<int, int>> todo_move_human;
        SafeQueue<std::pair<int, int>> todo_move_engine;
        SafeQueue<std::pair<std::vector<std::vector<PieceType>>, int>> boards;
        SafeQueue<std::pair<MoveReply, int>> move_replies;

        Backend(){};

        void enqueueBoard();
        void registerHumanMove(int, int);
        void registerEngineMove(int, int);

    public:
        static Backend &getBackend();

        Backend(const Backend &) = delete;
        Backend &operator=(const Backend &) = delete;
        Backend(Backend &&) = delete;
        Backend &operator=(Backend &&) = delete;

        // Actions
        void frontendMove(int row, int col);
        void frontendUndo();
        void engineMove(int row, int col);
        PieceType tomove();

        // Getters
        std::pair<std::vector<std::vector<GosFrontline::PieceType>>, int> getBoard();
        bool boardUpdated();

        // Running
        int run();
    };
} // namespace GosFrontline

GosFrontline::Backend &GosFrontline::Backend::getBackend()
{
    static Backend instance;
    return instance;
}

void GosFrontline::Backend::frontendMove(int row, int col)
{
    todo_move_human.push(std::make_pair(row, col));
    todo.push(Action::MoveHuman);
}

void GosFrontline::Backend::frontendUndo()
{
    todo.push(Action::Undo);
}

void GosFrontline::Backend::engineMove(int row, int col)
{
    todo_move_engine.push(std::make_pair(row, col));
    todo.push(Action::MoveEngine);
}

GosFrontline::PieceType GosFrontline::Backend::tomove()
{
    return game.toMove();
}

void GosFrontline::Backend::enqueueBoard()
{
    std::lock_guard<std::recursive_mutex> lock(game_mutex);
    boards.push(std::make_pair(game.getBoard(), game.movesMade()));
}

std::pair<std::vector<std::vector<GosFrontline::PieceType>>, int> GosFrontline::Backend::getBoard()
{
    return boards.pop();
}

bool GosFrontline::Backend::boardUpdated()
{
    return !boards.empty();
}

void GosFrontline::Backend::registerHumanMove(int row, int col)
{
    std::lock_guard<std::recursive_mutex> lock(game_mutex);
    if (game.toMove() == game.engineSide())
    {
        move_replies.push(std::make_pair(MoveReply::WrongTurn, game.movesMade() + 1));
        return;
    }
    if (not game.isEmpty(row, col))
    {
        move_replies.push(std::make_pair(MoveReply::InvalidLocation, game.movesMade() + 1));
        return;
    }
    if (game.violation(row, col))
    {
        move_replies.push(std::make_pair(MoveReply::Violation, game.movesMade() + 1));
        return;
    }
    if (not game.makeMove(row, col))
    {
        move_replies.push(std::make_pair(MoveReply::UnknownError, game.movesMade() + 1));
    }
    move_replies.push(std::make_pair(MoveReply::Success, game.movesMade())); // Move was made, movesMade was implicitly updated
    enqueueBoard();
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
            std::pair<int, int> move = todo_move_human.pop();
            registerHumanMove(move.first, move.second);
            break;
        }
        case Action::MoveEngine:
        {
            std::pair<int, int> move = todo_move_engine.pop();
        }
        case Action::SetEngineStatusOff:
        {
            game.setEngineStatus(PieceType::None);
            break;
        }
        }
    }
}

#endif // BACKEND_H