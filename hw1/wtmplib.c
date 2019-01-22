/* wtmp.lib */
#include  <stdio.h>
#include  <fcntl.h>
#include  <sys/types.h>
#include  <utmp.h>

/*
 *   library of functions to do buffered reads from wtmp file
 *
 *   reads NRECS per read and then doles them out from the
 *   buffer
 */
#define	  NRECS	    16
#define	  UTSIZE    (sizeof(struct utmp))

static	  char wtmpbuf[NRECS * UTSIZE];	    /* storage	    */
static	  int  num_recs;					/* num stored   */
static	  int  cur_rec;						/* next to go   */
static	  int  fd_wtmp = -1;				/* read from    */
static	  int  num_rload;					/* reload times */
static    off_t lastpos;					/* record pointer in file */

/*
 *  buffering package for wtmp files
 *  functions are
 *   wutmp_open(filename) - open file
 *		      returns -1 on error
 *   wutmp_prev( )	 - return pointer to previous struct
 *		      returns NULL on eof
 *   wutmp_close()	 - close file
 */

wtmp_open( filename )
char *filename;
{
     fd_wtmp = open( filename, O_RDONLY );	/* open it	 */
	 lastpos = NRECS*UTSIZE;				
     cur_rec = NRECS-1;
	 num_recs = num_rload = 0;				/* no recs yet */
     return fd_wtmp;						/* report */
}

struct utmp *
wtmp_prev()
{
     struct utmp *recp;				 /* holds record  */
     if ( fd_wtmp == -1 )		     /* error ?	      */
			return NULL;

     if ( cur_rec==-1 && wtmp_reload()==0 ) /* any more ?	 */
			return NULL;
	 /* get address of previous record	*/
     recp = (struct utmp *) &wtmpbuf[cur_rec * UTSIZE];
     cur_rec--;
     return recp;
}

static int
wtmp_reload()
/*
 *   read previous bunch of records into buffer
 */
{
     size_t amt_read;		/* amount of bytes read */
	 num_rload++;			/* new reload */

	 if (lastpos < NRECS*UTSIZE )	{
			amt_read = (size_t) lastpos;
			lastpos = lseek(fd_wtmp , 0 , SEEK_SET );
	 }	 else	{
			amt_read = NRECS*UTSIZE;
			lastpos = lseek(fd_wtmp ,(off_t) (NRECS*UTSIZE*num_rload*-1) , SEEK_END );
	 }

     amt_read = read(fd_wtmp , wtmpbuf, amt_read); /* read	 */
			
     num_recs = (int) amt_read/UTSIZE;	/* how many did we get?	 */
	   
	 cur_rec = num_recs - 1;			/* reset pointer    */
     return num_recs;
}

wtmp_close()
{
     if ( fd_wtmp != -1 )		/* don't close if not	 */
			close( fd_wtmp );	/* open	       */
}













