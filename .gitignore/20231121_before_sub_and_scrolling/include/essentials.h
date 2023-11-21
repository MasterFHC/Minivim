#pragma once
#include <ncurses.h>
#include <string.h>
#include <editor.h>
#include <command.h>
#include <fstream>

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
int ENABLED_LINENUM = 1; // 0: dont show linenum, 1: absolute value, 2: relative value
bool TRUNCATE_MODE = 0;  // 1: open file in truncate mode
bool READONLY_MODE = 0;  // 1: open file in readonly mode
char *filename;          // file + path i am about to modify
bool IS_BREAK_MODE = 1;  // 1: break mode of line wrapping, 0: scrolling
bool IS_TEMPFILE = 1;    // 0: is a existing file or has a filename

char CURRENT_MODE = 'n'; // n = normal, \: = command, i = insert
bool END_MINIVIM = false;
bool FILE_MODIFIED = false;

WINDOW *edtwin, *infowin, *cmdwin, *numwin; // editor,information,command,line_number

void read_cmdline(int argc, char *argv[]);
void Boot();
void nor_mode();
void ins_mode();
void cmd_mode();
void display(std::string win);
void input_normal_value(char ch, std::string win);
void move_cursor(int key, char mode);
int get_linenum(int key);
void show_linenum();
void process_command();
bool write_file();

void read_cmdline(int argc, char *argv[])
{
    bool check_mode = false;
    for (int k = 1; k < argc; k++)
    {
        if (check_mode)
        {
            if (!strcmp(argv[k], "break"))
                IS_BREAK_MODE = true;
            else if (!strcmp(argv[k], "scroll"))
                IS_BREAK_MODE = false;
            check_mode = false;
        }
        else if (!strcmp(argv[k], "-W"))
        {
            check_mode = true;
        }
        else if (!strcmp(argv[k], "-t"))
        {
            TRUNCATE_MODE = true;
        }
        else if (!strcmp(argv[k], "-R"))
        {
            READONLY_MODE = true;
        }
        else
        {
            filename = argv[k];
            IS_TEMPFILE = false;
        }
    }
}

void Boot()
{

    raw();
    noecho();
    keypad(stdscr, true); /*stdscr is the regular window*/
    curs_set(2);

    // init color
    start_color();
    // init_pair(STD_COLOR_NUM, COLOR_WHITE, COLOR_BLACK);
    // init_pair(EDT_COLOR_NUM, COLOR_YELLOW, COLOR_CYAN);
    // init_pair(INFO_COLOR_NUM, COLOR_GREEN, COLOR_RED);
    // init_pair(CMD_COLOR_NUM, COLOR_BLACK, COLOR_WHITE);
    // init_pair(NUM_COLOR_NUM, COLOR_BLACK, COLOR_WHITE);

    init_pair(STD_COLOR_NUM, COLOR_WHITE, COLOR_BLACK);
    init_pair(EDT_COLOR_NUM, COLOR_WHITE, COLOR_BLACK);
    init_pair(INFO_COLOR_NUM, COLOR_WHITE, COLOR_BLACK);
    init_pair(CMD_COLOR_NUM, COLOR_WHITE, COLOR_BLACK);
    init_pair(NUM_COLOR_NUM, COLOR_YELLOW, COLOR_BLACK);

    if (ENABLED_LINENUM)
    {                 // if line number enabled
        NUM_COLS = 7; // leave one column as buffer for asthetic
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

    editor.insert(editor.begin(), empty_line);
    line_cnt++;
    if (TRUNCATE_MODE or IS_TEMPFILE)
    {
        return;
    }
    std::ifstream freader(filename);
    if (freader.fail())
    {
        return;
    }
    char ch = 0;
    while (!freader.eof())
    {
        freader.get(ch);
        if (!freader)
            break;
        if (ch == '\n')
        {
            line_cnt++;
            editor.push_back(empty_line);
        }
        else if (ch == '\t')
        {
            for (int k = 0; k < 4; k++)
                editor[line_cnt - 1].push_back(' ');
        }
        else
        {
            editor[line_cnt - 1].push_back(ch);
        }
    }
    freader.close();
    display("edt");
}