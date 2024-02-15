#define _GNU_SOURCE
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define MAX_PATH_SIZE 1024
#define MAX_LINE 1024


void handle_input(char **, WINDOW *, int, int, FILE *);
void free_lines(char **, int);
void print_help(void);

static int tabcount(char * s)
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

static void init_curses()
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
        if(strcmp(argv[1],"-h") == 0 || strcmp(argv[1],"--help") == 0)
        {
            print_help();
            exit(EXIT_SUCCESS);
        }
        
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
        endwin();
        perror("Error allocating line buffer");
        exit(EXIT_FAILURE);
    }

    while (fgets(line, sizeof(line), f) != NULL) 
    {
       
        line[strcspn(line, "\n")] = '\0';

        buffer[size] = (char *)malloc(strlen(line) + 3 * tabcount(line) +  1);
        

        if (buffer[size] == NULL) 
        {
            endwin();
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
        size++;

        if (size >= capacity) 
        {
            capacity *= 2;
            buffer = realloc(buffer, sizeof(char *) * capacity);
            if (buffer == NULL) 
            {
                endwin();
                perror("Error reallocating memory");
                free_lines(buffer,size);
                if (exfile)
                    fclose(f);
                
                exit(EXIT_FAILURE);
            }
        }
    }
    
    handle_input(buffer,stdscr,size,exfile,f);

    return 0;
}
