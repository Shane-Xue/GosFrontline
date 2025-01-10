#ifndef INTERFACE_H
#define INTERFACE_H

#include <iostream>
#include <memory>

#include "Backend.h"
#include "Board.h"

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
    InterfaceCLI(){};
    ~InterfaceCLI() noexcept override = default;
    // void displayBoard() override;
    // void displayMessage(const std::string &message) override;
    // Move getPlayerMove() override;
    void run();
    void printWelcome() const;
    void printMenu() const;
    void clearScreen() const;
    std::string getInput(std::string prompt);
    // void printBoard() const;

  private:
    using Move = std::pair<int, int>;
    static const std::string welcome, menu_prompt, game_prompt;
    std::function<Backend&()> backend = []() -> Backend& { return Backend::getBackend(); };
    std::vector<std::string> input_history{};
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
                                           "\033[4mS\033[0mave Game\n";

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

  void InterfaceCLI::run(){
    printWelcome();
    std::string current_prompt = menu_prompt;
    while (true) {
      std::string input(getInput(current_prompt));
    }
  }

  std::string InterfaceCLI::getInput(std::string prompt)
  {
    std::cout << prompt;
    std::string input;
    std::getline(std::cin, input);
    input_history.push_back(input);
    return input;
  }
}

#endif // INTERFACE_H