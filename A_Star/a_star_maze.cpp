#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <cmath>
#include <unistd.h>
#include <string>

#define BLACK_TEXT	"\e[30;1m"
#define RED_TEXT	"\e[31;1m"
#define GREEN_TEXT	"\e[32;1m"
#define YELLOW_TEXT	"\e[33;1m"
#define BLUE_TEXT	"\e[34;1m"
#define DEFAULT		"\e[0m"

using namespace std;

class Node {
public:
  char tile, direction;
  vector<int> connectedX;
  vector<int> connectedY;
  int x, y, h, depth;
  bool visited;
  Node(int x, int y, char tile, int h, int depth, char direction){
    this->x = x;
    this->y = y;
    this->tile = tile;
    this->h = h;
    this->depth = depth;
    this->direction = direction;
    this->visited = false;
  }
};

struct CustomCompare{
  bool operator()(const Node* a, const Node* b){
    return a->h > b->h;
  }
};

void print_maze(vector< vector<char> > maze, int x, int y){
  for(int i=0; i<maze.size(); i++){
    for(int j=0; j<maze[i].size(); j++){
      if(j==x && i==y){
        printf(BLUE_TEXT);
        printf("X ");
        printf(DEFAULT);
      }
      else if(maze[i][j]=='<' || maze[i][j]=='>' || maze[i][j]=='^'
              || maze[i][j]=='v' || maze[i][j]=='S'){
        printf(GREEN_TEXT);
        printf("%c ", maze[i][j]);
        printf(DEFAULT);
      }
      else if(maze[i][j]=='G'){
        printf(RED_TEXT);
        printf("G ");
        printf(DEFAULT);
      }
      else if(maze[i][j]=='0'){
        printf("  ");
      }
      else {
        printf("%c ", maze[i][j]);
      }
    }
    printf("\n");
  }
  printf("\n\n");
}

int main(int argc, char* argv[]){
  if(argc != 5){
    printf("ERROR. 5 parameters expected: program name, labyrinth file name, # of rows, # of columns and delay seconds for output\n");
    return 0;
  }
  printf("< > ^ v\n");
  vector< vector<char> > maze;
  string file_name = argv[1];
  int rows = atoi(argv[2]);
  int columns = atoi(argv[3]);
  int wait_seconds = atoi(argv[4]);
  ifstream input_file(file_name);
  int startX, startY, goalX, goalY;
  char read;
  for(int i=0; i<rows; i++){
    vector<char> row;
    for(int j=0; j<columns; j++){
      input_file >> read;
      // printf("%c ", read);
      if(read == 'S'){
        startX = j;
        startY = i;
      }
      else if(read == 'G'){
        goalX = j;
        goalY = i;
      }
      row.push_back(read);
    }
    maze.push_back(row);
    // printf("\n");
  }
  printf("start at %d,%d!\n", startX, startY);
  printf("goal at %d,%d!\n", goalX, goalY);
  printf("\n\n");
  // print_maze(maze, startX, startY);
  // printf("\n\nroot: %c\n", maze[startY][startX]);
  priority_queue<Node*, vector<Node*>, CustomCompare> heap;
  heap.push(new Node(startX, startY, maze[startY][startX], 0, 0, 'S'));
  int currentX = startX;
  int currentY = startY;
  int depth = 0;
  while(true){
    Node* min = heap.top();
    heap.pop();
    // printf("\nlooking at %c: %d,%d -> %d\n", min->tile, min->x, min->y, min->h);
    currentX = min->x;
    currentY = min->y;
    depth = min->depth;
    print_maze(maze, currentX, currentY);
    if(min->tile == 'G'){
      printf(YELLOW_TEXT);
      printf("GOAL FOUND!\n\n");
      return 0;
    }
    maze[currentY][currentX] = min->direction;
    if(currentX>0){
      if(maze[currentY][currentX-1]=='0' || maze[currentY][currentX-1]=='G'){
        // printf("a) pushing %d,%d with h=%d+(%d-%d)+(%d-%d)=%d\n", currentX-1, currentY,
              // depth+1, goalY, currentY, goalX, currentX-1,
              // depth+1 + abs(goalY-currentY) + abs(goalX-(currentX-1)));
        heap.push(new Node(currentX-1, currentY, maze[currentY][currentX-1],
          depth+1 + abs(goalY-currentY) + abs(goalX-(currentX-1)), depth+1, '<'));
      }
    }
    if(currentX<columns-1){
      if(maze[currentY][currentX+1]=='0' || maze[currentY][currentX+1]=='G'){
        // printf("b) pushing %d,%d with h=%d+(%d-%d)+(%d-%d)=%d\n", currentX+1, currentY,
              // depth+1, goalY, currentY, goalX, currentX+1,
              // depth+1 + abs(goalY-currentY) + abs(goalX-(currentX+1)));
        heap.push(new Node(currentX+1, currentY, maze[currentY][currentX+1],
          depth+1 + abs(goalY-currentY) + abs(goalX-(currentX+1)), depth+1, '>'));
      }
    }
    if(currentY>0){
      if(maze[currentY-1][currentX]=='0' || maze[currentY-1][currentX]=='G'){
        // printf("c) pushing %d,%d with h=%d+(%d-%d)+(%d-%d)=%d\n", currentX, currentY-1,
              // depth+1, goalY, currentY-1, goalX, currentX-1,
              // depth+1 + abs(goalY-(currentY-1)) + abs(goalX-currentX));
        heap.push(new Node(currentX, currentY-1, maze[currentY-1][currentX],
          depth+1 + abs(goalY-(currentY-1)) + abs(goalX-currentX), depth+1, '^'));
      }
    }
    if(currentY<rows-1){
      if(maze[currentY+1][currentX]=='0' || maze[currentY+1][currentX]=='G'){
        // printf("d) pushing %d,%d with h=%d+(%d-%d)+(%d-%d)=%d\n", currentX, currentY+1,
              // depth+1, goalY, currentY+1, goalX, currentX,
              // depth+1 + abs(goalY-(currentY+1)) + abs(goalX-currentX));
        heap.push(new Node(currentX, currentY+1, maze[currentY+1][currentX],
          depth+1 + abs(goalY-(currentY+1)) + abs(goalX-currentX), depth+1, 'v'));
      }
    }
    min->visited = true;
    sleep(wait_seconds);
  }
  return 0;
}
