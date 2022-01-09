#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>

#include <keypadc.h>
#include <graphx.h>
#include <tice.h>

#include "game.h"
#include "usb_handler.h"
#include "helper.h"


void generateGridGame(struct Cell map[10][9]) 
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

    // spawn ships | 0 = vertical, 1 = horizontal
    for (int i = 5; i > 0; --i) 
    {
        int size = i;
        if (i == 2) {
            size = 3;
        }
        if (i == 1) {
            size = 2;
        }
        int distanceFromBoard = 10 - size;
        srand(time(NULL));
        int rotation = rand() % 2;
        bool existingShip = true;
        int randCol = rand() % 10;
        int randRow = rand() % distanceFromBoard;
        if (rotation == 0) 
        {
            while (existingShip == true) {
                int randCol = rand() % 10;
                int randRow = rand() % distanceFromBoard;
                for (int x = 0; x < size; ++x) {
                    if (map[randRow + x][randCol].isShip == true) {
                        existingShip = true;
                        srand(time(NULL));
                        break;
                    }
                    existingShip = false;
                }           
            }
            for (int x = 0; x < size; ++x) 
            {
                map[randRow + x][randCol].isShip = true;
                map[randRow + x][randCol].symbol = 'O';
            }
        }
        else
        {
            while (existingShip == true) {
                int randCol = rand() % distanceFromBoard;
                int randRow = rand() % 10;
                for (int x = 0; x < size; ++x) {
                    if (map[randRow + x][randCol].isShip == true) {
                        existingShip = true;
                        srand(time(NULL));
                        break;
                    }
                    existingShip = false;
                }           
            }
            for (int x = 0; x < size; ++x) 
            {
                map[randRow][randCol + x].isShip = true;
                map[randRow][randCol + x].symbol = 'O';
            }
        }
    }
}

void generateGridAttack(struct Cell map[10][9]) {
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
}


void put_char(const char c) {
    char buf[2];
    buf[0] = c;
    buf[1] = '\0';

    os_PutStrFull(buf);
}

void printBoard(struct Cell map[10][9]) 
{
    for (int i = 0; i < 9; ++i) {
        os_NewLine();
        os_MoveDown();
    }
    for (int i = 0; i < 9; ++i) 
    {
        for (int j = 0; j < 10; ++j) 
        {
            put_char(map[i][j].symbol);
            put_char(' ');
        }
        os_NewLine();
    }
}

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

int main(void)
{

    struct Cell gameMap[10][9];
    struct Cell attackMap[10][9];

    os_ClrHome();
    os_ClrTxtShd();

    os_PutStrFull("Generating boards....");
    os_NewLine();
    
    generateGridGame(gameMap);
    generateGridAttack(attackMap);
    os_PutStrLine("Boards generated!");
    printBoard(gameMap);
    os_SetCursorPos(0, 5);
    os_PutStrFull("Board View");
    os_SetCursorPos(9, 20);
    int x, y = 0;

    char buf[15];

    os_GetStringInput("     ", buf, 14);
    read_nums(&x, &y, buf);

    struct packet_t packet;
    packet.command = command_attack;
    packet.pd.position[0] = (byte)x; 
    packet.pd.position[1] = (byte)y; 
    send_packet(&packet);

    // switch to other view
    while (!os_GetCSC());
    return 0;
}
