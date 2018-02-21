#include <iostream>
#include <cmath>


const int PLAYER =1;
const int COMPUTER=2;

int boardSize;
const int amountOnARow=3;




enum direction{ right , down, upRight, downRight};

struct move{
    int score;
    int index;
};



char getCharForBoard(int content);
void printBoard(const int*);
int minMax(int * board, int player, int depth);
void doUserMove(int *board);
bool moveIsValid(const int *board, int movespot);
int getValue(const int* board, int x , int y);
void printBoardWithIndexes(int *board);
int getWinner(int* board);
//move* getAvailableMoves(int * board , int * size);
int neighbourCount(int *board, int x, int y, direction dir, int count);
void doComputerTurn(int *board);


bool hasEmptySpot(int *board);

move bestmove;
int lowestDepth=10000;

int main() {
    boardSize = 0;
    while (boardSize <= 0 || boardSize > 4) {
        std::cout << "Hello! Boardsize (below 4)?" << std::endl;
        std::cin >> boardSize;
    }


    int board[boardSize * boardSize];
    for (int i = 0; i < boardSize * boardSize; ++i) {
        board[i] = 0;
    }

    //printBoard(board);

    bool  userTurn=true;

    while ( true ) {
        //std::cout<< std::endl;
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

    printBoard(board);
    return 0;
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
    int score = minMax(board, COMPUTER,9);
    std::cout << " SCORE: " << score << std::endl;
    *(board + bestmove.index)= COMPUTER;
}

int minMax(int *board, int whoseTurn, int depth) {
    if(depth< lowestDepth){
        std::cout << "DEPTH: "<< depth  <<std::endl;
        lowestDepth= depth;
    }
    if(depth==0){
        return 0;
    }

    int winner = getWinner(board);
    if (winner==PLAYER){
        return -10;
    }
    else if (winner==COMPUTER){
        return 10;
    }

    int count =0;

    for (int i = 0; i < boardSize * boardSize; ++i) {
        if( *(board +i) == 0){
            count++;
        }
    }
    move options [count];
    int optionIndex=0;
    for (int j = 0; j < boardSize * boardSize; ++j) {
        if ( *(board +j) ==0){
            options[optionIndex].index=j;
            optionIndex++;
        }
        options[optionIndex].score=0;
    }


    //tie
    if (count==0){
        return 0;
    }
    //loop over all available moves
    move currentBestMove;
    if(whoseTurn==PLAYER){
        currentBestMove= {100,0};
    }else{
        currentBestMove= {-100,0};
    }


    for (int i = 0; i < count; ++i) {
        move m = options[i];
        *(board + m.index) = whoseTurn;
        if(whoseTurn==PLAYER) {
             m.score = minMax(board, COMPUTER, depth -1);
            if(m.score< currentBestMove.score){
                currentBestMove= m;
            }

        }else{
            m.score = minMax(board, PLAYER, depth -1);
            if(m.score> currentBestMove.score){
                currentBestMove= m;
            }

        }
        //make the board location empty again.
        *(board + m.index) = 0;
    }
    bestmove= currentBestMove;
    return currentBestMove.score;

}




int getWinner(int* board) {
    for (int x = 0; x < boardSize; ++x) {
        for (int y = 0; y < boardSize; ++y) {
            if(getValue(board, x, y) ==0){
                continue;
            }

            for ( int dirInt = right; dirInt <= downRight; dirInt++ ) {
                direction dir = static_cast<direction >(dirInt);
                int winner = neighbourCount(board, x, y, dir, 1);
                if (winner!=-1){
                    return winner;
                }

            }
            
        }
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
    *(board+ moveSpot)= PLAYER;

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
    if (x>=boardSize || y>= boardSize){
        return -1;
    }
    return *(board + (y * boardSize) + x);
}

int neighbourCount(int *board, int x, int y, direction dir, int count) {
    int value = getValue(board, x,y);

    int newX;
    int newY;

    if(dir== right){
        newY= y;
        newX= x+1;
            }
    else if (dir==down){
        newY= y +1;
        newX= x;
    }

    else if (dir == downRight){
        newY= y+1;
        newX= x+1;
    }

    else if( dir == upRight){
        newY= y-1;
        newX= x+1;
    }

    int newValue= getValue(board, newX, newY);
    if (newValue!=0 && newValue==value){
        
        count++;
        if (count==amountOnARow){
            return value;
        }
        return neighbourCount(board,newX, newY, dir, count );
    }

    return -1;

}











