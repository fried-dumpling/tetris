#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>

typedef unsigned char u_char;

void cursorView()
{
    CONSOLE_CURSOR_INFO cursorInfo = { 0, };
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void textColor(char colorNum) {
    int color = colorNum;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

//Dark Red, Dark Yellow, Yellow, Green, Sky Blue, Dark Blue, Dark Purple
const char BlockColor[7] = {4,6,14,10,11,1,5};

char Display[24][21][2];
//char,col
char GridDisplay[20][10][2];
u_char Grid[40][10];

//is,x,y,col
char Block[221][4];
u_char BlockId = 0;
u_char Tetromino[8];

u_char key[2] = {0,0};
//up,down,left,right,drop,turnClock,turnCounterClock,hold,escape,select
u_char keymap[10][2] = {{224,72},{224,80},{224,75},{224,77},{224,80},{224,72},{122,0},{99,0},{27,0},{13,0}};
char isKeyKeymap(char keymapNum) {
    char ret = 0;
    if (key[0] == keymap[keymapNum][0]&&key[1] == keymap[keymapNum][1])
        ret = 1;
    return ret;
}

//I,O,J,L,S,Z,T
const char Rotation[7][4][4][2] = {
    {{{-2,0},{-1,0},{0,0},{1,0}},{{0,2},{0,1},{0,0},{0,-1}},{{-2,0},{-1,0},{0,0},{1,0}},{{0,2},{0,1},{0,0},{0,-1}}},
    {{{-1,1},{0,1},{-1,0},{0,0}},{{-1,1},{0,1},{-1,0},{0,0}},{{-1,1},{0,1},{-1,0},{0,0}},{{-1,1},{0,1},{-1,0},{0,0}}},
    {{{-1,0},{0,0},{1,0},{1,-1}},{{0,1},{0,0},{-1,-1},{0,-1}},{{-1,0},{-1,1},{0,0},{1,0}},{{0,1},{1,1},{0,0},{0,-1}}},
    {{{-1,0},{0,0},{1,0},{-1,-1}},{{-1,1},{0,1},{0,0},{0,-1}},{{1,1},{-1,0},{0,0},{1,0}},{{0,1},{0,0},{0,-1},{1,-1}}},
    {{{0,0},{1,0},{-1,-1},{0,-1}},{{0,1},{0,0},{1,0},{1,-1}},{{0,0},{1,0},{-1,-1},{0,-1}},{{0,1},{0,0},{1,0},{1,-1}}},
    {{{-1,0},{0,0},{0,-1},{1,-1}},{{1,1},{0,0},{1,0},{0,-1}},{{-1,0},{0,0},{0,-1},{1,-1}},{{1,1},{0,0},{1,0},{0,-1}}},
    {{{-1,0},{0,0},{1,0},{0,-1}},{{0,1},{-1,0},{0,0},{0,-1}},{{0,1},{-1,0},{0,0},{1,0}},{{0,1},{0,0},{1,0},{0,-1}}}
};

void resetDisplay(void);
void resetGridDisplay(void);
void resetGrid(void);
void resetBlock(void);
void createBlock(char,char,char);
void deleteBlock(u_char);
void resetTetromino(void);
void deleteTetromino(void);
void createTetromino(char,char,char);
void fallTetromino(void);
void moveTetromino(char);
void rotateTetromino(char);
char canTetrominoRotate(char);
char isTetrominoAtWall(void);
char isTetrominoAtBottom(void);
char clearRow(void);
void compressRow(void);
void printToGridDisplay(void);
void printGame(char,char,char,char,char,char);
void printDisplay(void);
void printMainScreen(char);
void printEscapeScreen(char);
void updateKey(void);
void resetKey(void);
void updateKeymap(char [],char [],char [],char [],char [],char [],char [],char [],char [],char []);
void mainGame(void);

int main() {
    srand(time(NULL));
    cursorView();
    resetBlock();
    resetTetromino();
    resetGrid();
    resetGridDisplay();
    resetDisplay();
    char select = 0;
    printMainScreen(0);
    printDisplay();
    while (1) {
        resetKey();
        updateKey();
        select += (isKeyKeymap(1)&&(select <= 4));
        select -= (isKeyKeymap(0)&&(select >= 0));
        select %= 4;
        printMainScreen(select);
        system("cls");
        //printf("\e[1;1H\e[2J");
        printDisplay();
        Sleep(100);
        if (select == 1&&isKeyKeymap(9)) {
            mainGame();
        }
        if (select == 2&&isKeyKeymap(9)) {}
        if (select == 3&&isKeyKeymap(9))
            break;
    }
}

void mainGame(void) {
    char i;
    resetDisplay();
    char score = 0;
    char nextBlock = 0;
    char nextCol = 0;
    char holdBlock = 0;
    char holdCol = 0;
    char holdRot = 0;
    char holdBlockT = 0;
    char holdColT = 0;
    char holdRotT = 0;
    char holded = 0;
    char fall = 0;
    char fallspeed = 10;
    char cancelMul = 3;
    createTetromino(rand()%7,rand()%7,0);
    char moveDir = 0;
    char bottomTime = 0;
    while (1) {
        resetKey();
        updateKey();
        if (fall==0 && isKeyKeymap(4)==0 && isTetrominoAtBottom()==0) {
            fallTetromino();
        }
        if (isKeyKeymap(4) && isTetrominoAtBottom()==0) {
            fallTetromino();
            fall = 1;
        }
        if (isKeyKeymap(2) && (isTetrominoAtWall()!=1)) {
            moveTetromino(-1);
            fall = (fall-(fallspeed/cancelMul) > 0)*(fall-(fallspeed/cancelMul)-1)+1;
            resetKey();
        }
        if (isKeyKeymap(3) && (isTetrominoAtWall()!=2)) {
            moveTetromino(1);
            fall = (fall-(fallspeed/cancelMul) > 0)*(fall-(fallspeed/cancelMul)-1)+1;
            resetKey();
        }
        if (isKeyKeymap(5)) {
            if (canTetrominoRotate(1)) {
                rotateTetromino(1);
                fall = (fall-(fallspeed/cancelMul) > 0)*(fall-(fallspeed/cancelMul)-1)+1;
            }
            resetKey();
        }
        if (isKeyKeymap(6)) {
            if (canTetrominoRotate(3)) {
                rotateTetromino(3);
                fall = (fall-(fallspeed/cancelMul) > 0)*(fall-(fallspeed/cancelMul)-1)+1;
            }
            resetKey();
        }
        if (isKeyKeymap(7)) {
            if (holded == 0) {
                holdBlockT = Tetromino[5]+1;
                holdRotT = Tetromino[6];
                for (i = 0; i < 7; i++) {
                    if (BlockColor[i] == Block[Tetromino[1]][3])
                        holdColT = i;
                }
                deleteTetromino();
                if (holdBlock != 0) {
                    createTetromino(holdCol,holdBlock-1,holdRot);
                }
                holdBlock = holdBlockT;
                holdCol = holdColT;
                holdRot = holdRotT;
                holded = 1;
            }   
        }
        if (isTetrominoAtBottom()) {
            bottomTime++;
        }
        else {
            bottomTime = 0;
        }
        if (bottomTime >= fallspeed) {
            resetTetromino();
            holded = 0;
            score += clearRow();
            compressRow();
            createTetromino(nextCol,nextBlock,0);
            nextBlock = rand()%7;
            nextCol = rand()%7;
            bottomTime = 0;
        }
        printToGridDisplay();
        printGame(nextBlock,nextCol,score,holdBlock,holdCol,holdRot);
        system("cls");
        //printf("\e[1;1H\e[2J");
        printDisplay();
        Sleep(17);
        fall++;
        fall %= fallspeed;
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
    Tetromino[7] = 1;
}

void deleteTetromino(void) {
    char i;
    for (i = 1; i < 5; i++) {
        deleteBlock(Tetromino[i]);
    }
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
        if (Rotation[type][rotation][i][0] == 0 && Rotation[type][rotation][i][1] == 0)
            Tetromino[7] = i+1;
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

void moveTetromino(char direction) {
    char i;
    for (i = 0; i < 4; i++) {
        Block[Tetromino[i+1]][1] += direction;
    }
    updateGrid();
}

void rotateTetromino(char rotation) {
    char i;
    char tempX = 0;
    char tempY = 0;
    tempX = Block[Tetromino[Tetromino[7]]][1];
    tempY = Block[Tetromino[Tetromino[7]]][2];
    for (i = 0; i < 4; i++) {
        Block[Tetromino[i+1]][1] = Rotation[Tetromino[5]][(Tetromino[6]+rotation)%4][i][0]+tempX;
        printf("%d\n",Rotation[Tetromino[5]][(Tetromino[6]+rotation)%4][i][0]);
        Block[Tetromino[i+1]][2] = Rotation[Tetromino[5]][(Tetromino[6]+rotation)%4][i][1]+tempY;
        printf("%d\n",Rotation[Tetromino[5]][(Tetromino[6]+rotation)%4][i][1]);
        if (Rotation[Tetromino[5]][(Tetromino[6]+rotation)%4][i][0] == 0 && Rotation[Tetromino[5]][(Tetromino[6]+rotation)%4][i][1] == 0)
            Tetromino[7] = i+1;
    }
    Tetromino[6] = (Tetromino[6]+rotation)%4;
    updateGrid();
}

char canTetrominoRotate(char rotation) {
    char i;
    char x = 0;
    char y = 0;
    char ret = 1;
    for (i = 0; i < 4; i++) {
        x = Rotation[Tetromino[5]][(Tetromino[6]+rotation)%4][i][0]+Block[Tetromino[Tetromino[7]]][1];
        y = Rotation[Tetromino[5]][(Tetromino[6]+rotation)%4][i][1]+Block[Tetromino[Tetromino[7]]][2];
        if (0>x||x>9 || 0>y)
            ret = 0;
    }
    return ret;
}

char isTetrominoAtWall(void) {
    char ret = 0;
    char i;
    char j;
    char or = 0;
    for (i = 1; i < 5; i++) {
        or = 1;
        if ((Block[Tetromino[i]][1] == 0)) {
            ret = 1;
        }
        else {
            for (j = 1; j < 5; j++) {
                if (Grid[Block[Tetromino[i]][2]][Block[Tetromino[i]][1]-1] == Tetromino[j])
                    or = 0;
            }
            if (or && Block[Grid[Block[Tetromino[i]][2]][Block[Tetromino[i]][1]-1]][0])
                ret = 1;
        }
        or = 1;
        if ((Block[Tetromino[i]][1] == 9)) {
            ret = 2;
        }
        else {
            for (j = 1; j < 5; j++) {
                if (Grid[Block[Tetromino[i]][2]][Block[Tetromino[i]][1]+1] == Tetromino[j])
                    or = 0;
            }
            if (or && Block[Grid[Block[Tetromino[i]][2]][Block[Tetromino[i]][1]+1]][0])
                ret = 2;
        }
    }
    return ret;
}

char isTetrominoAtBottom(void) {
    char ret = 0;
    char or = 0;
    char i;
    char j;
    for (i = 1; i < 5; i++) {
        or = 1;
        if (Block[Tetromino[i]][2] == 0)
            ret = 1;
        for (j = 1; j < 5; j++) {
            if (Grid[Block[Tetromino[i]][2]-1][Block[Tetromino[i]][1]] == Tetromino[j])
                or = 0;
        }
        if (or && Block[Grid[Block[Tetromino[i]][2]-1][Block[Tetromino[i]][1]]][0])
            ret = 1;
    }
    return ret;
}

char clearRow(void) {
    char i;
    char j;
    char lineBlock = 0;
    char line = 0;
    for (i = 0; i < 40; i++) {
        lineBlock = 0;
        for (j = 0; j < 10; j++) {
            lineBlock += Block[Grid[i][j]][0];
        }
        if (lineBlock == 10) {
            for (j = 0; j < 10; j++) {
                deleteBlock(Grid[i][j]);
            }
            line++;
        }
    }
    return line;
}

void compressRow(void) {
    char i;
    char j;
    char k;
    char l;
    char lineBlock = 0;
    for (i = 39; i >= 0; i--) {
        lineBlock = 0;
        for (j = 0; j < 10; j++) {
            lineBlock += Block[Grid[i][j]][0];
        }
        if (lineBlock == 0) {
            for (k = i+1; k < 40; k++) {
                for (l = 0; l < 10; l++) {
                    Block[Grid[k][l]][2]--;
                }
            }
        }
    }
    updateGrid();
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

void printGame(char nextType,char nextCol,char score,char hold,char holdCol,char holdRot) {
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
        for (j = 0; j < 4; j++) {
            Display[i+7][j+15][0] = ' ';
            Display[i+7][j+15][1] = 15;
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
            if ((i == 15)||(i == 20)||(j == 14)||(j == 19)) {
                Display[i][j][0] = '%';
                Display[i][j][1] = 15;
            }
        }
    }
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            Display[i+16][j+15][0] = ' ';
            Display[i+16][j+15][1] = 15;
        }
    }
    if (hold != 0) {
        for (i = 0; i < 4; i++) {
            Display[-Rotation[hold-1][holdRot][i][1]+18][Rotation[hold-1][holdRot][i][0]+17][0] = '#';
            Display[-Rotation[hold-1][holdRot][i][1]+18][Rotation[hold-1][holdRot][i][0]+17][1] = BlockColor[holdCol];
        }
    }
}

void updateKeymap(char up[],char down[],char left[],char right[],char drop[],char turnClock[],char turnCounterClock[],char hold[],char escape[],char select[]) {
    char i;
    keymap[i][0] = up[0];
    keymap[i][1] = up[1];
    keymap[i][0] = down[0];
    keymap[i][1] = down[1];
    keymap[i][0] = left[0];
    keymap[i][1] = left[1];
    keymap[i][0] = right[0];
    keymap[i][1] = right[1];
    keymap[i][0] = drop[0];
    keymap[i][1] = drop[1];
    keymap[i][0] = turnClock[0];
    keymap[i][1] = turnClock[1];
    keymap[i][0] = turnCounterClock[0];
    keymap[i][1] = turnCounterClock[1];
    keymap[i][0] = hold[0];
    keymap[i][1] = hold[1];
    keymap[i][0] = escape[0];
    keymap[i][1] = escape[1];
    keymap[i][0] = select[0];
    keymap[i][1] = select[1];
}

void updateKey(void) {
    if (kbhit() != 0) {
        key[0] = getch();
        if ((key[0] == 0)||(key[0] == 224))
            key[1] = getch();
    }
}

void resetKey(void) {
    key[0] = 0;
    key[1] = 0;
}

void printMainScreen(char select) {
    char text1[6] = "Tetris";
    char text2[6] = "[Play]";
    char text3[6] = "[Load]";
    char text4[6] = "[Quit]";
    char i;
    for (i = 0; i < 6; i++) {
        Display[0][i][0] = text1[i];
        Display[0][i][1] = BlockColor[i];
        Display[2][i][0] = text2[i];
        Display[2][i][1] = (select==1)*(-5)+15;
        Display[3][i][0] = text3[i];
        Display[3][i][1] = (select==2)*(-5)+15;
        Display[4][i][0] = text4[i];
        Display[4][i][1] = (select==3)*(-5)+15;
    }
}

void printEscapeScreen(char select) {
    char text1[4] = "Menu";
    char text2[10] = "[Continue]";
    char text3[6] = "[Load]";
    char text4[8] = "[Option]";
    char i;
    for (i = 0; i < 4; i++) {
        Display[0][i][0] = text1[i];
        Display[0][i][1] = 15;
    }
    for (i = 0; i < 10; i++) {
        Display[1][i][0] = text2[i];
        Display[1][i][1] = (select==1)*(-5)+15;
    }
    for (i = 0; i < 6; i++) {
        Display[2][i][0] = text3[i];
        Display[2][i][1] = (select==2)*(-5)+15;
    }
    for (i = 0; i < 8; i++) {
        Display[3][i][0] = text4[i];
        Display[3][i][1] = (select==3)*(-5)+15;
    }
}

void printDisplay(void) {
    char x;
    char y;
    for(y = 0; y < 24; y++) {
        for (x = 0; x < 21; x++) {
            textColor(Display[y][x][1]);
            printf("%c",Display[y][x][0]);
        }
        printf("\n");
    }
}