#ifndef INTERFACE_H
#define INTERFACE_H

#include <iostream>
#include <memory>
#include <regex>

#include "Backend.h"
#include "Board.h"
#include "Logger.h"

// ANSI Escape Sequences
#define RESET "\033[0m"
#define BLACK "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define RGB(r, g, b) "\033[38;2;" #r ";" #g ";" #b "m"

namespace GosFrontline
{
  class InterfaceBase
  {
  protected:
    using Move = std::pair<int, int>;

  public:
    virtual ~InterfaceBase() = default;
    // virtual void displayBoard() = 0;
    // virtual void displayMessage(const std::string &message) = 0;
    // virtual Move getPlayerMove() = 0;
    virtual void run() = 0;

  private:
  };

  class InterfaceCLI : public InterfaceBase
  {
  public:
    InterfaceCLI() : position{CurrentState::MainMenu} {};
    ~InterfaceCLI() noexcept override = default;
    // void displayBoard() override;
    // void displayMessage(const std::string &message) override;
    // Move getPlayerMove() override;
    void run();
    void clearScreen() const;
    void setLogger(std::shared_ptr<Logger>);

  private:
    enum class Color
    {
      Black = 0,
      Red,
      Green,
      Yellow,
      Blue,
      Magenta,
      Cyan,
      White,
      Default,
    };

    void printWelcome() const;
    void printMenu() const;
    void printBoard() const;
    void printMsg(const std::string &, Color) const;
    void quit();
    std::pair<int, int> getNumbers(std::string);
    std::string getInput(std::string prompt);
    void printBadInput() const;

    using Move = std::pair<int, int>;
    static const std::string welcome, menu_prompt, game_prompt, main_quit, bad_input, move_count, ask_board_size;
    static const std::string default_size, game_over_prompt;
    static const int timeout; // millisecond timeout
    bool game_over;
    bool pve;

    static const std::regex two_numbers;
    int move_now;

    std::function<Backend &()> backend = []() -> Backend &
    { return Backend::getBackend(); };

    std::vector<std::string> input_history{};
    std::shared_ptr<Logger> logger;
    std::stringstream logstream;
    bool exit = false;

    enum class CurrentState
    {
      MainMenu,
      InGame
    };

    CurrentState position;
  };

  const std::string InterfaceCLI::welcome = "Welcome to GosFrontline!\n"
                                            "This program is a Renju game powered by C++.\n"
                                            "Written By Shane Xue\n"
                                            "Github: https://github.com/Shane-Xue/GosFrontline\n"
                                            "\n"
                                            "God Save the Commander! Send him victorious, happy and glorious.\n"
                                            "(c) Shane Xue 2024-2025\n",
                    InterfaceCLI::menu_prompt = "What do you want to do?\n"
                                                "1. Play PVE\n"
                                                "2. Play PVP\n"
                                                "3. Load Existing Game\n"
                                                "4. View Help\n"
                                                "5. Renju Rules\n"
                                                "6. Exit\n"
                                                "Enter number as your choice:",
                    InterfaceCLI::game_prompt = "Enter your move: (row, column)\n"
                                                "Or you can do the following by entering the corresponding letter\n"
                                                "\033[4mQ\033[0muit without saving\n"
                                                "Sa\033[4mv\033[0me and quit\n"
                                                "\033[4mS\033[0mave Game\n"
                                                "\033[4mE\033[0xit Program (Without Saving)]",
                    InterfaceCLI::game_over_prompt = "Game is over. What do you want to do now?\n"
                                                     "\033[4mQ\033[0muit to main menu\n"
                                                     "\033[4mE\033[0xit Program (Without Saving)]",
                    InterfaceCLI::bad_input = "Invalid input! Please try again.",
                    InterfaceCLI::move_count = "Such was the board after move ",
                    InterfaceCLI::ask_board_size = "Please enter the size of the board in form (row col): (Press enter to default to 15*15 board)",
                    InterfaceCLI::default_size = "Did not get a valid input! Defaulting to 15*15\n",
                    InterfaceCLI::main_quit = "Thank you for using this program! Hit any key to close this window.";

  const std::regex InterfaceCLI::two_numbers(R"(\s+(\d+)\s+(\d+)\s+)");
  const int InterfaceCLI::timeout = 10000;

  std::pair<int, int> InterfaceCLI::getNumbers(std::string input)
  {
    std::smatch match;
    if (std::regex_match(input, match, two_numbers))
    {
      return {std::stoi(match[1]), std::stoi(match[2])};
    }
    return {-1, -1};
  }

  void InterfaceCLI::clearScreen() const
  {
    std::cout << "\033[2J\033[1;1H";
  }

  void InterfaceCLI::printWelcome() const
  {
    std::cout << welcome;
  }

  void InterfaceCLI::printMenu() const
  {
    std::cout << menu_prompt;
  }

  std::string InterfaceCLI::getInput(std::string prompt)
  {
    std::cout << prompt;
    std::string input;
    std::getline(std::cin, input);
    input_history.push_back(input);
    return input;
  }

  void InterfaceCLI::setLogger(std::shared_ptr<Logger> lg)
  {
    logger = lg;
  }

  void InterfaceCLI::quit()
  {
    std::cout << main_quit;
    logger->log("Frontend recieved exit signal. Frontend cleaning up.");
    // while (std::cin.peek() == std::char_traits<char>::eof())
    //   std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    exit = true;
    backend().quit();
    logger->log("Frontend has alerted the backend to end. Frontend now exiting.");
  }

  void InterfaceCLI::printBadInput() const
  {
    std::cout << bad_input;
  }

  void InterfaceCLI::printBoard() const
  {
    auto board_future = backend().getBoard();
    std::pair<std::vector<std::vector<PieceType>>, int> board = board_future.get();

    for (size_t i = 0; i < board.first[0].size(); i++)
    {
      std::cout << std::setw(2) << std::left << i + 1;
    }
    std::cout << std::endl;
    for (int i = 0; i < board.first.size(); ++i)
    {

      for (int j = 0; j < board.first[i].size(); ++j)
      {
        switch (board.first.at(i).at(j))
        {
        case PieceType::None:
          std::cout << "_ ";
          break;
        case PieceType::Sente:
          std::cout << "@ ";
          break;
        case PieceType::Gote:
          std::cout << "O ";
          break;

        default:
          throw std::runtime_error("Invalid piece type");
          break;
        }
      }
      std::cout << i + 1 << std::endl;
    }
  }

  void InterfaceCLI::printMsg(const std::string &msg, Color c = Color::Default) const
  {
    const std::string ESCAPE_CODE_PREFIX = "\033[";
    const std::string ESCAPE_CODE_SUFFIX = "m";

    switch (c)
    {
    case Color::Black:
      std::cout << ESCAPE_CODE_PREFIX << "30" << ESCAPE_CODE_SUFFIX;
      break;
    case Color::Red:
      std::cout << ESCAPE_CODE_PREFIX << "31" << ESCAPE_CODE_SUFFIX;
      break;
    case Color::Green:
      std::cout << ESCAPE_CODE_PREFIX << "32" << ESCAPE_CODE_SUFFIX;
      break;
    case Color::Yellow:
      std::cout << ESCAPE_CODE_PREFIX << "33" << ESCAPE_CODE_SUFFIX;
      break;
    case Color::Blue:
      std::cout << ESCAPE_CODE_PREFIX << "34" << ESCAPE_CODE_SUFFIX;
      break;
    case Color::Magenta:
      std::cout << ESCAPE_CODE_PREFIX << "35" << ESCAPE_CODE_SUFFIX;
      break;
    case Color::Cyan:
      std::cout << ESCAPE_CODE_PREFIX << "36" << ESCAPE_CODE_SUFFIX;
      break;
    case Color::White:
      std::cout << ESCAPE_CODE_PREFIX << "37" << ESCAPE_CODE_SUFFIX;
      break;
    case Color::Default:
    default:
      std::cout << ESCAPE_CODE_PREFIX << "0" << ESCAPE_CODE_SUFFIX;
      break;
    }
    std::cout << msg << std::endl;
    std::cout << "\033[0m";
  }

  void InterfaceCLI::run()
  {
    printWelcome();
    std::string current_prompt = menu_prompt;
    while (not exit)
    {
      std::string input = getInput(current_prompt);
      if (position == CurrentState::MainMenu)
      {
        int select;
        try
        {
          select = std::stoi(input);
        }
        catch (std::invalid_argument &e)
        {
          printBadInput();
          logger->log(std::string("Got bad input for menu selection: ") + input, MessageType::WARNING);
          continue;
        }
        std::string parameters = ""; // Will host any parameters needed
        std::pair<int, int> res;
        switch (select)
        {
        case 1:
          parameters = getInput(ask_board_size);
          res = getNumbers(parameters);
          if (res != std::pair<int, int>(-1, -1))
          {
            int rows = res.first;
            int cols = res.second;
            backend().newGame(rows, cols);
            logger->log("New game started with board size " + std::to_string(rows) + "*" + std::to_string(cols));
          }
          else
          {
            printMsg("Input corrupt or did not recieve special input. Defaulting to default board size.\n");
            logger->log("Got bad input for board size. Defaulting to default board size.", MessageType::WARNING);
            backend().newGame();
          }
          printMsg("Let's get the show on the road!", Color::Cyan);
          game_over = false;
          printBoard();
          position = CurrentState::InGame;
          current_prompt = game_prompt;
          pve = true;
          break;
        case 2:
          break;
        case 3:
          break;
        case 4:
          break;
        case 5:
          break;
        case 6:
          quit();
          break;

        default:
          printBadInput();
          break;
        }
      }
      else if (position == CurrentState::InGame)
      {
        std::pair<int, int> res = getNumbers(input);
        if (res != std::pair<int, int>(-1, -1))
        {
          if (game_over)
          {
            printMsg("Game is over! You cannot make a move anymore.", Color::Yellow);
            logger->log("User tried to make a move after game was over.", MessageType::WARNING);
            continue;
          }

          int row = res.first - 1;
          int col = res.second - 1;
          std::future<std::pair<GosFrontline::MoveReply, int>> result_future = backend().frontendMove(row, col);
          printMsg("Processing your move... Please be patient.");

          logstream.str("");
          logstream << "User moved to " << row << ", " << col << ". Awaiting backend reply.";
          logger->log(logstream.str());
          logstream.str("");

#ifdef DEBUG
          auto reply = result_future.wait_for(std::chrono::milliseconds(timeout));

          while (reply != std::future_status::ready)
            reply = result_future.wait_for(std::chrono::milliseconds(timeout));
#else
          auto reply = result_future.wait_for(std::chrono::milliseconds(timeout));
#endif
#ifndef DEBUG
          if (reply == std::future_status::timeout)
          {
            printMsg("Backend timed out! Please contact support.", Color::Red);
            logger->log("Backend timed out while trying to log user move. Now exiting.", MessageType::FATAL);
            quit();
          }
          else
#endif
              if (reply == std::future_status::ready)
          {
            auto [status, move_count] = result_future.get();
            if (status == GosFrontline::MoveReply::Win)
            {
              game_over = true;
              logger->log("Human user won the game.");
              printMsg("Congrats! You won the game!");
              current_prompt = game_over_prompt;
            }
            else if (status == GosFrontline::MoveReply::Success)
            {
              clearScreen();
              printMsg("Move successful!");
              printBoard();

              logstream.str("");
              logstream << "Backend Move successful.";
              logger->log(logstream.str());
              logstream.str("");

              if (pve)
              {
                printMsg("The AI is making its move...");
                logger->log("User is playing against the Engine. The Engine is now making its move.", MessageType::INFO);
                auto signal = backend().callEngine();
                bool exit = false, got_signal = false;
                // std::thread wait_input(
                //     [this, &choose_exit, &got_signal]()
                //     {
                //       char input;
                //       while ((not choose_exit) and (not got_signal))
                //       {
                //         logger->log("Thread is now waiting for any input.", MessageType::INFO);
                //         if ((not std::cin.eof()) and std::cin >> input and input == 'q')
                //         {
                //           std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                //           logger->log("User has chosen to exit the game.", MessageType::INFO);
                //           choose_exit = true;
                //         }
                //         // 清除输入缓冲区
                //         std::cin.clear();
                //         std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                //         logger->log("Unkown input, ignoring.", MessageType::INFO);

                //         printMsg("Please wait while the Engine makes its move...");
                //       }
                //     });
                auto begin_time = std::chrono::system_clock::now();
                while (signal.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready and (not exit))
                {
                  if (exit)
                  {
                    break;
                  }
                  if (std::chrono::system_clock::now() - begin_time > std::chrono::seconds(timeout))
                  {
                    logger->log("Backend timed out while waiting for Engine to make its move. Now exiting.", MessageType::FATAL);
                    exit = true;
                    quit();
                  }

                  // std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
                got_signal = true;
                // wait_input.join();
                logger->log("Input waiting thread has now been joined.");
                // AI has finished by now, can start Accquiring Board.
                if (not exit)
                {
                  logger->log("AI move has finished. Now outputting board.", MessageType::DEBUG);
                  printBoard();
                }
              }
            }
            else
            {
              switch (status)
              {
              case GosFrontline::MoveReply::InvalidLocation:
                printMsg("Invalid location! Please try again.", Color::Yellow);
                logger->log("Backend reported invalid location while client tried to make move.", MessageType::WARNING);
                break;
              case GosFrontline::MoveReply::Violation:
                printMsg("Violation! Please try again.", Color::Yellow);
                logger->log("Backend reported violation while client tried to make move.", MessageType::WARNING);

                break;
              case GosFrontline::MoveReply::WrongTurn:
                printMsg("Wrong turn! Please wait for the bot to move.", Color::Yellow);
                logger->log("Backend reported wrong turn while client tried to make move.", MessageType::WARNING);

                break;
              default:
                printMsg("Unknown error! Exiting. Contact support.", Color::Red);
                quit();
                logger->log("Backend reported unknown status while client tried to make move.", MessageType::FATAL);

                break;
              } // switch
              printBoard();
            } // else
          } // reply status
        } // res != -1
        else
        { // res == (-1 -1) Now you are sure this is one char
          switch (input[0])
          {
          // case 'u':
          //   backend().frontendUndo();
          //   break;
          case 'q':
          case 'Q':
            current_prompt = menu_prompt;
            position = CurrentState::MainMenu;
            clearScreen();
            break;
          case 'e':
          case 'E':
            quit();
          default:
            printBadInput();
            break;
          }
        }
      }
    }
  }
}

#endif // INTERFACE_H