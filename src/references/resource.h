#include <string>
#include <iostream>
#include <sstream>
#include "jsoncpp/json.h"
using namespace std;
#define BOARD_LEN 15

using namespace std;

class CPoint
{
public:
  int x, y;

  CPoint() : x(0), y(0) {}

  CPoint(int initX, int initY) : x(initX), y(initY) {}

  bool operator==(const CPoint &that)
  {
    return x == that.x && y == that.y;
  }

  bool operator!=(const CPoint &that)
  {
    return x != that.x || y != that.y;
  }

  CPoint operator+(const CPoint &that)
  {
    return {x + that.x, y + that.y};
  }

  CPoint operator-(const CPoint &that)
  {
    return {x - that.x, y - that.y};
  }

  CPoint operator-()
  {
    return {-x, -y};
  }

  CPoint &operator+=(const CPoint &that)
  {
    x += that.x;
    y += that.y;
    return *this;
  }

  CPoint &operator-=(const CPoint &that)
  {
    x -= that.x;
    y -= that.y;
    return *this;
  }
};

class ForbiddenPointFinder
{
public:
  // standard renju / gomoku have 15 x 15 board
  static const int BoardSize = 15;

  enum class Stone
  {
    Black = 'X',
    White = 'O',
    Empty = '.',
    Wall = '$'
  };

  enum class Result
  {
    BlackWin = 0,
    WhiteWin = 1,
    Forbidden = 2,
    Draw = 3,
    Unknown = -1
  };

  int nForbiddenPoints = 0;
  CPoint ptForbidden[BoardSize * BoardSize] = {};

private:
  Stone cBoard[BoardSize + 2][BoardSize + 2];
  int nStones = 0;

public:
  ForbiddenPointFinder();

  virtual ~ForbiddenPointFinder();

  void clear();

  Result addStone(int x, int y, Stone cStone);

private:
  void setStone(int x, int y, Stone cStone);

  bool isFive(int x, int y, int nColor);

  bool isOverline(int x, int y);

  bool isFive(int x, int y, int nColor, int nDir);

  bool isFour(int x, int y, int nColor, int nDir);

  int isOpenFour(int x, int y, int nColor, int nDir);

  bool isOpenThree(int x, int y, int nColor, int nDir);

  bool isDoubleFour(int x, int y);

  bool isDoubleThree(int x, int y);

  void findForbiddenPoints();
};

ForbiddenPointFinder::ForbiddenPointFinder()
{
  clear();
}

ForbiddenPointFinder::~ForbiddenPointFinder()
{
}

//////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////

void ForbiddenPointFinder::clear()
{
  nForbiddenPoints = 0;
  nStones = 0;

  for (int i = 0; i < BoardSize + 2; i++)
  {
    cBoard[0][i] = Stone ::Wall;
    cBoard[(BoardSize + 1)][i] = Stone ::Wall;
    cBoard[i][0] = Stone ::Wall;
    cBoard[i][(BoardSize + 1)] = Stone ::Wall;
  }

  for (int i = 1; i <= BoardSize; i++)
    for (int j = 1; j <= BoardSize; j++)
      cBoard[i][j] = Stone::Empty;
}

ForbiddenPointFinder::Result ForbiddenPointFinder::addStone(int x, int y, Stone cStone)
{
  Result nResult = Result::Unknown;

  if (cStone == Stone::Black)
  {
    if (isFive(x, y, 0))
      nResult = Result::BlackWin;
    for (int i = 0; i < nForbiddenPoints; i++)
    {
      if (ptForbidden[i] == CPoint(x, y))
        nResult = Result::Forbidden;
    }
  }
  else if (cStone == Stone::White)
  {
    if (isFive(x, y, 1))
      nResult = Result::WhiteWin;
  }

  cBoard[x + 1][y + 1] = cStone;
  ++nStones;

  if (nResult == Result::Unknown)
  {
    findForbiddenPoints();
  }
  else
  {
    nForbiddenPoints = 0;
  }
  if (nResult == Result::Unknown && nStones >= BoardSize * BoardSize)
  {
    nResult = Result::Draw;
  }
  return nResult;
}

void ForbiddenPointFinder::setStone(int x, int y, Stone cStone)
{
  cBoard[x + 1][y + 1] = cStone;
}

bool ForbiddenPointFinder::isFive(int x, int y, int nColor)
{
  if (cBoard[x + 1][y + 1] != Stone::Empty)
    return false;

  if (nColor == 0) // black
  {
    setStone(x, y, Stone::Black);

    // detect black five
    int i, j;

    // 1 - horizontal direction
    int nLine = 1;
    i = x;
    while (i > 0)
    {
      if (cBoard[i--][y + 1] == Stone::Black)
        nLine++;
      else
        break;
    }
    i = x + 2;
    while (i < (BoardSize + 1))
    {
      if (cBoard[i++][y + 1] == Stone::Black)
        nLine++;
      else
        break;
    }
    if (nLine == 5)
    {
      setStone(x, y, Stone::Empty);
      return true;
    }

    // 2 - vertical direction
    nLine = 1;
    i = y;
    while (i > 0)
    {
      if (cBoard[x + 1][i--] == Stone::Black)
        nLine++;
      else
        break;
    }
    i = y + 2;
    while (i < (BoardSize + 1))
    {
      if (cBoard[x + 1][i++] == Stone::Black)
        nLine++;
      else
        break;
    }
    if (nLine == 5)
    {
      setStone(x, y, Stone::Empty);
      return true;
    }

    // 3 - diagonal direction (lower-left to upper-right: '/')
    nLine = 1;
    i = x;
    j = y;
    while ((i > 0) && (j > 0))
    {
      if (cBoard[i--][j--] == Stone::Black)
        nLine++;
      else
        break;
    }
    i = x + 2;
    j = y + 2;
    while ((i < (BoardSize + 1)) && (j < (BoardSize + 1)))
    {
      if (cBoard[i++][j++] == Stone::Black)
        nLine++;
      else
        break;
    }
    if (nLine == 5)
    {
      setStone(x, y, Stone::Empty);
      return true;
    }

    // 4 - diagonal direction (upper-left to lower-right: '\')
    nLine = 1;
    i = x;
    j = y + 2;
    while ((i > 0) && (j < (BoardSize + 1)))
    {
      if (cBoard[i--][j++] == Stone::Black)
        nLine++;
      else
        break;
    }
    i = x + 2;
    j = y;
    while ((i < (BoardSize + 1)) && (j > 0))
    {
      if (cBoard[i++][j--] == Stone::Black)
        nLine++;
      else
        break;
    }
    if (nLine == 5)
    {
      setStone(x, y, Stone::Empty);
      return true;
    }

    setStone(x, y, Stone::Empty);
    return false;
  }
  else if (nColor == 1) // white
  {
    setStone(x, y, Stone::White);

    // detect white five or more
    int i, j;

    // 1 - horizontal direction
    int nLine = 1;
    i = x;
    while (i > 0)
    {
      if (cBoard[i--][y + 1] == Stone::White)
        nLine++;
      else
        break;
    }
    i = x + 2;
    while (i < (BoardSize + 1))
    {
      if (cBoard[i++][y + 1] == Stone::White)
        nLine++;
      else
        break;
    }
    if (nLine >= 5)
    {
      setStone(x, y, Stone::Empty);
      return true;
    }

    // 2 - vertical direction
    nLine = 1;
    i = y;
    while (i > 0)
    {
      if (cBoard[x + 1][i--] == Stone::White)
        nLine++;
      else
        break;
    }
    i = y + 2;
    while (i < (BoardSize + 1))
    {
      if (cBoard[x + 1][i++] == Stone::White)
        nLine++;
      else
        break;
    }
    if (nLine >= 5)
    {
      setStone(x, y, Stone::Empty);
      return true;
    }

    // 3 - diagonal direction (lower-left to upper-right: '/')
    nLine = 1;
    i = x;
    j = y;
    while ((i > 0) && (j > 0))
    {
      if (cBoard[i--][j--] == Stone::White)
        nLine++;
      else
        break;
    }
    i = x + 2;
    j = y + 2;
    while ((i < (BoardSize + 1)) && (j < (BoardSize + 1)))
    {
      if (cBoard[i++][j++] == Stone::White)
        nLine++;
      else
        break;
    }
    if (nLine >= 5)
    {
      setStone(x, y, Stone::Empty);
      return true;
    }

    // 4 - diagonal direction (upper-left to lower-right: '\')
    nLine = 1;
    i = x;
    j = y + 2;
    while ((i > 0) && (j < (BoardSize + 1)))
    {
      if (cBoard[i--][j++] == Stone::White)
        nLine++;
      else
        break;
    }
    i = x + 2;
    j = y;
    while ((i < (BoardSize + 1)) && (j > 0))
    {
      if (cBoard[i++][j--] == Stone::White)
        nLine++;
      else
        break;
    }
    if (nLine >= 5)
    {
      setStone(x, y, Stone::Empty);
      return true;
    }

    setStone(x, y, Stone::Empty);
    return false;
  }
  else
    return false;
}

bool ForbiddenPointFinder::isOverline(int x, int y)
{
  if (cBoard[x + 1][y + 1] != Stone::Empty)
    return false;

  setStone(x, y, Stone::Black);

  // detect black overline
  int i, j;
  bool bOverline = false;

  // 1 - horizontal direction
  int nLine = 1;
  i = x;
  while (i > 0)
  {
    if (cBoard[i--][y + 1] == Stone::Black)
      nLine++;
    else
      break;
  }
  i = x + 2;
  while (i < (BoardSize + 1))
  {
    if (cBoard[i++][y + 1] == Stone::Black)
      nLine++;
    else
      break;
  }
  if (nLine == 5)
  {
    setStone(x, y, Stone::Empty);
    return false;
  }
  else
    bOverline |= (nLine >= 6);

  // 2 - vertical direction
  nLine = 1;
  i = y;
  while (i > 0)
  {
    if (cBoard[x + 1][i--] == Stone::Black)
      nLine++;
    else
      break;
  }
  i = y + 2;
  while (i < (BoardSize + 1))
  {
    if (cBoard[x + 1][i++] == Stone::Black)
      nLine++;
    else
      break;
  }
  if (nLine == 5)
  {
    setStone(x, y, Stone::Empty);
    return false;
  }
  else
    bOverline |= (nLine >= 6);

  // 3 - diagonal direction (lower-left to upper-right: '/')
  nLine = 1;
  i = x;
  j = y;
  while ((i > 0) && (j > 0))
  {
    if (cBoard[i--][j--] == Stone::Black)
      nLine++;
    else
      break;
  }
  i = x + 2;
  j = y + 2;
  while ((i < (BoardSize + 1)) && (j < (BoardSize + 1)))
  {
    if (cBoard[i++][j++] == Stone::Black)
      nLine++;
    else
      break;
  }
  if (nLine == 5)
  {
    setStone(x, y, Stone::Empty);
    return false;
  }
  else
    bOverline |= (nLine >= 6);

  // 4 - diagonal direction (upper-left to lower-right: '\')
  nLine = 1;
  i = x;
  j = y + 2;
  while ((i > 0) && (j < (BoardSize + 1)))
  {
    if (cBoard[i--][j++] == Stone::Black)
      nLine++;
    else
      break;
  }
  i = x + 2;
  j = y;
  while ((i < (BoardSize + 1)) && (j > 0))
  {
    if (cBoard[i++][j--] == Stone::Black)
      nLine++;
    else
      break;
  }
  if (nLine == 5)
  {
    setStone(x, y, Stone::Empty);
    return false;
  }
  else
    bOverline |= (nLine >= 6);

  setStone(x, y, Stone::Empty);
  return bOverline;
}

bool ForbiddenPointFinder::isFive(int x, int y, int nColor, int nDir)
{
  if (cBoard[x + 1][y + 1] != Stone::Empty)
    return false;

  Stone c;
  if (nColor == 0) // black
    c = Stone::Black;
  else if (nColor == 1) // white
    c = Stone::White;
  else
    return false;

  setStone(x, y, c);

  int i, j;
  int nLine;

  switch (nDir)
  {
  case 1: // horizontal direction
    nLine = 1;
    i = x;
    while (i > 0)
    {
      if (cBoard[i--][y + 1] == c)
        nLine++;
      else
        break;
    }
    i = x + 2;
    while (i < (BoardSize + 1))
    {
      if (cBoard[i++][y + 1] == c)
        nLine++;
      else
        break;
    }
    if (nLine == 5)
    {
      setStone(x, y, Stone::Empty);
      return true;
    }
    else
    {
      setStone(x, y, Stone::Empty);
      return false;
    }
    break;
  case 2: // vertial direction
    nLine = 1;
    i = y;
    while (i > 0)
    {
      if (cBoard[x + 1][i--] == c)
        nLine++;
      else
        break;
    }
    i = y + 2;
    while (i < (BoardSize + 1))
    {
      if (cBoard[x + 1][i++] == c)
        nLine++;
      else
        break;
    }
    if (nLine == 5)
    {
      setStone(x, y, Stone::Empty);
      return true;
    }
    else
    {
      setStone(x, y, Stone::Empty);
      return false;
    }
    break;
  case 3: // diagonal direction - '/'
    nLine = 1;
    i = x;
    j = y;
    while ((i > 0) && (j > 0))
    {
      if (cBoard[i--][j--] == c)
        nLine++;
      else
        break;
    }
    i = x + 2;
    j = y + 2;
    while ((i < (BoardSize + 1)) && (j < (BoardSize + 1)))
    {
      if (cBoard[i++][j++] == c)
        nLine++;
      else
        break;
    }
    if (nLine == 5)
    {
      setStone(x, y, Stone::Empty);
      return true;
    }
    else
    {
      setStone(x, y, Stone::Empty);
      return false;
    }
    break;
  case 4: // diagonal direction - '\'
    nLine = 1;
    i = x;
    j = y + 2;
    while ((i > 0) && (j < (BoardSize + 1)))
    {
      if (cBoard[i--][j++] == c)
        nLine++;
      else
        break;
    }
    i = x + 2;
    j = y;
    while ((i < (BoardSize + 1)) && (j > 0))
    {
      if (cBoard[i++][j--] == c)
        nLine++;
      else
        break;
    }
    if (nLine == 5)
    {
      setStone(x, y, Stone::Empty);
      return true;
    }
    else
    {
      setStone(x, y, Stone::Empty);
      return false;
    }
    break;
  default:
    setStone(x, y, Stone::Empty);
    return false;
    break;
  }
}

bool ForbiddenPointFinder::isFour(int x, int y, int nColor, int nDir)
{
  if (cBoard[x + 1][y + 1] != Stone::Empty)
    return false;

  if (isFive(x, y, nColor)) // five?
    return false;
  else if ((nColor == 0) && (isOverline(x, y))) // black overline?
    return false;
  else
  {
    Stone c;
    if (nColor == 0) // black
      c = Stone::Black;
    else if (nColor == 1) // white
      c = Stone::White;
    else
      return false;

    setStone(x, y, c);

    int i, j;

    switch (nDir)
    {
    case 1: // horizontal direction
      i = x;
      while (i > 0)
      {
        if (cBoard[i][y + 1] == c)
        {
          i--;
          continue;
        }
        else if (cBoard[i][y + 1] == Stone::Empty)
        {
          if (isFive(i - 1, y, 0, nDir))
          {
            setStone(x, y, Stone::Empty);
            return true;
          }
          else
            break;
        }
        else
          break;
      }
      i = x + 2;
      while (i < (BoardSize + 1))
      {
        if (cBoard[i][y + 1] == c)
        {
          i++;
          continue;
        }
        else if (cBoard[i][y + 1] == Stone::Empty)
        {
          if (isFive(i - 1, y, 0, nDir))
          {
            setStone(x, y, Stone::Empty);
            return true;
          }
          else
            break;
        }
        else
          break;
      }
      setStone(x, y, Stone::Empty);
      return false;
      break;
    case 2: // vertial direction
      i = y;
      while (i > 0)
      {
        if (cBoard[x + 1][i] == c)
        {
          i--;
          continue;
        }
        else if (cBoard[x + 1][i] == Stone::Empty)
        {
          if (isFive(x, i - 1, 0, nDir))
          {
            setStone(x, y, Stone::Empty);
            return true;
          }
          else
            break;
        }
        else
          break;
      }
      i = y + 2;
      while (i < (BoardSize + 1))
      {
        if (cBoard[x + 1][i] == c)
        {
          i++;
          continue;
        }
        else if (cBoard[x + 1][i] == Stone::Empty)
        {
          if (isFive(x, i - 1, 0, nDir))
          {
            setStone(x, y, Stone::Empty);
            return true;
          }
          else
            break;
        }
        else
          break;
      }
      setStone(x, y, Stone::Empty);
      return false;
      break;
    case 3: // diagonal direction - '/'
      i = x;
      j = y;
      while ((i > 0) && (j > 0))
      {
        if (cBoard[i][j] == c)
        {
          i--;
          j--;
          continue;
        }
        else if (cBoard[i][j] == Stone::Empty)
        {
          if (isFive(i - 1, j - 1, 0, nDir))
          {
            setStone(x, y, Stone::Empty);
            return true;
          }
          else
            break;
        }
        else
          break;
      }
      i = x + 2;
      j = y + 2;
      while ((i < (BoardSize + 1)) && (j < (BoardSize + 1)))
      {
        if (cBoard[i][j] == c)
        {
          i++;
          j++;
          continue;
        }
        else if (cBoard[i][j] == Stone::Empty)
        {
          if (isFive(i - 1, j - 1, 0, nDir))
          {
            setStone(x, y, Stone::Empty);
            return true;
          }
          else
            break;
        }
        else
          break;
      }
      setStone(x, y, Stone::Empty);
      return false;
      break;
    case 4: // diagonal direction - '\'
      i = x;
      j = y + 2;
      while ((i > 0) && (j < (BoardSize + 1)))
      {
        if (cBoard[i][j] == c)
        {
          i--;
          j++;
          continue;
        }
        else if (cBoard[i][j] == Stone::Empty)
        {
          if (isFive(i - 1, j - 1, 0, nDir))
          {
            setStone(x, y, Stone::Empty);
            return true;
          }
          else
            break;
        }
        else
          break;
      }
      i = x + 2;
      j = y;
      while ((i < (BoardSize + 1)) && (j > 0))
      {
        if (cBoard[i][j] == c)
        {
          i++;
          j--;
          continue;
        }
        else if (cBoard[i][j] == Stone::Empty)
        {
          if (isFive(i - 1, j - 1, 0, nDir))
          {
            setStone(x, y, Stone::Empty);
            return true;
          }
          else
            break;
        }
        else
          break;
      }
      setStone(x, y, Stone::Empty);
      return false;
      break;
    default:
      setStone(x, y, Stone::Empty);
      return false;
      break;
    }
  }
}

int ForbiddenPointFinder::isOpenFour(int x, int y, int nColor, int nDir)
{
  if (cBoard[x + 1][y + 1] != Stone::Empty)
    return 0;

  if (isFive(x, y, nColor)) // five?
    return 0;
  else if ((nColor == 0) && (isOverline(x, y))) // black overline?
    return 0;
  else
  {
    Stone c;
    if (nColor == 0) // black
      c = Stone::Black;
    else if (nColor == 1) // white
      c = Stone::White;
    else
      return 0;

    setStone(x, y, c);

    int i, j;
    int nLine;

    switch (nDir)
    {
    case 1: // horizontal direction
      nLine = 1;
      i = x;
      while (i >= 0)
      {
        if (cBoard[i][y + 1] == c)
        {
          i--;
          nLine++;
          continue;
        }
        else if (cBoard[i][y + 1] == Stone::Empty)
        {
          if (!isFive(i - 1, y, 0, nDir))
          {
            setStone(x, y, Stone::Empty);
            return 0;
          }
          else
            break;
        }
        else
        {
          setStone(x, y, Stone::Empty);
          return 0;
        }
      }
      i = x + 2;
      while (i < (BoardSize + 1))
      {
        if (cBoard[i][y + 1] == c)
        {
          i++;
          nLine++;
          continue;
        }
        else if (cBoard[i][y + 1] == Stone::Empty)
        {
          if (isFive(i - 1, y, 0, nDir))
          {
            setStone(x, y, Stone::Empty);
            return (nLine == 4 ? 1 : 2);
          }
          else
            break;
        }
        else
          break;
      }
      setStone(x, y, Stone::Empty);
      return 0;
      break;
    case 2: // vertial direction
      nLine = 1;
      i = y;
      while (i >= 0)
      {
        if (cBoard[x + 1][i] == c)
        {
          i--;
          nLine++;
          continue;
        }
        else if (cBoard[x + 1][i] == Stone::Empty)
        {
          if (!isFive(x, i - 1, 0, nDir))
          {
            setStone(x, y, Stone::Empty);
            return 0;
          }
          else
            break;
        }
        else
        {
          setStone(x, y, Stone::Empty);
          return 0;
        }
      }
      i = y + 2;
      while (i < (BoardSize + 1))
      {
        if (cBoard[x + 1][i] == c)
        {
          i++;
          nLine++;
          continue;
        }
        else if (cBoard[x + 1][i] == Stone::Empty)
        {
          if (isFive(x, i - 1, 0, nDir))
          {
            setStone(x, y, Stone::Empty);
            return (nLine == 4 ? 1 : 2);
          }
          else
            break;
        }
        else
          break;
      }
      setStone(x, y, Stone::Empty);
      return 0;
      break;
    case 3: // diagonal direction - '/'
      nLine = 1;
      i = x;
      j = y;
      while ((i >= 0) && (j >= 0))
      {
        if (cBoard[i][j] == c)
        {
          i--;
          j--;
          nLine++;
          continue;
        }
        else if (cBoard[i][j] == Stone::Empty)
        {
          if (!isFive(i - 1, j - 1, 0, nDir))
          {
            setStone(x, y, Stone::Empty);
            return 0;
          }
          else
            break;
        }
        else
        {
          setStone(x, y, Stone::Empty);
          return 0;
        }
      }
      i = x + 2;
      j = y + 2;
      while ((i < (BoardSize + 1)) && (j < (BoardSize + 1)))
      {
        if (cBoard[i][j] == c)
        {
          i++;
          j++;
          nLine++;
          continue;
        }
        else if (cBoard[i][j] == Stone::Empty)
        {
          if (isFive(i - 1, j - 1, 0, nDir))
          {
            setStone(x, y, Stone::Empty);
            return (nLine == 4 ? 1 : 2);
          }
          else
            break;
        }
        else
          break;
      }
      setStone(x, y, Stone::Empty);
      return 0;
      break;
    case 4: // diagonal direction - '\'
      nLine = 1;
      i = x;
      j = y + 2;
      while ((i >= 0) && (j <= (BoardSize + 1)))
      {
        if (cBoard[i][j] == c)
        {
          i--;
          j++;
          nLine++;
          continue;
        }
        else if (cBoard[i][j] == Stone::Empty)
        {
          if (!isFive(i - 1, j - 1, 0, nDir))
          {
            setStone(x, y, Stone::Empty);
            return 0;
          }
          else
            break;
        }
        else
        {
          setStone(x, y, Stone::Empty);
          return 0;
        }
      }
      i = x + 2;
      j = y;
      while ((i < (BoardSize + 1)) && (j > 0))
      {
        if (cBoard[i][j] == c)
        {
          i++;
          j--;
          nLine++;
          continue;
        }
        else if (cBoard[i][j] == Stone::Empty)
        {
          if (isFive(i - 1, j - 1, 0, nDir))
          {
            setStone(x, y, Stone::Empty);
            return (nLine == 4 ? 1 : 2);
          }
          else
            break;
        }
        else
          break;
      }
      setStone(x, y, Stone::Empty);
      return 0;
      break;
    default:
      setStone(x, y, Stone::Empty);
      return 0;
      break;
    }
  }
}

bool ForbiddenPointFinder::isDoubleFour(int x, int y)
{
  if (cBoard[x + 1][y + 1] != Stone::Empty)
    return false;

  if (isFive(x, y, 0)) // five?
    return false;

  int nFour = 0;
  for (int i = 1; i <= 4; i++)
  {
    if (isOpenFour(x, y, 0, i) == 2)
      nFour += 2;
    else if (isFour(x, y, 0, i))
      nFour++;
  }

  if (nFour >= 2)
    return true;
  else
    return false;
}

bool ForbiddenPointFinder::isOpenThree(int x, int y, int nColor, int nDir)
{
  if (isFive(x, y, nColor)) // five?
    return false;
  else if ((nColor == 0) && (isOverline(x, y))) // black overline?
    return false;
  else
  {
    Stone c;
    if (nColor == 0) // black
      c = Stone::Black;
    else if (nColor == 1) // white
      c = Stone::White;
    else
      return false;

    setStone(x, y, c);

    int i, j;

    switch (nDir)
    {
    case 1: // horizontal direction
      i = x;
      while (i > 0)
      {
        if (cBoard[i][y + 1] == c)
        {
          i--;
          continue;
        }
        else if (cBoard[i][y + 1] == Stone::Empty)
        {
          if ((isOpenFour(i - 1, y, nColor, nDir) == 1) && (!isDoubleFour(i - 1, y)) &&
              (!isDoubleThree(i - 1, y)))
          {
            setStone(x, y, Stone::Empty);
            return true;
          }
          else
            break;
        }
        else
          break;
      }
      i = x + 2;
      while (i < (BoardSize + 1))
      {
        if (cBoard[i][y + 1] == c)
        {
          i++;
          continue;
        }
        else if (cBoard[i][y + 1] == Stone::Empty)
        {
          if ((isOpenFour(i - 1, y, nColor, nDir) == 1) && (!isDoubleFour(i - 1, y)) &&
              (!isDoubleThree(i - 1, y)))
          {
            setStone(x, y, Stone::Empty);
            return true;
          }
          else
            break;
        }
        else
          break;
      }
      setStone(x, y, Stone::Empty);
      return false;
      break;
    case 2: // vertial direction
      i = y;
      while (i > 0)
      {
        if (cBoard[x + 1][i] == c)
        {
          i--;
          continue;
        }
        else if (cBoard[x + 1][i] == Stone::Empty)
        {
          if ((isOpenFour(x, i - 1, nColor, nDir) == 1) && (!isDoubleFour(x, i - 1)) &&
              (!isDoubleThree(x, i - 1)))
          {
            setStone(x, y, Stone::Empty);
            return true;
          }
          else
            break;
        }
        else
          break;
      }
      i = y + 2;
      while (i < (BoardSize + 1))
      {
        if (cBoard[x + 1][i] == c)
        {
          i++;
          continue;
        }
        else if (cBoard[x + 1][i] == Stone::Empty)
        {
          if ((isOpenFour(x, i - 1, nColor, nDir) == 1) && (!isDoubleFour(x, i - 1)) &&
              (!isDoubleThree(x, i - 1)))
          {
            setStone(x, y, Stone::Empty);
            return true;
          }
          else
            break;
        }
        else
          break;
      }
      setStone(x, y, Stone::Empty);
      return false;
      break;
    case 3: // diagonal direction - '/'
      i = x;
      j = y;
      while ((i > 0) && (j > 0))
      {
        if (cBoard[i][j] == c)
        {
          i--;
          j--;
          continue;
        }
        else if (cBoard[i][j] == Stone::Empty)
        {
          if ((isOpenFour(i - 1, j - 1, nColor, nDir) == 1) && (!isDoubleFour(i - 1, j - 1)) &&
              (!isDoubleThree(i - 1, j - 1)))
          {
            setStone(x, y, Stone::Empty);
            return true;
          }
          else
            break;
        }
        else
          break;
      }
      i = x + 2;
      j = y + 2;
      while ((i < (BoardSize + 1)) && (j < (BoardSize + 1)))
      {
        if (cBoard[i][j] == c)
        {
          i++;
          j++;
          continue;
        }
        else if (cBoard[i][j] == Stone::Empty)
        {
          if ((isOpenFour(i - 1, j - 1, nColor, nDir) == 1) && (!isDoubleFour(i - 1, j - 1)) &&
              (!isDoubleThree(i - 1, j - 1)))
          {
            setStone(x, y, Stone::Empty);
            return true;
          }
          else
            break;
        }
        else
          break;
      }
      setStone(x, y, Stone::Empty);
      return false;
      break;
    case 4: // diagonal direction - '\'
      i = x;
      j = y + 2;
      while ((i > 0) && (j < (BoardSize + 1)))
      {
        if (cBoard[i][j] == c)
        {
          i--;
          j++;
          continue;
        }
        else if (cBoard[i][j] == Stone::Empty)
        {
          if ((isOpenFour(i - 1, j - 1, nColor, nDir) == 1) && (!isDoubleFour(i - 1, j - 1)) &&
              (!isDoubleThree(i - 1, j - 1)))
          {
            setStone(x, y, Stone::Empty);
            return true;
          }
          else
            break;
        }
        else
          break;
      }
      i = x + 2;
      j = y;
      while ((i < (BoardSize + 1)) && (j > 0))
      {
        if (cBoard[i][j] == c)
        {
          i++;
          j--;
          continue;
        }
        else if (cBoard[i][j] == Stone::Empty)
        {
          if ((isOpenFour(i - 1, j - 1, nColor, nDir) == 1) && (!isDoubleFour(i - 1, j - 1)) &&
              (!isDoubleThree(i - 1, j - 1)))
          {
            setStone(x, y, Stone::Empty);
            return true;
          }
          else
            break;
        }
        else
          break;
      }
      setStone(x, y, Stone::Empty);
      return false;
      break;
    default:
      setStone(x, y, Stone::Empty);
      return false;
      break;
    }
  }
}

bool ForbiddenPointFinder::isDoubleThree(int x, int y)
{
  if (cBoard[x + 1][y + 1] != Stone::Empty)
    return false;

  if (isFive(x, y, 0)) // five?
    return false;

  int nThree = 0;
  for (int i = 1; i <= 4; i++)
  {
    if (isOpenThree(x, y, 0, i))
      nThree++;
  }

  if (nThree >= 2)
    return true;
  else
    return false;
}

void ForbiddenPointFinder::findForbiddenPoints()
{
  nForbiddenPoints = 0;
  for (int i = 0; i < BoardSize; i++)
  {
    for (int j = 0; j < BoardSize; j++)
    {
      if (cBoard[i + 1][j + 1] != Stone::Empty)
        continue;
      else
      {
        if (isOverline(i, j) || isDoubleFour(i, j) || isDoubleThree(i, j))
        {
          ptForbidden[nForbiddenPoints].x = i;
          ptForbidden[nForbiddenPoints].y = j;
          nForbiddenPoints++;
        }
      }
    }
  }
}

const int BLACK = 1;  // 棋盘为黑
const int WHITE = -1; // 棋盘为白
const int EMPTY = 0;  // 棋盘为空

int currentPlayer = BLACK;             // 当前玩家
int board[BOARD_LEN][BOARD_LEN] = {0}; // 当前棋盘

bool inBoard(int x, int y)
{
  return x >= 0 && y >= 0 && x < BOARD_LEN && y < BOARD_LEN;
}

ForbiddenPointFinder::Stone cast_player(int player)
{
  // when playing，case just black or white.
  if (player == WHITE)
    return ForbiddenPointFinder::Stone::White;
  return ForbiddenPointFinder::Stone::Black;
}

bool ProcStep(int x, int y, int player, ForbiddenPointFinder &finder, int &game_ret)
{
  if (!inBoard(x, y) || board[x][y])
    return false;
  board[x][y] = player;
  game_ret = static_cast<int>(finder.addStone(x, y, cast_player(currentPlayer)));
  return true;
}

int main()
{
  string str;
  getline(cin, str);
  Json::Reader reader;
  Json::Value input, output;
  reader.parse(str, input);
  input = input["log"];
  currentPlayer = 1; // 先手为黑
  ForbiddenPointFinder finder;
  int game_ret = -1;
  if (input.size() == 0)
  {
    output["display"] = "";
    output["command"] = "request";
    output["content"]["0"]["x"] = -1;
    output["content"]["0"]["y"] = -1;
  }
  else
  {
    for (int i = 1; i < input.size(); i += 2)
    {
      bool isLast = i == input.size() - 1;
      Json::Value content;
      Json::Value display;
      if (currentPlayer == 1) // 0号玩家 / 黑方
      {
        Json::Value answer = input[i]["0"]["response"].isNull() ? input[i]["0"]["content"] : input[i]["0"]["response"];
        if (((answer.isString() &&
              reader.parse(answer.asString(), content)) ||
             (answer.isObject() &&
              (content = answer, true))) &&
            content["x"].isInt() && content["y"].isInt()) // 保证输入格式正确
        {
          int currX = content["x"].asInt();
          int currY = content["y"].asInt();
          if ((!ProcStep(currX, currY, currentPlayer, finder, game_ret) && isLast)) // 不合法棋步！
          {
            stringstream ss;
            ss << "INVALID_MOVE  (" << currX << "," << currY << ")";
            if (inBoard(currX, currY) && board[currX][currY] != EMPTY)
              ss << " is not empty";
            string s;
            getline(ss, s);
            output["display"]["err"] = s;
            output["display"]["winner"] = 1;
            output["command"] = "finish"; // 判输
            output["content"]["0"] = 0;
            output["content"]["1"] = 1;
          }
          else if (isLast) // 正常棋步
          {
            output["display"]["x"] = currX;
            output["display"]["y"] = currY;
            output["display"]["color"] = 0;
            if (game_ret != -1)
            {
              if (game_ret == 1 || game_ret == 2)
              {
                output["display"]["winner"] = 1;
                output["command"] = "finish";
                output["content"]["0"] = 0;
                output["content"]["1"] = 1;
              }
              else if (game_ret == 0)
              {
                output["display"]["winner"] = 0;
                output["command"] = "finish";
                output["content"]["0"] = 1;
                output["content"]["1"] = 0;
              }
              else
              {
                output["display"]["winner"] = "none";
                output["command"] = "finish";
                output["content"]["0"] = 0.5;
                output["content"]["1"] = 0.5;
              }
            }
            else
            {
              output["command"] = "request";
              output["content"]["1"]["x"] = currX;
              output["content"]["1"]["y"] = currY;
            }
          }
        }
        else if (isLast)
        {
          output["display"]["err"] = "INVALID_INPUT_VERDICT_" + input[i]["0"]["verdict"].asString();
          output["display"]["winner"] = 1;
          output["command"] = "finish"; // 判输
          output["content"]["0"] = 0;
          output["content"]["1"] = 1;
        }
      }
      else
      {
        Json::Value answer = input[i]["1"]["response"].isNull() ? input[i]["1"]["content"] : input[i]["1"]["response"];
        if (((answer.isString() &&
              reader.parse(answer.asString(), content)) ||
             (answer.isObject() &&
              (content = answer, true))) &&
            content["x"].isInt() && content["y"].isInt()) // 保证输入格式正确
        {
          int currX = content["x"].asInt();
          int currY = content["y"].asInt();
          if (!ProcStep(currX, currY, currentPlayer, finder, game_ret) && isLast) // 不合法棋步！
          {
            stringstream ss;
            ss << "INVALID_MOVE  (" << currX << "," << currY << ")";
            if (inBoard(currX, currY) && board[currX][currY] != EMPTY)
              ss << " is not empty";
            string s;
            getline(ss, s);
            output["display"]["err"] = s;
            output["display"]["winner"] = 0;
            output["command"] = "finish"; // 判输
            output["content"]["0"] = 1;
            output["content"]["1"] = 0;
          }
          else if (isLast) // 正常棋步
          {
            output["display"]["x"] = currX;
            output["display"]["y"] = currY;
            output["display"]["color"] = 1;
            if (game_ret != -1)
            {
              if (game_ret == 1 || game_ret == 2)
              {
                output["display"]["winner"] = 1;
                output["command"] = "finish";
                output["content"]["0"] = 0;
                output["content"]["1"] = 1;
              }
              else if (game_ret == 0)
              {
                output["display"]["winner"] = 0;
                output["command"] = "finish";
                output["content"]["0"] = 1;
                output["content"]["1"] = 0;
              }
              else
              {
                output["display"]["winner"] = "none";
                output["command"] = "finish";
                output["content"]["0"] = 0.5;
                output["content"]["1"] = 0.5;
              }
            }
            else
            {
              output["command"] = "request";
              output["content"]["0"]["x"] = currX;
              output["content"]["0"]["y"] = currY;
            }
          }
        }
        else if (isLast)
        {
          output["display"]["err"] = "INVALID_INPUT_VERDICT_" + input[i]["1"]["verdict"].asString();
          output["display"]["winner"] = 0;
          output["command"] = "finish"; // 判输
          output["content"]["0"] = 1;
          output["content"]["1"] = 0;
        }
      }
      currentPlayer *= -1;
    }
  }
  Json::FastWriter writer;
  cout << writer.write(output) << endl;
  return 0;
}