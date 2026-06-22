#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <vector>
using namespace std;

class Position {
    private:
        int row;
        int col;
    public:
        Position(int x = 0, int y = 0)
        {
            row = x;
            col = y;
        }
        int getRow() const { return row; }
        int getCol() const { return col; }

        bool operator==(const Position& pos) const {
            return row == pos.row && col == pos.col;
        }

        Position operator+(const Position& pos) const {
            return Position(row + pos.row, col + pos.col);
        }
};

class Player {
    private:
        Position pos;
    public:
        Player (int row=0, int col=0){
            pos = Position(row, col);
        }
        Position getPos() const {
            return pos;
        }

        void move(Position delta) {
            pos = pos + delta;
        }

        void setPos(Position p) {
            pos = p;
        }

};

class Guard {
    private:
        Position pos;
        char direction;
    public:
        Guard (int row=0, int col=0, char dir='\0'){
            pos = Position(row, col);
            direction = dir;
        }
        Position getPos() const {
            return pos;
        }

        char getDirection() const {
            return direction;
        }

        void setDirection(char d) {
            direction = d;
        }

        void setPos(Position p) {
            pos = p;
        }
};

class Level {
    private:
        vector<string> map;
        Player player;
        vector<Guard> guards;
        Position goal;
        bool playerCaught = false;    
    public:
        //level functions
        Level() {}
        Level(vector<string> m) {
            map = m;
            for (int r = 0; r < map.size(); r++) {
                for (int c = 0; c < map[r].size(); c++) {

                    char tile = map[r][c];

                    if (tile == '@'){
                        player = Player(r, c);
                        map[r][c] = ' ';
                    }
                    else if (tile == '$'){
                        goal = Position(r, c);
                    }
                    else if (tile == '>' || tile == '<' || tile == '^' || tile == 'v'){
                        guards.push_back(Guard(r, c, tile));
                        map[r][c] = ' ';
                    }    
                }
            }
        } 
        void printLevel(){
            for (int r = 0; r < map.size(); r++) {
                for (int c = 0; c < map[r].size(); c++) {

                    Position p(r, c);

                    if (p == player.getPos()) {
                        cout << '@';
                    }
                    else {
                        bool foundGuard = false;

                        for (const Guard& g : guards) {
                            if (p == g.getPos()) {
                                cout << g.getDirection();
                                foundGuard = true;
                                break;
                            }
                        }

                        if (!foundGuard)
                            cout << map[r][c];
                    }
                }
                cout << '\n';
            }
        }
        
        
        
        void updateLevel() {
            moveGuards();
            checkForLoss();
        }
        //updates
        void checkForLoss() {
            // Player walked into a guard
            if (guardAt(player.getPos())) {
                playerCaught = true;
                return;
            }

            // Check if player is in front of a guard
            for (const Guard& g : guards) {

                Position delta = directionToDelta(g.getDirection());
                Position current = g.getPos() + delta;

                while (isMoveValid(current)) {
                    // Player spotted
                    if (current == player.getPos()) {
                        playerCaught = true;
                        return;
                    }
                    // Vision blocked by another guard
                    if (guardAt(current)) {
                        break;
                    }
                    current = current + delta;
                }
            }
        }
        void moveGuards(){
            for (Guard& g : guards) {
                Position delta = directionToDelta(g.getDirection());
                Position next = g.getPos() + delta;

                if (isMoveValid(next) && !guardAt(next)) {
                    g.setPos(next);
                } else {
                    // Change direction
                    if (g.getDirection() == '>'){
                        g.setDirection('<');
                    }    
                    else if (g.getDirection() == '<'){ 
                        g.setDirection('>');
                    }
                    else if (g.getDirection() == '^'){ 
                        g.setDirection('v');
                    }
                    else if (g.getDirection() == 'v'){ 
                        g.setDirection('^');
                    }
                    delta = directionToDelta(g.getDirection());
                    next = g.getPos() + delta;
                    g.setPos(next);
                }
            }
        }
        //helper functions
        Position directionToDelta(char dir) {
            if (dir == '>') return Position(0, 1);
            if (dir == '<') return Position(0, -1);
            if (dir == '^') return Position(-1, 0);
            return Position(1, 0);
        }
        bool playerWon() {
            return player.getPos() == goal;
        }
        bool playerLost(){
            return playerCaught;
        }
        bool guardAt(Position p) {
            for (const Guard& g : guards) {
                if (g.getPos() == p) {
                    return true;
                }
            }
            return false;
        }
        bool isMoveValid(Position next) {
            if (map[next.getRow()][next.getCol()] == '#') {
                return false;
            }
            return true;
        }
        bool movePlayer(Position delta){
            Position next = player.getPos() + delta;
            if (isMoveValid(next)) {
                player.move(delta);
                return true;
            }
            else {
                cout << "You can't move there!" << endl;
                return false;
            }
        }


};

class Game {
    private:
        Level CurrentLevel;
        bool stillPlaying = true;
        bool gameOver = false;
    public:
        vector<string> loadLevel(int levelNum) {
            if (levelNum == 1){
                return {
                "#########",
                "#@      #",
                "#       #",
                "#   >   #",
                "#       #",
                "#   $   #",
                "#########"
                };
            }
            else if (levelNum == 2){
                return {
                "#########",
                "#@      #",
                "#   <   #",
                "#   >   #",
                "#### ####",
                "#      $#",
                "#########"
                };
            }
            else if (levelNum == 3){
                return {
                "##############",
                "#     #      #",
                "#   ^ #      #",
                "#            #",
                "#     #      #",
                "# @   #  >   #",
                "#######      #",
                "#         ^  #",
                "#  $  ^      #",
                "#            #",
                "##############",
                };
            };
        
        }
        void mainMenu() {
            cout << "Welcome to 008-World's End!" << endl;
            
            while (stillPlaying) {
                gameOver=false;
                
                cout << "Select a Level:" << endl;
                cout << "1) First Light" << endl;
                cout << "2) Dark Time" << endl;
                cout << "3) World's End" << endl;
                cout << "4) Exit" << endl;
                string choice;
                cin >> ws;
                getline(cin, choice);
                choice = toLower(choice);
                if (choice == "1" || toLower(choice) == "first light") {
                    playLevel(1);
                }
                else if (choice == "2" || toLower(choice) == "dark time") {
                    playLevel(2);
                }
                else if (choice == "3" ||  toLower(choice) == "world's end") {
                    playLevel(3);
                }
                else if (choice == "4" || toLower(choice) == "exit") {
                    stillPlaying = false;
                    cout << "See you next time 008" << endl;
                }
                else {
                    cout << "Invalid choice. Please select a level." << endl;
                }
            }
        }
        string toLower(string s) {
            for (char &c : s){
                 c = tolower(c);
            };
            return s;
        }
        void playLevel(int levelNum){
            CurrentLevel = Level(loadLevel(levelNum));
            while(!gameOver){
                cout << "Map:" << endl;
                CurrentLevel.printLevel();
                cout << "Enter move (WASD): ";
                char move;
                cin >> move;

                Position delta;

                if (tolower(move) == 'w') {
                    delta = Position(-1, 0);
                } else if (tolower(move) == 's') {
                    delta = Position(1, 0);
                } else if (tolower(move) == 'a') {
                    delta = Position(0, -1);
                } else if (tolower(move) == 'd') {
                    delta = Position(0, 1);
                } else {
                    cout << "Invalid move. Use w/a/s/d to move." << endl;
                    continue;
                }
                
                if (!CurrentLevel.movePlayer(delta)) {
                    cout << "Invalid move. Try again." << endl;
                    continue;
                }
                CurrentLevel.updateLevel();
                if (CurrentLevel.playerLost()){
                    cout << "You were caught 008! Mission Over" << endl;
                    gameOver=true;
                }
                else if (CurrentLevel.playerWon()) {
                    cout << "Well done 008! You've completed the level!" << endl;
                    gameOver = true;
                }
            }
        }
};

int main() {
    Game game;
    game.mainMenu();
};
