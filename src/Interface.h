#ifndef INTERFACE_H
#define INTERFACE_H

#include <iostream>
#include "Board.h"

namespace GosFrontline
{
  class InterfaceBase
  {
  protected:
    using Move = std::pair<int, int>;

  public:
    virtual ~InterfaceBase() = default;
    virtual void displayBoard() = 0;
    virtual void displayMessage(const std::string &message) = 0;
    virtual Move getPlayerMove() = 0;

  private:
  };

  class InterfaceCLI : public InterfaceBase
  {
  public:
    ~InterfaceCLI() noexcept override = default;
    void displayBoard() override;
    void displayMessage(const std::string &message) override;
    Move getPlayerMove() override;

  private:
    using Move = std::pair<int, int>;
    static const std::string welcome, menu, prompt;
  };

  const std::string InterfaceCLI::welcome = "Welcome to GosFrontline!\n"
            "This program is a Renju game powered by C++.\n"
            "Written By Shane Xue\n"
            "Github: https://github.com/Shane-Xue/GosFrontline\n"
            "\n"
            "God Save the Commander! Send him victorious, happy and glorious.\n"
            "(c) Shane Xue 2024-2025\n",
  InterfaceCLI::menu = "What do you want to do?\n"
         "1. Play PVE\n"
         "2. Play PVP\n"
         "3. Load Existing Game\n"
         "4. View Help\n"
         "5. Renju Rules\n"
         "6. Exit\n"
         "Enter number as your choice:",
  InterfaceCLI::prompt = "Enter your move: (row, column)\n"
           "Or you can do the following by entering the corresponding letter\n"
           "\033[4mQ\033[0muit without saving\n"
           "Sa\033[4mv\033[0me and quit\n"
           "\033[4mS\033[0mave Game\n";
}

#endif // INTERFACE_H