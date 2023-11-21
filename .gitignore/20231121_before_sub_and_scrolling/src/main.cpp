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
            search and subsitution          70
            line number and jump            75
            shortcut key                    80
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
    wprintw(infowin, "%c", TRUNCATE_MODE ? ('1') : ('0'));
    wprintw(infowin, "%c", READONLY_MODE ? ('1') : ('0'));
    wprintw(infowin, "%c", IS_BREAK_MODE ? ('1') : ('0'));
    wprintw(infowin, "%c", IS_TEMPFILE ? ('1') : ('0'));
    if (!IS_TEMPFILE)
        wprintw(infowin, filename);
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
        mvwprintw(infowin, 0, 0, "sub=%d", top_subline);
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
        else if (ch == 'i' or ch == 'I') // insert mode
        {
            dd_alarm = false;
            CURRENT_MODE = 'i';
            break;
        }
        else if (ch == KEY_ESC) // quit minivim
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
            if (edt_x == 0 and edt_y != 0)
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
                    top_subline++;
                display("edt");
            }
            else
            {
                while (1)
                {
                    if (edt_x == editor[edt_y].size() - 1)
                    {
                        if (edt_y != line_cnt - 1)
                            edt_y++, edt_x = 0;
                        if (edtcur_y == EDT_LINES - 1)
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
        else if (ch == 'd')
        {
            if (dd_alarm == false)
                dd_alarm = true;
            else
            {
                dd_alarm = false;
                // if (edt_y == top_line)
                // {
                //     editor.erase(editor.begin() + edt_y);
                // }
                // else
                // {
                if (edt_y == top_line)
                    top_subline = 0, edt_x = 0;
                if (edt_y == line_cnt - 1)
                {
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
                    editor.erase(editor.begin() + edt_y);
                    line_cnt--;
                    edt_x = 0;
                    display("edt");
                }
                // }
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
        for (int k = 0; k < EDT_COLS; k++)
            wprintw(infowin, "%c", ' ');
        mvwprintw(infowin, 0, 0, "LINE:%d COL:%d", edt_y, edt_x);
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
                }
            }
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
    if (win == "edt")
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
        if (edtcur_x >= EDT_COLS and edtcur_y >= EDT_LINES - 1)
        {
            top_line++, top_subline++;
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
            // remember_pos = std::max(edt_x, remember_pos);
            remember_pos = edt_x;
            if (edt_y == top_line) // up out of border
                top_line = std::max(top_line - 1, 0), top_subline = 0;
            edt_y = std::max(edt_y - 1, 0);
            edt_x = std::min(remember_pos, (int)editor[edt_y].size());
            if (mode == 'n' and edt_x == (int)editor[edt_y].size() and edt_x != 0)
                edt_x--;
            display("edt");
        }
        else if (key == KEY_DOWN)
        {

            // todo:if already at bottom line, dont move
            if (edt_y + 1 < line_cnt)
            {
                // remember_pos = std::max(edt_x, remember_pos);
                remember_pos = edt_x;
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
                edt_y = std::min(edt_y + 1, line_cnt);
                edt_x = std::min(remember_pos, (int)editor[edt_y].size());
            }
            wrefresh(infowin);
            if (mode == 'n' and edt_x == (int)editor[edt_y].size() and edt_x != 0)
                edt_x--;
            display("edt");
        }
        else if (key == KEY_LEFT)
        {
            if (edtcur_x <= 1 and edtcur_y == 0 and edt_y == top_line and get_linenum(edt_x) != 1)
                top_subline--;
            edt_x = std::max(edt_x - 1, 0);
            if (mode == 'n' and edt_x == (int)editor[edt_y].size() and edt_x != 0)
                edt_x--;
            display("edt");
            // if (edtcur_x == EDT_COLS)
            //     edtcur_y++, edtcur_x = 0;
            wmove(edtwin, edtcur_y, edtcur_x);
            wrefresh(edtwin);
        }
        else if (key == KEY_RIGHT)
        {
            if (edtcur_x >= EDT_COLS - 1 and edtcur_y == EDT_LINES - 1 and get_linenum(edt_x) != get_linenum((int)editor[edt_y].size() - 1))
                top_subline++;
            edt_x = std::min(edt_x + 1, (int)editor[edt_y].size());
            if (mode == 'n' and edt_x == (int)editor[edt_y].size() and edt_x != 0)
                edt_x--;
            display("edt");
            if (edtcur_x == EDT_COLS and get_linenum(edt_x) != get_linenum((int)editor[edt_y].size()))
                edtcur_y++, edtcur_x = 0;
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
void process_command()
{
    // process command
    CMD new_cmd;
    int k = 0;
    while (command[k] != ' ' and k < command.size())
    {
        new_cmd.cmd.append(std::string(1, command[k]));
        k++;
    }
    for (; k < command.size(); k++)
    {
        if (command[k] == ' ')
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
                display("edt");
            }
            else
            {
                wmove(cmdwin, 0, 0);
                wprintw(cmdwin, "Invalid Line Number!!");
            }
        }
    }
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
