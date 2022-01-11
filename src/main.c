#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "keypadc.h"
#include "tice.h"

struct Cell 
{
    bool isShip;
    bool isHit;
    char symbol;
};

// generates grid with ships
void generateGridGame(struct Cell map[9][10]) 
{
    // make clear board
    for (int i = 0; i < 9; ++i) 
    {
        for (int j = 0; j < 10; ++j) 
        {
            map[i][j].isHit = false;
            map[i][j].isShip = false;
            map[i][j].symbol = '_';
        }
    }

    // spawn ships rewrite
    for (int i = 5; i > 0; --i) {
        delay(1000); // we need the delay to ensure random spawning locations since the seed is time based
        int size = i;
        if (i == 2) {
            size = 3;
        }
        if (i == 1) {
            size = 2;
        }
        srand(time(NULL));

        int rotation = rand() % 2;
        int randCol = rand() % 10;
        int randRow = rand() % 9;
        bool existingShip = true;
        // figure out how far away the front of the ship needs to spawn
        int distanceFromSide;
        if (rotation == 0) {
            distanceFromSide = 8 - size;
            while (existingShip) 
            {
                delay(1000);
                srand(time(NULL));
                randRow = rand() % distanceFromSide;
                randCol = rand() % 10;
                for (int x = 0; x < size; ++x) 
                {
                    if (map[randRow + x][randCol].isShip) {
                        existingShip = true;
                        break;
                    }
                    existingShip = false;
                }
            }
            for (int x = 0; x < size; ++x) 
            {
                map[randRow + x][randCol].isShip = true;
                //map[randRow + x][randCol].symbol = 'O'; // this line is just for visualization 
            }
        }
        else {
            distanceFromSide = 9 - size;
            while (existingShip) 
            {
                delay(1000);
                srand(time(NULL));
                randRow = rand() % 9;
                randCol = rand() % distanceFromSide;
                for (int x = 0; x < size; ++x) 
                {
                    if (map[randRow][randCol + x].isShip) {
                        existingShip = true;
                        break;
                    }
                    existingShip = false;
                }
            }
            for (int x = 0; x < size; ++x) 
            {
                map[randRow][randCol + x].isShip = true;
                //map[randRow][randCol + x].symbol = 'O'; // this line is just for visualization 
            }
        }
    }
}

// helps with printing board
void put_char(const char c) {
    char buf[2];
    buf[0] = c;
    buf[1] = '\0';

    os_PutStrFull(buf);
}

// actually prints board
void printBoard(struct Cell gameMap[9][10]) 
{
    os_ClrHome();
    os_ClrTxtShd();
    for (int i = 0; i < 9; ++i) {
        os_NewLine();
        os_MoveDown();
    }
    for (int i = 0; i < 9; ++i) 
    {
        for (int j = 0; j < 10; ++j) 
        {
            put_char(gameMap[i][j].symbol);
            put_char(' ');
        }
        os_NewLine();
    }
}

// input first column then row
void read_nums(int* n1, int* n2, char* buf) {
    char num1[10];
    char num2[10];
    char* ptr = buf;

    int i = 0;
    while (*ptr != ' ') {
        num1[i] = *ptr;
        ptr++;
        i++;
    }
    i = 0;
    ptr++;
    while (*ptr != '\0') {
        num2[i] = *ptr;
        ptr++;
        i++;
    }
    *n1 = atoi(num1);

    *n2 = atoi(num2);
}

void attackGrid(int col, int row, struct Cell map[9][10]) {
    map[row][col].isHit = true;
    if (map[row][col].isShip) {
        map[row][col].symbol = 'X';
    }
    else {
        map[row][col].symbol = '/';
    }
}

// very inneficient solution but we have 2 hours left before deadline
int checkGameStatus(struct Cell map[9][10]) {
    for (int i = 0; i < 9; ++i) 
    {
        for (int j = 0; j < 10; ++j) 
        {
            if (map[i][j].isShip && !map[i][j].isHit) {
                return 1;
            }
        }
    }
    return 0;
}

int main(void)
{

    struct Cell gameMap[9][10];

    // clears screen
    os_ClrHome();
    os_ClrTxtShd();

    os_PutStrFull("Generating boards....");
    os_NewLine();
    
    generateGridGame(gameMap);
    os_PutStrLine("Boards generated!");
    delay(1000);
    while (true) {
        printBoard(gameMap);
        os_SetCursorPos(0, 20);
        os_PutStrFull("Board");
        os_SetCursorPos(1, 20);
        os_PutStrFull("View");
        os_SetCursorPos(8, 20);
        os_PutStrFull("ColRow");
        os_SetCursorPos(9, 20);
        int x, y = 0;

        char buf[15];

        os_GetStringInput("     ", buf, 14);
        read_nums(&x, &y, buf);


        // special codes
        
        // exit code
        if (x == 15 && y == 15) {
            return 0;
        }

        // show all ships code
        if (x == 15 && y == 14) {
            for (int i = 0; i < 9; ++i) 
            {
                for (int j = 0; j < 10; ++j) 
                {
                    if (gameMap[i][j].isShip && !gameMap[i][j].isHit) {
                        gameMap[i][j].symbol = 'O';
                    }
                }
            }
        }

        attackGrid(x, y, gameMap);
        if (checkGameStatus(gameMap) == 0)
            break;
    }
    
    os_ClrHome();
    os_ClrTxtShd();
    os_SetCursorPos(0, 5);
    os_PutStrFull("You win!");
    while (!os_GetCSC());
    return 0;
}