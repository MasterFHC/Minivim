/*[Project Log]
2023/11/11  read github guidelines
2023/11/12  read ncurses files
            basic user interface            10
2023/11/13  insert mode start: display
2023/11/14  insert mode cont'd: arrow keys
2023/11/15  insert mode almost complete
            line numbers(2 versions)        35
2023/11/16  preparing for English exam
2023/11/17  touch fish
2023/11/18  path+relative path              40
            command mode basics
2023/11/19  file persistence                50
            command mode                    55
            command history                 60
            command line arguments          65
2023/11/20  normal mode
            line number and jump            70
            shortcut key                    75
2023/11/21  search and subsitution          80
            another way of line wrapping    80 + 10(scrolling)

*/

#include <ncurses.h>
#include <string>
#include <essentials.h>

int main(int argc, char *argv[])
{
    read_cmdline(argc, argv);
    initscr(); /* Start curses mode */
    Boot();

    wmove(infowin, 0, 0);

    wrefresh(infowin);
    display("edt");
    int ch = 0;
    // get char in loop until get `Esc`
    while (!END_MINIVIM)
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
    endwin(); /* End curses mode */
    return 0;
}
void nor_mode() // normal_mode
{
    if (edt_x == (int)editor[edt_y].size() and edt_x != 0)
        edt_x--;
    display("edt"); // drop back the cursor if at the end of the line
    bool dd_alarm = false;
    while (1)
    {
        wmove(infowin, 0, 0);
        for (int k = 0; k < COLS; k++)
            wprintw(infowin, "%c", ' ');
        getyx(edtwin, edtcur_y, edtcur_x);

        wmove(infowin, 0, 0);
        wprintw(infowin, "%c", '\"');
        wprintw(infowin, filename);
        wprintw(infowin, "%c", '\"');
        wprintw(infowin, " LINE:%d COL:%d", edt_y + 1, edt_x + 1);
        mvwprintw(infowin, 0, COLS - 15, " [Normal Mode]", CURRENT_MODE);

        wrefresh(infowin);
        wrefresh(cmdwin);
        if (ENABLED_LINENUM)
            wrefresh(numwin);
        wrefresh(edtwin);
        int ch = getch();
        if (ch == ':') // cmd mode
        {
            dd_alarm = false;
            CURRENT_MODE = ':';
            break;
        }
        else if ((ch == 'i' or ch == 'I') and !READONLY_MODE) // insert mode
        {
            dd_alarm = false;
            CURRENT_MODE = 'i';
            break;
        }
        else if (false and ch == KEY_ESC) // quit minivim, shut down for release
        {
            dd_alarm = false;
            END_MINIVIM = true;
            break;
        }
        else if (ch == KEY_UP or ch == KEY_DOWN or ch == KEY_LEFT or ch == KEY_RIGHT)
        {
            dd_alarm = false;
            move_cursor(ch, 'n');
        }
        else if (ch == '0')
        {
            dd_alarm = false;
            while (edt_x != 0)
            {
                move_cursor(KEY_LEFT, 'n');
            }
            display("edt");
        }
        else if (ch == '$')
        {
            dd_alarm = false;
            int to_point = std::max((int)editor[edt_y].size() - 1, 0);
            while (edt_x != to_point)
            {
                move_cursor(KEY_RIGHT, 'n');
            }
            display("edt");
        }
        else if (ch == 'b')
        {
            dd_alarm = false;
            if (edt_x == 0 and edt_y != 0 and IS_BREAK_MODE)
            {
                edt_y--;
                edt_x = std::max((int)editor[edt_y].size() - 1, 0);
                if (edtcur_y == 0)
                {
                    top_line--;
                    top_subline = get_linenum((int)editor[top_line].size()) - 1;
                }
                display("edt");
            }
            else
            {
                if (!IS_BREAK_MODE and edt_y != 0 and edt_x == 0)
                {

                    edt_x = editor[edt_y - 1].size();
                    if (edt_x < left_col or edt_x > left_col + EDT_COLS)
                        left_col = edt_x;
                    edt_y--;
                }
                bool start_word = false;
                while (1)
                {
                    if (edt_x == 0)
                        break;
                    if (!start_word and editor[edt_y][edt_x - 1] != ' ')
                        start_word = true;
                    else if (start_word and editor[edt_y][edt_x - 1] == ' ')
                        break;
                    move_cursor(KEY_LEFT, 'n');
                }
            }
        }
        else if (ch == 'w')
        {
            dd_alarm = false;
            bool start_word = false;
            if (editor[edt_y].size() == 0 and edt_x == 0)
            {
                edt_y++, edt_x = 0;
                if (edtcur_y == EDT_LINES - 1)
                {
                    if (IS_BREAK_MODE)
                        top_subline++;
                    else
                        top_line++;
                }
                display("edt");
            }
            else
            {
                while (1)
                {
                    if (edt_x == editor[edt_y].size() - 1)
                    {
                        if (edt_y != line_cnt - 1)
                        {
                            edt_y++, edt_x = 0;
                            if (!IS_BREAK_MODE)
                                left_col = 0;
                        }
                        if (edtcur_y == EDT_LINES - 1 and IS_BREAK_MODE)
                            top_subline++;
                        display("edt");
                        break;
                    }
                    if (!start_word and editor[edt_y][edt_x] == ' ')
                        start_word = true;
                    else if (start_word and editor[edt_y][edt_x] != ' ')
                        break;
                    move_cursor(KEY_RIGHT, 'n');
                }
            }
        }
        else if (ch == 'd' and !READONLY_MODE)
        {
            if (dd_alarm == false)
                dd_alarm = true;
            else
            {
                dd_alarm = false;
                if (edt_y == top_line)
                {
                    if (IS_BREAK_MODE)
                        top_subline = 0;
                    else
                        left_col = 0;
                    edt_x = 0;
                }
                if (edt_y == line_cnt - 1)
                {
                    if (!IS_BREAK_MODE)
                    {
                        left_col = 0;
                    }
                    if (edt_y == top_line)
                        top_line = std::max(0, edt_y - 1);
                    editor.erase(editor.begin() + edt_y);
                    line_cnt--;
                    edt_y--;
                    if (line_cnt == 0)
                        editor.push_back(empty_line), edt_y = 0, edt_x = 0, line_cnt++;
                    display("edt");
                }
                else
                {
                    if (!IS_BREAK_MODE)
                        left_col = 0;
                    editor.erase(editor.begin() + edt_y);
                    line_cnt--;
                    edt_x = 0;
                    display("edt");
                }
            }
        }
    }
}
void ins_mode() // insert_mode
{
    wmove(edtwin, edtcur_y, edtcur_x);
    while (1)
    {
        wmove(infowin, 0, 0);
        for (int k = 0; k < COLS; k++)
            wprintw(infowin, "%c", ' ');
        getyx(edtwin, edtcur_y, edtcur_x);
        wmove(infowin, 0, 0);
        wprintw(infowin, "%c", '\"');
        wprintw(infowin, filename);
        wprintw(infowin, "%c", '\"');
        wprintw(infowin, " LINE:%d COL:%d", edt_y + 1, edt_x + 1);
        mvwprintw(infowin, 0, COLS - 14, "[Insert Mode]", CURRENT_MODE);
        wrefresh(infowin);
        wrefresh(cmdwin);
        if (ENABLED_LINENUM)
            wrefresh(numwin);
        wrefresh(edtwin);
        int ch = getch();
        if (ch == KEY_ESC) // quit
        {
            CURRENT_MODE = 'n';
            break;
        }
        else if (ch == KEY_UP or ch == KEY_DOWN or ch == KEY_LEFT or ch == KEY_RIGHT) // arrow keys
        {
            move_cursor(ch, 'i');
        }
        else if (ch == KEY_BACKSPACE) // delete
        {
            if (edt_x == 0)
            {
                if (edt_y != 0)
                {
                    FILE_MODIFIED = true;
                    if (edtcur_y == 0)
                        top_line--, top_subline = 0;
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
                    if (!IS_BREAK_MODE and edt_x < left_col or edt_x > left_col + EDT_COLS)
                        left_col = edt_x;
                }
            }
            else if (!IS_BREAK_MODE and edtcur_x == 0)
                left_col--;
            else
            {
                FILE_MODIFIED = true;
                editor[edt_y].erase(editor[edt_y].begin() + edt_x - 1);
                edt_x--;
            }
            display("edt");
        }
        else if (ch == '\t') // tab
        {
            for (int i = 1; i <= 4; i++)
                input_normal_value(' ', "edt");
        }
        else if (ch == '\n') // new line
        {
            FILE_MODIFIED = true;
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
                top_line++, top_subline = 0;
                edtcur_y--;
            }
            if (!IS_BREAK_MODE)
                left_col = 0;
            display("edt");
            wmove(edtwin, edtcur_y, edtcur_x);
        }
        else // normal values
        {
            input_normal_value(ch, "edt");
        }
    }
}
void cmd_mode() // command_mode
{
    wmove(infowin, 0, 0);
    cmdcur = 0;
    for (int k = 0; k < COLS; k++)
        wprintw(infowin, "%c", ' ');
    mvwprintw(cmdwin, 0, 0, "%c", '\:');
    for (int k = 1; k < COLS; k++)
        wprintw(cmdwin, "%c", ' ');
    cmd_top = cmd_size;
    command.clear();
    wmove(cmdwin, 0, 1);
    while (1)
    {
        mvwprintw(infowin, 0, COLS - 15, "[Command Mode]", CURRENT_MODE);
        wmove(cmdwin, 0, cmdcur + 1);
        wrefresh(infowin);
        wrefresh(cmdwin);
        int ch = getch();
        if (ch == KEY_ESC)
        {
            mvwprintw(cmdwin, 0, 0, "%c", ' ');
            for (int k = 1; k < COLS; k++)
                wprintw(cmdwin, "%c", ' ');
            CURRENT_MODE = 'n';
            command.clear();
            display("cmd");
            wrefresh(cmdwin);
            break;
        }
        else if (ch == KEY_LEFT or ch == KEY_RIGHT or ch == KEY_UP or ch == KEY_DOWN)
        {
            move_cursor(ch, '\:');
        }
        else if (ch == KEY_BACKSPACE)
        {
            if (cmdcur > 0)
            {
                command.erase(command.begin() + cmdcur - 1);
                cmdcur--;
                display("cmd");
                wrefresh(cmdwin);
            }
        }
        else if (ch == '\n')
        {
            mvwprintw(cmdwin, 0, 0, "%c", ' ');
            for (int k = 1; k < CMD_COLS; k++)
                wprintw(cmdwin, "%c", ' ');
            if (command.size() != 0)
                process_command();

            CURRENT_MODE = 'n';
            cmdcur = 0;
            command.clear();
            wrefresh(cmdwin);
            break;
        }
        else
        {
            input_normal_value(ch, "cmd");
        }
    }
}
void display(std::string win)
{
    if (win == "edt" and IS_BREAK_MODE)
    {
        while (top_subline >= get_linenum((int)editor[top_line].size()))
        {
            top_subline -= get_linenum((int)editor[top_line].size());
            top_line++;
        }
        for (int i = 0; i < EDT_LINES; i++)
        {
            wmove(edtwin, i, 0);
            for (int j = 0; j <= EDT_COLS; j++)
            {
                wprintw(edtwin, "%c", ' ');
            }
        }
        wmove(edtwin, 0, 0);
        int editor_line = top_line;                     // real vector in editor
        int used_line = 0, now_line = 0, more_line = 0; // now line on screen!!
        while (used_line < EDT_LINES)
        {
            if (editor_line >= line_cnt)
            {
                if (!ENABLED_LINENUM)
                    mvwprintw(edtwin, now_line, 0, "%c", '~');
                else
                    mvwprintw(edtwin, now_line, 0, "%c", ' ');
                for (int k = 1; k <= EDT_COLS; k++)
                    wprintw(edtwin, "%c", ' ');
                used_line++;
                now_line++;
                continue;
            }
            more_line = get_linenum((int)editor[editor_line].size());
            if (editor_line == top_line)
                more_line -= top_subline;
            used_line += more_line;
            if (edt_y == editor_line)
            {
                if (edt_x == 0)
                    edtcur_y = now_line, edtcur_x = 0;
                else
                {
                    if (edt_y == top_line)
                        edtcur_y = (edt_x + EDT_COLS - 1) / EDT_COLS + now_line - 1 - top_subline;
                    else
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
                    if ((k + ((editor_line == top_line) ? (top_subline) : (0))) * EDT_COLS + j >= editor[editor_line].size())
                    {
                        for (int i = j; i <= EDT_COLS; i++)
                            wprintw(edtwin, "%c", ' ');
                        break;
                    }
                    if (editor_line == top_line)
                        wprintw(edtwin, "%c", editor[editor_line][(k + top_subline) * EDT_COLS + j]);
                    else
                        wprintw(edtwin, "%c", editor[editor_line][k * EDT_COLS + j]);
                }
                now_line++;
            }
            editor_line++;
        }
        wmove(edtwin, edtcur_y, edtcur_x);
        wrefresh(edtwin);
        if (ENABLED_LINENUM)
            show_linenum();
    }
    else if (win == "edt" and !IS_BREAK_MODE)
    {
        for (int i = 0; i < EDT_LINES; i++)
        {
            wmove(edtwin, i, 0);
            if (i + top_line >= line_cnt)
            {
                for (int j = 0; j <= EDT_COLS; j++)
                {
                    wprintw(edtwin, "%c", ' ');
                }
            }
            else
            {
                for (int j = 0; j <= EDT_COLS; j++)
                {
                    if (i + top_line == edt_y and j + left_col == edt_x)
                    {
                        edtcur_y = i;
                        edtcur_x = j;
                    }
                    if (left_col + j < editor[i + top_line].size())
                    {
                        wprintw(edtwin, "%c", editor[i + top_line][j + left_col]);
                    }
                    else
                    {
                        wprintw(edtwin, "%c", ' ');
                    }
                }
            }
        }
        wmove(edtwin, edtcur_y, edtcur_x);
        wrefresh(edtwin);
        if (ENABLED_LINENUM)
            show_linenum();
    }
    else if (win == "cmd")
    {
        wmove(cmdwin, 0, 1);
        for (int k = 0; k < command.size(); k++)
        {
            wprintw(cmdwin, "%c", command[k]);
        }
        for (int k = command.size(); k < COLS - 1; k++)
            wprintw(cmdwin, "%c", ' ');
        wmove(cmdwin, 0, cmdcur + 1);
        wrefresh(cmdwin);
    }
}
void input_normal_value(char ch, std::string win)
{
    if (win == "edt")
    {
        FILE_MODIFIED = true;
        editor[edt_y].insert(editor[edt_y].begin() + edt_x, ch);
        edt_x++;
        if (IS_BREAK_MODE)
        {
            if (edtcur_x >= EDT_COLS and edtcur_y >= EDT_LINES - 1)
            {
                top_subline++;
            }
            display("edt");
            if (edtcur_x >= EDT_COLS + 1)
            {
                if (edtcur_y + 1 >= EDT_LINES)
                {
                    int elapsed_line;
                    elapsed_line = get_linenum((int)editor[top_line - 1].size()) - top_subline;
                    wmove(edtwin, edtcur_y - elapsed_line + 1, 1); // already exist a char
                }
                else
                    wmove(edtwin, edtcur_y + 1, 1);
            }
            else
                wmove(edtwin, edtcur_y, edtcur_x);
            wrefresh(edtwin);
        }
        else
        {
            if (edtcur_x >= EDT_COLS)
            {
                left_col++;
            }
            display("edt");
        }
    }
    else if (win == "cmd")
    {
        command.insert(command.begin() + cmdcur, ch);
        cmdcur++;
        display("cmd");
        wrefresh(cmdwin);
    }
}
void move_cursor(int key, char mode)
{
    if (mode == 'i' or mode == 'n')
    {
        if (key == KEY_UP)
        {
            remember_pos = std::max(edt_x, remember_pos);
            if (edt_y == top_line) // up out of border
                top_line = std::max(top_line - 1, 0), top_subline = 0;
            edt_y = std::max(edt_y - 1, 0);
            edt_x = std::min(remember_pos, (int)editor[edt_y].size());
            if (!IS_BREAK_MODE and (edt_x < left_col or edt_x > left_col + EDT_COLS))
            {
                left_col = edt_x;
            }
            if (mode == 'n' and edt_x == (int)editor[edt_y].size() and edt_x != 0)
                edt_x--;
            display("edt");
        }
        else if (key == KEY_DOWN)
        {

            // todo:if already at bottom line, dont move
            if (edt_y + 1 < line_cnt)
            {
                remember_pos = std::max(edt_x, remember_pos);
                if (IS_BREAK_MODE)
                {
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
                        top_subline = 0;
                    }
                }
                edt_y = std::min(edt_y + 1, line_cnt);
                edt_x = std::min(remember_pos, (int)editor[edt_y].size());
                if (!IS_BREAK_MODE)
                {
                    if (edt_x < left_col or edt_x > left_col + EDT_COLS)
                        left_col = edt_x;
                    if (edt_y == top_line + EDT_LINES)
                        top_line++;
                }
            }
            wrefresh(infowin);
            if (mode == 'n' and edt_x == (int)editor[edt_y].size() and edt_x != 0)
                edt_x--;
            display("edt");
        }
        else if (key == KEY_LEFT)
        {
            if (IS_BREAK_MODE)
            {
                if (edtcur_x <= 1 and edtcur_y == 0 and edt_y == top_line and get_linenum(edt_x) != 1)
                    top_subline--;
                edt_x = std::max(edt_x - 1, 0);
                if (mode == 'n' and edt_x == (int)editor[edt_y].size() and edt_x != 0)
                    edt_x--;
            }
            else
            {
                if (edt_x != 0)
                {
                    if (edtcur_x == 0)
                        left_col--;
                    else
                        edt_x--;
                    if (mode == 'n' and edt_x == (int)editor[edt_y].size() and edt_x != 0)
                        edt_x--;
                }
            }
            display("edt");
            remember_pos = edt_x;
            wmove(edtwin, edtcur_y, edtcur_x);
            wrefresh(edtwin);
        }
        else if (key == KEY_RIGHT)
        {
            if (IS_BREAK_MODE)
            {
                if (edtcur_x >= EDT_COLS - 1 and edtcur_y == EDT_LINES - 1 and get_linenum(edt_x) != get_linenum((int)editor[edt_y].size() - 1))
                    top_subline++;
                edt_x = std::min(edt_x + 1, (int)editor[edt_y].size());
                if (mode == 'n' and edt_x == (int)editor[edt_y].size() and edt_x != 0)
                    edt_x--;
                display("edt");
                if (edtcur_x == EDT_COLS and get_linenum(edt_x) != get_linenum((int)editor[edt_y].size()))
                    edtcur_y++, edtcur_x = 0;
            }
            else
            {
                if (edt_x != editor[edt_y].size())
                {
                    if (edtcur_x >= EDT_COLS)
                        left_col++;
                    else
                        edt_x++;
                }
                if (mode == 'n' and edt_x == (int)editor[edt_y].size() and edt_x != 0)
                    edt_x--;
                display("edt");
            }
            remember_pos = edt_x;
            wmove(edtwin, edtcur_y, edtcur_x);
            wrefresh(edtwin);
        }
    }
    else if (mode == '\:')
    {
        if (key == KEY_LEFT)
        {
            cmdcur = std::max(0, cmdcur - 1);
            wmove(cmdwin, 0, cmdcur);
        }
        else if (key == KEY_RIGHT)
        {
            cmdcur = std::min((int)command.size(), cmdcur + 1);
            wmove(cmdwin, 0, cmdcur + 1);
        }
        else if (key == KEY_UP)
        {
            command.clear();
            if (cmd_history.size() == 0)
                return;
            cmd_top = std::max(0, cmd_top - 1);
            for (int k = 0; k < cmd_history[cmd_top].cmd.size(); k++)
            {
                command.push_back(cmd_history[cmd_top].cmd[k]);
            }
            for (int i = 0; i < cmd_history[cmd_top].args; i++)
            {
                command.push_back(' ');
                for (int k = 0; k < cmd_history[cmd_top].arg[i].size(); k++)
                {
                    command.push_back(cmd_history[cmd_top].arg[i][k]);
                }
            }
            cmdcur = command.size();
            display("cmd");
        }
        else if (key == KEY_DOWN)
        {
            command.clear();
            cmd_top = std::min(cmd_size, cmd_top + 1);
            if (cmd_top != cmd_size)
            {
                for (int k = 0; k < cmd_history[cmd_top].cmd.size(); k++)
                {
                    command.push_back(cmd_history[cmd_top].cmd[k]);
                }
                for (int i = 0; i < cmd_history[cmd_top].args; i++)
                {
                    command.push_back(' ');
                    for (int k = 0; k < cmd_history[cmd_top].arg[i].size(); k++)
                    {
                        command.push_back(cmd_history[cmd_top].arg[i][k]);
                    }
                }
            }
            else
            {
                command.clear();
            }
            cmdcur = command.size();
            display("cmd");
        }
    }
}
int get_linenum(int key)
{ // e.g: 0~9=1, 10~18=2, ....
    return (key == 0) ? (1) : ((key + EDT_COLS - 1) / EDT_COLS);
}
void show_linenum()
{
    if (IS_BREAK_MODE)
    {
        for (int i = 0; i < NUM_LINES; i++)
        {
            wmove(numwin, i, 0);
            for (int k = 0; k < NUM_COLS; k++)
                wprintw(numwin, "%c", ' ');
        }
        int cnt_line = 0, cur_line = top_line, cur_subline = top_subline;
        while (cnt_line < EDT_LINES)
        {
            if (cur_line >= line_cnt)
            {
                for (int i = cnt_line; i < NUM_LINES; i++)
                {
                    wmove(numwin, i, 0);
                    wprintw(numwin, "%c", '~');
                }
                break;
            }

            int temp_line;
            std::vector<char> line_output({});

            if (ENABLED_LINENUM == 1)
            {
                temp_line = cur_line + 1; // temporary line number value
                while (temp_line)
                {
                    line_output.push_back('0' + temp_line % 10);
                    temp_line /= 10;
                }
                wmove(numwin, cnt_line, NUM_COLS - 1 - line_output.size());
                for (int k = line_output.size() - 1; k >= 0; k--)
                    wprintw(numwin, "%c", line_output[k]);
            }
            else if (ENABLED_LINENUM == 2)
            {
                temp_line = (cur_line - edt_y >= 0) ? (cur_line - edt_y) : (edt_y - cur_line);
                if (temp_line == 0)
                {
                    temp_line = edt_y + 1;
                    while (temp_line)
                    {
                        line_output.push_back('0' + temp_line % 10);
                        temp_line /= 10;
                    }
                    wmove(numwin, cnt_line, 0);
                    for (int k = line_output.size() - 1; k >= 0; k--)
                        wprintw(numwin, "%c", line_output[k]);
                }
                else
                {
                    while (temp_line)
                    {
                        line_output.push_back('0' + temp_line % 10);
                        temp_line /= 10;
                    }
                    wmove(numwin, cnt_line, NUM_COLS - 1 - line_output.size());
                    for (int k = line_output.size() - 1; k >= 0; k--)
                        wprintw(numwin, "%c", line_output[k]);
                }
            }
            int addition_line = get_linenum((int)editor[cur_line++].size()) - cur_subline;
            cur_subline = 0;
            cnt_line += addition_line;
        }
        wrefresh(numwin);
    }
    else
    {
        for (int i = 0; i < NUM_LINES; i++)
        {
            wmove(numwin, i, 0);
            for (int k = 0; k < NUM_COLS; k++)
                wprintw(numwin, "%c", ' ');
        }
        int temp_line;
        for (int i = 0; i < NUM_LINES; i++)
        {
            std::vector<char> line_output({});
            wmove(numwin, i, 0);
            if (top_line + i >= line_cnt)
            {
                for (int k = i; k < NUM_LINES; k++)
                {
                    wmove(numwin, k, 0);
                    wprintw(numwin, "%c", '~');
                }
                break;
            }
            if (ENABLED_LINENUM == 1)
            {
                temp_line = top_line + i + 1; // temporary line number value
                while (temp_line)
                {
                    line_output.push_back('0' + temp_line % 10);
                    temp_line /= 10;
                }
                wmove(numwin, i, NUM_COLS - 1 - line_output.size());
                for (int k = line_output.size() - 1; k >= 0; k--)
                    wprintw(numwin, "%c", line_output[k]);
            }
            else if (ENABLED_LINENUM == 2)
            {
                temp_line = (top_line + i - edt_y >= 0) ? (top_line + i - edt_y) : (edt_y - top_line + i);
                if (temp_line == 0)
                {
                    temp_line = edt_y + 1;
                    while (temp_line)
                    {
                        line_output.push_back('0' + temp_line % 10);
                        temp_line /= 10;
                    }
                    wmove(numwin, i, 0);
                    for (int k = line_output.size() - 1; k >= 0; k--)
                        wprintw(numwin, "%c", line_output[k]);
                }
                else
                {
                    while (temp_line)
                    {
                        line_output.push_back('0' + temp_line % 10);
                        temp_line /= 10;
                    }
                    wmove(numwin, i, NUM_COLS - 1 - line_output.size());
                    for (int k = line_output.size() - 1; k >= 0; k--)
                        wprintw(numwin, "%c", line_output[k]);
                }
            }
        }
    }
}
void process_command()
{
    // process command
    CMD new_cmd;
    bool sub_start = false;
    int k = 0;
    while (command[k] != ' ' and k < command.size())
    {
        new_cmd.cmd.append(std::string(1, command[k]));
        k++;
    }
    for (; k < command.size(); k++)
    {
        if (new_cmd.cmd == "sub")
        {
            if (command[k] == ' ' and !sub_start)
                continue;
            if (!sub_start and command[k] == '\"')
                new_cmd.args++, sub_start = true;
            else if (sub_start and command[k] == '\"')
                sub_start = false;
        }
        else if (command[k] == ' ')
        {
            new_cmd.args++;
            continue;
        }
        new_cmd.arg[new_cmd.args - 1].append(std::string(1, command[k]));
    }
    if (cmd_size == 0 or (!command.empty() and !(new_cmd == cmd_history[cmd_size - 1])))
    {
        cmd_history.push_back(new_cmd);
        cmd_size++;
        cmd_top = cmd_size;
    }
    command.clear();
    wmove(cmdwin, 0, 1);
    display("cmd");

    // execute command
    if (new_cmd.cmd == "w")
    {
        write_file();
    }
    else if (new_cmd.cmd == "q")
    {
        if (!FILE_MODIFIED)
            END_MINIVIM = true;
        else
        {
            wmove(cmdwin, 0, 0);
            wprintw(cmdwin, "File not saved!!");
        }
    }
    else if (new_cmd.cmd == "q!")
        END_MINIVIM = true;
    else if (new_cmd.cmd == "wq")
    {
        if (write_file() and !FILE_MODIFIED)
            END_MINIVIM = true;
    }
    else if (new_cmd.cmd == "set")
    {
        if (new_cmd.arg[0] == "linenum")
        {
            if (new_cmd.arg[1] == "absolute")
                ENABLED_LINENUM = 1;
            else if (new_cmd.arg[1] == "relative")
                ENABLED_LINENUM = 2;
            display("edt");
        }
    }
    else if (new_cmd.cmd == "mv")
    {
        if (new_cmd.args >= 1)
        {
            int linenum = 0;
            for (int i = 0; i < new_cmd.arg[0].size(); i++)
            {
                linenum *= 10;
                linenum += new_cmd.arg[0][i] - '0';
            }
            edt_x = linenum;
        }
        display("edt");
    }
    else if (new_cmd.cmd == "jmp")
    {
        if (new_cmd.args >= 1)
        {
            int linenum = 0;
            for (int i = 0; i < new_cmd.arg[0].size(); i++)
            {
                linenum *= 10;
                linenum += new_cmd.arg[0][i] - '0';
            }
            linenum--;
            if (linenum >= 0 and linenum <= line_cnt - 1)
            {
                top_line = linenum, top_subline = 0;
                edt_y = top_line, edt_x = 0;
                if (!IS_BREAK_MODE)
                    left_col = 0;
                display("edt");
            }
            else
            {
                wmove(cmdwin, 0, 0);
                wprintw(cmdwin, "Invalid Line Number!!");
            }
        }
    }
    else if (new_cmd.cmd == "sub")
    {
        if (new_cmd.args >= 2 and new_cmd.arg[0].size() >= 2 and new_cmd.arg[1].size() >= 2)
        {
            if (new_cmd.arg[0][0] == '\"' and new_cmd.arg[0][new_cmd.arg[0].size() - 1] == '\"' and new_cmd.arg[1][0] == '\"' and new_cmd.arg[1][new_cmd.arg[1].size() - 1] == '\"')
            {
                std::string str_from, str_to;
                str_from = new_cmd.arg[0].substr(1, new_cmd.arg[0].size() - 2);
                str_to = new_cmd.arg[1].substr(1, new_cmd.arg[1].size() - 2);
                if (str_from.size() == 0)
                {
                    wmove(cmdwin, 0, 0);
                    wprintw(cmdwin, "Invalid Arguments!!");
                }
                else if (!READONLY_MODE)
                {
                    FILE_MODIFIED = true;
                    substitute(str_from, str_to);
                    wmove(edtwin, 0, 0);
                    wmove(cmdwin, 0, 0);
                    wprintw(cmdwin, "Successfully substituted!!");
                    wrefresh(cmdwin);
                    wrefresh(edtwin);
                }
                else
                {
                    wmove(cmdwin, 0, 0);
                    wprintw(cmdwin, "File modified in READ-ONLY MODE!!");
                }
            }
            else
            {
                wmove(cmdwin, 0, 0);
                wprintw(cmdwin, "Invalid Arguments!!");
            }
        }
        else
        {
            wmove(cmdwin, 0, 0);
            wprintw(cmdwin, "Invalid Arguments!!");
        }
    }
}
void substitute(std::string from, std::string to)
{
    wmove(cmdwin, 0, 0);
    wrefresh(cmdwin);
    for (int i = 0; i < line_cnt; i++)
    {
        int sub_x = 0;
        while (sub_x <= (int)editor[i].size() - (int)from.size())
        {
            for (int j = sub_x; j < sub_x + (int)from.size(); j++)
            {
                if (editor[i][j] != from[j - sub_x])
                {
                    sub_x++;
                    break;
                }
                if (j == sub_x + (int)from.size() - 1) // do the substitution
                {
                    for (int k = 0; k < (int)from.size(); k++)
                        editor[i].erase(editor[i].begin() + sub_x);
                    for (int k = 0; k < (int)to.size(); k++)
                        editor[i].insert(editor[i].begin() + sub_x + k, to[k]);
                    sub_x += to.size();
                    break;
                }
            }
        }
    }
    top_subline = 0;
    display("edt");
}
bool write_file() // if wq, return if quit or not
{
    CMD new_cmd = cmd_history[cmd_size - 1];
    // w             Truncate mode     Read-only mode    Is tempfile
    // 000   write to that file
    // 001   no existing file
    // 010   if not modified, write to that file, else show its in read-only mode
    // 011   no existing file
    // 100   write to that file
    // 101   no existing file
    // 110   if not modified, write to that file, else show its in read-only mode
    // 111   no existing file
    if (new_cmd.args == 0)
    {
        if (IS_TEMPFILE)
        {
            wmove(cmdwin, 0, 0);
            wprintw(cmdwin, "No existing file!!");
            return false;
        }
        else if (READONLY_MODE and FILE_MODIFIED)
        {
            wmove(cmdwin, 0, 0);
            wprintw(cmdwin, "File modified in READ-ONLY MODE!!");
            return false;
        }
        else
        {
            FILE_MODIFIED = false;
            std::ofstream fwriter(filename);
            for (int i = 0; i < line_cnt; i++)
            {
                if (i != 0)
                    fwriter << '\n';
                for (int k = 0; k < editor[i].size(); k++)
                {
                    fwriter << editor[i][k];
                }
            }
            fwriter.close();
            wmove(cmdwin, 0, 0);
            wprintw(cmdwin, "File successfully saved to ");
            wprintw(cmdwin, filename);
            return true;
        }
    }
    // w [filename]  Truncate mode     Read-only mode    Is tempfile
    // 000   write to [filename]
    // 001   write to [filename]
    // 010   if not modified, write to [filename], else show its in read-only mode
    // 011   if not modified, write to [filename], else show its in read-only mode
    // 100   write to [filename]
    // 101   write to [filename]
    // 110   if not modified, write to [filename], else show its in read-only mode
    // 111   if not modified, write to [filename], else show its in read-only mode
    else
    {
        const char *to_filename = new_cmd.arg[0].c_str();
        if (READONLY_MODE and FILE_MODIFIED)
        {
            wmove(cmdwin, 0, 0);
            wprintw(cmdwin, "File modified in READ-ONLY MODE!!");
            return false;
        }
        else
        {
            FILE_MODIFIED = false;
            std::ofstream fwriter(to_filename);
            for (int i = 0; i < line_cnt; i++)
            {
                if (i != 0)
                    fwriter << '\n';
                for (int k = 0; k < editor[i].size(); k++)
                {
                    fwriter << editor[i][k];
                }
            }
            fwriter.close();
            wmove(cmdwin, 0, 0);
            wprintw(cmdwin, "File successfully saved to ");
            wprintw(cmdwin, to_filename);
            return true;
        }
    }
}
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

    init_pair(STD_COLOR_NUM, COLOR_WHITE, COLOR_BLACK);
    init_pair(EDT_COLOR_NUM, COLOR_WHITE, COLOR_BLACK);
    init_pair(INFO_COLOR_NUM, COLOR_GREEN, COLOR_BLACK);
    init_pair(CMD_COLOR_NUM, COLOR_RED, COLOR_WHITE);
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