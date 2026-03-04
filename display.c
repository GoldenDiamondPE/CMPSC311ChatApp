/*
HEADER
	- Gives Special Commands
		- User Name
		- Quit

BODY
	- Prints Output from user
		- 1.1.1.1: Hello World :)
	- Dont clear entire screen, save user messages
	- Scrollable depening on message length

TEXT INPUT
	- Message: ____________
	- Limit message length

*/

#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void boarder(WINDOW *win);
void boarderTest(int coll, int row);
void printCentered(WINDOW *win, int y, const char *mesg);

int main(int argc, char *argv[]) {
	int row, coll; // Window Dimesnions
	char *mesg; // Message to Print to Screen
    int height, width, start_y, start_x; // Dim of Main Window
    int ch;
    char userInput[256];
    
	initscr(); // Start ncurses	
	cbreak(); // Recive CMD keyboard Shortcuts
	keypad(stdscr, TRUE); // Full Keyboard
	noecho(); // Dont output user input
	getmaxyx(stdscr,row,coll); // Screen dimensions
	scrollok(stdscr, FALSE);
	//WINDOW *winMain = newwin(height, width, start_y, start_x); // Initialize Windows

	// Sees if termial supports colors and initilizes
	if(has_colors() == FALSE) {	
		endwin();
		printf("Your terminal does not support color\n");
		exit(1);
	} else {
	    start_color();
		init_pair(1, COLOR_WHITE, COLOR_BLUE);
		bkgd(COLOR_PAIR(1));
	}

	/*
	STARTUP MESSAGE
	*/
	clear();
	boarder(stdscr);

	attron(A_BOLD);
	printCentered(stdscr, 3, "BOOT UP"); //Middle of screen display
	attroff(A_BOLD);

	char info[50];
	sprintf(info, "Row: %d Coll: %d", row, coll);
	printCentered(stdscr, 4, info);

	mesg="Press W To Enter Window Mode to Type";
	printCentered(stdscr, 5, "Press W To Enter Window Mode to Type");

	/*
	ch = getch();
	if(ch == 'w' || ch == 'W'){
		mesg="You Pressed w or W";
		mvprintw((row/2)+6, (coll-strlen(mesg))/2, "%s", mesg); 
	}
	*/

	refresh();
	ch = getch();


	clear();

	boarder(stdscr);
	mesg="SHUTTING DOWN";
	mvprintw((row/2), (coll-strlen(mesg))/2, "%s", mesg); //Middle of screen display
	refresh();
	sleep(5);

	attroff(COLOR_PAIR(1));
	endwin();
	return 0;
}


void boarder(WINDOW *win)
{
    int row, col;
    getmaxyx(win, row, col);

    // Top & Bottom
    for (int x = 1; x < col - 1; x++){
        mvwaddch(win, 0, x, 'G');
    	mvwaddch(win, row - 1, x, 'H');
    }
        
    // Left & Right
    for (int y = 1; y < row - 0; y++) {
        mvwaddch(win, y, 0, 'E');
        mvwaddch(win, y, col - 1, 'F');
    }   
    // Corners
    mvwaddch(win, 0, 0, 'A');
    mvwaddch(win, 0, col - 1, 'B');
    mvwaddch(win, row - 1, 0, 'C');
    mvwaddch(win, row - 1, col - 1, 'D');
}


void printCentered(WINDOW *win, int y, const char *mesg) {
    int row, col;
    getmaxyx(win, row, col);

    int x = (col - strlen(mesg)) / 2;
    if (x < 0) x = 0;

    mvwprintw(win, y, x, "%s", mesg);
}