#include <ncurses.h>
#include <essentials.h>
#include <math.h>

int main()
{

    initscr(); /* Start curses mode */
    Boot();
    display();
    int ch = 0;
    // get char in loop until get `Esc`
    while (END_MINIVIM != true)
    {
        if (CURRENT_MODE == 'n')
        {
            nor_mode();
        }
        else if (CURRENT_MODE == '\:')
        {
            cmd_mode();
        }
        else if (CURRENT_MODE == 'i')
        {
            ins_mode();
        }
    }
    while ((ch = getch()) != 27)
    {
        wprintw(edtwin, "%c", ch);
        wrefresh(edtwin);
        wrefresh(stdscr);
    }
    endwin(); /* End curses mode */
    return 0;
}
void nor_mode() // normal_mode
{
    while (1)
    {
        mvwprintw(infowin, 0, COLS - 14, "[Normal Mode]", CURRENT_MODE);
        wrefresh(infowin);
        int ch = getch();
        if (ch == ':')//cmd mode
        {
            CURRENT_MODE = ':';
            break;
        }
        else if (ch == 'i' or ch == 'I')//insert mode
        {
            CURRENT_MODE = 'i';
            break;
        }
        else if (ch == KEY_ESC)//quit minivim
        {
            END_MINIVIM = true;
            break;
        }
    }
}
void ins_mode() // insert_mode
{
    wmove(edtwin, edtcur_y, edtcur_x);
    while (1)
    {
        getyx(edtwin, edtcur_y, edtcur_x);
        wmove(infowin, 0, 0);
        for (int k = 0; k < EDT_COLS; k++)
            wprintw(infowin, "%c", ' ');
        mvwprintw(infowin, 0, 0, "LINE:%d COL:%d REM:%d", edt_y, edt_x, remember_pos);
        mvwprintw(infowin, 0, COLS - 14, "[Insert Mode]", CURRENT_MODE);
        wrefresh(infowin);
        wrefresh(cmdwin);
        wrefresh(edtwin);
        int ch = getch();
        if (ch == KEY_ESC) // quit
        {
            CURRENT_MODE = 'n';
            break;
        }
        else if (ch == KEY_UP or ch == KEY_DOWN or ch == KEY_LEFT or ch == KEY_RIGHT) // arrow keys
        {
            move_cursor(ch, 'n');
        }
        else if (ch == KEY_BACKSPACE) // delete
        {
            if (edt_x == 0)
            {
                if (edt_y != 0)
                {
                    if (edtcur_y == 0)
                        top_line--;
                    edt_x = editor[edt_y - 1].size();
                    for (int i = 0; i < editor[edt_y].size(); i++)
                    {
                        editor[edt_y - 1].push_back(editor[edt_y][i]);
                    }
                    for (int i = 0; i < editor[edt_y].size(); i++)
                    {
                        editor[edt_y].pop_back();
                    }
                    editor.erase(editor.begin() + edt_y);
                    line_cnt--;
                    edt_y--;
                }
            }
            else
            {
                editor[edt_y].erase(editor[edt_y].begin() + edt_x - 1);
                edt_x--;
            }
            display();
        }
        else if (ch == '\t') // tab
        {
            for (int i = 1; i <= 4; i++)
                input_normal_value(' ');
        }
        else if (ch == '\n') // new line
        {
            remember_pos = std::max(edt_x, remember_pos);
            editor.insert(editor.begin() + edt_y + 1, empty_line);
            int orgline_size = editor[edt_y].size();
            for (int i = edt_x; i < orgline_size; i++)
            {
                editor[edt_y + 1].push_back(editor[edt_y][i]);
            }
            for (int i = edt_x; i < orgline_size; i++)
            {
                editor[edt_y].pop_back();
            }
            edt_y++, edt_x = 0;
            line_cnt++;
            edtcur_y++;
            edtcur_x = 0;
            if (edtcur_y >= EDT_LINES)
            { // if new line exceeds the screen, push it down
                top_line++;
                edtcur_y--;
            }
            display();
            wmove(edtwin, edtcur_y, edtcur_x);
        }
        else // normal values
        {
            input_normal_value(ch);
        }
    }
}
void cmd_mode() // command_mode
{
    while (1)
    {
        mvwprintw(infowin, 0, COLS - 15, "[Command Mode]", CURRENT_MODE);
        wrefresh(infowin);
        int ch = getch();
        if (ch == KEY_ESC)
        {
            CURRENT_MODE = 'n';
            break;
        }
    }
}
void display()
{
    edtcur_vaccant = 0;
    int editor_line = top_line;                     // real vector in editor
    int used_line = 0, now_line = 0, more_line = 0; // now line on screen!!
    while (used_line < EDT_LINES)
    {
        if (editor_line >= line_cnt)
        {
            wmove(edtwin, now_line, 0);
            wprintw(edtwin, "%c", '~');
            for (int k = 1; k < EDT_COLS; k++)
                wprintw(edtwin, "%c", ' ');
            edtcur_vaccant++;
            used_line++;
            now_line++;
            continue;
        }
        more_line = get_linenum((int)editor[editor_line].size());
        used_line += more_line;
        if (edt_y == editor_line)
        {
            if (edt_x == 0)
                edtcur_y = now_line, edtcur_x = 0;
            else
            {
                edtcur_y = (edt_x + EDT_COLS - 1) / EDT_COLS + now_line - 1;
                edtcur_x = (edt_x - 1) % EDT_COLS + 1;
            }
        }
        for (int k = 0; k < more_line; k++)
        {
            wmove(edtwin, now_line, 0);
            if (now_line >= EDT_LINES)
                break;
            for (int j = 0; j < EDT_COLS; j++)
            {
                if (k * EDT_COLS + j >= editor[editor_line].size())
                {
                    for (int i = j; i < EDT_COLS; i++)
                        wprintw(edtwin, "%c", ' ');
                    break;
                }
                wprintw(edtwin, "%c", editor[editor_line][k * EDT_COLS + j]);
            }
            now_line++;
        }
        editor_line++;
    }

    wmove(edtwin, edtcur_y, edtcur_x);
    wrefresh(edtwin);
}
void input_normal_value(char ch)
{
    editor[edt_y].insert(editor[edt_y].begin() + edt_x, ch);
    edt_x++;
    if (edtcur_x >= EDT_COLS and edtcur_y >= EDT_LINES - 1)
    {
        top_line++;
    }
    display();
    if (edtcur_x >= EDT_COLS + 1)
    {
        if (edtcur_y + 1 >= EDT_LINES)
        {
            int elapsed_line;
            elapsed_line = get_linenum((int)editor[top_line - 1].size());
            wmove(edtwin, edtcur_y - elapsed_line + 1, 1); // already exist a char
        }
        else
            wmove(edtwin, edtcur_y + 1, 1);
    }
    else
        wmove(edtwin, edtcur_y, edtcur_x);
    wrefresh(edtwin);
}
void move_cursor(int key, char mode)
{
    if (key == KEY_UP)
    {
        remember_pos = std::max(edt_x, remember_pos);
        if (edt_y == top_line) // up out of border
            top_line = std::max(top_line - 1, 0);
        edt_y = std::max(edt_y - 1, 0);
        edt_x = std::min(remember_pos, (int)editor[edt_y].size());
        display();
    }
    else if (key == KEY_DOWN)
    {

        // todo:if already at bottom line, dont move
        if (edt_y + 1 < line_cnt)
        {
            remember_pos = std::max(edt_x, remember_pos);
            int assumed_x = std::min(remember_pos, (int)editor[edt_y + 1].size()); // the x-real position of the cursor afterwards
            int assumed_pre = get_linenum(edt_x);                                  // e.g: 0~9=1, 10~18=2, ....
            int assumed_after = get_linenum(assumed_x);                            // e.g: 0~9=1, 10~18=2, ....
            int assumed_size = get_linenum((int)editor[edt_y].size());             // e.g: 0~9=1, 10~18=2, ....
            int assumed_addition = assumed_after - assumed_pre + assumed_size;
            if (edtcur_y + assumed_addition >= EDT_LINES)
            { // if it dose goes bottom out of range
                int accumu_line = 0;
                do
                {
                    accumu_line += get_linenum((int)editor[top_line++].size());
                } while (accumu_line < assumed_addition);
            }
            edt_y = std::min(edt_y + 1, line_cnt);
            edt_x = std::min(remember_pos, (int)editor[edt_y].size());
        }
        wrefresh(infowin);
        display();
    }
    else if (key == KEY_LEFT)
    {
        edt_x = std::max(edt_x - 1, 0);
        display();
        if (edtcur_x == EDT_COLS)
            edtcur_y++, edtcur_x = 0;
        wmove(edtwin, edtcur_y, edtcur_x);
        wrefresh(edtwin);
    }
    else if (key == KEY_RIGHT)
    {
        edt_x = std::min(edt_x + 1, (int)editor[edt_y].size());
        display();
        if (edtcur_x == EDT_COLS and get_linenum(edt_x) != get_linenum((int)editor[edt_y].size()))
            edtcur_y++, edtcur_x = 0;
        wmove(edtwin, edtcur_y, edtcur_x);
        wrefresh(edtwin);
    }
}
int get_linenum(int key)
{ // e.g: 0~9=1, 10~18=2, ....
    return (key == 0) ? (1) : ((key + EDT_COLS - 1) / EDT_COLS);
}