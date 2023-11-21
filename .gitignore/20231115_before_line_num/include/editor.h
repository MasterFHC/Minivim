#pragma once
#include <vector>
#include <essentials.h>

std::vector<char> empty_line;
std::vector<std::vector<char>> editor;

int remember_pos = 0;

int edtcur_x = 0, edtcur_y = 0;// on screeen
int edt_x = 0, edt_y = 0;//in editor, real position
int edtcur_vaccant = -1; // vaccant lines

int top_line = 0; // top line of the real position, i.e. the first vector to display
int line_cnt = 0;