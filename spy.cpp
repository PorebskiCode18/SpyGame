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
class Object {
    protected:
        Position pos;
    public:
        Object (int row=0, int col=0){
            pos = Position(row, col);
        }
        Position getPos() const {
            return pos;
        }

        void setPos(Position p) {
            pos = p;
        }
};
class Door : public Object {
private:
    int group;
    bool open;

public:
    Door(int r = 0, int c = 0, int g = 0): Object(r, c) {
        group = g;
        open = false;
    }

    int getGroup() const { return group; }

    bool isOpen() const { return open; }

    void toggle() {
        open = !open;
    }
};
class Switch : public Object {
private:
    int group;

public:
    Switch(int r = 0, int c = 0, int g = 0): Object(r, c) {
        group = g;
    }

    int getGroup() const { return group; }
};

class Player : public Object {
    public:
        Player (int row=0, int col=0) : Object(row, col) {}
        void move(Position delta) {
            pos = pos + delta;
        }
};

class Guard : public Object {
    private:
        char direction;
        int type;
    public:
        Guard (int row=0, int col=0, char dir='\0',int t = 0) : Object(row, col), direction(dir), type(t) {
            direction = dir;
            type = t;
        }

        char getDirection() const {
            return direction;
        }

        int getType() const {
            return type;
        }
        void setDirection(char d) {
            direction = d;
        }
};

class Level {
    private:
        vector<string> map;
        Player player;
        vector<Guard> guards;
        vector<Door> doors;
        vector<Switch> switches;
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
                        srand(time(0)); 
                        int randType = rand() % 2;
                        guards.push_back(Guard(r, c, tile, randType));
                        map[r][c] = ' ';
                    }else if (tile >= 'A' && tile <= 'Z') {
                        doors.push_back(Door(r, c, tile - 'A'));
                        map[r][c] = ' ';
                    }
                    else if (tile >= 'a' && tile <= 'z') {
                        switches.push_back(Switch(r, c, tile - 'a'));
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

                        if (!foundGuard) {

                            bool printed = false;

                            for (const Door &d : doors) {

                                if (d.getPos() == p) {

                                    if (d.isOpen())
                                        cout << ' ';
                                    else
                                        cout << char('A' + d.getGroup());

                                    printed = true;
                                    break;
                                }
                            }

                            if (!printed) {

                                for (const Switch &s : switches) {

                                    if (s.getPos() == p) {

                                        cout << char('a' + s.getGroup());

                                        printed = true;
                                        break;
                                    }
                                }
                            }

                            if (!printed)
                                cout << map[r][c];
                        }
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
                    activateSwitch(g.getPos());
                } else {
                    // Change direction
                    if (g.getType() ==0){
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
                    }else{
                        if (g.getDirection() == '>'){
                            g.setDirection('v');
                        }    
                        else if (g.getDirection() == '<'){ 
                            g.setDirection('^');
                        }
                        else if (g.getDirection() == '^'){ 
                            g.setDirection('>');
                        }
                        else if (g.getDirection() == 'v'){ 
                            g.setDirection('<');
                        }
                    }
                    delta = directionToDelta(g.getDirection());
                    next = g.getPos() + delta;
                    if (isMoveValid(next) && !guardAt(next)) {
                        g.setPos(next);
                        activateSwitch(g.getPos());
                    }

                    
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
        bool doorAt(Position p) {
            for (const Door &d : doors)
                if (d.getPos() == p)
                    return true;

            return false;
        }

        Door* getDoor(Position p) {
            for (Door &d : doors)
                if (d.getPos() == p)
                    return &d;

            return nullptr;
        }

        Switch* getSwitch(Position p) {
            for (Switch &s : switches)
                if (s.getPos() == p)
                    return &s;

            return nullptr;
        }
        bool isMoveValid(Position next) {
            if (map[next.getRow()][next.getCol()] == '#'){
                return false;
            }
            Door *d = getDoor(next);

            if (d != nullptr && !d->isOpen()){
                return false;
            }
            return true;
        }
        bool movePlayer(Position delta){
            Position next = player.getPos() + delta;
            if (isMoveValid(next)) {
                player.move(delta);
                activateSwitch(player.getPos());
                return true;
            }
            else {
                cout << "You can't move there!" << endl;
                return false;
            }
        }
        void inspectTiles(int row,int col){
            if (row >= 0 && row < map.size() && col >= 0 && col < map[0].size()) {
                char tile = map[row][col];
                Position p = Position(row, col);
                if (tile == '#') {
                    cout << "Inspection: Wall." << endl;
                }  else if (goal == p) {
                    cout << "Inspection: Mission Goal." << endl;
                } else if (player.getPos() == p) {
                    cout << "Inspection: Player (008)." << endl;
                } else if (guardAt(p)) {
                    for (const Guard &g : guards) {
                        if (g.getPos() == p) {
                            cout << "Inspection: Guard facing " << g.getDirection() << "." << endl;
                            if (g.getType() == 0)
                                cout << "Movement: Back-Forth (reverses direction when blocked)" << endl;
                            else
                                cout << "Movement: Circular (Clockwise turn when blocked)" << endl;

                            return;
                        }
                    }
                } else if (doorAt(p)) {

                    Door *d = getDoor(p);

                    cout << "Inspection: Door" << endl;
                    cout << "Group: " << char('A' + d->getGroup()) << endl;
                    cout << "State: ";

                    if (d->isOpen()){
                        cout << "Open" << endl;
                    }else{
                        cout << "Closed" << endl;
                    }
                }
                else if (getSwitch(p) != nullptr) {

                    Switch *s = getSwitch(p);

                    cout << "Inspection: Switch" << endl;
                    cout << "Group: " << char('A' + s->getGroup()) << endl;
                }else if (tile == ' ') {
                    cout << "Inspection: Empty space." << endl;
                }
            } else {
                cout << "Position out of bounds." << endl;
            }
        }
        void activateSwitch(Position p) {
            Switch *sw = getSwitch(p);
            if (sw == nullptr){
                return;
            };    
            for (Door &d : doors) {

                if (d.getGroup() == sw->getGroup())
                    d.toggle();
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
                "#@     a#",
                "#   <   #",
                "#   >   #",
                "####A####",
                "#    A $#",
                "#########"
                };
            }
            else if (levelNum == 3){
                return {
                "##############",
                "# a   #     b#",
                "#   ^ #   <  #",
                "#     A      #",
                "#     #      #",
                "# @   #  >   #",
                "#######BBBBBB#",
                "#    #    ^  #",
                "#  $ B^      #",
                "#    #       #",
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
                string input;
                cin >> input;
                input = toLower(input);

                if (input == "inspect") {
                    int row, col;
                    cout << "Enter row: ";
                    cin >> row;
                    cout << "Enter column: ";
                    cin >> col;
                    CurrentLevel.inspectTiles(row, col);
                    continue;
                }

                Position delta;
                if (input == "w") {
                    delta = Position(-1, 0);
                } else if (input == "s") {
                    delta = Position(1, 0);
                } else if (input == "a") {
                    delta = Position(0, -1);
                } else if (input == "d") {
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
