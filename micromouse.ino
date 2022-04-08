#include <SoftwareSerial.h>
#include <Encoder.h>
#include <HCSR04.h>
#include <Array.h>

SoftwareSerial serial(8, 7);

Encoder lenc(18, 19), renc(20, 21);

HCSR04 left(3, 4);
HCSR04 front(5, 6);
HCSR04 right(9, 10);
HCSR04 hc[] = {left, front, right};

struct Square
{
  // 1 is wall, 0 is no wall, 2 is don't know
  byte up = 2, right = 2, down = 2, left = 2;
  bool visited = false;
};

struct Stack
{

  Array<Array<int, 2>, 100> stack;

  bool isEmpty()
  {
    return stack.size() == 0;
  }

  void push(Array<int, 2> v)
  {
    stack.push_back(v);
  }

  Array<int, 2> pop()
  {
    Array<int, 2> val = stack[stack.size() - 1];
    stack.pop_back();
    return val;
  }

  Array<int, 2> peek()
  {
    return stack[stack.size() - 1];
  }
};

struct Queue
{

  Array<Array<int, 2>, 100> queue;

  bool isEmpty()
  {
    return queue.size() == 0;
  }

  void enqueue(Array<int, 2> v)
  {
    queue.push_back(v);
  }

  Array<int, 2> dequeue()
  {
    Array<int, 2> val = queue[0];
    queue.remove(0);
    return val;
  }

  Array<int, 2> peek()
  {
    return queue[0];
  }
};

// represents the maze as 2d array of squares
Square squares[10][10];
// trail/path of the robot, used for backtracking purposes
Stack trail;

// location of the bot (starts at top left corner)
int row = 0, col = 0;
// 0 is up, 1 is right, 2 is down, 3 is left (starts facing right)
byte dir = 1;

// coords of middle/winning square (updated in checkWin)
int winX = -1, winY = -1;

void setup()
{
  Serial.begin(38400);
  serial.begin(115200);

  // sets back wall of first square to closed and marks it as visited
  squares[0][0].left = 1;
  squares[0][0].visited = true;

  explore();
  solve();
}

void loop()
{
}

// explores the maze, gathering data and finding the winning square
void explore()
{
  // stack of squares we want to visit next
  Stack toVisit;
  // counter for checkWin
  int counter = 0;

  while (true)
  {
    // update data abount current square
    setSquare();

    // update trail, which is used for backtracking
    Array<int, 2> vect;
    vect.push_back(row);
    vect.push_back(col);
    trail.push(vect);

    // check for the middle. if we found the middle, break out of the loop
    if (checkWin())
      break;

    bool rightOpen = squares[row][col].right == 0;
    bool leftOpen = squares[row][col].left == 0;
    bool downOpen = squares[row][col].down == 0;
    bool upOpen = squares[row][col].up == 0;

    Array<int, 2> nextLoc;
    nextLoc[0] = row;
    nextLoc[1] = col + 1;

    // pushes locations to toVisit if we haven't visited them yet and they're open
    if (rightOpen && squares[nextLoc[0]][nextLoc[1]].visited == false)
    {
      toVisit.push(nextLoc);
    }
    nextLoc[0] = row;
    nextLoc[1] = col - 1;

    if (leftOpen && squares[nextLoc[0]][nextLoc[1]].visited == false)
    {
      toVisit.push(nextLoc);
    }
    nextLoc[0] = row + 1;
    nextLoc[1] = col;

    if (downOpen && squares[nextLoc[0]][nextLoc[1]].visited == false)
    {
      toVisit.push(nextLoc);
    }
    nextLoc[0] = row - 1;
    nextLoc[1] = col;

    if (upOpen && squares[nextLoc[0]][nextLoc[1]].visited == false)
    {
      toVisit.push(nextLoc);
    }
    // move to the first location on the stack
    Array<int, 2> next = toVisit.pop();
    moveTo(next[0], next[1]);
  }
  // move back to the beginning after exploring
  moveTo(0, 0);
}

void solve()
{
  int currRow = row;
  int currCol = col;
  // [row of where it came from, col of where it came from, visited (0 unvisited, 1 visited)]
  int locs[10][10][3];

  Queue toVisit;
  Array<int, 2> start;
  start[0] = 0;
  start[1] = 0;
  toVisit.enqueue(start);

  while (!toVisit.isEmpty())
  {
    Array<int, 2> nextLoc = toVisit.dequeue();
    currRow = nextLoc[0];
    currCol = nextLoc[1];
    locs[currRow][currCol][2] = 1;
    if (currRow == winX && currCol == winY)
    {
      break;
    }
    if (squares[currRow][currCol].up == 0 && locs[currRow - 1][currCol][2] == 0)
    {
      // enqueue the next location into toVisit
      Array<int, 2> temp;
      temp[0] = currRow - 1;
      temp[1] = currCol;
      toVisit.enqueue(temp);
      // updates locs for the next location
      locs[currRow - 1][currCol][0] = currRow;
      locs[currRow - 1][currCol][1] = currCol;
      locs[currRow - 1][currCol][2] = 0;
    }
    if (squares[currRow][currCol].right == 0 && locs[currRow][currCol + 1][2] == 0)
    {
      Array<int, 2> temp;
      temp[0] = currRow;
      temp[1] = currCol + 1;
      toVisit.enqueue(temp);

      locs[currRow][currCol + 1][0] = currRow;
      locs[currRow][currCol + 1][1] = currCol;
      locs[currRow][currCol + 1][2] = 0;
    }
    if (squares[currRow][currCol].down == 0 && locs[currRow + 1][currCol][2] == 0)
    {
      Array<int, 2> temp;
      temp[0] = currRow + 1;
      temp[1] = currCol;
      toVisit.enqueue(temp);

      locs[currRow + 1][currCol][0] = currRow;
      locs[currRow + 1][currCol][1] = currCol;
      locs[currRow + 1][currCol][2] = 0;
    }
    if (squares[currRow][currCol].left == 0 && locs[currRow][currCol - 1][2] == 0)
    {
      Array<int, 2> temp;
      temp[0] = currRow;
      temp[1] = currCol - 1;
      toVisit.enqueue(temp);

      locs[currRow][currCol - 1][0] = currRow;
      locs[currRow][currCol - 1][1] = currCol;
      locs[currRow][currCol - 1][2] = 0;
    }
  }
  Stack shortestPath;
  Array<int, 3> nextSquare = locs[winX][winY];
  Array<int, 2> winnerSquare;
  winnerSquare[0] = winX;
  winnerSquare[1] = winY;
  shortestPath.push(winnerSquare);
  while (nextSquare[0] != row && nextSquare[1] != col)
  {
    Array<int, 2> loc1;
    loc1[0] = nextSquare[0];
    loc1[1] = nextSquare[1];
    shortestPath.push(loc1);
    nextSquare = locs[nextSquare[0]][nextSquare[1]];
  }

  // moves robot to middle through shortest path
  while (row != winX && col != winY && !shortestPath.isEmpty())
  {
    Array<int, 2> pathSquare = shortestPath.pop();
    moveTo(pathSquare[0], pathSquare[1]);
  }
}

// 0 is up, 1 is right, 2 is down, 3 is left
void moveTo(int r, int c)
{
  if (r == row && c == col)
    return;
  // if the target square is adjacent to the current square
  bool adj = ((r == row - 1 || r == row + 1) && c == col) || ((c == col - 1 || c == col + 1) && r == row);

  int rdir = r - row, cdir = c - col;
  if (adj && ((rdir == -1 && squares[row][col].up == 0) ||
              (cdir == 1 && squares[row][col].right == 0) ||
              (rdir == 1 && squares[row][col].down == 0) ||
              (cdir == -1 && squares[row][col].left == 0)))
  {
    if (r == row - 1)
      if (dir == 2)
        if (squares[row][col].right == 1 && squares[row][col].left == 1)
        {
          moveBackOne();
        }
        else
        {
          turnLeft(2);
          moveOne();
        }
      else
      {
        turnLeft(dir);
        moveOne();
      }
    if (r == row + 1)
      if (dir == 0)
        if (squares[row][col].right == 1 && squares[row][col].left == 1)
        {
          moveBackOne();
        }
        else
        {
          turnLeft(2);
          moveOne();
        }
      else
      {
        turnLeft((dir + 2) % 4);
        moveOne();
      }
    if (c == col + 1)
      if (dir == 3)
        if (squares[row][col].up == 1 && squares[row][col].down == 1)
        {
          moveBackOne();
        }
        else
        {
          turnLeft(2);
          moveOne();
        }
      else
      {
        turnLeft((dir + 3) % 4);
        moveOne();
      }
    if (c == col - 1)
      if (dir == 1)
        if (squares[row][col].up == 1 && squares[row][col].down == 1)
        {
          moveBackOne();
        }
        else
        {
          turnLeft(2);
          moveOne();
        }
      else
      {
        turnLeft((dir + 1) % 4);
        moveOne();
      }
  }

  // if the target square is not adjacent to the current square, backtrack along trail
  else
  {
    Array<int, 2> curr = trail.pop();
    while (!(((curr[0] == r - 1 || curr[0] == r + 1) && curr[1] == c) || ((curr[1] == c - 1 || curr[1] == c + 1) && curr[0] == r)))
    {
      moveTo(curr[0], curr[1]);
      curr = trail.pop();
    }
    moveTo(curr[0], curr[1]);
    trail.push(curr);
    moveTo(r, c);
  }
  // marks target square as visited
  squares[r][c].visited = true;
}

// 0 is left, 1 is front, 2 is right
void setSquare()
{
  if (dir == 0)
  {
    if (getSquares(0) > 0)
      squares[row][col].left = 0;
    else
      squares[row][col].left = 1;
    if (getSquares(1) > 0)
      squares[row][col].up = 0;
    else
      squares[row][col].up = 1;
    if (getSquares(2) > 0)
      squares[row][col].right = 0;
    else
      squares[row][col].right = 1;
  }
  else if (dir == 1)
  {
    if (getSquares(0) > 0)
      squares[row][col].up = 0;
    else
      squares[row][col].up = 1;
    if (getSquares(1) > 0)
      squares[row][col].right = 0;
    else
      squares[row][col].right = 1;
    if (getSquares(2) > 0)
      squares[row][col].down = 0;
    else
      squares[row][col].down = 1;
  }
  else if (dir == 2)
  {
    if (getSquares(0) > 0)
      squares[row][col].right = 0;
    else
      squares[row][col].right = 1;
    if (getSquares(1) > 0)
      squares[row][col].down = 0;
    else
      squares[row][col].down = 1;
    if (getSquares(2) > 0)
      squares[row][col].left = 0;
    else
      squares[row][col].left = 1;
  }
  else
  {
    if (getSquares(0) > 0)
      squares[row][col].down = 0;
    else
      squares[row][col].down = 1;
    if (getSquares(1) > 0)
      squares[row][col].left = 0;
    else
      squares[row][col].left = 1;
    if (getSquares(2) > 0)
      squares[row][col].up = 0;
    else
      squares[row][col].up = 1;
  }
  // updates adjacent squares
  if (row > 0)
    squares[row - 1][col].down = squares[row][col].up;
  if (row < 9)
    squares[row + 1][col].up = squares[row][col].down;
  if (col > 0)
    squares[row][col - 1].right = squares[row][col].left;
  if (col < 9)
    squares[row][col + 1].left = squares[row][col].right;
}

bool checkWin()
{
  for (int r = 0; r < 9; r++)
    for (int c = 0; c < 9; c++)
      if (squares[r][c].right == 0 && squares[r][c].down == 0 && squares[r + 1][c].up == 0 && squares[r + 1][c].right == 0 &&
          squares[r][c + 1].left == 0 && squares[r][c + 1].down == 0 && squares[r + 1][c + 1].up == 0 && squares[r + 1][c + 1].left == 0)
      {
        winX = r;
        winY = c;
        return true;
      }
  return false;
}

/**
 * 0 - left
 * 1 - front
 * 2 - right
 **/
byte getSquares(byte d)
{
  delay(25);
  byte s = rnd(hc[d].dist() / 25.4); // (23.495 + 25.4) / 2 = 24.4475
  return s > 10 ? 0 : s;
}

void move(byte squares)
{
  for (byte i = 0; i < squares; i++)
    moveOne();
}

void turnLeft(byte turns)
{
  if (turns % 4 != 0)
  {
    dir += turns * 3;
    dir %= 4;
  }

  // bool wall = getSquares(1) == 0;
  if (turns % 4 == 3)
  {
    trnRight(347, 400);
    mve(150, 400, false);
    trnRight(347, 400);
    mveBack(550, 400);
  }
  /*else
  {
    mve(100, 400, false);
    trnRight(730, 400);
    mveBack(400, 400);
  }*/
  if (turns % 4 == 2)
    if (getSquares(0) > 0)
    {
      trnLeft(303, 400);
      mve(150, 400, false);
      trnLeft(700, 400);
      mveBack(250, 400);
      trnLeft(303, 400);
    }
    else if (getSquares(2) > 0)
    {
      trnRight(345, 400);
      mve(150, 400, false);
      trnRight(720, 400);
      mveBack(250, 400);
      trnRight(345, 400);
    }
  if (turns % 4 == 1)
  {
    trnLeft(290, 400);
    mve(150, 400, false);
    trnLeft(290, 400);
    mveBack(550, 400);
  }
  /*else
  {
    mve(100, 400, false);
    trnLeft(642, 400);
    mveBack(400, 400);
  }*/
}

void moveBackOne()
{
  mveBack(1400, 400);

  switch (dir)
  {
  case 0:
    row++;
    break;
  case 2:
    row--;
    break;
  case 1:
    col--;
    break;
  default:
    col++;
    break;
  }
}

void moveOne()
{
  mve(1395, 400, true);

  switch (dir)
  {
  case 0:
    row--;
    break;
  case 2:
    row++;
    break;
  case 1:
    col++;
    break;
  default:
    col--;
    break;
  }
}

void mve(int pulses, int dlay, bool ismv1)
{
  lenc.write(0);
  renc.write(0);

  byte ls = 84, rs = 212;
  unsigned long adjustmentDebounce = millis();

  serial.write(ls);
  serial.write(rs);

  bool i = ismv1 && getSquares(1) < 2;

  while (i ? hc[1].dist() > 5.5 : lenc.read() < pulses)
    if (millis() - adjustmentDebounce >= 200)
    {
      adjustmentDebounce = millis();

      unsigned long lReading = lenc.read(), rReading = renc.read();

      if (rReading < lReading)
        if (rs < 215)
        {
          rs++;
          serial.write(rs);
        }
        else
        {
          ls--;
          serial.write(ls);
        }

      if (lReading < rReading)
        if (ls < 87)
        {
          ls++;
          serial.write(ls);
        }
        else
        {
          rs--;
          serial.write(rs);
        };
    }
  serial.write(64);
  serial.write(192);

  delay(dlay);
}

void mveBack(int pulses, int dlay)
{
  lenc.write(0);
  renc.write(0);

  byte ls = 44, rs = 172;
  unsigned long adjustmentDebounce = millis();

  serial.write(ls);
  serial.write(rs);

  while ((lenc.read() * -1) < pulses)
    if (millis() - adjustmentDebounce >= 200)
    {
      adjustmentDebounce = millis();

      unsigned long lReading = (lenc.read() * -1), rReading = (renc.read() * -1);

      if (rReading < lReading)
        if (rs > 169)
        {
          rs--;
          serial.write(rs);
        }
        else
        {
          ls++;
          serial.write(ls);
        }

      if (lReading < rReading)
        if (ls > 41)
        {
          ls--;
          serial.write(ls);
        }
        else
        {
          rs++;
          serial.write(rs);
        };
    }

  serial.write(64);
  serial.write(192);

  delay(dlay);
}

void trnRight(int pulses, int dlay)
{
  lenc.write(0);
  renc.write(0);

  byte ls = 84, rs = 172;
  unsigned long adjustmentDebounce = millis();

  serial.write(ls);
  serial.write(rs);

  while (lenc.read() < pulses)
    if (millis() - adjustmentDebounce >= 200)
    {
      adjustmentDebounce = millis();

      unsigned long lReading = lenc.read(), rReading = (renc.read() * -1);

      if (rReading < lReading)
        if (rs > 169)
        {
          rs--;
          serial.write(rs);
        }
        else
        {
          ls--;
          serial.write(ls);
        }

      if (lReading < rReading)
        if (ls < 87)
        {
          ls++;
          serial.write(ls);
        }
        else
        {
          rs++;
          serial.write(rs);
        };
    }

  serial.write(64);
  serial.write(192);

  delay(dlay);
}

void trnLeft(int pulses, int dlay)
{
  lenc.write(0);
  renc.write(0);

  byte ls = 44, rs = 212;
  unsigned long adjustmentDebounce = millis();

  serial.write(ls);
  serial.write(rs);

  while ((lenc.read() * -1) < pulses)
    if (millis() - adjustmentDebounce >= 200)
    {
      adjustmentDebounce = millis();

      unsigned long lReading = (lenc.read() * -1), rReading = renc.read();

      if (rReading < lReading)
        if (rs < 215)
        {
          rs++;
          serial.write(rs);
        }
        else
        {
          ls++;
          serial.write(ls);
        }

      if (lReading < rReading)
        if (ls > 41)
        {
          ls--;
          serial.write(ls);
        }
        else
        {
          rs--;
          serial.write(rs);
        };
    }
  serial.write(64);
  serial.write(192);

  delay(dlay);
}

int rnd(float n)
{
  if (n - (byte)n < 0.5)
    return (int)n;
  else
    return (int)n + 1;
}
