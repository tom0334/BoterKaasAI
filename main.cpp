#include <iostream>
#include <cmath>
#include <vector>
#include <unordered_map>


const int PLAYER =1;
const int COMPUTER=2;

int boardSize;
const int amountOnARow=3;




enum direction{ right , down, upRight, downRight};

struct move{
    int score;
    int index;
};

struct posChecker{
    bool horizontal;
    bool vertical;
    bool upRight;
    bool downRight;
};

struct position{
    short key;
    short x;
    short y;
    posChecker check;
};




char getCharForBoard(int content);
void printBoard(const int*);
move minMax(int * board, int player, int depth);
void doUserMove(int *board);
bool moveIsValid(const int *board, int movespot);
int getValue(const int* board, int x , int y);
void printBoardWithIndexes(int *board);
int getWinner(int* board);
//move* getAvailableMoves(int * board , int * size);
int checkIfWonAtSpot(int *board, int x, int y, direction dir, int count);
void doComputerTurn(int *board);
int evalPlayer(int *board, int player);
int eval(int * board);


bool hasEmptySpot(int *board);

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
    move m= minMax(board, COMPUTER,10);
    printBoard(board);
    std::cout << " SCORE: " << m.score << std::endl;
    *(board + m.index)= COMPUTER;
}

move minMax(int *board, int whoseTurn, int depth) {
    if(depth< lowestDepth){
        std::cout << "DEPTH: "<< depth  <<std::endl;
        lowestDepth= depth;
    }



    if(depth==0){
        int score = evalPlayer(board, whoseTurn);
        return {score, -1};
    }


    std::vector<move> possiblemoves(0);
    for (int i = 0; i < boardSize * boardSize; ++i) {
        if( *(board +i) == 0){
            possiblemoves.push_back({0,i});
        }
    }

    //tie
    if (possiblemoves.empty()){
        int score = evalPlayer(board, whoseTurn);
        return {score,-1};

    }


    for(int i=0; i< possiblemoves.size();i ++) {
        *(board +  possiblemoves.at(i).index) = whoseTurn;

        int score;
        if(whoseTurn==PLAYER){
            score= minMax(board, COMPUTER, depth-1).score;
        }else{
            score = minMax(board, PLAYER, depth-1).score;
        }
        possiblemoves.at(i).score= score;
        *(board + possiblemoves.at(i).index) = 0;
    }


    move bestMove{0,-1};
    if (whoseTurn == PLAYER) {
        int bestScore= INT32_MAX;
        for (move m : possiblemoves) {
            if(m.score< bestScore){
                bestMove= m;
                bestScore=m.score;
            }

        }
    }else{

        int bestScore= INT32_MIN;
        for (move m : possiblemoves) {
            if(m.score> bestScore){
                bestMove= m;
                bestScore=m.score;
            }

        }


    }


    return bestMove;





}




int getWinner(int* board) {
    for (int x = 0; x < boardSize; ++x) {
        for (int y = 0; y < boardSize; ++y) {
            if(getValue(board, x, y) ==0){
                continue;
            }

            for ( int dirInt = right; dirInt <= downRight; dirInt++ ) {
                direction dir = static_cast<direction >(dirInt);
                int winner = checkIfWonAtSpot(board, x, y, dir, 1);
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
    if (x>=boardSize || y>= boardSize || x<0 || y<0){
        return -1;
    }
    return *(board + (y * boardSize) + x);
}

int checkIfWonAtSpot(int *board, int x, int y, direction dir, int count) {
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
    position positionMap[boardSize * boardSize];
    std::vector<position> possibleMoves(0);

    for (short x = 0; x < boardSize; ++x) {
        for (short y = 0; y < boardSize; ++y) {
            short index =  (y *  (short)boardSize) + x;
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
                possibleMoves.push_back(thePos);
                positionMap[index] = thePos;
            }else {
                positionMap[index] = {-1, -1, -1, {true, true, true, true}};
            }
        }
    }

    int counts[10];
    for(int i=0; i< 10; i++){
        counts[i]=0;
    }


    for(position p: possibleMoves) {
        int count=1;
        position current = p;

        //search for horizontal lines
        while ( !current.check.horizontal && getValue(board, current.x +1, current.y) == player){
            count++;
            //current.check.horizontal=true;
            current = positionMap[ current.key+1];
        }
        counts[count]++;


        count=1;
        current = p;
        //search for vertical lines
        while ( !current.check.vertical && getValue(board, current.x, current.y+1) == player){
            count++;
            //current.check.vertical=true;
            current = positionMap[ current.key + boardSize];
        }
        counts[count]++;


        count=1;
        current = p;
        //search for downright lines
        while ( !current.check.downRight  && getValue(board, current.x +1, current.y+1) == player){
            count++;
            //current.check.downRight=true;
            current = positionMap[ current.key + boardSize+1];
        }
        counts[count]++;



        count=1;
        current = p;
        //search for upright lines
        while ( !current.check.upRight && current.y -1 >= 0  && current.x +1 < boardSize && getValue(board, current.x+1, current.y-1) == player){
            count++;
            //current.check.upRight=true;
            current = positionMap[ current.key + boardSize-1];
        }
        counts[count]++;
    }


    int score = 10000 * counts[3] + 100 * counts[2] + counts[1];
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

int eval(int *board) {
    int score = evalPlayer(board, PLAYER) + evalPlayer(board,COMPUTER);
    //std::cout<< "TOTAL SCORE: " <<score << std::endl;
    //printBoard(board);
    return score;

}
















