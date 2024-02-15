#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define MAX_LINE 1024

void free_lines(char **buffer, int size)
{

    for (int i = 0; i < size; i++) {
        free(buffer[i]);
    }

    free(buffer);
}


void handle_input(char ** buffer, WINDOW * w, int size, int exfile, FILE * f)
{
    int lineno;
    int start = 0;
    int shift = 0;
    int max = 0;
    int ch;
    int x,y;

    char opt[MAX_LINE];
    while(1)
    {
        lineno = 0;

        getmaxyx(w,y,x);
        clear();
        
        for (int i = start; i < y - 1 + start; i++) 
        {
            int len = strlen(buffer[i]);
            if(len > max)
                max = len;
            if(shift <= len)
            {
                snprintf(opt,x,"%s",buffer[i] + shift);
                mvprintw(lineno,0,"%s",opt);
            }
            lineno++;
        }

        attron(A_REVERSE);
        
        if(size - y + 1 - start != 0)
            mvprintw(lineno,0,"Lines left: %d (shift: %d)",size - y + 1 - start,shift);
        else 
            mvprintw(lineno,0,"(END)");
        
        attroff(A_REVERSE);
        
        ch = getch();
        
        switch(ch)
        {
            case KEY_UP:
                if(start > 0) start--;
                break;
            case KEY_DOWN:
                if(start < size - y + 1) start++;
                break;
            case KEY_LEFT:
                if(shift > 0) shift--;
                break;
            case KEY_RIGHT:
                shift++;
                break;
            case 's':
                start = size - y + 1;
                break;
            case 'w':
                start = 0;
                break;
            case 'a':
                shift = 0;
                break;
            case 'd':
                if(max >= x) shift = x;
                break;
            case '\t':
                shift +=4;
                break;
            case 'q':
                goto out;
        }

    }

    out:
    
    free_lines(buffer,size);
    endwin();
    if (exfile)
        fclose(f);

}