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
void populateGrid(struct Cell map[9][10])
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
    for (int i = 5; i > 0; --i)
    {
        //delay(1000); // we need the delay to ensure random spawning locations but im pretty sure i did it wrong
        int size = i;
        if (i == 2)
        {
            size = 3;
        }
        if (i == 1)
        {
            size = 2;
        }
        

        int rotation = rand() % 2;
        int randCol = rand() % 10;
        int randRow = rand() % 9;
        bool existingShip = true;
        // figure out how far away the front of the ship needs to spawn
        int distanceFromSide;
        if (rotation == 0)
        {
            distanceFromSide = 8 - size;
            while (existingShip) // this can probably end in an infinite loop but it's unlikely. what is a reset button for if you dont use it?
            {
                //delay(1000);
                randRow = rand() % distanceFromSide;
                randCol = rand() % 10;
                for (int x = 0; x < size; ++x)
                {
                    // check in a 3x3 grid around point
                    if (map[randRow + x][randCol].isShip || map[randRow + x + 1][randCol].isShip || map[randRow + x - 1][randCol].isShip || map[randRow + x][randCol + 1].isShip || map[randRow + x][randCol - 1].isShip || map[randRow + x + 1][randCol + 1].isShip || map[randRow + x - 1][randCol + 1].isShip || map[randRow + x + 1][randCol - 1].isShip || map[randRow + x - 1][randCol - 1].isShip)
                    {
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
        else
        {
            distanceFromSide = 9 - size;
            while (existingShip)
            {
                //delay(1000);
                randRow = rand() % 9;
                randCol = rand() % distanceFromSide;
                for (int x = 0; x < size; ++x)
                {
                    if (map[randRow][randCol + x].isShip || map[randRow][randCol + x + 1].isShip || map[randRow][randCol + x - 1].isShip || map[randRow + 1][randCol + x].isShip || map[randRow - 1][randCol + x].isShip || map[randRow + 1][randCol + x + 1].isShip || map[randRow + 1][randCol + x - 1].isShip || map[randRow - 1][randCol + x + 1].isShip || map[randRow - 1][randCol + x - 1].isShip)
                    {
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
void put_char(const char c)
{
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
    for (int i = 0; i < 9; ++i)
    {
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
void read_nums(int *n1, int *n2, char *buf)
{
    char num1[10];
    char num2[10];
    char *ptr = buf;

    int i = 0;
    while (*ptr != ' ')
    {
        num1[i] = *ptr;
        ptr++;
        i++;
    }
    i = 0;
    ptr++;
    while (*ptr != '\0')
    {
        num2[i] = *ptr;
        ptr++;
        i++;
    }
    *n1 = atoi(num1);

    *n2 = atoi(num2);
}

void attackGrid(int col, int row, struct Cell map[9][10])
{
    map[row][col].isHit = true;
    if (map[row][col].isShip)
    {
        map[row][col].symbol = 'X';
    }
    else
    {
        map[row][col].symbol = '/';
    }
}

// very inneficient solution but we have 2 hours left before deadline
int checkGameStatus(struct Cell map[9][10])
{
    for (int i = 0; i < 9; ++i)
    {
        for (int j = 0; j < 10; ++j)
        {
            if (map[i][j].isShip && !map[i][j].isHit)
            {
                return 1;
            }
        }
    }
    return 0;
}

// this function will be in charge of the turn management and will be called from main, no setup will be done here
int playGame(struct Cell map1[9][10], struct Cell map2[9][10])
{
    //int rounds = 0;
    while (true)
    {
        // player 1 turn, player 1 attacks map2 (opposite board)
        printBoard(map2);
        os_SetCursorPos(0, 20);
        os_PutStrFull("User 1");
        os_SetCursorPos(1, 20);
        os_PutStrFull("Turn");
        os_SetCursorPos(8, 20);
        os_PutStrFull("ColRow");
        os_SetCursorPos(9, 20);
        int x, y = 0;

        char buf[15];

        os_GetStringInput("     ", buf, 14);
        read_nums(&x, &y, buf);

        // special codes

        // exit code
        if (x == 15 && y == 15)
        {
            return 1;
        }

        // show all ships code
        if (x == 15 && y == 14)
        {
            for (int i = 0; i < 9; ++i)
            {
                for (int j = 0; j < 10; ++j)
                {
                    if (map2[i][j].isShip && !map2[i][j].isHit)
                    {
                        map2[i][j].symbol = 'O';
                    }
                }
            }
        }

        // autowin code
        if (x == 16 && y == 16)
        {
            for (int i = 0; i < 9; ++i)
            {
                for (int j = 0; j < 10; ++j)
                {
                    if (map2[i][j].isShip && !map2[i][j].isHit)
                    {
                        map2[i][j].isHit = true;
                    }
                }
            }
        }

        attackGrid(x, y, map2);
        if (checkGameStatus(map2) == 0)
            return 0;
        printBoard(map2);
        delay(3000);

        // player 2 turn. probably a better way to do this but if it works i dont care
        printBoard(map1);
        os_SetCursorPos(0, 20);
        os_PutStrFull("User 2");
        os_SetCursorPos(1, 20);
        os_PutStrFull("Turn");
        os_SetCursorPos(8, 20);
        os_PutStrFull("ColRow");
        os_SetCursorPos(9, 20);
        x, y = 0;

        buf[15];

        os_GetStringInput("     ", buf, 14);
        read_nums(&x, &y, buf);

        // special codes

        // exit code
        if (x == 15 && y == 15)
        {
            return 1;
        }

        // show all ships code
        if (x == 15 && y == 14)
        {
            for (int i = 0; i < 9; ++i)
            {
                for (int j = 0; j < 10; ++j)
                {
                    if (map1[i][j].isShip && !map1[i][j].isHit)
                    {
                        map1[i][j].symbol = 'O';
                    }
                }
            }
        }

        // autowin code
        if (x == 16 && y == 16)
        {
            for (int i = 0; i < 9; ++i)
            {
                for (int j = 0; j < 10; ++j)
                {
                    if (map1[i][j].isShip && !map1[i][j].isHit)
                    {
                        map1[i][j].isHit = true;
                    }
                }
            }
        }

        attackGrid(x, y, map1);
        if (checkGameStatus(map1) == 0)
            return 2;
        printBoard(map1);
        delay(3000);
    }
}

int main(void)
{
    struct Cell playerMap1[9][10];
    struct Cell playerMap2[9][10];

    srand(rtc_Time());

    // clears screen
    os_ClrHome();
    os_ClrTxtShd();

    os_PutStrFull("Generating boards....");
    os_NewLine();

    populateGrid(playerMap1);
    populateGrid(playerMap2);
    os_PutStrLine("Boards generated!");

    int exitPath = playGame(playerMap1, playerMap2);
    if (exitPath == 1)
        return 0;
    os_ClrHome();
    os_ClrTxtShd();
    os_SetCursorPos(4, 6);
    if (exitPath == 0)
        os_PutStrFull("Player 1 wins!"); // there is obviously a more efficient way to print victory messages rather than 2 conditionals but the ti84 library is not the best
    if (exitPath == 2)
        os_PutStrFull("Player 2 wins!");
    while (!os_GetCSC())
        ;
    return 0;
}