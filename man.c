#include<stdio.h>
#include<stdlib.h>

void print_help()
{
	printf(
		"USAGE\n"
		"\tleast [filename/-h/--help]\n"
		"DESCRIPTION\n"
		"\tleast - minimalist barebones command line pager\n"
		"OPTIONS\n"
		"\tleast [filename] - read filename\n"
		"\tleast [--help/-h] - display this menu and quit\n"
		"COMMANDS\n"
		"\tUp/Down arrow key - Scroll up/down one line if it exists\n"
		"\tI/K - Scroll up/down an entire screen\n"
		"\tRight/Left arrow key - Shift the view by 1 character to the right/left\n"
		"\tTab - Shift the view by 4 characters to the right\n"
		"\tW/S - Go to the start/end of file\n"
		"\tD/A - Show text cut off by terminal/return\n"
		"\tq - Quit program\n"
		);

}