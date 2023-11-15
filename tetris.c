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

char Display[24][21][2];

//char,col
char GridDisplay[20][10][2];

u_char Grid[40][10];

//is,x,y,col
char Block[221][4];

u_char BlockId = 0;
char Tetromino[7];

u_char key[2] = {0,0};

//up,down,left,right,drop,turnLeft,turnRight,hold,escape,select
u_char keymap[10][2];

//I,O,J,L,S,Z,T
char Rotation[7][4][4][2] = {
    {{{-2,0},{-1,0},{0,0},{1,0}},{{0,2},{0,1},{0,0},{0,-1}},{{-2,0},{-1,0},{0,0},{1,0}},{{0,2},{0,1},{0,0},{0,-1}}},
    {{{-1,1},{0,1},{-1,0},{0,0}},{{-1,1},{0,1},{-1,0},{0,0}},{{-1,1},{0,1},{-1,0},{0,0}},{{-1,1},{0,1},{-1,0},{0,0}}},
    {{{-1,0},{0,0},{1,0},{1,-1}},{{0,1},{0,0},{-1,0},{0,-1}},{{-1,1},{-1,0},{0,0},{1,0}},{{0,1},{1,1},{0,0},{0,-1}}},
    {{{-1,0},{0,0},{1,0},{-1,-1}},{{-1,-1},{0,1},{0,0},{0,-1}},{{ 1,-1},{-1,0},{0,0},{1,0}},{{0,1},{0,0},{0,-1},{1,-1}}},
    {{{0,0},{1,0},{-1,-1},{0,-1}},{{0,1},{0,0},{1,0},{1,-1}},{{0,0},{1,0},{-1,-1},{0,-1}},{{0,1},{0,0},{1,0},{1,-1}}},
    {{{-1,0},{0,0},{0,-1},{1,-1}},{{1,1},{0,0},{1,0},{0,-1}},{{-1,0},{0,0},{0,-1},{1,-1}},{{1,1},{0,0},{1,0},{0,-1}}},
    {{{-1,0},{0,0},{1,0},{0,-1}},{{0,1},{-1,0},{0,0},{0,-1}},{{0,1},{-1,0},{0,0},{1,0}},{{0,-1},{0,0},{1,0},{0,-1}}}
};

void resetDisplay(void);
void resetGridDisplay(void);
void resetGrid(void);
void resetBlock(void);
void createBlock(char,char,char);
void deleteBlock(u_char);
void resetTetromino(void);
void createTetromino(char,char,char);
void fallTetromino(void);
void moveTetromino(char,char);
char isTetrominoAtWall(void);
char isTetrominoAtBottom(void);
void printToGridDisplay(void);
void printGame(char,char,char,char,char);
void printDisplay(void);

int main() {
    srand(time(NULL));
}

void mainGame(void) {
    char score = 0;
    char nextBlock = rand()%7;
    char nextCol = rand()%7;
    char holdBlock = 0;
    char holdCol = 0;
    createTetromino(rand()%7,rand()%7,0);
    while (1) {
        nextBlock = rand()%7;
        nextCol = rand()%7;
        fallTetromino();
        printToGridDisplay();
        printGame(nextBlock,nextCol,score,holdBlock,holdCol);
        printDisplay();
    }
}

void resetDisplay(void) {
    char i;
    char j;
    for (i = 0; i < 24; i++) {
        for (j = 0; j < 21; j++) {
            Display[i][j][0] = ' ';
            Display[i][j][1] = 15;
        }
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
        createBlock(6+(Rotation[type][rotation][i][0]),21+(Rotation[type][rotation][i][1]),BlockColor[col]);
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

void printGame(char nextType,char nextCol,char score,char hold,char holdCol) {
    char i;
    char j;
    char text1[6] = "Tetris";
    char text2[5] = "score";
    char text3[4] = "next";
    char text4[4] = "hold";
    for (i = 0; i < 6; i++) {
        Display[0][i+2][0] = text1[i];
        Display[0][i+2][1] = 15;
    }
    for (i = 2; i < 24; i++) {
        for (j = 0; j < 12; j++) {
            if ((i == 2)||(i == 23)||(j == 0)||(j == 11)) {
                Display[i][j][0] = '%';
                Display[i][j][1] = 15;
            }
        }
    }
    for (i = 0; i < 20; i++) {
        for (j = 0; j < 10; j++) {
            Display[i+3][j+1][0] = GridDisplay[19-i][j][0];
            Display[i+3][j+1][1] = GridDisplay[19-i][j][1];
        }
    }
    for (i = 0; i < 5; i++) {
        Display[2][i+14][0] = text2[i];
        Display[2][i+14][1] = 15;
    }
    char scoreC[6] = {0,0,0,0,0,0};
    scoreC[0] = score%1000000/100000;
    scoreC[1] = score%100000/10000;
    scoreC[2] = score%10000/1000;
    scoreC[3] = score%1000/100;
    scoreC[4] = score%100/10;
    scoreC[5] = score%10;
    for (i = 0; i < 6; i++) {
        Display[3][i+14][0] = scoreC[i]+48;
        Display[3][i+14][1] = 15;
    }
    for (i = 0; i < 4; i++) {
        Display[5][i+15][0] = text3[i];
        Display[5][i+15][1] = 15;
    }
    for (i = 6; i < 12; i++) {
        for (j = 14; j < 20; j++) {
            if ((i == 6)||(i == 11)||(j == 14)||(j == 19)) {
                Display[i][j][0] = '%';
                Display[i][j][1] = 15;
            }
        }
    }
    for (i = 0; i < 4; i++) {
        Display[-Rotation[nextType][0][i][1]+9][Rotation[nextType][0][i][0]+17][0] = '#';
        Display[-Rotation[nextType][0][i][1]+9][Rotation[nextType][0][i][0]+17][1] = BlockColor[nextCol];
    }
    for (i = 0; i < 4; i++) {
        Display[14][i+15][0] = text4[i];
        Display[14][i+15][1] = 15;
    }
    for (i = 15; i < 21; i++) {
        for (j = 14; j < 20; j++) {
            if ((i == 6)||(i == 11)||(j == 14)||(j == 19)) {
                Display[i][j][0] = '%';
                Display[i][j][1] = 15;
            }
        }
    }
    if (hold != 0) {
        for (i = 0; i < 4; i++) {
            Display[-Rotation[hold-1][0][i][1]+9][Rotation[hold-1][0][i][0]+17][0] = '#';
            Display[-Rotation[hold-1][0][i][1]+9][Rotation[hold-1][0][i][0]+17][1] = BlockColor[holdCol];
        }
    }
}

void updateKeymap(void) {
}

void updateKey(void) {
    key[0] = 0;
    key[1] = 0;
    key[0] = getch();
    if ((key[0] == 0)||(key[0] == 224))
        key[1] = getch();
}

void printMainscreen(void) {
}

void printDisplay(void) {
    char x;
    char y;
    //cls
    for(y = 0; y < 24; y++) {
        for (x = 0; x < 21; x++) {
            printf("%c",Display[y][x]);
        }
        printf("\n");
    }
}