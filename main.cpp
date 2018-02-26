#include <iostream>
#include <cmath>
#include <vector>
#include <unordered_map>
#include "Utils.h"

const int PLAYER =1;
const int COMPUTER=2;

int boardSize;
const int amountOnARow=3;




enum CheckDirection{ right , down, upRight, downRight};

struct MoveStruct{
    int score;
    int index;
};

struct PosCheckStruct{
    bool horizontal;
    bool vertical;
    bool upRight;
    bool downRight;
};

struct position{
    short key;
    short x;
    short y;
    PosCheckStruct check;
};

void mainloop();

char getCharForBoard(int content);
void printBoard(const int*);
MoveStruct minMax(int * board, int player, int depth);
void doUserMove(int *board);
bool moveIsValid(const int *board, int movespot);
int getValue(const int* board, int x , int y);
void printBoardWithIndexes(int *board);
int getWinner(int* board);
int checkIfWonAtSpot(int *board, int x, int y, CheckDirection dir, int count);
void doComputerTurn(int *board);
int evalPlayer(int *board, int player);
std::vector<position> getTakenSpots(int * board, int player);
bool playerWon(int * board, int player);


bool hasEmptySpot(int *board);
int lowestDepth=10000;


int main() {
    boardSize = Utils::askUserForInt("Hi! What boardsize do you want?", 3, 4);

    bool exit = false;
    while (! exit){
        mainloop();
        exit = ! Utils::askUserForBool("Would you like to play another game?");
    }
    std::cout<< "Bye!"<<std::endl;
    return 0;
}


void mainloop() {
    //create the board array and init all values to zero
    //this array will keep what pieces are on the board.
    int board[boardSize * boardSize];
    for (int i = 0; i < boardSize * boardSize; ++i) {
        board[i] = 0;
    }


    bool userTurn=true;

    while ( true ) {
        if(getWinner(board)!=-1){
            std::cout<< getCharForBoard(getWinner(board)) << " Has won!" <<std::endl;
            break;
        }
        else if( !hasEmptySpot(board)) {
            std::cout << "It's a tie!" << std::endl;
            break;
        }

        if (userTurn) {
            std::cout << std::endl;
            doUserMove(board);
        }
        else{doComputerTurn(board);}
        userTurn=! userTurn;

    }
    std::cout << "Final board status: " << std::endl;
    printBoard(board);
}




bool hasEmptySpot(int *board) {
    for (int i = 0; i < boardSize * boardSize; ++i) {
        if( *(board +i) ==0){
            return true;
        }
    }
    return false;
}


void doComputerTurn(int *board) {
    lowestDepth= 100000;
    MoveStruct m= minMax(board, COMPUTER,6);
    printBoard(board);
    std::cout << " SCORE: " << m.score << std::endl;
    std::cout << " INDEX " << m.index <<std::endl;
    *(board + m.index)= COMPUTER;
}

MoveStruct minMax(int *board, int whoseTurn, int depth) {
    if(depth< lowestDepth){
        std::cout << "DEPTH: "<< depth  <<std::endl;
        lowestDepth= depth;
    }


    int winner = getWinner(board);
    if(winner==PLAYER){
        return {-32000 -depth ,-1};
    }
    else if(winner==COMPUTER){
        return {32000 +depth ,-1};
    }


    if(depth==0 ){
        int score = evalPlayer(board, whoseTurn);
        return {score, -1};
    }



    //find all the possible moves
    std::vector<MoveStruct> possiblemoves(0);
    for (int i = 0; i < boardSize * boardSize; ++i) {
        if( *(board +i) == 0){
            possiblemoves.push_back({0,i});
        }
    }

    //tie
    if (possiblemoves.empty() ){
        return {0,-1};

    }


    for (auto &possiblemove : possiblemoves) {
        //do the move
        *(board + possiblemove.index) = whoseTurn;

        int score;
        if(whoseTurn==PLAYER){
            score= minMax(board, COMPUTER, depth-1).score;
        }else{
            score = minMax(board, PLAYER, depth-1).score;
        }
        possiblemove.score= score;
        //undo the move
        *(board + possiblemove.index) = 0;
    }


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
    return bestMove;





}



int getWinner(int* board) {
    if(playerWon(board, COMPUTER)){
        return COMPUTER;
    }
    if(playerWon(board, PLAYER)){
        return PLAYER;
    }
    return -1;
}








void doUserMove(int *board) {

    int moveSpot=-1;

    while ( ! moveIsValid(board,moveSpot)){
        printBoard(board);
        std::cout << "Where would you like to place your next item?"<<std::endl;
        printBoardWithIndexes(board);
        std::cin >> moveSpot;
    }
     * (board+ moveSpot)= PLAYER;

}




void printBoardWithIndexes(int *board) {
    for (int i = 0; i < boardSize; ++i) {
        std::cout << "|";
        for (int j = 0; j < boardSize; ++j) {

            int index =(i * boardSize) + j;


            int length;
            if (index==-0){
                length=1;
            }
            else{
                length=  (int)(log10(index)+1);
            }


            int maxLength = (int) log10( boardSize * boardSize) +1;

            while (length>0 && length < maxLength){
                std::cout<< " ";
                length++;
            }

            std::cout << getCharForBoard( *(board +index)) << " :"  <<index;
            std::cout << "|";
        }
        std::cout << std::endl;
    }
}

bool moveIsValid(const int *board, int movespot) {
    return *(board +movespot)==0;
}




char getCharForBoard(int content) {
    switch (content){
        case 0:
            return ' ';
        case PLAYER:
            return 'X';
        case COMPUTER:
            return 'O';
        default:
            return 'e';
    }
}

void printBoard(const int * point) {
    int index =0;

    while (index < boardSize * boardSize){
        std::cout<< '|';
        std::cout<< getCharForBoard(  *(point +index) );
        index++;
        if (index %boardSize==0){
            std::cout<< "|" << std::endl;
        }
    }
}

int getValue(const int *board, int x, int y) {
    //index out of range
    if (x>=boardSize || y>= boardSize || x<0 || y<0){
        return -1;
    }
    return *(board + (y * boardSize) + x);
}

int checkIfWonAtSpot(int *board, int x, int y, CheckDirection dir, int count) {
    int value = getValue(board, x, y);

    int newX;
    int newY;

    if (dir == right) {
        newY = y;
        newX = x + 1;
    } else if (dir == down) {
        newY = y + 1;
        newX = x;
    } else if (dir == downRight) {
        newY = y + 1;
        newX = x + 1;
    } else if (dir == upRight) {
        newY = y - 1;
        newX = x + 1;
    }

    int newValue = getValue(board, newX, newY);
    if (newValue != 0 && newValue == value) {

        count++;
        if (count == amountOnARow) {
            return value;
        }
        return checkIfWonAtSpot(board, newX, newY, dir, count);
    }

    return -1;
}






int evalPlayer(int *board, int player) {


    //this is a list of only the indexes taken by the current player
    std::vector<position> takenSpots= getTakenSpots(board , player);

    //this will keep the positions on the board. The index is the same as the index in the normal board[]
    position boardPositions[boardSize * boardSize];

    for (auto pos : takenSpots) {
        boardPositions[pos.key]= pos;
    }

    //this will keep the amount of times the index of this array is found in the board for this player
    //for instance, if there are 3 rows of 2, counts[2] = 3
    int counts[boardSize+1];
    for (int &count : counts) {
        count =0;
    }


    for(position p: takenSpots) {
        int count=1;
        position current = p;

        //search for horizontal lines
        while ( !current.check.horizontal && getValue(board, current.x +1, current.y) == player){
            count++;
            current.check.horizontal=true;
            current = boardPositions[ current.key+1];
        }
        counts[count]++;


        count=1;
        current = p;
        //search for vertical lines
        while ( !current.check.vertical && getValue(board, current.x, current.y+1) == player){
            count++;
            current.check.vertical=true;
            current = boardPositions[ current.key + boardSize];
        }
        counts[count]++;


        count=1;
        current = p;
        //search for downright lines
        while ( !current.check.downRight  && getValue(board, current.x +1, current.y+1) == player){
            count++;
            current.check.downRight=true;
            current = boardPositions[ current.key + boardSize+1];
        }
        counts[count]++;



        count=1;
        current = p;
        //search for upright lines
        while ( !current.check.upRight && current.y -1 >= 0  && current.x +1 < boardSize && getValue(board, current.x+1, current.y-1) == player){
            count++;
            current.check.upRight=true;
            current = boardPositions[ current.key + boardSize-1];
        }
        counts[count]++;
    }



    int score = 10000 * counts[3] + 50 * counts[2] + counts[1];


    if(player==COMPUTER){
        //std::cout <<  "COMPUTER: score for the following board: "<< score << std::endl;
        //std::cout << "COUNTS: "<< counts[1] <<" " << counts[2]<< " " <<counts[3]<< std::endl;
        //printBoard(board);
        return score;
    }
    else{
        //std::cout << "PLAYER: score for the following board"<< -score << std::endl;
        //std::cout << "COUNTS: "<< counts[1] <<" " << counts[2]<< " " <<counts[3]<< std::endl;
        //printBoard(board);
        return -score;

    }

}

bool playerWon(int *board, int player) {
    int evaluation= evalPlayer(board, player);
    return evaluation>=10000 || evaluation <=-10000;
}


std::vector<position> getTakenSpots(int *board, int player) {
    std::vector<position> takenSpots(0);

    for (short x = 0; x < boardSize; ++x) {
        for (short y = 0; y < boardSize; ++y) {
            short index =  (y *  (short)boardSize) + x;

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
            }
        }
    }
    return takenSpots;
}















