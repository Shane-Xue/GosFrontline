#ifndef INTERFACE_H
#define INTERFACE_H

#include <iostream>
#include <memory>
#include <regex>

#include "Backend.h"
#include "Board.h"
#include "Logger.h"
#include "Utilities.h"

// ANSI Escape Sequences
#define RESET "\e[0m"
#define BLACK "\e[30m"
#define RED "\e[31m"
#define GREEN "\e[32m"
#define YELLOW "\e[33m"
#define BLUE "\e[34m"
#define MAGENTA "\e[35m"
#define CYAN "\e[36m"
#define WHITE "\e[37m"
#define RGB(r, g, b) "\e[38;2;" #r ";" #g ";" #b "m"

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
    void undo();
    void save();
    bool readGame();
    std::pair<int, int> getNumbers(std::string);
    std::string getInput(std::string prompt, std::string fallback);
    void printBadInput() const;

    using Move = std::pair<int, int>;
    static const std::string welcome, menu_prompt, game_prompt, main_quit, bad_input, move_count, ask_board_size;
    static const std::string default_size, game_over_prompt, filename_prompt_save, ask_side;
    std::string save_location = "./saved_games";
    std::string autosave_file = "autosave.gfl";
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
                                                // "2. Play PVP\n"
                                                "2. Load Existing Game\n"
                                                // "4. View Help\n"
                                                // "5. Renju Rules\n"
                                                "3. Exit\n"
                                                "Enter number as your choice:",
                    InterfaceCLI::game_prompt = "Enter your move: (row, column)\n"
                                                "Or you can do the following by entering the corresponding letter\n"
                                                "\e[4mQ\e[0muit without saving\n"
                                                "Sa\e[4mv\e[0me and quit\n"
                                                "\e[4mS\e[0mave Game\n"
                                                "\e[4mE\e[0mxit Program (Without Saving)\n"
                                                "\e[4mU\e[0mndo last move\n",
                    InterfaceCLI::game_over_prompt = "Game is over. What do you want to do now?\n"
                                                     "\e[4mQ\e[0muit to main menu\n"
                                                     "\e[4mE\e[0mxit Program (Without Saving)\n",
                    InterfaceCLI::bad_input = "Invalid input! Please try again.",
                    InterfaceCLI::move_count = "Such was the board after move ",
                    InterfaceCLI::ask_board_size = "Please enter the size of the board in form (row col): (Press enter to default to 15*15 board)",
                    InterfaceCLI::default_size = "Did not get a valid input! Defaulting to 15*15\n",
                    InterfaceCLI::filename_prompt_save = "Please enter the name of the file you wish to save to:"
                                                         "\n(Entering nothing will result in saving to a default file, enter name to save to a custom file) ",
                    InterfaceCLI::ask_side = "Which side would you like to play, \e[4mS\e[0mente or \e[4mG\e[0mote? ",
                    InterfaceCLI::main_quit = "Thank you for using this program! Hit any key to close this window.";

  const std::regex InterfaceCLI::two_numbers(R"(\s*(\d+)\s+(\d+).*)");
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
    std::cout << "\e[2J\e[1;1H";
  }

  void InterfaceCLI::printWelcome() const
  {
    std::cout << welcome;
  }

  void InterfaceCLI::printMenu() const
  {
    std::cout << menu_prompt;
  }

  std::string InterfaceCLI::getInput(std::string prompt, std::string fallback = "q")
  {
    if (not std::cin.good())
    {
      if (std::cin.eof())
      {
        std::cout << "You entered Ctrl+Z.\n";
        logger->log("Frontend recieved EOF. Frontend immediately aborting.");
        quit();
      }
      else
      {
        std::cerr << "Error reading from stdin. Exiting." << std::endl;
        logger->log("Error reading from stdin. Exiting.");
      }
      return fallback;
    }

    std::cout << prompt;
    std::string input;
    std::getline(std::cin, input);
    logger->log(std::string("User input: ") + input, MessageType::INFO);
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

    std::cout << "Such was the board after move " << board.second << "\n";

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

  void InterfaceCLI::undo()
  {
    logger->log("Frontend requested an undo. Now trying undo.");
#ifdef DEBUG
    backend()
        .frontendUndo()
        .wait();
#else
    auto future_readiness = backend().frontendUndo().wait_for(std::chrono::milliseconds(timeout));
    if (future_readiness != std::future_status::ready)
    {
      logger->log("Frontend failed to undo within timeout threshold. Aborting.", MessageType::FATAL);
      printMsg("Sorry, the undo failed, we are aborting. Please contact support.", Color::Red);
      quit();
    }
#endif
  }

  void InterfaceCLI::save()
  {
    logger->log("Frontend requested a save. Now trying save.");
    auto input = getInput(filename_prompt_save);
    if (not std::filesystem::exists(save_location))
    {
      std::filesystem::create_directory(save_location);
    }
    std::string file_path = save_location + std::string("/") + GosFrontline::sanitize_filename(input) + ".gfl";

    logstream.str("");
    logstream << "Saving to " << file_path << "";
    logger->log(logstream.str());
    logstream.str("");

    if (std::filesystem::exists(file_path))
    {
      while (1)
      {
        printMsg("File already exists. Overwrite?(y/n)", Color::Yellow);
        logger->log("File already exists. Asking for client's choice.", MessageType::WARNING);
        auto result = getInput("");
        if (result[0] == 'y')
        {
          printMsg("Overwriting file.", Color::Green);
          logger->log("Client Chose to overwrite file.");
          break;
        }
        else if (result[0] == 'n')
        {
          printMsg("You chose not to overwrite. Saving to autosave file.", Color::Yellow);
          logger->log("Client Chose not to overwrite file. Writing to default autosave.", MessageType::WARNING);
          file_path = save_location + std::string("/") + autosave_file;
          break;
        }
      }
    }

    auto reply = backend().save(file_path);
    auto response = reply.wait_for(std::chrono::milliseconds(timeout));
    if (response == std::future_status::timeout)
    {
      printMsg("Sorry, the save failed, try again later.", Color::Red);
      logger->log("Frontend failed to save within timeout threshold. Giving up on saving.", MessageType::WARNING);
    }
    else if (response == std::future_status::ready)
    {
      printMsg("Saved to " + file_path, Color::Green);
    }
  }

  void InterfaceCLI::printMsg(const std::string &msg, Color c = Color::Default) const
  {
    const std::string ESCAPE_CODE_PREFIX = "\e[";
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
    std::cout << "\e[0m";
  }

  bool InterfaceCLI::readGame()
  {
    clearScreen();
    std::vector<std::string> files{};
    logger->log("User requested a read game. Now trying read game. Files in directory include: ");
    for (const auto &file : std::filesystem::directory_iterator(save_location))
    {
      logger->log(file.path().filename().string());
      files.push_back(file.path().filename().string());
    }
    std::string toload = "";
    while (toload == "")
    {
      printMsg("Here are the saved games:");
      for (int i = 0; i < files.size(); i++)
      {
        printMsg(std::to_string(i + 1) + ". " + files[i]);
      }
      std::string ipt = getInput("Which file would you like to load? (Enter number) ");
      int sequence;
      try
      {
        sequence = std::stoi(ipt) - 1;
      }
      catch (std::invalid_argument &e)
      {
        clearScreen();
        printMsg("Invalid input. Please enter a number.", Color::Red);
        continue;
      }
      if (sequence < files.size())
        toload = files.at(sequence);
      else
      {
        clearScreen();
        printMsg("Invalid input. Please enter a number within the range of " + std::to_string(files.size()) + ".", Color::Red);
        continue;
      }
    }
    auto reply = backend().loadGame(save_location + std::string("/") + toload);
    reply.wait();
    return reply.get();
  }

  void
  InterfaceCLI::run()
  {
    printWelcome();

    logstream.str("");
    logstream << "Frontend has been started. Now running on thread " << std::this_thread::get_id() << ".";
    logger->log(logstream.str());
    logstream.str("");

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
        std::string side = "NOTHING HERE";
        bool engineFirst = false;
        int ask_side_fail_count = 0;
        switch (select)
        {
        case 1:
          while (true)
          {
            side = getInput(ask_side);
            if (std::tolower(side[0]) == 's')
            {
              break;
            }
            else if (std::tolower(side[0]) == 'g')
            {
              engineFirst = true;
              break;
            }
            else
            {
              if (ask_side_fail_count > 2)
              {
                printMsg("You gave wrong input too many times. Assume you want to play sente.");
                logstream.str("");
                logstream << "Got bad input for side selection: failed 3 times. Defaulting to sente.";
                logger->log(logstream.str(), MessageType::WARNING);
                logstream.str("");
                break;
              }

              logstream.str("");
              logstream << "Got bad input for side selection: failed " << ++ask_side_fail_count << " times.";
              logger->log(logstream.str(), MessageType::WARNING);
              logstream.str("");

              printMsg("Oops, don't know what you mean... Which side do you want to play?");
            }
          }
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
          clearScreen();
          printMsg("Let's get the show on the road!", Color::Cyan);
          game_over = false;
          if (engineFirst)
          {
            backend().reverseSides();
            backend().callEngine();
          }

          printBoard();
          position = CurrentState::InGame;
          current_prompt = game_prompt;
          pve = true;
          break;
        case 2:
          logger->log("User input requested a read game.");
          if (readGame())
          {
          position = CurrentState::InGame;
           logger->log("Read game successful!");
           printBoard();
           current_prompt = game_prompt;
           game_over = false;
           // Backend will handle engine move if applicable.
          }else{
            logger->log("Read game failed!", MessageType::ERROR);
            printMsg("Could not read the saved game. Is the file corrupt?", Color::Red);
          }
          
          break;
        case 3:
          quit();
          break;
        case 4:
          break;
        case 5:
          break;
        case 6:
          break;

        default:
          printBadInput();
          break;
        }
      }
      else if (position == CurrentState::InGame)
      {
        // printBoard();
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
              clearScreen();
              printMsg("Move successful!");
              printBoard();
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
        { // res == (-1 -1) Now you are sure this is one char (or nothing!)
          if (input == "")
          {
            clearScreen();
            printBoard();
            printBadInput();
          }

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
            break;
          case 's':
          case 'S':
            clearScreen();
            save();
            printBoard();
            break;
          case 'u':
          case 'U':
            if (game_over)
            {
              printMsg("Game is over! You cannot undo anymore.", Color::Yellow);
              logger->log("User tried to make an undo after game was over.", MessageType::WARNING);
              continue;
            }
            clearScreen();
            undo();
            printBoard();

            break;
          case 'v':
          case 'V':
            clearScreen();
            save();
            current_prompt = menu_prompt;
            position = CurrentState::MainMenu;
            clearScreen();
            break;
          default:
            clearScreen();
            printBoard();
            printBadInput();
            break;
          }
        }
      }
    }
  }
}

#endif // INTERFACE_H