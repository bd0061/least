#define _GNU_SOURCE
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define MAX_PATH_SIZE 1024
#define MAX_LINE 1024

int tabcount(char * s)
{
    int t = 0;
    while(*s)
    {
        if(*s == '\t')
            t++;
        s++;
    }
    return t;
}

void init_curses()
{
   //mora se ovako inicijalizovati da bi radio pipe
    FILE *fs = fopen("/dev/tty", "r+");
    SCREEN *screen = newterm(NULL, fs, fs);
    set_term(screen);
    noecho();
    keypad(stdscr, TRUE);
    raw();
    curs_set(0);
}


void free_lines(char **buffer, int size)
{

    for (int i = 0; i < size; i++) {
        free(buffer[i]);
    }

    free(buffer);
}


int main(int argc, char **argv) {
    int exfile = 0;
    FILE *f = NULL;

    if (argc > 2) 
    {
        fprintf(stderr, "Usage: least <filename>\n");
        exit(EXIT_FAILURE);
    } 
    else if (argc == 2) 
    {
        
        char filepath[MAX_PATH_SIZE];
        
        if (argv[1][0] == '/')
        {
            snprintf(filepath, sizeof(filepath), "%s", argv[1]);
        } 
        else 
        {
            snprintf(filepath, sizeof(filepath), "%s/%s", get_current_dir_name(), argv[1]);
        }
        

        f = fopen(filepath, "r");
        
        if (f == NULL) 
        {
            fprintf(stderr, "Couldn't open %s: %s\n", filepath, strerror(errno));
            exit(EXIT_FAILURE);
        }
        exfile = 1;
    } 
    else 
    {
        f = stdin;
        flushinp();
    }

    char line[MAX_LINE];
    int x, y;
    
    init_curses();
    getmaxyx(stdscr, y, x);
    int capacity = y;
    int size = 0;
    
    char **buffer = (char **)malloc(sizeof(char *) * capacity);

    if (buffer == NULL) 
    {
        perror("Error allocating line buffer");
        exit(EXIT_FAILURE);
    }

    while (fgets(line, sizeof(line), f) != NULL) 
    {
       
        line[strcspn(line, "\n")] = '\0';

        buffer[size] = (char *)malloc(strlen(line) + 3 * tabcount(line) +  1);
        

        if (buffer[size] == NULL) 
        {
            perror("Error allocating memory for another line");

            free_lines(buffer,size);
            
            if (exfile)
                fclose(f);
            
            exit(EXIT_FAILURE);
        }
        
        int linelen = strlen(line);
        int z = 0;
        
        for(int m = 0; m < linelen && z < MAX_LINE; m++)
        {
            if(line[m] == '\t')
            {
                for(int k = 0; k < 4; k++)
                    buffer[size][z++] = ' ';
            }
            else 
            {
                buffer[size][z++] = line[m];
            }
        }

        if(z == MAX_LINE) z--;
        
        buffer[size][z] = '\0';

        //strcpy(buffer[size],line);
        
        size++;

        if (size >= capacity) 
        {
            capacity *= 2;
            buffer = realloc(buffer, sizeof(char *) * capacity);
            if (buffer == NULL) 
            {
                perror("Error reallocating memory");
                
                free_lines(buffer,size);
                
                if (exfile)
                    fclose(f);
                
                exit(EXIT_FAILURE);
            }
        }
    }
    
    int lineno;
    int start = 0;
    int shift = 0;
    int max = 0;
    int ch;
    while(1)
    {
        
        lineno = 0;

        getmaxyx(stdscr,y,x);
        clear();
        
        for (int i = start; i < y - 1 + start; i++) 
        {
            int len = strlen(buffer[i]);
            if(len > max)
                max = len;
            if(shift <= len)
                mvprintw(lineno,0,"%s",buffer[i] + shift);
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

    return 0;
}
