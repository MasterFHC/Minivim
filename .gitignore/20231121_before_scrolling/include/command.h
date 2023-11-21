#pragma once
#include <string>
#include <list>

int cmdcur = 0;

std::vector<char> command;

struct CMD{
    std::string cmd;
    std::string arg[50];
    int args = 0; //number of arguments
    bool operator==(const CMD &rhs) const
    {
        if(rhs.cmd != this->cmd) return false;
        if(rhs.args != this->args) return false;
        for(int it = 0; it < args; it++)
        {
            if(rhs.arg[it] != this->arg[it]) return false;
        }
        return true;
    }
};

std::vector<CMD> cmd_history;
int cmd_size=0;
int cmd_top;