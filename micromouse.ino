#include <SoftwareSerial.h>
#include <Encoder.h>
#include <HCSR04.h>

#include <Vector.h>

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

  Square() {
    up = 2, right = 2, down = 2, left = 2;
  }
};

struct Stack 
{
  Vector<Vector<int>> stack;

  bool isEmpty() {
    return stack.size() == 0;
  }

  void push(Vector<int> v) {
    stack.push_back(v);
  }

  Vector<int> pop() {
    Vector<int> val = stack[stack.size()-1];
    stack.pop_back();
    return val;
  }

  Vector<int> peek() {
    return stack[stack.size()-1];
  }

};

Square squares[10][10];
Stack trail;

// location of the bots
int row = 0, col = 0;
// 0 is up, 1 is right, 2 is down, 3 is left
byte dir = 1;

byte winX = -1, winY = -1;

void setup()
{
  serial.begin(19200);
  // sets back wall of first square to closed
  squares[0][0].down = 1;
  
}

void loop()
{
  
}

void explore() {
  Stack toVisit;
  byte counter = 0;
  
  while(true) {

    // update data abount current square
    setSquare();

    // update trail, which is used for backtracking
    Vector<int> vect;
    vect.push_back(row); vect.push_back(col);
    trail.push(vect);

    // if we found the middle, break out of loop
    counter++;
    counter %= 6;
    if(counter == 5) {
      if(checkWin) {
        break;
      }
    }

    bool rightOpen = squares[row][col].right == 0;
    bool leftOpen = squares[row][col].left == 0;
    bool downOpen = squares[row][col].down == 0;
    bool upOpen = squares[row][col].up == 0;

    Vector<int> nextLoc;
    nextLoc.push_back(row); nextLoc.push_back(col+1);
    Vector<int> toPush;
    toPush.push_back(-1); toPush.push_back(-1);

    if(rightOpen && (squares[nextLoc[0]][nextLoc[1]].up != 2 || squares[nextLoc[0]][nextLoc[1]].down != 2 || squares[nextLoc[0]][nextLoc[1]].right != 2 || squares[nextLoc[0]][nextLoc[1]].left != 2)) {
      toPush[0] = row; toPush[1] = col+1;
      toVisit.push(toPush);
    }
    nextLoc[1] = col-1;
    if(leftOpen && (squares[nextLoc[0]][nextLoc[1]].up != 2 || squares[nextLoc[0]][nextLoc[1]].down != 2 || squares[nextLoc[0]][nextLoc[1]].right != 2 || squares[nextLoc[0]][nextLoc[1]].left != 2)) {
      toPush[0] = row; toPush[1] = col-1;
      toVisit.push(toPush);
    }
    nextLoc[0] = row+1; nextLoc[1] = col;
    if(downOpen && (squares[nextLoc[0]][nextLoc[1]].up != 2 || squares[nextLoc[0]][nextLoc[1]].down != 2 || squares[nextLoc[0]][nextLoc[1]].right != 2 || squares[nextLoc[0]][nextLoc[1]].left != 2)) {
      toPush[0] = row+1; toPush[1] = col;
      toVisit.push(toPush);
    }
    nextLoc[0] = row-1; nextLoc[1] = col;
    if(upOpen && (squares[nextLoc[0]][nextLoc[1]].up != 2 || squares[nextLoc[0]][nextLoc[1]].down != 2 || squares[nextLoc[0]][nextLoc[1]].right != 2 || squares[nextLoc[0]][nextLoc[1]].left != 2)) {
      toPush[0] = row-1; toPush[1] = col;
      toVisit.push(toPush);
    }
    Vector<int> next = toVisit.pop();
    moveTo(next[0], next[1]);
  }
}

// 0 is up, 1 is right, 2 is down, 3 is left
void moveTo(int r, int c) {
  if(((r == row-1 || r == row+1) && c == col) || ((c == col-1 || c == col+1) && r == row)) {
    if(r == row-1) {
      if(dir == 0) {
        moveSquares(1,false);
      }
      else if(dir == 1) {
        turn(true);
        moveSquares(1,false);
      }
      else if(dir == 2) {
        turn(true); turn(true);
        moveSquares(1, false);
      }
      else {
        turn(false);
        moveSquares(1, false);
      }
    }
    if(r == row+1) {
      if(dir == 0) {
        turn(true); turn(true);
        moveSquares(1,false);
      }
      else if(dir == 1) {
        turn(false);
        moveSquares(1,false);
      }
      else if(dir == 2) {
        moveSquares(1, false);
      }
      else {
        turn(true);
        moveSquares(1, false);
      }
    }
    if(c == col+1) {
      if(dir == 0) {
        turn(false);
        moveSquares(1,false);
      }
      else if(dir == 1) {
        moveSquares(1,false);
      }
      else if(dir == 2) {
        turn(true);
        moveSquares(1, false);
      }
      else {
        turn(true); turn(true);
        moveSquares(1, false);
      }
    }
    if(c == col-1) {
      if(dir == 0) {
        turn(true);
        moveSquares(1,false);
      }
      else if(dir == 1) {
        turn(false); turn(false);
        moveSquares(1,false);
      }
      else if(dir == 2) {
        turn(false);
        moveSquares(1, false);
      }
      else {
        moveSquares(1, false);
      }
    }
  }
  else {
    Vector<int> curr = trail.pop();
    while(!((curr[0] == r-1 || curr[0] == r+1) && curr[1] == c) || ((curr[1] == c-1 || curr[1] == c+1) && curr[0] == r)) {
      moveTo(curr[0], curr[1]);
      curr = trail.pop();
    }
    moveTo(curr[0],curr[1]);
    moveTo(r, c);
  }
}

void setSquare()
{
  if (dir == 0)
  {
    if (getSquares(0) > 0)
      squares[row][col].left = 1;
    else
      squares[row][col].left = 0;
    if (getSquares(1) > 0)
      squares[row][col].up = 1;
    else
      squares[row][col].up = 0;
    if (getSquares(2) > 0)
      squares[row][col].right = 1;
    else
      squares[row][col].right = 0;
  }
  else if (dir == 1)
  {
    if (getSquares(0) > 0)
      squares[row][col].up = 1;
    else
      squares[row][col].up = 0;
    if (getSquares(1) > 0)
      squares[row][col].right = 1;
    else
      squares[row][col].right = 0;
    if (getSquares(2) > 0)
      squares[row][col].down = 1;
    else
      squares[row][col].down = 0;
  }
  else if (dir == 2)
  {
    if (getSquares(0) > 0)
      squares[row][col].right = 1;
    else
      squares[row][col].right = 0;
    if (getSquares(1) > 0)
      squares[row][col].down = 1;
    else
      squares[row][col].down = 0;
    if (getSquares(2) > 0)
      squares[row][col].left = 1;
    else
      squares[row][col].left = 0;
  }
  else
  {
    if (getSquares(0) > 0)
      squares[row][col].down = 1;
    else
      squares[row][col].down = 0;
    if (getSquares(1) > 0)
      squares[row][col].left = 1;
    else
      squares[row][col].left = 0;
    if (getSquares(2) > 0)
      squares[row][col].up = 1;
    else
      squares[row][col].up = 0;
  }
  /*
  if(row > 0) squares[row-1][col].down = squares[row][col].up;
  if(row < 9) squares[row+1][col].down = squares[row][col].up;
  if(col > 0) squares[row-1][col].down = squares[row][col].up;
  if(col < 9) squares[row-1][col].down = squares[row][col].up;
  */
}

bool checkWin()
{
  for (byte r = 0; r < 9; r++)
  {
    for (byte c = 0; c < 9; c++)
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

// 0 is left, 1 is front, 2 is right
byte getSquares(byte d)
{
  return rnd(hc[d].dist() / 25.4);
}

void moveSquares(byte squares, bool b)
{
  for (byte i = 0; i < squares; i++)
  {
    int numHoles = 0;
    byte encValue = 0;

    serial.write(b ? 1 : 127);
    serial.write(b ? 128 : 255);
    while (numHoles <= (b ? 50 : 303))
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
    delay(1000);
  }
}

// 0 is up, 1 is right, 2 is down, 3 is left
void turn(bool left)
{
  int numHoles = 0;
  byte encValue = 0;


  if (left)
  {
    if(--dir < 0) dir = 3;
    serial.write(127);
    serial.write(128);
  }
  else
  {
    if(++dir > 3) dir = 0;
    serial.write(1);
    serial.write(255);
  }
  while (numHoles <= (left ? 95 : 80))
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
  delay(1000);
  moveSquares(1, true);
}

int rnd(float n)
{
  if (n - (byte)n < 0.5)
    return (int)n;
  else
    return (int)n + 1;
}
