#define	TRUE		1
#define	FALSE		0
#define UNDEF		2

#define	MAXARG		20
#define	MAXCMDLEN	512
#define	DFL_PROMPT	"> "

/* smsh_driver.c  functions */
int getNextCommand( char *prompt, char *buffer, FILE *input_stream );
int flowControl( int argc, char *argv[] );
int builtInCommand(int argc, char *argv[]);
int execute(int argc, char * argv[]);

/* smsh_parsers.c functions */
int splitline(char *cmdline, int *argcp, char *argv[], int max);
char* parseLine( char* arg );