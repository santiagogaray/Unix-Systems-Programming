#include	<stdio.h>
#include	<sys/types.h>
#include	<dirent.h>
#include	<sys/stat.h>
#include	<errno.h>
/**
 **	lsr1ff
 **		purpose  list contents of directory and subdirectories
 **		action   if no args, use .  else list files and directories
 **		in args
 **		
 **/

static	short bfirst = 1;
char * classify ( char * name );
char * addpath (char * dirname, char * name);

/**
 ** 
 **		detects parameters type (file or directory)
 **		if file: shows file w/type
 **		if directory: calls list_dir function
 **
 **/
main(int ac, char *av[])
{
	char * id;
	if ( ac == 1 )
		list_dir( "." );
	else
		while ( --ac ){
			id = classify( *++av );				/* get file type  */
			if ( strncmp(id, "error") != 0 ) {
				if ( strncmp(id , "/" ) == 0)	
					list_dir( *av );			/* if a dir, list it */
				else
					printf( "%s%s\n" ,*av, id);	/* else display it */
			}
		}
}

list_dir( char* dirname )
/**
 **		lists files in directory and
 **  	its subdirectories recursively
 **/
{
	char * id;					/* file type */
	char * pathname;			/* Each directory found*/
    	char ** dirbuf;			/* is stored here      */
	int ndirs = 0; int i=0; 
	DIR		*dir_ptr;			/* the directory */
	struct dirent	*direntp;	/* each entry	 */

	if ( ( dir_ptr = opendir( dirname ) ) == NULL ) {
		fprintf(stderr,"lsr1ff: %s: ", dirname);	/*error?*/
		perror();
	} else {
		if ( bfirst == 0 )				/* if not root dir(.)  */
			printf ("%s:\n", dirname);	/* display dir title */
		else
			bfirst = 0;

		dirbuf = (char**) malloc (sizeof(char**));

		while ( ( direntp = readdir( dir_ptr ) ) != NULL ) {
			pathname = addpath(dirname, direntp->d_name); /* build full path*/
			id = classify(pathname);					  /* classify entry */
			printf( "%s%s\n" ,direntp->d_name, id);	      /* display entry */

			if ( strncmp(id , "/") == 0 ) {		/* if it's a directory */
				dirbuf[ndirs]  = pathname;		/* store it in buffer  */
				ndirs++;
				dirbuf = (char**) realloc (dirbuf, sizeof(char**)*(ndirs+1));
			}
		}
		closedir(dir_ptr);				/* close dir file*/
		printf("\n");
	}

	for (i = 0; i < ndirs ; i++) {		/* start displaying*/
		if ( i >= 2 )					/* stored dirs */
			list_dir (dirbuf[i]);		/* in buffer */			
		free (dirbuf[i]);		
	}

}

/**
 **
 **
 **		uses lstat() to
 **		determine the file type.
 **
 **	classify( char *name )
 **		returns the character used in ls 
 **		command to identify different file
 **		types.
 **
 **/
char *
classify( char *name )
{
	struct stat info;	
	if ( lstat(name,&info)!=-1) {			/* get mode info */
		switch(info.st_mode & S_IFMT)		/* check mode */ 
		{
		case S_IFLNK:
			return "@";
			break;
		case S_IFDIR:
			return "/";
			break;
		case S_IFIFO:
			return "|";
			break;
		case S_IFSOCK:
			return "=";
			break;
		default:
			if ((info.st_mode & S_IXUSR) > 0)
				return "*";
			else
				return "";
		}
	} else {
		fprintf(stderr,"lsr1ff: %s: ", name);	/*error?*/
		perror();
	}
}
/**
 **  	it creates a dinamic string and
 ** 	stores the path/name on it.
 **
 **      addpath( char* dir, char* name)
 ** 		returns the resulting string
 **/
char *
addpath (char* dir, char* name)
{
	char * path;

	if (strncmp(dir, ".") == 0) {
							/* if dir is '.' */
		path = (char*) malloc (strlen(name)+1); /* discard it */
		strcpy (path, name);
	} else {
							/*if not add it*/
		path = (char*) malloc (strlen(dir) + strlen(name) + 2);
		strcpy(path, dir); strcat( path, "/"); strcat(path, name);
	}
	return path;
}
