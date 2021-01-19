#include <iostream>
#include <stdlib.h>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <thread>
#include <cmath>

using namespace std;

int     SIM_TIME = 0;
double  INFECT_CHANCE = 12.8;
int     SEATS_COUNT = 382;

int     PERSON_COUNT = 299;
int     INFECTED_COUNT = 0;

int     SEATS_DISTANCE = 0;
int     DISTANCE = 0;
bool    MASKS = false;
int     SPREADER_COUNT = 1;
int     PLAY_LENGTH = 2; 

bool    EXPERIMENTAL_RESULTS = false;

class Person{
    private:
        bool isInfected;
        bool isSpreader;
        int seat;

    public: 
        Person(){
            this->isSpreader = false;
            this->isInfected = false;
        }

        bool getIsSpreader(){
            return this->isSpreader;
        }
        bool getIsInfected(){
            return this->isInfected;
        }
        int getSeat(){
            return this->seat;
        }

        void setIsSpreader(bool isSpreader){
            this->isSpreader = isSpreader;
        }
        void setIsInfected(bool isInfected){
            this->isInfected = isInfected;
        }
        void setSeat(int seat){
            this->seat = seat;
        }
};

class Field{
    private:
        int row;
        int col;
        Person *person;

    public:
        Field(){
            this->row = 0;
            this->col = 0;
            this->person = NULL;
        }

        Field(int row, int col){
            this->row = row;
            this->col = col;
            this->person = NULL;
        }

        int getRow(){
            return this->row;
        }
        int getCol(){
            return this->col;
        }
        Person* getPerson(){
            return this->person;
        }

        void setRow(int row){
            this->row = row;
        }
        void setCol(int col){
            this->col = col;
        }
        void setPerson(Person& person){
            this->person = &person;
        }
        bool isEmpty(){
            return (this->person == NULL);
        }
        void removePerson(){
            this->person = NULL;
        }
};

class Room{
    private:
        int height;
        int width;
        vector<vector<Field>> fields;

    public:
        Room(int height, int width){
            this->height = height;
            this->width = width;

            for(int i = 0; i < height ; i++){
                fields.push_back(vector<Field>());

                for(int y = 0; y < width; y++){
                    fields[i].push_back(Field(i, y));
                }              
            }  
        }
        int getWidth(){
            return this->width;
        }
        int getHeight(){
            return this->height;
        }
        vector<vector<Field>> getFields(){
            return this->fields;
        }

        void setHeight(int height){
            this->height = height;
        }
        void setWidth(int width){
            this->width = width;
        }
        void addRowToFieldsAt(int i, vector<Field> row){
            this->fields.insert(this->fields.begin() + i, row);
        }

        /* These function serve as a "shortcut" to maintain ease-of-access */
        Field getFieldAt(int row, int col){
            return this->fields[row][col];
        }
        Person* getPersonAt(int row, int col){
            return this->fields[row][col].getPerson();
        }
        bool isFieldEmptyAt(int row, int col){
            return this->fields[row][col].isEmpty();
        }
        bool isPersonAtSpreader(int row, int col){
            return this->fields[row][col].getPerson()->getIsSpreader();
        }
        void setRowOfFieldAt(int newRow, int row, int col){
            this->fields[row][col].setRow(newRow);
        }
        void setColOfFieldAt(int newCol, int row, int col){
            this->fields[row][col].setCol(newCol);
        }
        void setPersonAt(int row, int col, Person& person){
            this->fields[row][col].setPerson(person);
        }
        void clearFields(){
            this->fields.clear();
        }
        void removePersonFrom(int row, int col){
            return this->fields[row][col].removePerson();
        }
        void movePersonFromTo(int originRow, int originCol, int destRow, int destCol){
            Person* person = this->getPersonAt(originRow, originCol);
            this->removePersonFrom(originRow, originCol);
            this->setPersonAt(destRow, destCol, *person);
        }
        bool isAnyoneInRange(int currentRow, int currentCol, int range){
            int neighbourRow;
            int neighbourCol;
            for (int i = -range; i <= range; i++){
                neighbourRow = currentRow + i;

                if (neighbourRow > (this->getHeight() -1) || neighbourRow < 0){
                    continue;
                }
                for (int j = -range; j <= range; j++){
                    neighbourCol = currentCol + j;
            
                    if (neighbourCol > (this->getWidth() -1) || neighbourCol < 0 || (i == 0 && j == 0)){
                        continue;
                    }

                    if(!this->getFieldAt(neighbourRow, neighbourCol).isEmpty()){
                        return true;
                    }
                }
            }

            return false;
        }

};

int generateIntInRange(int min, int max){
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> distribution(min, max);
    return distribution(gen);
}

void assignSpreaders(vector<Person>& persons){
    for(int i = 0; i < SPREADER_COUNT; i++){
        persons[i].setIsSpreader(true);

        INFECTED_COUNT++;
    }
}

void assignPersonsRandomlyToRoom(Room& room, vector<Person>& persons){

    for(int i = 0; i < (int)persons.size(); i++){
        int row, col;
        do{
            row = rand() % room.getHeight();
            col = rand() % room.getWidth();
        }
        while(!room.getFieldAt(row, col).isEmpty() || room.isAnyoneInRange(row, col, DISTANCE));

        room.setPersonAt(row, col, persons[i]);
    }
}

void limitToBoundaries(int& n, int max){
    if (n < 0)
        n = 0;
    if (n > max)
        n = max;
}

void tryToInfectForEachSpreaderAround(int currentRow, int currentCol, Room room){
    int neighbourRow;
    int neighbourCol; 
    for (int i = -4; i <= 4; i++){
        neighbourRow = currentRow + i;

        if (neighbourRow > (room.getHeight() -1) || neighbourRow < 0){
            continue;
        }
        for (int j = -4; j <= 4; j++){
            neighbourCol = currentCol + j;
            
            if (neighbourCol > (room.getWidth() -1) || neighbourCol < 0 || (i == 0 && j == 0)){
                continue;
            }

            if (!room.isFieldEmptyAt(neighbourRow, neighbourCol)){
                if (room.isPersonAtSpreader(neighbourRow, neighbourCol)){ 
                    
                    int chance = (int)(INFECT_CHANCE * 100);   
                    if (abs(i) > 1 || abs(j) > 1){          
                        chance = 260;                        
                        if (abs(i) > 3 || abs(j) > 3){   
                            chance = chance / 2;          
                        }
                    }
                    if (MASKS){
                        int maskEffectivty = generateIntInRange(41, 53);
                        chance = (chance * 10) / maskEffectivty;       
                    }

                    if (generateIntInRange(1, 10000) <= chance){
                        room.getPersonAt(currentRow, currentCol)->setIsInfected(true);

                        INFECTED_COUNT++;
                    }
                }
            }   
        }
    }
}

void assignSeats(vector<Person>& persons, vector<int>& seats){
    
    for(Person& person : persons){
        int remaining = seats.size()-1;
        int index = generateIntInRange(0, remaining);

        person.setSeat(seats[index]);
        seats.erase(seats.begin() + index);
    }
}

void assignPersonsToHall(Room& balcony, Room& ground, vector<Person>& persons){
    for(Person& person : persons){

        if (person.getSeat() < (balcony.getWidth() * balcony.getHeight())){
            int seatRow = (int)person.getSeat() / balcony.getWidth();
            int seatCol = person.getSeat() - seatRow * balcony.getWidth();

            balcony.setPersonAt(seatRow, seatCol, person);
        }
        
        else{
            int tmpSeat = person.getSeat() - balcony.getWidth() * balcony.getHeight();
            int seatRow = (int)(tmpSeat / ground.getWidth());
            int seatCol = tmpSeat - seatRow * ground.getWidth();

            ground.setPersonAt(seatRow, seatCol, person);
        }
    }
}

void printStats(){
    cout    << "Simulation time: "<< SIM_TIME 
            << "\tInfected: " << INFECTED_COUNT 
            << "\tSusceptible:" << PERSON_COUNT - INFECTED_COUNT 
            << "\t\tPercentage: " << (double)INFECTED_COUNT / PERSON_COUNT * 100 << "%" << endl;
}

void printRoom(Room room, string name){
    cout << "=================== " << name << " ===================" << endl;
    for (vector<Field> row : room.getFields()){
        for (Field field : row){
            if(!field.isEmpty()){
                if(field.getPerson()->getIsSpreader()){
                    cout << "|O|";
                }
                else if (field.getPerson()->getIsInfected()){
                    cout << " # ";
                }
                else{
                    cout << " . ";
                }
            }
            else{
                cout << "   ";
            }
        }
        cout << endl;
    }
}

void movePersonToNewPosition(Field originField, Room& tmpRoom, Room waitingRoom){
    
    int newRow = originField.getRow() + generateIntInRange(-1,1);
    int newCol = originField.getCol() + generateIntInRange(-1,1);

    limitToBoundaries(newRow, waitingRoom.getHeight() - 1);
    limitToBoundaries(newCol, waitingRoom.getWidth() - 1);

    if (tmpRoom.isFieldEmptyAt(newRow, newCol)){
        tmpRoom.movePersonFromTo(originField.getRow(), originField.getCol(), newRow, newCol);

        if (tmpRoom.isAnyoneInRange(newRow, newCol, DISTANCE)){
            tmpRoom.movePersonFromTo(newRow, newCol, originField.getRow(), originField.getCol());
        }
    }
}

void executeBehavior(Room& room, Room& tmpRoom, bool movementAllowed){
    for (vector<Field>& row : room.getFields()){
        for (Field& field : row){ 

            if(!field.isEmpty()){
                if (!field.getPerson()->getIsSpreader() && 
                    !field.getPerson()->getIsInfected()){
                    tryToInfectForEachSpreaderAround(field.getRow(), field.getCol(), room);
                }                    

                if (movementAllowed){
                    movePersonToNewPosition(field, tmpRoom, room);
                }
            }
        }
    }
    if(EXPERIMENTAL_RESULTS){
        this_thread::sleep_for(chrono::milliseconds(25)); 
    }
}

void executeWaitingRoom(Room& room, vector<Person> persons, int timeLimit){

    int time = 0;
    Room tmpRoom = room;
    
    while(time < timeLimit){
        
        
        tmpRoom = room;
        executeBehavior(room, tmpRoom, true);
        room = tmpRoom;

        if (EXPERIMENTAL_RESULTS){
            cout << "\x1B[2J\x1B[H";
            printRoom(room, "WAITING ROOM");
        }

        printStats();
    
        time += 1; 
        SIM_TIME++;
    }
}

void executeHallRoom(Room& balcony, Room& ground, vector<Person> persons, int timeLimit){

    int time = 0;
    Room tmpBalcony = balcony;
    Room tmpGround = ground;
    
    while(time < timeLimit){

        tmpBalcony = balcony;
        executeBehavior(balcony, tmpBalcony, false);
        balcony = tmpBalcony;
        
        tmpGround = ground;
        executeBehavior(ground, tmpGround, false);
        ground = tmpGround;

        if (EXPERIMENTAL_RESULTS){
            cout << "\x1B[2J\x1B[H";
            printRoom(balcony, "BALCONY");
            printRoom(ground, "GROUND");
        }

        printStats();
        
        time += 1; 
        SIM_TIME++;
    }
}

void adjustGridToMatchLegSpace(Room& room){

    int originRowCount = room.getFields().size();

    vector<Field> emptyRow(room.getWidth());

    for(int i = 0; i < originRowCount; i++){
        room.addRowToFieldsAt(i * 2, emptyRow);
    }
    room.setHeight(room.getFields().size());

    for(int i = 0; i < (int)room.getFields().size(); i++){
        for(int j = 0; j < room.getWidth(); j++){
            room.setRowOfFieldAt(i, i, j);
            room.setColOfFieldAt(j, i, j);
        }
    }
}  

void checkArgs(int argc, char **argv){

    vector<string> args(argv, argv+argc);

  	for (int i = 1; i < (int)args.size(); ++i){
	    if (args[i] == "-help") {
            cout    << " -sd    Seats' spacing     " << endl
                    << " -d     Social distancing  " << endl
                    << " -p     Person count       " << endl
                    << " -m     Masks              " << endl
                    << " -i     Number of spreaders" << endl
                    << " -t     Play length        " << endl
                    << "=====================================================" << endl;
	    	exit(EXIT_SUCCESS);
	    } 
        // number of persons
	    else if (args[i] == "-p") {
            if ((i + 1) > (argc - 1)){
                cout << "ERROR: No value found for option \'-p\'."<< endl;
                exit(EXIT_FAILURE);
            }
            try{
                PERSON_COUNT = stoi(args[++i]);
            }
            catch(...){
                cout << "ERROR: Invalid int value: \'" << args[i] << "\' provided for option \'-p\'."<< endl;
	    		exit(EXIT_FAILURE);
            }
            if (PERSON_COUNT < 50 || PERSON_COUNT > 382){
                cout << "ERROR: Number of persons has to be a integer in range 50-382" << endl;
                exit(EXIT_FAILURE);
            }
	    }
	    // masks
	    else if (args[i] == "-m") {
	    	MASKS = true;
	    }
	    // seats' distance
	    else if (args[i] == "-sd") {
            if ((i + 1) > (argc - 1)){
                cout << "ERROR: No value found for option \'-sd\'."<< endl;
                exit(EXIT_FAILURE);
            }
            try{
                SEATS_DISTANCE = stoi(args[++i]);
            }
            catch(...){
                cout << "ERROR: Invalid int value: \'" << args[i] << "\' provided for option \'-sd\'."<< endl;
	    		exit(EXIT_FAILURE);
            }
            if (SEATS_DISTANCE < 0 || SEATS_DISTANCE > 2){
                cout << "ERROR: Maximum distance between seats is 2 and minimum is 0." << endl;
                exit(EXIT_FAILURE);
            }
	    }
	    // movement distance
	    else if (args[i] == "-d") {
	    	DISTANCE = 2;
	    }
	    // spreader count
	    else if(args[i] == "-i") {
            if ((i + 1) > (argc - 1)){
                cout << "ERROR: No value found for option \'-i\'."<< endl;
                exit(EXIT_FAILURE);
            }
	    	try {
	    		SPREADER_COUNT = stoi(args[++i]);
	    	}
	    	catch (...) 
	    	{
	    		cout << "ERROR: Invalid int value: \'" << args[i] << "\' provided for option \'-i\'."<< endl;
	    		exit(EXIT_FAILURE);
	    	}

	    	if (SPREADER_COUNT < 1 || SPREADER_COUNT > 10){
	    		cout << "ERROR: Maximum number of spreaders is 10 and minimum is 1." << endl;
	    		exit(EXIT_FAILURE);
	    	}
	    }	
	    // play length
	    else if(args[i] == "-t") {
            if ((i + 1) > (argc - 1)){
                cout << "ERROR: No value found for option \'-t\'."<< endl;
                exit(EXIT_FAILURE);
            }
	    	try {
	    		PLAY_LENGTH = stoi(args[++i]);
	    	}
	    	catch (...) {
	    		cout << "ERROR: Invalid int value: \'" << args[i] << "\' provided for option \'-t\'."<< endl;
	    		exit(EXIT_FAILURE);
	    	}

	    	if (PLAY_LENGTH < 1 || PLAY_LENGTH > 3){
	    		cout << "ERROR: Play duration must be between 1 and 3" << endl;
	    		exit(EXIT_FAILURE);
	    	}
	    }
        // experimental view of results
	    else if(args[i] == "-grid") {
            EXPERIMENTAL_RESULTS = true;
	    }
  	}
}

vector<int> createAvailableSeats(){
    vector<int> seats(0);
    for(int i = 0; i < SEATS_COUNT; i += (1 + SEATS_DISTANCE)){
        seats.push_back(i);
    }
    return seats;
}

int main(int argc, char **argv)
{
    srand(time(NULL));
    cout << "=====================================================" << endl;
    checkArgs(argc, argv);

    vector<int> seatNumbers = createAvailableSeats();
    Room waitingRoom = Room(46, 46);    //  46x46
    Room balcony = Room(7, 16);         //  7x14
    Room ground = Room(15, 18);         //  15x18

    // Check fatal minimum to successfull pseudo-random person generating across waiting room
    // Moore 5x5
    if (DISTANCE == 2){
        int maxPersonCount = waitingRoom.getHeight() * waitingRoom.getWidth() / pow((DISTANCE * 2 + 1), 2);
        if (PERSON_COUNT > maxPersonCount){
            cout << "Warning: Since you demanded -d, number of total people has been reduced to: " << maxPersonCount << endl;
            PERSON_COUNT =  maxPersonCount;
        }
    }

    // If number people is more than number of seats, adjust number of people
    if (PERSON_COUNT > (int)seatNumbers.size()){
        cout    << "Warning: you demanded -sd with certain value, number of total people has been reduced to: " << seatNumbers.size() << endl;
        PERSON_COUNT = (int)seatNumbers.size();
    }

    vector<Person> persons(PERSON_COUNT);

    cout    << " -sd    Seats' spacing:       \t" << SEATS_DISTANCE << (SEATS_DISTANCE == 0 ? "\t(default)":"") << endl
            << " -d     Social distancing:    \t" << (DISTANCE == 0 ? "False\t(default)":"True") << endl
            << " -p     Person count:         \t" << PERSON_COUNT << (PERSON_COUNT == 299 ? "\t(default)":"") << endl
            << " -m     Masks:                \t" << (MASKS ? "True" : "False\t(default)") << endl
            << " -i     Number of spreaders:  \t" << SPREADER_COUNT << (SPREADER_COUNT == 1 ? "\t(default)": "") << endl
            << " -t     Play length:          \t" << PLAY_LENGTH << (PLAY_LENGTH == 2 ? "\t(default)": "") << endl
            << "=====================================================" << endl;

    this_thread::sleep_for(chrono::milliseconds(2000)); 

    assignSpreaders(persons);
    assignSeats(persons, seatNumbers);
    
    assignPersonsToHall(balcony, ground, persons);
    assignPersonsRandomlyToRoom(waitingRoom, persons);

    adjustGridToMatchLegSpace(balcony);
    adjustGridToMatchLegSpace(ground);

    cout << endl << "=============== WAITING ROOM BEFORE PLAY STARTS ===============" << endl;
    executeWaitingRoom(waitingRoom, persons, 30);

    for (int i = 0; i < PLAY_LENGTH; i++){

        if (!EXPERIMENTAL_RESULTS){
            cout << "=============== BALCONY & GROUND DURING PLAY ===============" << endl;
        }
        
        executeHallRoom(balcony, ground, persons, 60);

        waitingRoom = Room(46, 46);

        assignPersonsRandomlyToRoom(waitingRoom, persons);

        if (!EXPERIMENTAL_RESULTS){
            cout << "=============== PAUSE IN WAITING ROOM ===============" << endl;
        }
        
        executeWaitingRoom(waitingRoom, persons, 15);
    }

    return 0;
}