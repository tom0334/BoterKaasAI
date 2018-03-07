#include <iostream>
#include <cmath>
#include <vector>
#include <unordered_map>
#include "Utils.h"
#include <chrono>
#include <string>

const short PLAYER =1;
const short COMPUTER=2;

//the size and the amount of spots on the board (width *height)
int width;
int height;
int BOARDSPOTS;

//the amount of items on a row a player must have
int amountOnARow;
//who gets to place the first item?
bool userStarts;
//If false, its regular tic tac toe. If true, it turns into connect 4
bool gravity;

//the possible directions in which you can get 3 on a row. ( left is the same as right  so you don't need to check)
enum CheckDirection{ right , down, upRight, downRight};

struct MoveStruct{
    int score;
    int index;
};

//this is for the evalution function. It uses it to keep track of what directions are already checked to avoid doing to many checks
struct PosCheckStruct{
    bool horizontal;
    bool vertical;
    bool upRight;
    bool downRight;

};
//the key in here is the index in the board array
struct position{
    short key;
    short x;
    short y;
    PosCheckStruct check;

/*    position(short key, short x, short y){
        this->key= key;
        this->x= x;
        this->y= y;
        this->check= {false, false , false , false};
    }*/

};

struct boardPos{
    short key;
    short x;
    short y;
    short val;

    boardPos(short key, short x, short y){
        this->key=key;
        this->x = x;
        this->y=y;
        this->val=0;
    }
};


void mainloop();
std::string getCharForBoard(int content);
void printBoard(std::vector<boardPos> *);
MoveStruct minMax(std::vector<boardPos> *board, int whoseTurn, int depth, int index, int prevScore);
void doUserMove(std::vector<boardPos> *board);
bool moveIsValid(std::vector<boardPos> *board, int movespot);
int getValue(std::vector<boardPos> *board, int x , int y);
int getValue(std::vector<boardPos> *board, int index);
void printBoardWithIndexes(std::vector<boardPos> *board);
int evalPlayer(std::vector<boardPos> *board, int player, int nIndex, int prevScore);
int getWinner(std::vector<boardPos> *board);
void doComputerTurn(std::vector<boardPos> *board);
inline std::vector<position> getTakenSpots(std::vector<boardPos> *board, int player);
bool playerWon(std::vector<boardPos> *board, int player, int lastIndex);
inline std::vector<MoveStruct> getPossibleMoves(std::vector<boardPos> *board);
bool hasEmptySpot(std::vector<boardPos> *board);
std::string hashKey(std::vector<boardPos> *board);
inline int getOtherPlayer(int player);
std::string addInBetweenLine(std::string strPntr);

//these are for statistics only
int callcount= 0;
int hashFound=0;

std::unordered_map<std::string,int> hashScores;


int main() {
    gravity= Utils::askUserForBool("Hi! Gravity?");

    if(gravity){
        width = Utils::askUserForInt("Width?", 3, 6);
        height = Utils::askUserForInt("Height?", 3, 6);
    }else{
        width = Utils::askUserForInt("Width?", 3, 5);
        height = Utils::askUserForInt("Height?", 3, 5);
    }
    BOARDSPOTS= width * height;

    amountOnARow= Utils::askUserForInt("How many on a row to win?",2, std::max(width, height));
    userStarts= Utils::askUserForBool("Do you want to start?");


    bool exit = false;
    while (! exit){
        mainloop();
        exit = ! Utils::askUserForBool("Would you like to play another game?");
    }
    std::cout<< "Bye!"<<std::endl;
    return 0;
}


void mainloop() {
    std::vector<boardPos>  board;
    board.reserve(BOARDSPOTS);
    for (int i = 0; i < BOARDSPOTS; ++i) {
        short x= i % width;
        short y= i / width;
        board.emplace_back(i,x,y);
    }
    auto boardPntr = &board;

    bool userTurn=userStarts;

    while ( true ) {
        if(getWinner(boardPntr)!=-1){
            std::cout<< getCharForBoard(getWinner(boardPntr)) << " Has won!" <<std::endl;
            break;
        }
        else if( !hasEmptySpot(boardPntr)) {
            std::cout << "It's a tie!" << std::endl;
            break;
        }

        if (userTurn) {
            std::cout << std::endl;
            doUserMove(boardPntr);
        }
        else{ doComputerTurn(boardPntr);
        }

        userTurn=! userTurn;
    }
    std::cout << "Final board status: " << std::endl;
    printBoard(boardPntr);
}




bool hasEmptySpot(std::vector<boardPos> *board) {
    for (int i = 0; i < BOARDSPOTS; ++i) {
        if( board->at(i).val ==0){
            return true;
        }
    }
    return false;
}


void doComputerTurn(std::vector<boardPos> *board) {
    //reset statistics
    callcount=0;
    hashFound=0;

    int depth;

    //if the board is small, we can afford to go the full depth and win or tie the game every time
    //it it isn't, this may take a lot of time so we will depend more on the evaluation function
    if(std::max(width, height)<5 || gravity){
        depth=9;
    }
    else {
        depth=6;
    }

    hashScores.clear();
    hashScores.reserve(15000000);

    auto start = std::chrono::high_resolution_clock::now();

    //Find the best move
    MoveStruct m= minMax(board, COMPUTER,depth,-1,0);

    auto finish = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = finish - start;

    //print statistics and the new board
    float calcountInMillions= (float) callcount /1000000;
    double callsPerSecond= (double) callcount / elapsed.count();
    std::cout << "SCORE: " << m.score << std::endl;
    std::cout << "INDEX " << m.index <<std::endl;
    std::cout << "Elapsed time: " << elapsed.count() << std::endl;
    std::cout << "Callcount: " << calcountInMillions << "M" <<std::endl;
    std::cout << "Calls per second: " << callsPerSecond << std::endl;
    std::cout << "HashMap size: " << hashScores.size()<< " Hashmoves found: "<< (float)hashFound/ (float)callcount <<  std::endl;


    //show the board before doing the move
    printBoard(board);
    //do the best move
    board->at(m.index).val= COMPUTER;

}



MoveStruct minMax(std::vector<boardPos> *board, int whoseTurn, int depth, int index, int prevScore) {
    callcount++;


    const int scoreOnThisLevel= evalPlayer(board, getOtherPlayer(whoseTurn),index,prevScore);

/*
    std::string key = hashKey(board);
    auto iter = hashScores.find(key);
    if (iter!= hashScores.end()){
        //std::cout<< "FOUND" << std::endl;
        hashFound++;
        return {iter->second ,index};
    }
*/


    //see if we have a winner already. if we do, we don't have to continue.
    if (whoseTurn==COMPUTER && scoreOnThisLevel> 10000){
        //std::cout<<"FOUND A WINNING BOARD. WINNER IS COMPUTER"<<std::endl;
        //printBoard(board);
        return {32000 +depth ,index};
    }
    else if (whoseTurn==PLAYER && scoreOnThisLevel < -10000){
        //std::cout<<"FOUND A WINNING BOARD. WINNER IS PLAYER"<<std::endl;
        //printBoard(board);
        return {-32000 -depth ,index};
    }


    //if we reach max depth, evaluate the board and return
    if(depth==0 ){
        return {scoreOnThisLevel, index};
    }


    //find all the possible moves
    std::vector<MoveStruct> possiblemoves= getPossibleMoves(board);


    //tie
    if (possiblemoves.empty() ){
        return {0,-1};

    }

    //try all the moves and save the scores it finds deeper in the search
    for (auto &possiblemove : possiblemoves) {
        //do the move
        board->at(possiblemove.index).val = whoseTurn;

        int score;
        if(whoseTurn==PLAYER){
            score= minMax(board, COMPUTER, depth-1, possiblemove.index, scoreOnThisLevel).score;
        }else{
            score = minMax(board, PLAYER, depth-1, possiblemove.index, scoreOnThisLevel).score;
        }
        possiblemove.score= score;
        //undo the move
        board->at(possiblemove.index).val = 0;
    }

    //find the highest or lowest (depending on player) score to return it
    MoveStruct bestMove{0,-1};
    if (whoseTurn == PLAYER) {
        int bestScore= INT32_MAX;
        for (MoveStruct m : possiblemoves) {
            if(m.score< bestScore){
                bestMove= m;
                bestScore=m.score;
            }

        }
    }else{

        int bestScore= INT32_MIN;
        for (MoveStruct m : possiblemoves) {
            if(m.score> bestScore){
                bestMove= m;
                bestScore=m.score;
            }

        }


    }
    //hashScores.insert({hashKey(board),bestMove.score});
    return bestMove;
}



int getWinner(std::vector<boardPos> *board) {
    for (auto pos : *board) {
        if( pos.val == COMPUTER && playerWon(board, COMPUTER, pos.key)){
            return COMPUTER;
        }
        if(pos.val == PLAYER && playerWon(board, PLAYER,pos.key)){
            return PLAYER;
        }
    }

    return -1;
}





void doUserMove(std::vector<boardPos> *board) {
    //wait for the user to input a valid move
    int moveSpot=-1;
    while ( ! moveIsValid(board,moveSpot)){
        printBoard(board);
        std::cout << "Where would you like to place your next item?"<<std::endl;
        printBoardWithIndexes(board);
        std::cin >> moveSpot;
    }
    //do the move
    board->at(moveSpot).val = PLAYER;
}



//this prints the board with the index numbers to ask the user to type where he wants to place its item
void printBoardWithIndexes(std::vector<boardPos> *board) {
    //the length the max index possible for this boardsize
    //with length of the number, i mean  as string. for example, 403 is 3 chars long
    int maxLength = (int) log10( BOARDSPOTS) +1;

    for (int x = 0; x < height; ++x) {
        std::cout << "|";
        for (int y = 0; y < width; ++y) {

            int index =(x * width) + y;


            int length;
            //log10 ( 0+1) doesnt work...
            if (index==0){
                length=1;
            }
            else{
                length=  (int)(log10(index)+1);
            }

            //print spaces for to align all items
            while (length>0 && length < maxLength){
                std::cout<< " ";
                length++;
            }

            std::cout << getCharForBoard( board->at(index).val) << " :"  <<index;
            std::cout << "|";
        }
        std::cout << std::endl;
    }
}

bool moveIsValid(std::vector<boardPos> *board, int movespot) {
    if(movespot<0 || movespot>= BOARDSPOTS){
        return false;
    }

    if(gravity){
        std::vector<MoveStruct> possibleMoves= getPossibleMoves(board);
        for (auto &move : possibleMoves){
            if ( movespot == move.index){
                return true;
            }
        }

        return false;
    }
    return board->at(movespot).val==0;
}




std::string getCharForBoard(int content) {
    switch (content){
        case 0:
            return " ";
        case PLAYER:
            return "❌";
        case COMPUTER:
            return "◉";
        default:
            return "e";
    }
}

void printBoard(std::vector<boardPos> *point) {

    std::string res;
    res = addInBetweenLine(res);
    for(int i=0;i<BOARDSPOTS; i++){
        res +="|";
        res = res + " " + getCharForBoard( point->at(i).val) + " ";
        if ( point->at(i).x == width-1 ){
            res+="|\n";
            res = addInBetweenLine(res);
        }
    }

    std::cout<< res<< std::endl;

}



int getValue(std::vector<boardPos> *board, int x, int y) {
    //index out of range
    if (x>=width || y>= height|| x<0 || y<0){
        return -1;
    }
    int index =(y * width) + x;
    return board->at(index).val;
}




bool playerWon(std::vector<boardPos> *board, int player, int lastIndex) {
    int evaluation= evalPlayer(board,player,lastIndex,0);
    return evaluation>=10000 || evaluation <=-10000;
}


inline std::vector<position> getTakenSpots(std::vector<boardPos> *board, int player) {
    std::vector<position> takenSpots;
    takenSpots.reserve(static_cast<unsigned long>(BOARDSPOTS));

    for (short x = 0; x < width; ++x) {
        for (short y = 0; y < height; ++y) {
            short index =  (y *  (short)width) + x;

            //check if this value is taken by the player.
            //if it is, put it in the takenspots and the boardpositions
            if(getValue(board,x,y)==player){

                position thePos= {
                        key: index,
                        x: x,
                        y :y,
                        check:{
                                false,
                                false,
                                false,
                                false
                        }
                };
                takenSpots.push_back(thePos);
                //takenSpots.emplace_back(index, x, y);
            }
        }
    }
    return takenSpots;
}

inline std::vector<MoveStruct> getPossibleMoves(std::vector<boardPos> *board) {
    std::vector<MoveStruct> possiblemoves;
    possiblemoves.reserve(BOARDSPOTS);

    if (gravity) {
        //if we play with gravity, only the top spot in every column is a valid move

        //loop over all columns and find the top empty spot
        for( int x= 0; x<width ; x++){
            //note the reversed order. Start at the bottom...
            for( int y = height; y>=0; y--){
                if(getValue(board, x, y) == 0 ){
                    int index= y * width + x;
                    possiblemoves.push_back( {0,index});
                    //We found the top spot. Continue with the other columns
                    break;
                }

            }
        }


    }else{
        //the possible moves are simply all empty spots
        for (int i = 0; i < BOARDSPOTS; ++i) {
            if ( board->at(i).val == 0) {
                possiblemoves.push_back({0, i});
            }
        }
    }


    return possiblemoves;
}

std::string hashKey(std::vector<boardPos> *board) {
    std::string code;
    code.reserve(BOARDSPOTS);
    for (int i = 0; i < BOARDSPOTS; ++i) {
        code+= std::to_string(board->at(i).val);
    }
    return code;
}

int evalPlayer(std::vector<boardPos> *board, int player, int nIndex, int prevScore) {

    if(nIndex==-1){ return 0;}
    //this will keep the amount of times the index of this array is found in the board for this player
    //for instance, if there are 3 rows of 2, counts[2] = 3
    int counts[10];
    for (int &count : counts) {
        count = 0;
    }


    //save the position we are checking now to
    int count = 1;
    boardPos current = board->at(nIndex);

    //search for horizontal lines
    while (getValue(board, current.x + 1, current.y) == player) {
        count++;
        current = board->at(current.key + 1);
    }
    current = board->at(nIndex);
    while (getValue(board, current.x - 1, current.y) == player) {
        count++;
        current = board->at(current.key - 1);
    }

    counts[count]++;

    count = 1;
    current = board->at(nIndex);
    //search for vertical lines
    while (getValue(board, current.x, current.y + 1) == player) {
        count++;
        current = board->at(current.key + width);
    }
    current = board->at(nIndex);
    while (getValue(board, current.x, current.y - 1) == player) {
        count++;
        current = board->at(current.key - width);
    }

    counts[count]++;


    count = 1;
    current = board->at(nIndex);
    //search for downright lines
    while (getValue(board, current.x + 1, current.y + 1) == player) {
        count++;
        current = board->at(current.key + width + 1);
    }
    current = board->at(nIndex);
    while (getValue(board, current.x - 1, current.y - 1) == player) {
        count++;
        current = board->at(current.key - width - 1);
    }
    counts[count]++;


    count = 1;
    current = board->at(nIndex);
    //search for upright lines
    while (getValue(board, current.x + 1, current.y - 1) == player) {
        count++;
        current = board->at(current.key - width + 1);
    }
    current = board->at(nIndex);
    while (getValue(board, current.x - 1, current.y + 1) == player) {
        count++;
        current = board->at(current.key + width - 1);
    }
    counts[count]++;


    int score = 200000 * counts[amountOnARow] + 100 * counts[3] + 10 * counts[2] + counts[1];


    //std::cout<< prevScore<< std::endl;
    if (player == COMPUTER) {
        //std::cout <<  "COMPUTER: score for the following board: "<< score << std::endl;
        //std::cout << "COUNTS: "<< counts[1] <<" " << counts[2]<< " " <<counts[3]<< std::endl;
        //printBoard(board);
        return score + prevScore;
    } else {
        //std::cout << "PLAYER: score for the following board"<< -score << std::endl;
        //std::cout << "COUNTS: "<< counts[1] <<" " << counts[2]<< " " <<counts[3]<< std::endl;
        //printBoard(board);
        return -score + prevScore;

    }
}

int getValue(std::vector<boardPos> *board, int index) {
    if (index>=0 && index< board->size()){
        return board->at(index).val;
    }
    return -1;
}

int getOtherPlayer(int player) {
    if (player== COMPUTER){
        return PLAYER;
    }
    return COMPUTER;
}

std::string addInBetweenLine(std::string res) {
    res+="+";
    for (int i = 0; i < width; ++i) {
        res+= "———+";
    }
    res+= "\n";
    return res;
}















