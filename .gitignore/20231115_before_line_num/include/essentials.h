#pragma once
#include <ncurses.h>
#include <editor.h>
#define STD_COLOR_NUM 1
#define EDT_COLOR_NUM 2
#define INFO_COLOR_NUM 3
#define CMD_COLOR_NUM 4
#define NUM_COLOR_NUM 5
#define KEY_ESC 27
// #define COLS 10

int EDT_LINES, INFO_LINES, CMD_LINES, NUM_LINES;
int EDT_COLS, INFO_COLS, CMD_COLS, NUM_COLS = 0;

// syntax:
int ENABLED_LINENUM = 2; // 0: dont show linenum, 1: absolute value, 2: relative value

char CURRENT_MODE = 'n'; // n = normal, \: = command, i = insert
bool END_MINIVIM = false;

WINDOW *edtwin, *infowin, *cmdwin, *numwin; // editor,information,command,line_number

void nor_mode();
void ins_mode();
void cmd_mode();
void display();
void input_normal_value(char ch);
void move_cursor(int key, char mode);
int get_linenum(int key);

void Boot()
{

    raw();
    noecho();
    keypad(stdscr, true); /*stdscr is the regular window*/
    curs_set(2);

    // init color
    start_color();
    init_pair(STD_COLOR_NUM, COLOR_WHITE, COLOR_BLACK);
    init_pair(EDT_COLOR_NUM, COLOR_YELLOW, COLOR_CYAN);
    init_pair(INFO_COLOR_NUM, COLOR_GREEN, COLOR_RED);
    init_pair(CMD_COLOR_NUM, COLOR_BLACK, COLOR_WHITE);
    init_pair(NUM_COLOR_NUM, COLOR_BLACK, COLOR_GREEN);

    // init_pair(STD_COLOR_NUM, COLOR_WHITE, COLOR_BLACK);
    // init_pair(EDT_COLOR_NUM, COLOR_WHITE, COLOR_BLACK);
    // init_pair(INFO_COLOR_NUM, COLOR_WHITE, COLOR_BLACK);
    // init_pair(CMD_COLOR_NUM, COLOR_WHITE, COLOR_BLACK);
    // init_pair(NUM_COLOR_NUM, COLOR_WHITE, COLOR_BLACK);

    if (ENABLED_LINENUM)
    { // if line number enabled
        NUM_COLS = 6;
    }
    else
    { // if not enabled
        NUM_COLS = 0;
    }
    NUM_LINES = LINES - 2;
    // EDT_LINES = LINES - 2, EDT_COLS = COLS - 1;
    EDT_LINES = LINES - 2, EDT_COLS = COLS - 1 - NUM_COLS;
    if (ENABLED_LINENUM)
    {
        numwin = newwin(NUM_LINES, NUM_COLS, 0, 0);
    }
    // edtwin = newwin(EDT_LINES, EDT_COLS + 1, 0, 0); // added a BUFFER
    edtwin = newwin(EDT_LINES, EDT_COLS + 1, 0, NUM_COLS); // added a BUFFER
    infowin = newwin(1, COLS, LINES - 2, 0);
    cmdwin = newwin(1, COLS, LINES - 1, 0);

    // create customized window
    keypad(edtwin, true);
    keypad(infowin, true);
    keypad(cmdwin, true);
    if (ENABLED_LINENUM)
        keypad(numwin, true);

    wbkgd(stdscr, COLOR_PAIR(STD_COLOR_NUM));   // background
    wbkgd(edtwin, COLOR_PAIR(EDT_COLOR_NUM));   /*set customized window color*/
    wbkgd(infowin, COLOR_PAIR(INFO_COLOR_NUM)); /*set customized window color*/
    wbkgd(cmdwin, COLOR_PAIR(CMD_COLOR_NUM));   /*set customized window color*/
    if (ENABLED_LINENUM)
        wbkgd(numwin, COLOR_PAIR(NUM_COLOR_NUM)); /*set customized window color*/

    wrefresh(stdscr);
    wrefresh(edtwin);
    wrefresh(infowin);
    wrefresh(cmdwin);
    if (ENABLED_LINENUM)
        wrefresh(numwin);

    if (line_cnt == 0)
    {
        editor.insert(editor.begin(), empty_line);
        line_cnt++;
    }
}