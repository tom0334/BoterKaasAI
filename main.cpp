#include <iostream>
#include <cmath>
#include <vector>
#include <unordered_map>
#include "Utils.h"
#include <chrono>

const int PLAYER =1;
const int COMPUTER=2;

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

void mainloop();
char getCharForBoard(int content);
void printBoard(const int*);
MoveStruct minMax(int * board, int player, int depth);
void doUserMove(int *board);
bool moveIsValid(const int *board, int movespot);
int getValue(const int* board, int x , int y);
void printBoardWithIndexes(int *board);
int getWinner(int* board);
void doComputerTurn(int *board);
int evalPlayer(int *board, int player);
inline std::vector<position> getTakenSpots(int * board, int player);
bool playerWon(int * board, int player);
inline std::vector<MoveStruct> getPossibleMoves(const int * board);
bool hasEmptySpot(int *board);

//these are for statistics only
int callcount= 0;


int main() {
    gravity= Utils::askUserForBool("Hi! Gravity?");

    if(gravity){
        width = Utils::askUserForInt("Width?", 3, 6);
        height = Utils::askUserForInt("Height?", 3, 6);
    }else{
        width = Utils::askUserForInt("Width?", 3, 4);
        height = Utils::askUserForInt("Height?", 3, 4);
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
    //create the board array and init all values to zero
    //this array will keep what pieces are on the board.
    //the values will be 0, COMPUTER or PLAYER
    int *board = (int*) (malloc (BOARDSPOTS * sizeof *board));
    for (int i = 0; i < BOARDSPOTS; ++i) {
        *(board + i)= 0;
    }


    bool userTurn=userStarts;

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
        else{ doComputerTurn(board);
        }

        userTurn=! userTurn;
    }
    std::cout << "Final board status: " << std::endl;
    printBoard(board);
}




bool hasEmptySpot(int *board) {
    for (int i = 0; i < BOARDSPOTS; ++i) {
        if( *(board +i) ==0){
            return true;
        }
    }
    return false;
}


void doComputerTurn(int *board) {
    //reset statistics
    callcount=0;

    int depth;

    //if the board is small, we can afford to go the full depth and win or tie the game every time
    //it it isn't, this may take a lot of time so we will depend more on the evaluation function
    if(std::max(width, height)==3 || gravity){
        depth=9;
    }
    else {
        depth=6;
    }


    auto start = std::chrono::high_resolution_clock::now();

    //Find the best move
    MoveStruct m= minMax(board, COMPUTER,depth);

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


    //show the board before doing the move
    printBoard(board);
    //do the best move
    *(board + m.index)= COMPUTER;

}



MoveStruct minMax(int *board, int whoseTurn, int depth) {
    callcount++;

    //see if we have a winner already. if we do, we don't have to continue.
    if (whoseTurn==COMPUTER && playerWon(board, COMPUTER)){
        return {32000 +depth ,-1};
    }
    else if (whoseTurn==PLAYER && playerWon(board, PLAYER)){
        return {-32000 -depth ,-1};
    }


    //if we reach max depth, evaluate the board and return
    if(depth==0 ){
        int score = evalPlayer(board, whoseTurn);
        return {score, -1};
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
    //wait for the user to input a valid move
    int moveSpot=-1;
    while ( ! moveIsValid(board,moveSpot)){
        printBoard(board);
        std::cout << "Where would you like to place your next item?"<<std::endl;
        printBoardWithIndexes(board);
        std::cin >> moveSpot;
    }
    //do the move
     * (board+ moveSpot)= PLAYER;

}



//this prints the board with the index numbers to ask the user to type where he wants to place its item
void printBoardWithIndexes(int *board) {
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

            std::cout << getCharForBoard( *(board +index)) << " :"  <<index;
            std::cout << "|";
        }
        std::cout << std::endl;
    }
}

bool moveIsValid(const int *board, int movespot) {
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

    while (index < BOARDSPOTS){
        std::cout<< '|';
        std::cout<< getCharForBoard(  *(point +index) );
        index++;
        if (index %width==0){
            std::cout<< "|" << std::endl;
        }
    }
}

int getValue(const int *board, int x, int y) {
    //index out of range
    if (x>=width || y>= height|| x<0 || y<0){
        return -1;
    }
    return *(board + (y * width) + x);
}



int evalPlayer(int *board, int player) {


    //this is a list of only the indexes taken by the current player
    std::vector<position> takenSpots= getTakenSpots(board , player);

    //this will keep the positions on the board. The index is the same as the index in the normal board[]
    std::vector<position> boardPositions;
    boardPositions.reserve(BOARDSPOTS);

    for (int i = 0; i < BOARDSPOTS; ++i) {
        boardPositions[i]= {};
    }

    for (auto pos : takenSpots) {
        boardPositions[pos.key]= pos;
    }

    //this will keep the amount of times the index of this array is found in the board for this player
    //for instance, if there are 3 rows of 2, counts[2] = 3
    int counts[ 10];
    for (int &count : counts) {
        count =0;
    }


    //look at all the taken spots to find if it is the start of a row
    for(position p: takenSpots) {
        int count=1;

        //save the position we are checking now to
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
            current = boardPositions[ current.key + width];
        }
        counts[count]++;


        count=1;
        current = p;
        //search for downright lines
        while ( !current.check.downRight  && getValue(board, current.x +1, current.y+1) == player){
            count++;
            current.check.downRight=true;
            current = boardPositions[ current.key + width+1];
        }
        counts[count]++;



        count=1;
        current = p;
        //search for upright lines
        while ( !current.check.upRight >= 0  && getValue(board, current.x+1, current.y-1) == player){
            count++;
            current.check.upRight=true;
            current = boardPositions[ current.key + width-1];
        }
        counts[count]++;
    }



    int score = 10000 * counts[amountOnARow] + 100* counts[3] + 10 * counts[2] + counts[1];


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


inline std::vector<position> getTakenSpots(int *board, int player) {
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

inline std::vector<MoveStruct> getPossibleMoves(const int *board) {
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
            if (*(board + i) == 0) {
                possiblemoves.push_back({0, i});
            }
        }
    }


    return possiblemoves;
}















