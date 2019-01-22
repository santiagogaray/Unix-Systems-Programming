/* llast.c */
#include  <stdio.h>
#include  <sys/types.h>
#include  "utmp.h"
#include  <fcntl.h>
#include  <time.h>
#include  <string.h>

/*
 *   llast version 1.0				- read wtmp and list info therein
 */
#define	  SHOWHOST

static	  long t_out = -1;			/* log out time */
static	  short bshut = 0;			/* shutdown handler */
static	  short bcrash = 0;			/* system crash handler */


main(ac, av)
int ac; char *av[];
{
     struct utmp    *wtbufp,		/* holds pointer to next rec  */
				    *wtmp_prev();	/* returns pointer to prev    */
	 char * filename; char * tty; 

	 /* check for correct usage of llast in command line */
	 if ( ac == 1 ){
			printf("Usage: %s [<-f filename>] <[tty]name> \n", av[0]);
			exit(1);
	 }
	 if (strncmp( av[1] , "-f", 2) == 0) {			/* with -f option */
			if ( ac < 4) {
				 printf("Usage: %s [<-f filename>] <[tty]name> \n", av[0]);
				 exit(1);
			} else {
			
				 filename = av[2];					
				 tty = av[3];
			}

	 }  else  {										/* without -f option */
				filename = WTMP_FILE;
		 		tty = av[1];
	 }

	 if ( strncmp( tty, "shutdown") == 0 )	{		/* shutdowns? */
			bshut = 1;
			strcpy(tty, "~");
	 } else 
			if ( strncmp( tty, "tty",3) != 0 )		/* format tty name */
					strcpy( tty, strcat("tty", tty));

	 if ( wtmp_open( filename ) == -1 ){			
			printf("Cannot open file %s\n", filename);
			exit(1);
	 }

	 while ( ( wtbufp = wtmp_prev() ) != NULL )		/* display the */
		 check_info( wtbufp , tty );					/* records */

     wtmp_close( );									
}
/*
 *		   check_info()
 *	       checks the contents of the wtmp recs
 *	       and displays line when info is complete
 *	       
 */
check_info( wtbufp , tty )
struct utmp *wtbufp; char * tty;
{
  if ( strncmp( wtbufp->ut_line, "system boot", 11) == 0 && t_out <= 0 )	{
			bcrash = 1;						/* handle system crashes */
			return;							/* when tty not logged out */
	 } 
	 if (strncmp( tty, wtbufp->ut_line) != 0)	/* if other tty */
			return;								/* skip it  */


	 if ( wtbufp->ut_name[0] == '\0' )	{		/* log out time */
			t_out = wtbufp->ut_time;			/* in this record? */
			bcrash = 0;
			return;
	 }
	 if ( t_out == 0)							/* if one record found */
			t_out = wtbufp->ut_time;			/* for this user */
		 
	 show_info( wtbufp );					/* ready for display user */

	 t_out = 0;								/* reset handlers */
	 bcrash = 0;							/* for next user  */
}

show_info( wtbufp )
struct utmp *wtbufp;
/*
 * Displays time range and elapse time logged in.
 * handles system crashes and still logged in users
 */
{
     char *ctime();	 /* convert long to ascii */
     char *cp;		 /* address for time fields */
	 long tel;		 /* elapse time  */
	 long tin;		 /* log-in time  */

	 printf("%-8.8s", wtbufp->ut_name);	    /* the logname */
     printf("  ");							/* a space     */
     printf("%-8.8s", wtbufp->ut_line);	    /* the tty	    */
     printf("     ");						/* a space     */
	 printf("%-16.16s ", wtbufp->ut_host);	/* the host    */

	 tin = wtbufp->ut_time;
     cp = ctime( &tin );					/* display log-in time */
	 printf("%16.16s", cp);
	 
	 if (t_out != -1 && bshut == 0 && bcrash == 0)	{
			 cp = ctime( &t_out );			/* if log out time found */
			 printf(" - %5.5s", cp+11);		/* display log out time */
			 tel = t_out - tin + 18000; 
			 cp = ctime( &tel );
			 printf("  (%5.5s)", cp+11);	/* and elapsed time */
	 }
													/* if not log out time */
	 if (t_out == -1 && bshut == 0 && bcrash == 0)	/* at the end of file */
			printf("   still logged in");			/* and system not crashed*/
													/* then still logged in */
	 if (bcrash == 1 && bshut == 0)
			printf("   system crash");		/* system crashed */

     printf("\n");							/* new line */


}





