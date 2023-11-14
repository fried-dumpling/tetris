#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>

typedef unsigned char u_char;

void textColor(char colorNum) {
    int color = colorNum;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

//Dark Red, Dark Yellow, Yellow, Green, Sky Blue, Dark Blue, Dark Purple
char BlockColor[7] = {4,6,14,10,11,1,5};

//Null
char Display[2] = {0,1};

//char,col
char GridDisplay[20][10][2];

u_char Grid[40][10];

//is,x,y,col
char Block[221][4];

u_char BlockId = 0;
char Tetromino[7];

//I,O,J,L,S,Z,T
char Rotation[7][4][4][2] = {
    {{{2,0},{1,0},{0,0},{-1,0}},{{0,2},{0,1},{0,0},{0,-1}},{{2,0},{1,0},{0,0},{-1,0}},{{0,2},{0,1},{0,0},{0,-1}}},
    {{{-1,1},{0,1},{-1,0},{0,0}},{{-1,-1},{0,-1},{-1,0},{0,0}},{{-1,-1},{0,-1},{-1,0},{0,0}},{{-1,-1},{0,-1},{-1,0},{0,0}}},
    {{{-1,0},{0,0},{1,0},{1,1}},{{0,1},{0,0},{-1,0},{0,-1}},{{-1,1},{-1,0},{0,0},{1,0}},{{0,1},{1,1},{0,0},{0,-1}}},
    {{{-1,0},{0,0},{1,0},{-1,-1}},{{-1,-1},{0,1},{0,0},{0,-1}},{{ 1,-1},{-1,0},{0,0},{1,0}},{{0,1},{0,0},{0,-1},{1,-1}}},
    {{{0,0},{1,0},{-1,-1},{0,-1}},{{0,1},{0,0},{1,0},{1,-1}},{{0,0},{1,0},{-1,-1},{0,-1}},{{0,1},{0,0},{1,0},{1,-1}}},
    {{{-1,0},{0,0},{0,-1},{1,-1}},{{1,1},{0,0},{1,0},{0,-1}},{{-1,0},{0,0},{0,-1},{1,-1}},{{1,1},{0,0},{1,0},{0,-1}}},
    {{{-1,0},{0,0},{1,0},{0,-1}},{{0,1},{-1,0},{0,0},{0,-1}},{{0,1},{-1,0},{0,0},{1,0}},{{0,-1},{0,0},{1,0},{0,-1}}}
};

void resetGridDisplay(void);
void resetGrid(void);
void resetBlock(void);
void createBlock(char,char,char);
void deleteBlock(u_char);
void resetTetromino(void);
void createTetromino(char,char,char);
void fallTetromino(void);
void moveTetromino(char,char);char isTetrominoAtWall(void);
char isTetrominoAtBottom(void);
void printToGridDisplay(void);

int main() {
    srand(time(NULL));
    resetGridDisplay();
    resetGrid();
    resetBlock();
    resetTetromino();
    char i;
    char j;
    createTetromino(10,5,3);
    while (1) {
        fallTetromino();
        printToGridDisplay();
        for (i = 19; i > -1; i--) {
            for (j = 0; j < 10; j++) {
                textColor(GridDisplay[i][j][1]);
                printf("%c",GridDisplay[i][j][0]);
            }
            printf("\n");
        }
        printf("__________________\n");
        Sleep(1000);
    }
}

void resetGridDisplay(void) {
    char i;
    char j;
    for (i = 0; i < 20; i++) {
        for (j = 0; j < 10; j++) {
            GridDisplay[i][j][0] = ' ';
            GridDisplay[i][j][1] = 15;
        }
    }
}

void resetGrid(void) {
    char i;
    char j;
    for (i = 0; i < 40; i++) {
        for (j = 0; j < 10; j++) {
            Grid[i][j] = 220;
        }
    }
}

void updateGrid(void) {
    resetGrid();
    u_char i;
    for (i = 0; i < 220; i++) {
        if (Block[i][0])
            Grid[Block[i][2]][Block[i][1]] = i;
    }
}

void resetBlock(void) {
    u_char i;
    for (i = 0; i < 221; i++) {
        Block[i][0] = 0;
        Block[i][1] = 0;
        Block[i][2] = 0;
        Block[i][3] = 0;
    }
}

void resetTetromino(void) {
    Tetromino[0] = 0;
    Tetromino[1] = 0;
    Tetromino[2] = 0;
    Tetromino[3] = 0;
    Tetromino[4] = 0;
    Tetromino[5] = 0;
    Tetromino[6] = 0;
}

void createBlock(char x,char y,char col) {
    BlockId++;
    BlockId %= 220;
    Block[BlockId][0] = 1;
    Block[BlockId][1] = x;
    Block[BlockId][2] = y;
    Block[BlockId][3] = col;
    Grid[y][x] = BlockId;
}

void deleteBlock(u_char id) {
    Block[id][0] = 0;
    Block[id][1] = 0;
    Block[id][2] = 0;
    Block[id][3] = 0;
}

void createTetromino(char col,char type,char rotation) {
    char i;
    Tetromino[0] = 1;
    for (i = 0; i < 4; i++) {
        createBlock(6+(Rotation[type][rotation][i][0]),21+(Rotation[type][rotation][i][1]),col);
        Tetromino[i+1] = BlockId;
    }
    Tetromino[5] = type;
    Tetromino[6] = rotation;
} 

void fallTetromino(void) {
    char i;
    for (i = 1; i < 5; i++) {
        Block[Tetromino[i]][2]--;
    }
    updateGrid();
}

void moveTetromino(char rotation,char direction) {
    char i;
    for (i = 0; i < 4; i++) {
        Block[Tetromino[i+1]][1] = Rotation[Tetromino[5]][(Tetromino[6]+rotation)%4][i][0];
        Block[Tetromino[i+1]][2] = Rotation[Tetromino[5]][(Tetromino[6]+rotation)%4][i][1];
        Block[Tetromino[i+1]][1] += direction;
    }
    updateGrid();
}

char isTetrominoAtWall(void) {
    char ret = 0;
    char i;
    for (i = 1; i < 5; i++) {
        if ((Block[Tetromino[i]][1] == 0)||(Block[Tetromino[i]][1] == 9)) {
            ret = 1;
        }
    }
    return ret;
}

char isTetrominoAtBottom(void) {
    char ret = 0;
    char i;
    char j;
    for (i = 1; i < 5; i++) {
        if (Block[Tetromino[i]][2] == 0)
            ret = 1;
        for (j = 1; j < 5; j++) {
            if (Grid[Block[Tetromino[i]][2]-1][Block[Tetromino[i]][1]] != Tetromino[j])
                ret = 1;
        }
    }
    return ret;
}

void printToGridDisplay(void) {
    char i;
    char j;
    for (i = 0; i < 20; i++) {
        for (j = 0; j < 10; j++) {
            GridDisplay[i][j][0] = Block[Grid[i][j]][0]*('#'-' ')+' ';
            GridDisplay[i][j][1] = Block[Grid[i][j]][3];
        }
    }
}