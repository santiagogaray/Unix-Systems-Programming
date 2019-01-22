#include	"sttyl.h"
#include	<stdio.h>
#include	<string.h>
#include	<termios.h>

/**
 **	sttyl.c
 **
 **	purpose:	Without arguments: Displays current tty settings
 **				With argument list: Sets the specified tty features.
 **
 ** usage:		sttyl [args...]
 **/

int main(int ac, char * av[])
{

	struct termios ttyinfo;

	if ( tcgetattr( 0 , &ttyinfo ) ){			/* get info */
		perror( "cannot get params about stdin");
		exit(1);
	}

	if( ac == 1 ) {			/* display tty settings */
		show_tty(ttyinfo);
		exit (0);
	}

	while (--ac){		/* check command line args */
						/* within argument tables   */

		/* it's a local flag? */
		if ( ! check_flags(*++av, &ttyinfo.c_lflag, local_flags) ) { 
			/* it's an input flag? */
			if ( ! check_flags(*av, &ttyinfo.c_iflag, input_flags) ) {
				/* it's an output flag ? */
				if ( ! check_flags(*av, &ttyinfo.c_oflag, output_flags) ) {
					/* it's an c_cc array option? */
					if( !  check_ccvals( av, ac, ttyinfo.c_cc) ) {
						/* arg not found in tables */	
						fprintf(stderr, "%s: unknown mode\n", *av);	
						exit(1);									
					} else { ac--; av++; } /* skip the extra argument */
				}						   /* used in c_cc setting    */
			}	
		}

	}

	tcsetattr(0, TCSANOW, &ttyinfo);
	return 0;
}

/**
 ** show_tty
 **		Displays the current settings of all the
 **		features found in tty info tables (stty.h)
 **/
void show_tty(struct termios info)
{
	int  i; char val, ctrl;
	/* diaply speed */
	int speed = cfgetospeed( &info );
	printf("sspeed: %d baud", speed);
	printf("\n");
	
	/* dislay cc_c array values */
	for (i=0; cc_ind[i].fl_name ; i++) { 
		ctrl = NULL;
		if (i%6 == 0)	printf("\n");

		if (info.c_cc[cc_ind[i].fl_value] == 255)
			printf("%s = <undef> ; ", cc_ind[i].fl_name );
		else {
			val = info.c_cc[cc_ind[i].fl_value];
			if (val < 32) {
				val = val - 1 + 'A';
				ctrl = '^';
			}
			printf("%s = %c%c ; ", cc_ind[i].fl_name, ctrl, val);
		}

	}
	printf("\n");

	/* display flags values */
	show_flags(info.c_iflag, input_flags);
	show_flags(info.c_oflag, output_flags);
	show_flags(info.c_lflag, local_flags);

}

/**
 ** show_flags
 **		Displays the flag values found in the flagset table
 **/
void show_flags( int theflag, struct flaginfo flagset[])
{


	int	  i; char val;

	for (i=0; flagset[i].fl_value ; i++) {	/*traverse the table */
		val = NULL;
		if (i%8 == 0) printf("\n");
		if (! (theflag & flagset[i].fl_value) )	/* is his flag off? */
			val = '-';							/* append a '-' sign */
		printf("%c%s ; ", val, flagset[i].fl_name); /* display the flag */
	}

	printf("\n");

}

/**
 ** check_flags
 **		checks if the parameter is valid in the flagset table
 **		and sets the corresponding flag value.
 **/
int check_flags( char * param, int * theflag, struct flaginfo flagset[])
{

	int	  i, setoff = 0;
	
	if (param[0] == '-') { setoff = 1; param++; } /* set off the flag */

	for (i=0; flagset[i].fl_value ; i++) {
		if ( strcmp(flagset[i].fl_name, param) == 0) { /* find the flag*/
			if (setoff)
				*theflag &= ~flagset[i].fl_value; /* set it off */
			else
				*theflag |= flagset[i].fl_value;  /* set it on */
			return 1;	/* successful */
		}
	}

	return 0;	/* flag not found in this table */
}

/**
 ** check_ccvals
 **		checks if the parameter is valid in the cc_c array
 **		and sets the specified value
 **/
int check_ccvals( char * param[], int isparam, char * thecc)
{
	int	  i;
	char  ch, * theval, * theparam;
	theparam = param[0];

	for (i=0; cc_ind[i].fl_value ; i++) { /* traverse the cc_c table */

		if ( strcmp(cc_ind[i].fl_name, theparam) == 0) { /* option found? */

			if (isparam == 1) { /* check for value in argument list */
				fprintf(stderr, "sttyl: missing argument to `%s'\n", theparam);
				exit(1);
			}
			theval = param[1]; /* check for apropiate argument value */
			if ( (strlen(theval) == 2 && theval[0] != '^') ||
				  strlen(theval) > 2 ) {
				fprintf(stderr,"sttyl: invalid integer argument '%s'\n", theval);	/*error?*/;
				exit(1);
			}

			if (theval[0] == '^')	/* is a 'Ctrl +' value? */
				ch = theval[1] - 'A' + 1;
			else
				ch = theval[0];

			thecc[cc_ind[i].fl_value] = ch;	/* set the value */

			return 1;	/* successful */
		}
	}

	return 0; /* option not found in this table */
}
