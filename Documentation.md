# GosFrontline API Documentation

## Board.h

### Class: `Board<T>`

#### Methods:

- **`Board()`**
  - **Parameters:** None
  - **Return Value:** None
  - **Description:** Default constructor.

- **`Board(size_t r, size_t c, T init_val = T())`**
  - **Parameters:** 
    - `size_t r`: Number of rows.
    - `size_t c`: Number of columns.
    - `T init_val`: Initial value for elements (default is default-initialized `T`).
  - **Return Value:** None
  - **Description:** Initializes the board with specified dimensions and initial value.

- **`Board(const Board &other)`**
  - **Parameters:** 
    - `const Board &other`: Another `Board` object to copy from.
  - **Return Value:** None
  - **Description:** Copy constructor.

- **`Board operator=(const Board &other)`**
  - **Parameters:** 
    - `const Board &other`: Another `Board` object to assign from.
  - **Return Value:** `Board&`
  - **Description:** Copy assignment operator.

- **`Board(Board &&other) noexcept`**
  - **Parameters:** 
    - `Board &&other`: Another `Board` object to move from.
  - **Return Value:** None
  - **Description:** Move constructor.

- **`Board operator=(Board &&other) noexcept`**
  - **Parameters:** 
    - `Board &&other`: Another `Board` object to move from.
  - **Return Value:** `Board&`
  - **Description:** Move assignment operator.

- **`const std::vector<T> &operator[](size_t index)`**
  - **Parameters:** 
    - `size_t index`: Index of the row.
  - **Return Value:** `const std::vector<T>&`
  - **Description:** Accesses a row (read-only).

- **`template <typename IndexType> T &operator[](std::pair<IndexType, IndexType> indices)`**
  - **Parameters:** 
    - `std::pair<IndexType, IndexType> indices`: Pair of indices for row and column.
  - **Return Value:** `T&`
  - **Description:** Accesses an element for modification.

- **`template <typename IndexType> const T &operator[](std::pair<IndexType, IndexType> indices)`**
  - **Parameters:** 
    - `std::pair<IndexType, IndexType> indices`: Pair of indices for row and column.
  - **Return Value:** `const T&`
  - **Description:** Accesses an element (read-only).

- **`const T &at(size_t row_param, size_t col_param) const`**
  - **Parameters:** 
    - `size_t row_param`: Row index.
    - `size_t col_param`: Column index.
  - **Return Value:** `const T&`
  - **Description:** Gets an element at specified coordinates.

- **`void set(size_t row_param, size_t col_param, T value)`**
  - **Parameters:** 
    - `size_t row_param`: Row index.
    - `size_t col_param`: Column index.
    - `T value`: Value to set.
  - **Return Value:** None
  - **Description:** Sets an element at specified coordinates.

- **`void set_all(T value) noexcept`**
  - **Parameters:** 
    - `T value`: Value to set all elements to.
  - **Return Value:** None
  - **Description:** Sets all elements to a specified value.

- **`const Grid &get_board() const`**
  - **Parameters:** None
  - **Return Value:** `const Grid&` (a.k.a. `const std::vector<std::vector<T>>&`)
  - **Description:** Returns the entire board (read-only).

- **`size_t row_count() const`**
  - **Parameters:** None
  - **Return Value:** `size_t`
  - **Description:** Returns the number of rows.

- **`size_t col_count() const`**
  - **Parameters:** None
  - **Return Value:** `size_t`
  - **Description:** Returns the number of columns.

- **`bool validateCoords(size_t row_param, size_t col_param) const`**
  - **Parameters:** 
    - `size_t row_param`: Row index.
    - `size_t col_param`: Column index.
  - **Return Value:** `bool`
  - **Description:** Validates if given coordinates are within bounds.

- **`size_t count(const T &element) const`**
  - **Parameters:** 
    - `const T &element`: Element to count.
  - **Return Value:** `size_t`
  - **Description:** Counts occurrences of a specific element in the board.

---

## Gaming.h

### Enumerations:

- **`PieceType`**:
  - `None = 0`
  - `Sente = 1`
  - `Gote = 2`

- **`ViolationPolicy`**:
  - `Strict`
  - `Remind`
  - `Off`

- **`Direction`**:
  - `Horizontal = 0`
  - `Vertical = 1`
  - `Diagonal = 2`
  - `AntiDiagonal = 3`

### Class: `Gaming`

#### Constructors:

- **`Gaming()`**
  - **Parameters:** None
  - **Return Value:** None
  - **Description:** Default constructor.

- **`Gaming(int rows, int cols, const std::string &sente, const std::string &gote, PieceType engineStat = PieceType::Gote)`**
  - **Parameters:** 
    - `int rows`: Number of rows.
    - `int cols`: Number of columns.
    - `const std::string &sente`: Name of Sente player.
    - `const std::string &gote`: Name of Gote player.
    - `PieceType engineStat`: Engine status (default is `PieceType::Gote`).
  - **Return Value:** None
  - **Description:** Initializes with board dimensions and player names.

- **`Gaming(int rows, int cols, PieceType engineStat)`**
  - **Parameters:** 
    - `int rows`: Number of rows.
    - `int cols`: Number of columns.
    - `PieceType engineStat`: Engine status.
  - **Return Value:** None
  - **Description:** Initializes with board dimensions and engine status.

- **`Gaming(const std::string &sente, const std::string &gote, PieceType engineStat = PieceType::Gote)`**
  - **Parameters:** 
    - `const std::string &sente`: Name of Sente player.
    - `const std::string &gote`: Name of Gote player.
    - `PieceType engineStat`: Engine status (default is `PieceType::Gote`).
  - **Return Value:** None
  - **Description:** Initializes with player names and engine status.

- **`Gaming(const std::string &sente, const std::string &gote)`**
  - **Parameters:** 
    - `const std::string &sente`: Name of Sente player.
    - `const std::string &gote`: Name of Gote player.
  - **Return Value:** None
  - **Description:** Initializes with player names only.

#### Public Methods:

- **`void setViolationPolicy(ViolationPolicy policy)`**
  - **Parameters:** 
    - `ViolationPolicy policy`: Violation policy to set.
  - **Return Value:** None
  - **Description:** Sets the violation policy.

- **`void setSenteName(const std::string &name)`**
  - **Parameters:** 
    - `const std::string &name`: Name of Sente player.
  - **Return Value:** None
  - **Description:** Sets the name of the Sente player.

- **`void setGoteName(const std::string &name)`**
  - **Parameters:** 
    - `const std::string &name`: Name of Gote player.
  - **Return Value:** None
  - **Description:** Sets the name of the Gote player.

- **`void setEngineStatus(PieceType status)`**
  - **Parameters:** 
    - `PieceType status`: Engine status.
  - **Return Value:** None
  - **Description:** Sets the engine status.

- **`void setNames(const std::string &sente, const std::string &gote)`**
  - **Parameters:** 
    - `const std::string &sente`: Name of Sente player.
    - `const std::string &gote`: Name of Gote player.
  - **Return Value:** None
  - **Description:** Sets both player names.

- **`int maxConnect(int row, int col) const`**
  - **Parameters:** 
    - `int row`: Row index.
    - `int col`: Column index.
  - **Return Value:** `int`
  - **Description:** Returns the maximum number of connected stones in all four directions.

- **`PieceType checkWinFull() const`**
  - **Parameters:** None
  - **Return Value:** `PieceType`
  - **Description:** Checks if anyone has won the game.

- **`PieceType checkCurrentWin(int row, int col) const`**
  - **Parameters:** 
    - `int row`: Row index.
    - `int col`: Column index.
  - **Return Value:** `PieceType`
  - **Description:** Checks if there is a winner at a specific position.

- **`bool willBe(int row, int col, PieceType tp, int length)`**
  - **Parameters:** 
    - `int row`: Row index.
    - `int col`: Column index.
    - `PieceType tp`: Type of piece.
    - `int length`: Length to check.
  - **Return Value:** `bool`
  - **Description:** Checks if placing a piece results in a specific length of connected stones.

- **`bool willFive(int row, int col, PieceType tp)`**
  - **Parameters:** 
    - `int row`: Row index.
    - `int col`: Column index.
    - `PieceType tp`: Type of piece.
  - **Return Value:** `bool`
  - **Description:** Checks if placing a piece results in five connected stones.

- **`bool willFour(int row, int col, PieceType tp)`**
  - **Parameters:** 
    - `int row`: Row index.
    - `int col`: Column index.
    - `PieceType tp`: Type of piece.
  - **Return Value:** `bool`
  - **Description:** Checks if placing a piece results in four connected stones.

- **`bool willThree(int row, int col, PieceType tp)`**
  - **Parameters:** 
    - `int row`: Row index.
    - `int col`: Column index.
    - `PieceType tp`: Type of piece.
  - **Return Value:** `bool`
  - **Description:** Checks if placing a piece results in three connected stones.

- **`bool willBe(int row, int col, PieceType tp, int length, Direction dir)`**
  - **Parameters:** 
    - `int row`: Row index.
    - `int col`: Column index.
    - `PieceType tp`: Type of piece.
    - `int length`: Length to check.
    - `Direction dir`: Direction to check.
  - **Return Value:** `bool`
  - **Description:** Checks if placing a piece results in a specific length of connected stones in a specific direction.

- **`bool willLong(int row, int col, PieceType tp)`**
  - **Parameters:** 
    - `int row`: Row index.
    - `int col`: Column index.
    - `PieceType tp`: Type of piece.
  - **Return Value:** `bool`
  - **Description:** Checks if placing a piece results in more than five connected stones.

- **`bool hasFive(int row, int col) const`**
  - **Parameters:** 
    - `int row`: Row index.
    - `int col`: Column index.
  - **Return Value:** `bool`
  - **Description:** Checks if there is a genuine five at a specific position.

- **`int countLength(int row, int col, int length) const`**
  - **Parameters:** 
    - `int row`: Row index.
    - `int col`: Column index.
    - `int length`: Length to count.
  - **Return Value:** `int`
  - **Description:** Counts occurrences of a specific length of connected stones.

- **`int countThree(int row, int col) const`**
  - **Parameters:** 
    - `int row`: Row index.
    - `int col`: Column index.
  - **Return Value:** `int`
  - **Description:** Counts occurrences of three connected stones.

- **`int countFour(int row, int col) const`**
  - **Parameters:** 
    - `int row`: Row index.
    - `int col`: Column index.
  - **Return Value:** `int`
  - **Description:** Counts occurrences of four connected stones.

- **`int countLiveThree(int row, int col) const`**
  - **Parameters:** 
    - `int row`: Row index.
    - `int col`: Column index.
  - **Return Value:** `int`
  - **Description:** Counts occurrences of live three connected stones.

- **`bool isLong(int row, int col) const`**
  - **Parameters:** 
    - `int row`: Row index.
    - `int col`: Column index.
  - **Return Value:** `bool`
  - **Description:** Checks if placing a piece results in more than five connected stones.

- **`bool violationAt(int row, int col)`**
  - **Parameters:** 
    - `int row`: Row index.
    - `int col`: Column index.
  - **Return Value:** `bool`
  - **Description:** Checks if a move at a specific position violates rules.

- **`bool violation(int row, int col)`**
  - **Parameters:** 
    - `int row`: Row index.
    - `int col`: Column index.
  - **Return Value:** `bool`
  - **Description:** Checks if a move violates rules.

- **`PieceType toMove() const`**
  - **Parameters:** None
  - **Return Value:** `PieceType`
  - **Description:** Returns the current player's turn.

- **`bool makeMove(int row, int col)`**
  - **Parameters:** 
    - `int row`: Row index.
    - `int col`: Column index.
  - **Return Value:** `bool`
  - **Description:** Makes a move by a human player.

- **`bool undo()`**
  - **Parameters:** None
  - **Return Value:** `bool`
  - **Description:** Undoes the last move.

- **`const std::vector<std::vector<PieceType>> &getBoard() const`**
  - **Parameters:** None
  - **Return Value:** `const std::vector<std::vector<PieceType>>&`
  - **Description:** Returns the current board state.

- **`int movesMade() const`**
  - **Parameters:** None
  - **Return Value:** `int`
  - **Description:** Returns the number of moves made.

- **`bool isEmpty(int row, int col) const`**
  - **Parameters:** 
    - `int row`: Row index.
    - `int col`: Column index.
  - **Return Value:** `bool`
  - **Description:** Checks if a specific position is empty.

- **`PieceType engineSide()`**
  - **Parameters:** None
  - **Return Value:** `PieceType`
  - **Description:** Returns the side controlled by the engine.

- **`bool makeMoveEngine(int row, int col)`**
  - **Parameters:** 
    - `int row`: Row index.
    - `int col`: Column index.
  - **Return Value:** `bool`
  - **Description:** Makes a move by the engine.

- **`bool isValidCoord(int row, int col)`**
  - **Parameters:** 
    - `int row`: Row index.
    - `int col`: Column index.
  - **Return Value:** `bool`
  - **Description:** Validates if given coordinates are within bounds.

- **`void clearBoard()`**
  - **Parameters:** None
  - **Return Value:** None
  - **Description:** Clears the board.

- **`void clearBoard(int row, int col)`**
  - **Parameters:** 
    - `int row`: Number of rows.
    - `int col`: Number of columns.
  - **Return Value:** None
  - **Description:** Clears the board with specified dimensions.

---

## Backend.h

### Enumerations:

- **`MoveReply`**:
  - `Success`
  - `Win`
  - `Violation`
  - `WrongTurn`
  - `InvalidLocation`
  - `UnknownError`

- **`Action`**:
  - `MoveHuman`
  - `MoveEngine`
  - `Undo`
  - `Quit`
  - `GetBoard`
  - `SetEngineStatus`
  - `SetEngineStatusOff`
  - `NewGame`
  - `SetSenteName`
  - `SetGoteName`
  - `SetViolationPolicy`
  - `CallEngine`

### Class: `Backend`

#### Constructors and Destructor:

- **`Backend()`**
  - **Parameters:** None
  - **Return Value:** None
  - **Description:** Private constructor to prevent instantiation outside the class.

- **`static Backend &getBackend()`**
  - **Parameters:** None
  - **Return Value:** `Backend&`
  - **Description:** Singleton method to get the backend instance.

#### Deleted Methods:

- **`Backend(const Backend &) = delete`**
  - **Parameters:** None
  - **Return Value:** None
  - **Description:** Deleted copy constructor.

- **`Backend &operator=(const Backend &) = delete`**
  - **Parameters:** None
  - **Return Value:** None
  - **Description:** Deleted copy assignment operator.

- **`Backend(Backend &&) = delete`**
  - **Parameters:** None
  - **Return Value:** None
  - **Description:** Deleted move constructor.

- **`Backend &operator=(Backend &&) = delete`**
  - **Parameters:** None
  - **Return Value:** None
  - **Description:** Deleted move assignment operator.

#### Public Methods:

- **`std::future<MoveAndStep> frontendMove(int row, int col)`**
  - **Parameters:** 
    - `int row`: Row index.
    - `int col`: Column index.
  - **Return Value:** `std::future<MoveAndStep>` (a.k.a. `std::future<std::pair<GosFrontline::MoveReply, int>>`)
  - **Description:** Initiates a move by a human player and returns a future for the result of the move.

- **`void frontendUndo()`**
  - **Parameters:** None
  - **Return Value:** None
  - **Description:** Undoes the last move made by either player.

- **`void engineMove(int row, int col, PromiseWrapper<void> status)`**
  - **Parameters:** 
    - `int row`: Row index.
    - `int col`: Column index.
    - `PromiseWrapper<void> status`: Shared pointer to a promise that will be set when the move is processed.
  - **Return Value:** None
  - **Description:** Initiates a move by the engine and sets the provided promise when the move is processed.

- **`std::future<void> callEngine()`**
  - **Parameters:** None
  - **Return Value:** `std::future<void>`
  - **Description:** Calls the engine to make a move and returns a future that resolves when the engine has moved.

- **`void newGame(int row = default_size, int col = default_size)`**
  - **Parameters:** 
    - `int row`: Number of rows (default is `default_size`).
    - `int col`: Number of columns (default is `default_size`).
  - **Return Value:** None
  - **Description:** Starts a new game with specified dimensions.

- **`void quit()`**
  - **Parameters:** None
  - **Return Value:** None
  - **Description:** Signals the backend to quit processing actions.

- **`PieceType tomove()`**
  - **Parameters:** None
  - **Return Value:** `PieceType`
  - **Description:** Returns the current player's turn.

- **`std::future<BoardAndStep> getBoard()`**
  - **Parameters:** None
  - **Return Value:** `std::future<BoardAndStep>` (a.k.a. `std::future<std::pair<std::vector<std::vector<GosFrontline::PieceType>>, int>>`)
  - **Description:** Requests the current state of the board and returns a future for the board and move count.

- **`bool boardUpdated()`**
  - **Parameters:** None
  - **Return Value:** `bool`
  - **Description:** Checks if the board has been updated since the last request.

- **`std::future<void> checkWin()`**
  - **Parameters:** None
  - **Return Value:** `std::future<void>`
  - **Description:** Checks if there is a winner and returns a future that resolves when the check is complete.

- **`void setLogger(std::shared_ptr<Logger>)`**
  - **Parameters:** 
    - `std::shared_ptr<Logger>`: Shared pointer to a logger object.
  - **Return Value:** None
  - **Description:** Sets the logger for the backend.

- **`int run()`**
  - **Parameters:** None
  - **Return Value:** `int`
  - **Description:** Runs the backend loop, processing actions and handling game logic.

#### Private Methods:

- **`std::pair<GosFrontline::MoveReply, int> registerHumanMove(int row, int col)`**
  - **Parameters:** 
    - `int row`: Row index.
    - `int col`: Column index.
  - **Return Value:** `std::pair<GosFrontline::MoveReply, int>`
  - **Description:** Registers a move made by a human player and returns the result of the move.

- **`std::pair<GosFrontline::MoveReply, int> registerEngineMove(int row, int col)`**
  - **Parameters:** 
    - `int row`: Row index.
    - `int col`: Column index.
  - **Return Value:** `std::pair<GosFrontline::MoveReply, int>`
  - **Description:** Registers a move made by the engine and returns the result of the move.

---

### Summary of Key Operations

- **Initialization and Setup:**
  - `newGame(int row, int col)`: Starts a new game with specified dimensions.
  - `setLogger(std::shared_ptr<Logger>)`: Sets the logger for logging backend operations.

- **Move Handling:**
  - `frontendMove(int row, int col)`: Handles moves made by the human player.
  - `engineMove(int row, int col, PromiseWrapper<void> status)`: Handles moves made by the engine.
  - `callEngine()`: Calls the engine to make a move.
  - `frontendUndo()`: Undoes the last move.

- **State Management:**
  - `getBoard()`: Retrieves the current state of the board.
  - `boardUpdated()`: Checks if the board has been updated.
  - `checkWin()`: Checks if there is a winner.

- **Termination:**
  - `quit()`: Signals the backend to stop processing actions.
  - `run()`: Main loop that processes actions and handles game logic.

---


## Interface.h

### Class: `InterfaceBase`

#### Public Methods:

- **`virtual ~InterfaceBase() = default;`**
  - **Parameters:** None
  - **Return Value:** None
  - **Description:** Virtual destructor for the base interface class.

- **`virtual void run() = 0;`**
  - **Parameters:** None
  - **Return Value:** None
  - **Description:** Pure virtual function that defines the main loop for running the interface. Must be implemented by derived classes.

---

### Class: `InterfaceCLI` (Derived from `InterfaceBase`)

#### Constructors and Destructor:

- **`InterfaceCLI()`**
  - **Parameters:** None
  - **Return Value:** None
  - **Description:** Constructor initializes the position to `MainMenu`.

- **`~InterfaceCLI() noexcept override = default;`**
  - **Parameters:** None
  - **Return Value:** None
  - **Description:** Noexcept destructor for the CLI interface.

#### Public Methods:

- **`void run();`**
  - **Parameters:** None
  - **Return Value:** None
  - **Description:** Main loop for the CLI interface, handling user input and game flow.

- **`void clearScreen() const;`**
  - **Parameters:** None
  - **Return Value:** None
  - **Description:** Clears the terminal screen using ANSI escape codes.

- **`void setLogger(std::shared_ptr<Logger>);`**
  - **Parameters:** 
    - `std::shared_ptr<Logger>`: Shared pointer to a logger object.
  - **Return Value:** None
  - **Description:** Sets the logger for logging CLI operations.

#### Private Methods:

- **`void printWelcome() const;`**
  - **Parameters:** None
  - **Return Value:** None
  - **Description:** Prints the welcome message to the console.

- **`void printMenu() const;`**
  - **Parameters:** None
  - **Return Value:** None
  - **Description:** Prints the main menu options to the console.

- **`void printBoard() const;`**
  - **Parameters:** None
  - **Return Value:** None
  - **Description:** Prints the current state of the board to the console.

- **`void printMsg(const std::string &, Color) const;`**
  - **Parameters:** 
    - `const std::string &`: Message to print.
    - `Color`: Color enum value for text color.
  - **Return Value:** None
  - **Description:** Prints a message with the specified color using ANSI escape codes.

- **`void quit();`**
  - **Parameters:** None
  - **Return Value:** None
  - **Description:** Handles the quit operation, cleaning up resources and exiting the program.

- **`std::pair<int, int> getNumbers(std::string);`**
  - **Parameters:** 
    - `std::string`: Input string containing numbers.
  - **Return Value:** `std::pair<int, int>` (a.k.a. `std::pair<int, int>`)
  - **Description:** Extracts two integers from the input string using regex. Returns a pair of integers or `{-1, -1}` if extraction fails.

- **`std::string getInput(std::string prompt);`**
  - **Parameters:** 
    - `std::string prompt`: Prompt message for user input.
  - **Return Value:** `std::string`
  - **Description:** Prompts the user for input and returns the input string.

- **`void printBadInput() const;`**
  - **Parameters:** None
  - **Return Value:** None
  - **Description:** Prints an invalid input message to the console.

#### Private Members:

- **`enum class Color`**:
  - `Black = 0`
  - `Red`
  - `Green`
  - `Yellow`
  - `Blue`
  - `Magenta`
  - `Cyan`
  - `White`
  - `Default`

- **`using Move = std::pair<int, int>;`**
  - **Alias for:** `std::pair<int, int>`
  - **Description:** Represents a move as a pair of integers (row, column).

- **Static Constants:**

  - **`static const std::string welcome`**
    - **Value:** Welcome message string.
  
  - **`static const std::string menu_prompt`**
    - **Value:** Menu prompt string.
  
  - **`static const std::string game_prompt`**
    - **Value:** Game prompt string.
  
  - **`static const std::string main_quit`**
    - **Value:** Quit message string.
  
  - **`static const std::string bad_input`**
    - **Value:** Bad input message string.
  
  - **`static const std::string move_count`**
    - **Value:** Move count message string.
  
  - **`static const std::string ask_board_size`**
    - **Value:** Board size request message string.
  
  - **`static const std::string default_size`**
    - **Value:** Default board size message string.
  
  - **`static const std::string game_over_prompt`**
    - **Value:** Game over prompt string.
  
  - **`static const int timeout`**
    - **Value:** Timeout duration in milliseconds (10000 ms).
  
  - **`static const std::regex two_numbers`**
    - **Value:** Regex pattern for extracting two numbers from a string.

- **`bool game_over`**
  - **Description:** Flag indicating if the game is over.

- **`bool pve`**
  - **Description:** Flag indicating if the game mode is Player vs Engine.

- **`int move_now`**
  - **Description:** Current move number (unused in provided code).

- **`std::function<Backend &()> backend`**
  - **Description:** Function to get the backend instance.

- **`std::vector<std::string> input_history`**
  - **Description:** History of user inputs.

- **`std::shared_ptr<Logger> logger`**
  - **Description:** Shared pointer to a logger object.

- **`std::stringstream logstream`**
  - **Description:** Stream for logging messages.

- **`bool exit`**
  - **Description:** Flag indicating if the program should exit.

- **`enum class CurrentState`**:
  - `MainMenu`
  - `InGame`

- **`CurrentState position`**
  - **Description:** Current state of the interface (MainMenu or InGame).

---

### Summary of Key Operations

- **Initialization and Setup:**
  - `setLogger(std::shared_ptr<Logger>)`: Sets the logger for logging CLI operations.
  - `printWelcome()`: Prints the welcome message.

- **User Interaction:**
  - `getInput(std::string prompt)`: Prompts the user for input.
  - `printMenu()`: Displays the main menu.
  - `printBoard()`: Displays the current state of the board.
  - `printMsg(const std::string &, Color)`: Prints a colored message.
  - `printBadInput()`: Prints an invalid input message.

- **Game Flow:**
  - `run()`: Main loop handling user input and game flow.
  - `quit()`: Handles the quit operation.
  - `getNumbers(std::string)`: Extracts two integers from the input string.

- **State Management:**
  - `CurrentState position`: Tracks the current state (MainMenu or InGame).
  - `game_over`, `pve`, `exit`: Flags for game state and exit condition.

---


## Logger.h

### Class: `Logger`

#### Enumerations:

- **`MessageType`**:
  - `INFO = 0`
  - `WARNING`
  - `ERROR`
  - `DEBUG`
  - `FATAL`

#### Public Methods:

- **`Logger(std::string fileName)`**
  - **Parameters:** 
    - `std::string fileName`: Name of the log file.
  - **Return Value:** None
  - **Description:** Constructor that initializes the logger with a default path `"log/"`.

- **`Logger(std::string fileName, std::string pathName)`**
  - **Parameters:** 
    - `std::string fileName`: Name of the log file.
    - `std::string pathName`: Path where the log file will be saved.
  - **Return Value:** None
  - **Description:** Constructor that initializes the logger with a specified path.

- **`~Logger()`**
  - **Parameters:** None
  - **Return Value:** None
  - **Description:** Destructor that closes the log file.

- **`void log(const std::string &message, MessageType type = MessageType::INFO)`**
  - **Parameters:** 
    - `const std::string &message`: Message to log.
    - `MessageType type`: Type of message (default is `INFO`).
  - **Return Value:** None
  - **Description:** Logs a message with the specified type into the queue for writing.

- **`void infinite_run()`**
  - **Parameters:** None
  - **Return Value:** None
  - **Description:** Continuously processes and writes messages from the queue until an end flag is set. Sleeps for 100ms if the queue is empty but the end flag is not set.

- **`void run()`**
  - **Parameters:** None
  - **Return Value:** None
  - **Description:** Processes and writes all messages currently in the queue. Stops when the queue is empty.

- **`void setEndFlag()`**
  - **Parameters:** None
  - **Return Value:** None
  - **Description:** Sets the end flag to stop the `infinite_run` method.

- **`std::string getFileName() const`**
  - **Parameters:** None
  - **Return Value:** `std::string`
  - **Description:** Returns the name of the log file.

#### Private Methods:

- **`void write(const std::string &message)`**
  - **Parameters:** 
    - `const std::string &message`: Message to write to the log file.
  - **Return Value:** None
  - **Description:** Writes the given message to the log file.

#### Private Members:

- **`std::string fileName`**
  - **Description:** Name of the log file.

- **`std::string pathName`**
  - **Description:** Path where the log file will be saved.

- **`std::ofstream m_file`**
  - **Description:** File stream for writing logs.

- **`SafeQueue<std::string> m_queue`**
  - **Description:** Queue for storing log messages before writing.

- **`static const std::vector<std::string> types`**
  - **Value:** Vector containing string representations of `MessageType` enums (`{"INFO", "WARNING", "ERROR", "DEBUG", "FATAL"}`).

- **`bool endFlag`**
  - **Description:** Flag to indicate when to stop processing messages in `infinite_run`.

---

### Summary of Key Operations

- **Initialization and Setup:**
  - `Logger(std::string fileName)`: Initializes the logger with a default path.
  - `Logger(std::string fileName, std::string pathName)`: Initializes the logger with a specified path.
  - `setEndFlag()`: Sets the end flag to stop the logger's infinite run loop.

- **Logging Messages:**
  - `log(const std::string &message, MessageType type)`: Adds a message to the queue with the specified type.
  - `write(const std::string &message)`: Writes a message to the log file.

- **Processing Messages:**
  - `infinite_run()`: Continuously processes and writes messages from the queue until an end flag is set.
  - `run()`: Processes and writes all messages currently in the queue.

- **File Management:**
  - `getFileName()`: Retrieves the name of the log file.
  - Destructor (`~Logger()`): Closes the log file.

This documentation provides a clear and concise overview of the `Logger` class's methods, members, and their descriptions. It also includes a summary of key operations to give an overall understanding of how the logger functions within the `GosFrontline` project.

---


## MCTS.h

### Class: `MCTS`

#### Constructors and Destructor:

- **`MCTS()`**
  - **Parameters:** None
  - **Return Value:** None
  - **Description:** Constructor that initializes the random number generator.

#### Public Methods:

- **`std::pair<int, int> getRandomMove(const Gaming &game)`**
  - **Parameters:** 
    - `const Gaming &game`: Reference to the current game state.
  - **Return Value:** `std::pair<int, int>` (a.k.a. `std::pair<int, int>`)
  - **Description:** Generates a random valid move for the given game state. Returns a pair of integers representing the row and column indices. If no valid moves are available, returns `{-1, -1}`.

---

## SafeQueue.h

### Class: `SafeQueue<T>`

#### Template Parameters:

- **`T`**: Type of elements stored in the queue.

#### Public Methods:

- **`void push(const T &item)`**
  - **Parameters:** 
    - `const T &item`: Item to be added to the queue.
  - **Return Value:** None
  - **Description:** Adds an item to the queue in a thread-safe manner and notifies one waiting thread.

- **`T pop()`**
  - **Parameters:** None
  - **Return Value:** `T`
  - **Description:** Removes and returns the front item from the queue in a thread-safe manner. Waits if the queue is empty.

- **`bool empty() const`**
  - **Parameters:** None
  - **Return Value:** `bool`
  - **Description:** Checks if the queue is empty in a thread-safe manner.

#### Private Members:

- **`std::queue<T> queue`**
  - **Description:** Internal queue to store items.

- **`mutable std::mutex mutex`**
  - **Description:** Mutex to ensure thread safety.

- **`std::condition_variable condition`**
  - **Description:** Condition variable to handle synchronization when the queue is empty.

---

## UUID.h

### Struct: `UUIDHash`

#### Public Methods:

- **`size_t operator()(const std::array<uint8_t, 16> &uuid) const`**
  - **Parameters:** 
    - `const std::array<uint8_t, 16> &uuid`: Array of 16 bytes representing a UUID.
  - **Return Value:** `size_t`
  - **Description:** Hash function for UUIDs using two 64-bit hashes combined with XOR.

### Class: `UUID`

#### Constructors and Destructor:

- **`UUID()`**
  - **Parameters:** None
  - **Return Value:** None
  - **Description:** Default constructor that generates a new random UUID.

- **`explicit UUID(const std::array<uint8_t, 16> &uuid_bytes)`**
  - **Parameters:** 
    - `const std::array<uint8_t, 16> &uuid_bytes`: Array of 16 bytes representing a UUID.
  - **Return Value:** None
  - **Description:** Constructor that initializes the UUID with the provided byte array.

#### Public Methods:

- **`std::string str() const`**
  - **Parameters:** None
  - **Return Value:** `std::string`
  - **Description:** Converts the UUID to a string in the standard format (e.g., "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx").

#### Private Members:

- **`std::array<uint8_t, 16> uuid`**
  - **Description:** Underlying UUID represented as an array of 16 bytes.

#### Static Private Methods:

- **`static std::array<uint8_t, 16> generateUUID()`**
  - **Parameters:** None
  - **Return Value:** `std::array<uint8_t, 16>`
  - **Description:** Generates a random UUID ensuring uniqueness by checking against previously generated UUIDs. Sets version 4 and variant 2 according to RFC 4122.

---

### Summary of Key Operations

#### MCTS:
- **Initialization and Setup:**
  - `MCTS()`: Initializes the random number generator.
  
- **Generating Moves:**
  - `getRandomMove(const Gaming &game)`: Generates a random valid move for the given game state.

#### SafeQueue:
- **Queue Management:**
  - `push(const T &item)`: Adds an item to the queue safely.
  - `pop()`: Removes and returns the front item from the queue safely.
  - `empty() const`: Checks if the queue is empty safely.

#### UUID:
- **Initialization and Setup:**
  - `UUID()`: Generates a new random UUID.
  - `UUID(const std::array<uint8_t, 16> &uuid_bytes)`: Initializes with a given UUID byte array.
  
- **String Conversion:**
  - `str() const`: Converts the UUID to a string in standard format.
  
- **UUID Generation:**
  - `generateUUID()`: Generates a unique random UUID ensuring no duplicates.

This documentation provides a clear and concise overview of the APIs for the `MCTS`, `SafeQueue`, and `UUID` classes within the `GosFrontline` project. It also includes a summary of key operations to give an overall understanding of how these components function.