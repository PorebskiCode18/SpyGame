#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <fstream>

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
    bool numbered;
public:
    Door(int r = 0, int c = 0, int g = 0, bool n = false): Object(r, c) {
        group = g;
        cout << g << endl;
        open = false;
        numbered = n;
    }

    int getGroup() const { return group; }
    void setGroup(int g) {
        group = g;
    }
    bool isNumbered() const { return numbered; }

    bool isOpen() const { return open; }

    void toggle() {
        open = !open;
    }
};
class Switch : public Object {
private:
    int group;
    bool numbered;
public:
    Switch(int r = 0, int c = 0, int g = 0, bool n = false): Object(r, c) {
        group = g;
        numbered = n;
    }

    int getGroup() const { return group; }
    void setGroup(int g) {
        group = g;
    }
    bool isNumbered() const { return numbered; }
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
                        guards.push_back(Guard(r, c, tile, 0));
                        map[r][c] = ' ';
                    }
                    else if (tile == 'R' || tile == 'L' || tile == 'U' || tile == 'D'){
                        char dir;
                        if (tile == 'R') dir = '>';
                        if (tile == 'L') dir = '<';
                        if (tile == 'U') dir = '^';
                        if (tile == 'D') dir = 'v';
                        
                        guards.push_back(Guard(r, c, dir, 1));
                        map[r][c] = ' ';
                    }
                    else if (tile >= 'A' && tile <= 'Z') {
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
        //for loading levels with numbered doors and switches
        Level(vector<string> m, const vector<Door>& numD, const vector<Switch>& numS) {
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
                        guards.push_back(Guard(r, c, tile, 0));
                        map[r][c] = ' ';
                    }
                    else if (tile == 'R' || tile == 'L' || tile == 'U' || tile == 'D'){
                        char dir;
                        if (tile == 'R') dir = '>';
                        if (tile == 'L') dir = '<';
                        if (tile == 'U') dir = '^';
                        if (tile == 'D') dir = 'v';
                        
                        guards.push_back(Guard(r, c, dir, 1));
                        map[r][c] = ' ';
                    }
                    else if (tile >= 'A' && tile <= 'Z') {
                        doors.push_back(Door(r, c, tile - 'A'));
                        map[r][c] = ' ';
                    }
                    else if (tile == '|') {

                        for (const Door &d : numD) {

                            if (d.getPos() == Position(r, c)) {
                                doors.push_back(d);
                                break;
                            }
                        }

                        map[r][c] = ' ';
                    }
                    else if (tile >= 'a' && tile <= 'z') {
                        switches.push_back(Switch(r, c, tile - 'a'));
                        map[r][c] = ' ';
                    }
                    else if (tile == '*') {

                        for (const Switch &s : numS) {

                            if (s.getPos() == Position(r, c)) {
                                switches.push_back(s);
                                break;
                            }
                        }

                        map[r][c] = ' ';
                    }
                }
            }
        } 
        void printLevel(){
            // Print column numbers
            cout << " ";
            for (int c = 0; c < map[0].size(); c++) {
                cout << c % 10;
            }
            cout << endl;
            for (int r = 0; r < map.size(); r++) {
                // Print row number
                cout << r % 10;
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
                                        cout << "|";

                                    printed = true;
                                    break;
                                }
                            }

                            if (!printed) {

                                for (const Switch &s : switches) {

                                    if (s.getPos() == p) {

                                        cout << "*";

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
                    if (d->isNumbered()) {
                        cout << "Group: " << d->getGroup() << endl;
                    }
                    else {
                        cout << "Group: " << char('A' + d->getGroup()) << endl;
                    }
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
                    if (s->isNumbered()) {
                        cout << "Group: " << s->getGroup() << endl;
                    }
                    else {
                        cout << "Group: " << char('A' + s->getGroup()) << endl;
                    }
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
class CustomLevel {
    private:
        string levelName;
        vector<string> levelData;
        vector<Door> levelDoors;
        vector<Switch> levelSwitches;

    public:
        CustomLevel(string name,
                vector<string> data,
                vector<Door> doors = {},
                vector<Switch> switches = {})
    {
        levelName = name;
        levelData = data;
        levelDoors = doors;
        levelSwitches = switches;
    }

        string getName() const {
            return levelName;
        }

        vector<string> getData() const {
            return levelData;
        }
        vector<Door> getDoors() const {
            return levelDoors;
        }
        vector<Switch> getSwitches() const {
            return levelSwitches;
        }
};

class Game {
    private:
        Level CurrentLevel;
        bool stillPlaying = true;
        bool gameOver = false;
        vector<CustomLevel> customLevels;
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
                "#   U #   <  #",
                "#     A      #",
                "#     #      #",
                "# @   #  R   #",
                "#######BBBBBB#",
                "#    #    ^  #",
                "#  $ BU      #",
                "#    #       #",
                "##############",
                };
            };
        
        }
        void loadLevelFromFile(string filename,vector<string> &level,
            vector<Door> &numberedDoors,
            vector<Switch> &numberedSwitches) {
            if (filename.length() < 4 ||filename.substr(filename.length() - 4) != ".lvl")
                filename += ".lvl";

            ifstream fin(filename);

            if (!fin) {
                cout << "Unable to open level." << endl;
                return;
            }
            string word;

            while (fin >> word) {

                if (word == "MAP")
                    break;

                int row, col, group;

                fin >> row >> col >> group;

                if (word == "DOOR") {
                    numberedDoors.push_back(Door(row, col, group,true));
                }
                else if (word == "SWITCH") {
                    numberedSwitches.push_back(Switch(row, col, group,true));
                }
            }
            fin.ignore();
            string line;

            while (getline(fin, line)) {
                level.push_back(line);
            }

            fin.close();

        }
        void mainMenu() {
            cout << "Welcome to 008-World's End!" << endl;
            
            while (stillPlaying) {
                gameOver=false;
                
                cout << "Select a Level:" << endl;
                cout << "1) First Light" << endl;
                cout << "2) Dark Time" << endl;
                cout << "3) World's End" << endl;
                cout << "4) Create New Level" << endl;
                cout << "5) Load Level" << endl;
                cout << "6) Exit" << endl;
                if (!customLevels.empty()) {
                    cout << "Custom Levels:" << endl;
                    int customLevelCount = 0;
                    for (const CustomLevel& lvl : customLevels) {
                        cout << (customLevelCount + 7) << ") " << lvl.getName() << endl;
                        customLevelCount++;
                    }
                }
                
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
                else if (choice == "4" || toLower(choice) == "level editor") {
                    levelEditor();
                }
                else if (choice == "5" || toLower(choice) == "load level"){
                    cout << "Enter level filename: ";
                    string filename;
                    cin >> ws;
                    getline(cin, filename);
                    vector<string> level;
                    vector<Door> numberedDoors;
                    vector<Switch> numberedSwitches;

                    loadLevelFromFile(filename, level, numberedDoors, numberedSwitches);

                    if (!level.empty()) {

                        customLevels.push_back(
                            CustomLevel(filename, level, numberedDoors, numberedSwitches)
                        );
                    }
                }
                else if (choice == "6" || toLower(choice) == "exit") {
                    stillPlaying = false;
                    cout << "See you next time 008" << endl;
                }
                else {
                    // Check if it's a custom level number
                    try {
                        int choiceNum = stoi(choice);
                        if (choiceNum >= 7 && choiceNum < (7 + customLevels.size())) {
                            int customIndex = choiceNum - 7;
                            cout << endl;
                            cout << customLevels[customIndex].getName() << endl;
                            cout << "1) Play" << endl;
                            cout << "2) Edit" << endl;
                            cout << "3) Back" << endl;

                            int option;
                            cin >> option;

                            if (option == 1) {

                                playLevel(customIndex, true);

                            }
                            else if (option == 2) {

                                levelEditor(customIndex);

                            }
                        }
                        else {
                            cout << "Invalid choice. Please select a level." << endl;
                        }
                    } catch (...) {
                        cout << "Invalid choice. Please select a level." << endl;
                    }
                }
            }
        }
        string toLower(string s) {
            for (char &c : s){
                 c = tolower(c);
            };
            return s;
        }
        void printLevel(const vector<string>& level) {
            int rows = level.size();
            int cols = level[0].size();

            // Print column numbers
            cout << " ";
            for (int c = 0; c < cols; c++) {
                cout << c % 10;
            }
            cout << endl;

            // Print each row
            for (int r = 0; r < rows; r++) {
                cout << r %10;

                for (int c = 0; c < cols; c++) {
                    cout << level[r][c];
                }

                cout << endl;
            }
        }
        void saveLevel(string levelName, const vector<string>& level,const vector<Door>& numberedDoors,
               const vector<Switch>& numberedSwitches) {

            if (levelName.length() < 4 || levelName.substr(levelName.length() - 4) != ".lvl"){
                levelName += ".lvl";
            }
            ofstream fout(levelName);
            // Save numbered doors
            for (const Door &d : numberedDoors) {
                fout << "DOOR "
                    << d.getPos().getRow() << " "
                    << d.getPos().getCol() << " "
                    << d.getGroup() << endl;
            }

            // Save numbered switches
            for (const Switch &s : numberedSwitches) {
                fout << "SWITCH "
                    << s.getPos().getRow() << " "
                    << s.getPos().getCol() << " "
                    << s.getGroup() << endl;
            }

            fout << "MAP" << endl;
            for (const string& row : level) {
                fout << row << endl;
            }

            fout.close();

            cout << "Level saved successfully as " << levelName << "!" << endl;
        }
        void levelEditor(int customIndex = -1) {
            
            cout << "Welcome to the Level Editor!" << endl;

            string levelName;
            vector<string> Level;
            vector<Door> numberedDoors;
            vector<Switch> numberedSwitches;

            int rows, columns;

            if (customIndex == -1) {

                cout << "Enter level name: ";
                cin >> ws;
                getline(cin, levelName);

                cout << "Enter number of rows: ";
                cin >> rows;

                cout << "Enter number of columns: ";
                cin >> columns;

                Level = vector<string>(rows, string(columns, ' '));

                for (int r = 0; r < rows; r++) {
                    for (int c = 0; c < columns; c++) {

                        if (r == 0 || r == rows - 1 ||
                            c == 0 || c == columns - 1)
                            Level[r][c] = '#';
                    }
                }

                cout << "Level created successfully!" << endl;
            }
            else {

                levelName = customLevels[customIndex].getName();

                Level = customLevels[customIndex].getData();

                numberedDoors = customLevels[customIndex].getDoors();

                numberedSwitches = customLevels[customIndex].getSwitches();

                rows = Level.size();
                columns = Level[0].size();

                cout << "Editing " << levelName << endl;
            }
            
            bool inEditor = true;
            while (inEditor) {
                cout << levelName << ":" << endl;
                printLevel(Level);
                cout << "\nObjects:" << endl;
                cout << "1) Wall (#)" << endl;
                cout << "2) Empty Space" << endl;
                cout << "3) Player (@)" << endl;
                cout << "4) Goal ($)" << endl;
                cout << "5) Guard" << endl;
                cout << "6) Door" << endl;
                cout << "7) Switch" << endl;
                cout << "8) Inspect" << endl;
                cout << "9) Save & Quit" << endl;
                int choice;
                cout << "\nSelect object: ";
                cin >> choice;

                if (choice == 9) {
                    saveLevel(levelName, Level, numberedDoors, numberedSwitches);

                    if (customIndex == -1) {

                        customLevels.push_back(
                            CustomLevel(levelName,
                                        Level,
                                        numberedDoors,
                                        numberedSwitches));
                    }
                    else {

                        customLevels[customIndex] =
                            CustomLevel(levelName,
                                        Level,
                                        numberedDoors,
                                        numberedSwitches);
                    }

                    inEditor = false;
                    continue;
                }

                int row, col;
                cout << "Row: ";
                cin >> row;
                cout << "Column: ";
                cin >> col;

                if (row < 0 || row >= rows || col < 0 || col >= columns) {
                    cout << "Invalid position!" << endl;
                    continue;
                }
                switch (choice) {
                case 1:
                    Level[row][col] = '#';
                    break;

                case 2:
                    Level[row][col] = ' ';
                    break;
                case 3:
                    // Remove any existing player
                    for (int r = 0; r < rows; r++) {
                        for (int c = 0; c < columns; c++) {
                            if (Level[r][c] == '@'){
                                Level[r][c] = ' ';
                            }
                        }
                    }
                    Level[row][col] = '@';
                    break;
                case 4:
                    // Remove any existing goal
                    for (int r = 0; r < rows; r++)
                        for (int c = 0; c < columns; c++)
                            if (Level[r][c] == '$')
                                Level[r][c] = ' ';

                    Level[row][col] = '$';
                    break;
                case 5: {
                    int type;
                    char dir;

                    cout << "Guard Type" << endl;
                    cout << "0) Back and Forth" << endl;
                    cout << "1) Circular" << endl;
                    cin >> type;

                    cout << "Direction (>, <, ^, v): ";
                    cin >> dir;

                    if (type == 0) {
                        Level[row][col] = dir;
                    }
                    else {
                        if (dir == '>') Level[row][col] = 'R';
                        else if (dir == '<') Level[row][col] = 'L';
                        else if (dir == '^') Level[row][col] = 'U';
                        else if (dir == 'v') Level[row][col] = 'D';
                        else {
                            cout << "Invalid direction!" << endl;
                        }
                    }

                    break;
                }
                case 6: {
                    cout << "Group Type:" << endl;
                    cout << "1) Letter (A-Z)" << endl;
                    cout << "2) Number" << endl;

                    int groupType;
                    cin >> groupType;

                    if (groupType == 1) {

                        char group;

                        cout << "Door Group (A-Z): ";
                        cin >> group;

                        Level[row][col] = toupper(group);

                    }
                    else if (groupType == 2) {
                        int group;

                        cout << "Door Group Number: ";
                        cin >> group;

                        Level[row][col] = '|';

                        numberedDoors.push_back(Door(row, col, group,true));
                    }

                    break;
                }
                case 7: {

                    cout << "Group Type:" << endl;
                    cout << "1) Letter (A-Z)" << endl;
                    cout << "2) Number" << endl;

                    int groupType;
                    cin >> groupType;

                    if (groupType == 1) {

                        char group;

                        cout << "Switch Group (A-Z): ";
                        cin >> group;

                        Level[row][col] = tolower(group);

                    }
                    else if (groupType == 2) {

                        int group;

                        cout << "Switch Group Number: ";
                        cin >> group;

                        Level[row][col] = '*';

                        numberedSwitches.push_back(Switch(row, col, group,true));
                    }

                    break;
                }
                case 8: {      
                    

                    if (row < 0 || row >= rows ||
                        col < 0 || col >= columns) {
                        cout << "Invalid position!" << endl;
                        break;
                    }

                    char object = Level[row][col];

                    switch (object) {

                    case '#':
                        cout << "Wall" << endl;
                        break;

                    case ' ':
                        cout << "Empty Space" << endl;
                        break;

                    case '@':
                        cout << "Player Spawn" << endl;
                        break;

                    case '$':
                        cout << "Goal" << endl;
                        break;

                    case '>':
                        cout << "Guard" << endl;
                        cout << "Movement: Back and Forth" << endl;
                        cout << "Facing: Right" << endl;
                        break;

                    case '<':
                        cout << "Guard" << endl;
                        cout << "Movement: Back and Forth" << endl;
                        cout << "Facing: Left" << endl;
                        break;

                    case '^':
                        cout << "Guard" << endl;
                        cout << "Movement: Back and Forth" << endl;
                        cout << "Facing: Up" << endl;
                        break;

                    case 'v':
                        cout << "Guard" << endl;
                        cout << "Movement: Back and Forth" << endl;
                        cout << "Facing: Down" << endl;
                        break;

                    case 'R':
                        cout << "Guard" << endl;
                        cout << "Movement: Circular" << endl;
                        cout << "Facing: Right" << endl;
                        break;

                    case 'L':
                        cout << "Guard" << endl;
                        cout << "Movement: Circular" << endl;
                        cout << "Facing: Left" << endl;
                        break;

                    case 'U':
                        cout << "Guard" << endl;
                        cout << "Movement: Circular" << endl;
                        cout << "Facing: Up" << endl;
                        break;

                    case 'D':
                        cout << "Guard" << endl;
                        cout << "Movement: Circular" << endl;
                        cout << "Facing: Down" << endl;
                        break;

                    default:
                        if (object == '|') {

                            bool found = false;

                            for (const Door &d : numberedDoors) {
                                if (d.getPos() == Position(row, col)) {
                                    cout << "Door" << endl;
                                    cout << "Group: " << d.getGroup() << endl;
                                    found = true;
                                    break;
                                }
                            }

                            if (!found)
                                cout << "Unknown numbered door." << endl;
                        }
                        else if (object == '*') {

                            bool found = false;

                            for (const Switch &s : numberedSwitches) {
                                if (s.getPos() == Position(row, col)) {
                                    cout << "Switch" << endl;
                                    cout << "Group: " << s.getGroup() << endl;
                                    found = true;
                                    break;
                                }
                            }

                            if (!found)
                                cout << "Unknown numbered switch." << endl;
                        }
                        else if (isupper(object)) {
                            cout << "Door" << endl;
                            cout << "Group: " << object << endl;
                        }
                        else if (islower(object)) {
                            cout << "Switch" << endl;
                            cout << "Group: " << (char)toupper(object) << endl;
                        }
                        else {
                            cout << "Unknown Object" << endl;
                        }
                    }

                    break;
                
                }
                default:
                    cout << "Invalid choice!" << endl;
                }
                
            }
        }
        void playLevel(int levelNum){
            CurrentLevel = Level(loadLevel(levelNum));
            playLevelLoop();
        }

        void playLevel(int customIndex, bool isCustom) {
            if (!isCustom) {
                playLevel(customIndex); return;
            }
            CurrentLevel = Level( customLevels[customIndex].getData(), customLevels[customIndex].getDoors(), customLevels[customIndex].getSwitches() );
            playLevelLoop();
        }

        void playLevelLoop(){
            while(!gameOver){
                cout << "Map:" << endl;
                CurrentLevel.printLevel();
                cout << "Enter action (WASD, 'inspect'): ";
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
