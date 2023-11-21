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
void substitute(std::string from, std::string to);
bool write_file();