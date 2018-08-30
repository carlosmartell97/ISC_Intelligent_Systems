#include <cstdio>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <utility>
#include <queue>
#include <chrono>

using namespace std;
using ns = chrono::nanoseconds;
using get_time = chrono::steady_clock;

class State {
public:
  vector< vector<int> > tiles;
  vector<string> children;
  string father;
  int x, y;
  string direction;

  State(string tiles, string direction){
    // printf("creating new State %s %s %s\n",
    //   tiles.substr(0,3).c_str(),
    //   tiles.substr(3,3).c_str(),
    //   tiles.substr(6,3).c_str()
    // );
    this->direction = direction;
    vector<int> row;
    for(int i=1; i<=9; i++){
      int number = tiles[i-1] - 48;
      if(number == 0){
        this->x = (i-1)%3;
        this->y = (i-1)/3;
        // printf("zero at x: %d,y: %d\n", this->x, this->y);
      }
      if(i%3==0){
        row.push_back(number);
        // printf("pushing %d, %d, %d\n", row[0], row[1], row[2]);
        this->tiles.push_back(row);
        row.clear();
      } else {
        // printf("pushing %d\n", number);
        row.push_back(number);
      }
    }
  }
};

vector< vector<int> > swap(vector< vector<int> > tiles,
                          int x1, int y1, int x2, int y2){
  // printf(" swap() %d:%d <-> %d:%d\n", x1, y1, x2, y2);
  int temp = tiles[y1][x1];
  tiles[y1][x1] = tiles[y2][x2];
  tiles[y2][x2] = temp;
  return tiles;
}

string tiles_to_string(vector< vector<int> > tiles){
  string tiles_string;
  for(int i=0; i<3; i++){
    for(int j=0; j<3; j++){
      tiles_string.append(to_string(tiles[i][j]));
    }
  }
  return tiles_string;
}

vector< pair<string, string> > possible_moves(State* state){
  vector< pair<string, string> > moves;
  if(state->x > 0){
    // printf("x > 0  LEFT\n");
    moves.push_back(
      make_pair(
        tiles_to_string(
          swap(
            state->tiles, state->x, state->y, state->x-1, state->y
          )
        ),
        "LEFT"
      )
    );
  }
  if(state->x < 2){
    // printf("x < 2  RIGHT\n");
    moves.push_back(
      make_pair(
        tiles_to_string(
          swap(
            state->tiles, state->x, state->y, state->x+1, state->y
          )
        ),
        "RIGHT"
      )
    );
  }
  if(state->y > 0){
    // printf("y > 0  UP\n");
    moves.push_back(
      make_pair(
        tiles_to_string(
          swap(
            state->tiles, state->x, state->y, state->x, state->y-1
          )
        ),
        "UP"
      )
    );
  }
  if(state->y < 2){
    // printf("y < 2  DOWN\n");
    moves.push_back(
      make_pair(
        tiles_to_string(
          swap(
            state->tiles, state->x, state->y, state->x, state->y+1
          )
        ),
        "DOWN"
      )
    );
  }
  return moves;
}

void print_tiles(vector< vector<int> > tiles){
  for(int y=0; y<3; y++){
    for(int x=0; x<3; x++){
      printf("[%d:%d %d] ", x, y, tiles[y][x]);
      if(tiles[y][x] == 0){
        printf("[ ] ");
      } else {
        printf("[%d] ", tiles[y][x]);
      }
    }
    printf("\n");
  }
}

void print_tiles(string tiles){
  for(int i=0; i<tiles.size(); i++){
    if(tiles[i] == '0'){
      printf("[ ] ");
    } else {
      printf("[%c] ", tiles[i]);
    }
    if((i+1)%3==0) printf("\n");
  }
}

int main(){
  auto start = get_time::now();
  printf("\n8 puzzle program\n\n");
  ifstream input_file("input.txt");
  int tile;
  vector<int> initial_tiles;
  string initial_tiles_text;
  while(input_file >> tile){
    initial_tiles.push_back(tile);
    initial_tiles_text.append(to_string(tile));
  }
  printf("initial tiles: \n");
  print_tiles(initial_tiles_text);
  printf("%s\n\n", initial_tiles_text.c_str());
  map<string, State*> tree;
  tree[initial_tiles_text] = new State(initial_tiles_text, "initial");
  // print_tiles(tree[initial_tiles_text]->tiles);

  printf("looking for solution...\n");
  queue<string> q;
  q.push(initial_tiles_text);
  while(!q.empty()){
    string next = q.front();
    q.pop();
    // printf("\n---------------\nlooking at:\n");
    printf(".");
    // print_tiles(next);
    vector< pair<string, string> > next_moves = possible_moves(tree[next]);
    // printf("\n");
    for(int i=0; i<next_moves.size(); i++){
      string next_move_tiles_text = next_moves[i].first;
      string next_move_direction = next_moves[i].second;
      // printf("\tpossible move %d, %s:\n", i+1, next_move_direction.c_str());
      // print_tiles(next_move_tiles_text);
      tree[next]->children.push_back(next_move_tiles_text);
      if(tree.find(next_move_tiles_text) == tree.end()){  // state has not been explored
        tree[next_move_tiles_text] = new State(next_move_tiles_text, next_move_direction);
        tree[next_move_tiles_text]->father = next;
        tree[next_move_tiles_text]->direction = next_move_direction;
        q.push(next_move_tiles_text);
      }
      if(next == "012345678"){
        printf("\n----------SOLUTION FOUND!----------\n\n");
        string father = "?";
        vector<string> solution;
        while(father != initial_tiles_text){
          solution.push_back(tree[next]->direction);
          father = tree[next]->father;
          next = father;
        }
        for(int i=solution.size()-1; i>=0; i--){
          printf("step #%d: %s\n", (int)solution.size()-i, solution[i].c_str());
        }
        printf("\nDEPTH of solution: %d\n", (int)solution.size()+1);
        printf("expanded visited nodes: %d\n", (int)tree.size());
        auto end = get_time::now();
        auto time_difference = end - start;
        printf("elapsed program execution time: %f seconds\n",
              chrono::duration_cast<ns>(time_difference).count()*1.0 / 1000000000
        );
        printf("if every node required 72 bytes, %d bytes would've been used\n",
              (int)tree.size()*72
        );
        return 0;
      }
    }
  }
  printf("\nNO SOLUTION was found... are you sure the input file is correct?\nYou must have entered an impossible configuration.\n");
  auto end = get_time::now();
  auto time_difference = end - start;
  printf("elapsed program execution time: %f seconds\n",
        chrono::duration_cast<ns>(time_difference).count()*1.0 / 1000000000
  );
  return 0;
}
