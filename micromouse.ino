#include <SoftwareSerial.h>
#include <Encoder.h>
#include <HCSR04.h>
#include <Array.h>

SoftwareSerial serial(7, 8);
Encoder e(18, 19);

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
  // Serial.begin(38400);
  serial.begin(19200);
  // sets back wall of first square to closed and marks it as visited
  squares[0][0].down = 1;
  squares[0][0].visited = true;
  explore();
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
    if (checkWin)
    {
      break;
    }

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
  moveTo(0,0);
}

void solve() {
  int currRow = row; 
  int currCol = col;
  // [row of where it came from, col of where it came from, visited (0 unvisited, 1 visited)]
  Array<Array<Array<int,3>,10>,10> locs;
  Queue toVisit;
  Array<int, 3> start;
  start[0] = 0; start[1] = 0; start[2] = 0;
  toVisit.enqueue(start);

  while(!toVisit.isEmpty()) {
    Array<int, 3> nextLoc = toVisit.dequeue();
    currRow = nextLoc[0]; currCol = nextLoc[1];
    nextLoc[2] = 1;
    if(currRow == winX && currCol == winY) {
      break;
    }
    if(squares[currRow][currCol].up == 0 && locs[currRow-1][currCol][2] == 0) {
      // enqueue the next location into toVisit
      Array<int, 2> temp;
      temp[0] = currRow-1; temp[1] = currCol;
      toVisit.enqueue(temp);
      // updates locs
      Array<int, 3> temp1;
      temp1[0] = currRow; temp1[1] = currCol; temp1[2] = 0;
      locs[currRow-1][currCol] = temp1;
    }
    if(squares[currRow][currCol].right == 0 && locs[currRow][currCol+1][2] == 0) {
      Array<int, 2> temp;
      temp[0] = currRow; temp[1] = currCol+1;
      toVisit.enqueue(temp);

      Array<int, 3> temp1;
      temp1[0] = currRow; temp1[1] = currCol; temp1[2] = 0;
      locs[currRow][currCol+1] = temp1;
    }
    if(squares[currRow][currCol].down == 0 && locs[currRow+1][currCol][2] == 0) {
      Array<int, 2> temp;
      temp[0] = currRow+1; temp[1] = currCol;
      toVisit.enqueue(temp);

      Array<int, 3> temp1;
      temp1[0] = currRow; temp1[1] = currCol; temp1[2] = 0;
      locs[currRow+1][currCol] = temp1;
    }
    if(squares[currRow][currCol].left == 0 && locs[currRow][currCol-1][2] == 0) {
      Array<int, 2> temp;
      temp[0] = currRow; temp[1] = currCol-1;
      toVisit.enqueue(temp);

      Array<int, 3> temp1;
      temp1[0] = currRow; temp1[1] = currCol; temp1[2] = 0;
      locs[currRow][currCol-1] = temp1;
    }
  }
  Stack shortestPath;
  Array<int, 3> nextSquare = locs[winX][winY];
  while(nextSquare[0] != row && nextSquare[1] != col) {
    Array<int, 2> loc1; loc1[0] = nextSquare[0]; loc1[1] = nextSquare[1];
    shortestPath.push(loc1);
    nextSquare = locs[nextSquare[0]][nextSquare[1]];
  }
  
  // moves robot to middle through shortest path
  while(row != winX && col != winY && !shortestPath.isEmpty()) {
    Array<int, 2> pathSquare = shortestPath.pop();
    moveTo(pathSquare[0], pathSquare[1]);
  }
}

// 0 is up, 1 is right, 2 is down, 3 is left
void moveTo(int r, int c)
{
  if(r == row && c == col) {
    return;
  }
  // if the target square is adjacent to the current square
  if (((r == row - 1 || r == row + 1) && c == col) || ((c == col - 1 || c == col + 1) && r == row))
  {
    if (r == row - 1)
    {
      if (dir == 0)
      {
        moveSquares(1, false);
      }
      else if (dir == 1)
      {
        turn(true);
        moveSquares(1, false);
      }
      else if (dir == 2)
      {
        turn(true);
        turn(true);
        moveSquares(1, false);
      }
      else
      {
        turn(false);
        moveSquares(1, false);
      }
    }
    if (r == row + 1)
    {
      if (dir == 0)
      {
        turn(true);
        turn(true);
        moveSquares(1, false);
      }
      else if (dir == 1)
      {
        turn(false);
        moveSquares(1, false);
      }
      else if (dir == 2)
      {
        moveSquares(1, false);
      }
      else
      {
        turn(true);
        moveSquares(1, false);
      }
    }
    if (c == col + 1)
    {
      if (dir == 0)
      {
        turn(false);
        moveSquares(1, false);
      }
      else if (dir == 1)
      {
        moveSquares(1, false);
      }
      else if (dir == 2)
      {
        turn(true);
        moveSquares(1, false);
      }
      else
      {
        turn(true);
        turn(true);
        moveSquares(1, false);
      }
    }
    if (c == col - 1)
    {
      if (dir == 0)
      {
        turn(true);
        moveSquares(1, false);
      }
      else if (dir == 1)
      {
        turn(false);
        turn(false);
        moveSquares(1, false);
      }
      else if (dir == 2)
      {
        turn(false);
        moveSquares(1, false);
      }
      else
      {
        moveSquares(1, false);
      }
    }
  }
  // if the target square is not adjacent to the current square, backtrack along trail
  else
  {
    Array<int, 2> curr = trail.pop();
    while (!((curr[0] == r - 1 || curr[0] == r + 1) && curr[1] == c) || ((curr[1] == c - 1 || curr[1] == c + 1) && curr[0] == r))
    {
      moveTo(curr[0], curr[1]);
      curr = trail.pop();
    }
    moveTo(curr[0], curr[1]);
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
  {
    for (int c = 0; c < 9; c++)
    {
      if (squares[r][c].right == 0 && squares[r][c].down == 0 && squares[r + 1][c].up == 0 && squares[r + 1][c].right == 0 &&
          squares[r][c + 1].left == 0 && squares[r][c + 1].down == 0 && squares[r + 1][c + 1].up == 0 && squares[r + 1][c + 1].left == 0)
      {
        winX = r;
        winY = c;
        return true;
      }
    }
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
  return rnd(hc[d].dist() / 24.4475); // (23.495 + 25.4) / 2
}

void moveSquares(byte squares, bool b)
{
  int numHoles = 0;
  byte encValue = 0, ls = 100, rs = 225;

  serial.write(b ? 1 : ls);   // left
  serial.write(b ? 128 : rs); // right
  while (numHoles <= (b ? 100 : 310))
  {
    if (squares % 300 == 150 && !b)
    {
      if (hc[2].dist() > hc[1].dist())
        if (ls < 127)
        {
          ls++;
          serial.write(ls);
        }
        else
        {
          rs -= 2;
          serial.write(rs);
        }
      if (hc[1].dist() > hc[2].dist())
        if (rs < 127)
        {
          rs++;
          serial.write(rs);
        }
        else
        {
          ls -= 2;
          serial.write(ls);
        }
    }
    if (e.read() != encValue)
    {
      encValue = e.read();
      if (encValue == 0)
      {
        numHoles++;
        encValue = 0;
      }
    }
  }
  serial.write(64);
  serial.write(192);
  if (dir == 0)
  {
    row -= squares;
  }
  else if (dir == 2)
  {
    row += squares;
  }
  else if (dir == 1)
  {
    col += squares;
  }
  else
  {
    col -= squares;
  }
  delay(750);
}

// 0 is up, 1 is right, 2 is down, 3 is left
void turn(bool left)
{
  int numHoles = 0;
  byte encValue = 0;

  if (left)
  {
    if (--dir < 0)
      dir = 3;
    serial.write(1);
    serial.write(255);
  }
  else
  {
    if (++dir > 3)
      dir = 0;
    serial.write(127);
    serial.write(128);
  }
  while (numHoles <= 128)
  {
    if (e.read() != encValue)
    {
      encValue = e.read();
      if (encValue == 0)
      {
        numHoles++;
        encValue = 0;
      }
    }
  }
  serial.write(64);
  serial.write(192);
  moveSquares(1, true);
  delay(750);
}

int rnd(float n)
{
  if (n - (byte)n < 0.5)
    return (int)n;
  else
    return (int)n + 1;
}
