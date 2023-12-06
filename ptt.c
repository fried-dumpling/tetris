#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>

void GotoXY(int x, int y) {
	COORD Pos;
	Pos.X = x;
	Pos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

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

int main() {
    char num = 1;
    wchar_t exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);
    wchar_t folderPath[wcslen(exePath) + 1];
    wcscpy(folderPath, exePath);
    wchar_t* lb = wcsrchr(folderPath, L'\\');
    if (lb!=NULL)
        *lb = L'\0';
    printf("\n%s\n",folderPath);
    printf("%td\n",lb);
    FILE *f;
    f = fopen("C:\\Users\\taeho\\Desktop\\C\\c\\games\\tetris\\save\\save1.txt","w");
    if (f = NULL) {
        printf("Error\n");
        return 0;
    }
    fputs("FileFileFile\n",f);
    fclose(f);
    printf("Succsess\n");
    return 0;
}