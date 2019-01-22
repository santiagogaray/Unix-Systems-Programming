#include	<stdio.h>
#include	<signal.h>
#include	<string.h>
#include	"smsh_driver.h"
#include	"varlib.h"

extern char **environ;

/* global variables used in flow routine */
int IFSPACE = FALSE;
int THENSPACE = FALSE;
int ELSESPACE = FALSE;
int IFRESULT = UNDEF;

/**		smsh.c											**
 **		Command line:									**
 **			Usage: smsh <filename>						**
 **			Executes the script in filename.			**
 **			If not script file specified, accepts		**
 **			input from stdin.							**
 **		Details:										**
 **			Handles a line of command input				**
 **			in a string.								**
 **			Checks the line through the parser			**
 **			routine.									**
 **			Detects tokens separated by spaces			**
 **			and splits the line in an array of strings.	**
 **			Sends the array to the flow controller		**
 **			routine to process strings.					**
 **			Loops and asks for a new input line.		**/			
int main( int ac, char *av[])
{
	FILE * pstream;		/* the input stream */

	char	*argv[MAXARG + 1], *prompt, *ret,
		cmdline[MAXCMDLEN];
	int	argc;

	/* Define an input stream source */
	if (ac > 1) {			/* is there a file to read from? */
		if( (pstream = fopen( av[1], "r" )) == NULL )
			perror("fopen");
		prompt = "";
	} else {
		pstream = stdin;	/* read from keyboard */
		prompt = DFL_PROMPT ;
	}

	signal(SIGINT,  SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	if ( VLenviron2table(environ) == 0 )
		exit(1);

	/* Main business loop */ 
	while ( getNextCommand( prompt, cmdline, pstream ) == TRUE ) {
			/* parse line and replace variables with values */
		if ( (ret = parseLine( cmdline )) != NULL ) {
			fprintf(stderr, "%s: Variable not defined.\n", ret );
			free(ret);
			continue;
		}
			/* split line in array of args */
		if ( splitline(cmdline, &argc, argv, MAXARG) == FALSE )
			continue;
			/* flow control */
		if ( flowControl(argc, argv) == FALSE )
			fprintf(stderr, "error: unexpected %s statement.\n", argv[0]);
	}
	if ( IFSPACE )
		fprintf(stderr, "error: unexpected EOF");
	return 0;
}


/*
 *  read next line from input_stream.  Return FALSE on EOF
 */
int getNextCommand( char *prompt, char *buffer, FILE *input_stream )
{
	printf("%s", prompt);							/* prompt user	*/
	if ( fgets(buffer, MAXCMDLEN, input_stream) )	/* get line	 */
	{
		buffer[strlen(buffer) - 1] = '\0';			/* remove nl */
		return TRUE;								/* say ok	 */
	}
	return FALSE;									/* no more	 */
}


/*
 *	flowControl( int argc, char *argv[] )
 *		Implements the execution of built-in 
 *		commands or system commands within
 *		an if-then-else structure or without
 *		this structure in a simple sequential
 *		flow.
 *		
 */
int flowControl( int argc, char *argv[] )
{
	int ret_val;

	while(TRUE) {
		if ( strcmp(argv[0], "if") == 0 ){	/* 'if' statement	*/
			if (IFSPACE)
				return FALSE;				/* bad place for an 'if' */
			IFSPACE = TRUE;
			argv++, argc--;					/* point to the next argument */
			break;
		}

		if ( strcmp(argv[0], "then") == 0 ){	/* 'then' statement */
			if ( ! IFSPACE || THENSPACE )
				return FALSE;					/* bad place for a 'then' */
			THENSPACE = TRUE;
			if (IFRESULT == TRUE)
				argv++, argc--;					/* point to the next argument */
			else
				return TRUE;
			break;
		}

		if ( strcmp(argv[0], "else") == 0 ){	/* 'else' statement */
			if (! IFSPACE || ELSESPACE || ! THENSPACE )
				return FALSE;					/* bad place for an 'else' */
			THENSPACE = FALSE;
			ELSESPACE = TRUE;
			if ( IFRESULT == FALSE )
				argv++, argc--;					/* point to the next argument */
			else
				return TRUE;
			break;
		}

		if ( strcmp(argv[0], "fi") == 0 ){		/* 'fi' statement	*/
			if (! IFSPACE && ( ! THENSPACE || ! ELSESPACE ) )
				return FALSE;					/* bad place for a 'fi' */
			IFSPACE = THENSPACE = ELSESPACE = FALSE;
			IFRESULT = UNDEF;
			return TRUE;
		}
		break;
	}

		/* ignore commands in invalid space */
	if ( THENSPACE && IFRESULT == FALSE )
		return TRUE;
	if ( ELSESPACE && IFRESULT == TRUE )
		return TRUE;

		
	if ( argc == 0 )	/* no more commands following if-then-else? */
		return TRUE;

		/* check for a built-in function or system function */
	if ( (ret_val = builtInCommand( argc, argv )) != TRUE )
		ret_val = ! execute( argc, argv );

		/* in IF space a return is important */
	if ( IFSPACE && !THENSPACE && !ELSESPACE ) {
		IFRESULT = ret_val;
	}
	return TRUE;
}


/*	built_in_command(int argc, char *argv[])
 *		Checks for built-in commands and
 *		executes them or return FALSE
 *		if not found.
 */
int builtInCommand(int argc, char *argv[])
{
	char	*cp, * buffer;
	
	if ( (cp=strchr(argv[0], '=')) != NULL ){	/* var=val ?*/
		*cp = '\0';							/* yes			*/
		if ( validateIdent( argv[0] ) )
			VLstore(argv[0], cp+1);			/* add to vartab*/
		else
			return FALSE;
		return TRUE;
	}
	if ( strcmp(argv[0], "set") == 0 ){		/* set command	*/
		VLset();							/* y: do it		*/
		return TRUE;
	}
	if ( strcmp(argv[0], "export") == 0 ){	/* export cmd?	*/
		VLexport(argv[1]);					/* need argc	*/
		return TRUE;						/* check here	*/
	}
	if ( strcmp(argv[0], "exit") == 0 ){	/* exit cmd?	  */
		if (argc > 1)
			exit(atoi(argv[1]));			/* arg with exit? */
		else
			exit(0);
	}
	if ( strcmp(argv[0], "read") == 0 ){		/* read cmd? */
		buffer = malloc(MAXCMDLEN);
		getNextCommand( "", buffer, stdin);		/* get value */
		VLstore(argv[1], buffer);				/* store it	 */
		free(buffer);
		return TRUE;
	}
	if ( strcmp(argv[0], "cd" ) == 0 ){			/* cd cmd ?		  */
		if ( chdir(argv[1]) == -1)				/* error in path? */
			fprintf(stderr, "%s: No such file or directory.\n",argv[1]);
		return TRUE;
	}

	return FALSE;
}


int validateIdent( char * ident ) 
{
	if ( strchr(ident, '.') != NULL )		/* look for '.' */
		return FALSE;

	if	( *ident >= '0' && *ident <= '9')   /* 0-9 in first char ? */
		return FALSE;

	return TRUE;
}
/*
 *	argv is all set to pass to execvp, and argc is nice to know about
 *	but execvp uses the terminating NULL in argv.
 */
int execute(int argc, char * argv[])
{
	int	pid = fork();
	int	child_info;

	if ( pid == -1 )
		perror("fork");
	else if ( pid == 0 ){
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		environ = VLtable2environ();
		execvp(argv[0], argv);
		perror("cannot execute command");
		exit(1);
	}
	else {
		if ( wait( &child_info) == -1 )
			perror("wait");
	}
	return child_info>>8;
}
