## [Project Log]
**2023/11/11** 

read github guidelines

**2023/11/12** 

read ncurses files

basic user interface **10pts**

**2023/11/13**

 insert mode start: display
 
**2023/11/14**

 insert mode cont'd: arrow keys
 
**2023/11/15**

 insert mode almost complete
 
line numbers(2 versions) **35pts**

**2023/11/16**

 preparing for English exam
 
**2023/11/17**

 touch fish
 
**2023/11/18**

 path+relative path **40pts**
 
command mode basics

**2023/11/19** 

file persistence **50pts**

command mode **55pts**

command history **60pts**

command line arguments **65pts**

**2023/11/20** 

normal mode

line number and jump **70pts**

shortcut key **75pts**

**2023/11/21** 
search and subsitution **80pts**

another way of line wrapping **80pts + 10pts**(scrolling)

## [Normal Mode Features]

 - use arrow keys to browse the opened file
 - `dd` - delete the entire line that the cursor is currently on. 
 - `0` -  Move the cursor to the beginning of the line.
 - `$` - Move the cursor to the end of the line.
 - `w` - Move forward one word (delimited by space).
 - `b` - Move backward one word (delimited by space).
## [Command Line Features]

- `minivim [-t] [-R] [-W scroll/break] <filename>`

	`-t` - open file in truncation mode
	
 	`-R` - open file in read-only mode where editing is forbidden
	
 	`-W` - choose line wrapping mode, break is the default mode
- path and relative path is supported !!

## [Command Mode Features]
- `:jmp <Line Number>` - jump to the line number given
- `:sub <from> <to>` - substitute words
- `:w` - save the file
- `:q` - quit
- `:q!` - force quit
- `:wq` - save then quit

## [Special Features]
- `:w <File Path>` - save the current file to the given file path
- `:set linenum [relative/absolute]` - change the way the line number is displayed
- users might encounter potential bugs, we think it's a special feature
