#include	<stdio.h>
#include	<termios.h>


/*
 *	flaginfo
 *		data structure used for tables
 *		holding the sttyl command line arguments
 *		and their associated flag value
 *
 */
struct flaginfo { int	fl_value; char	*fl_name; };

struct flaginfo input_flags[] = {

	{	IGNBRK	,	"ignbrk"	},
	{	BRKINT	,	"brkint"	},
	{	IGNPAR	,	"ignpar"	},
	{	PARMRK	,	"parmrk"	},
	{	INPCK	,	"inpck"		},
	{	ISTRIP	,	"istrip"	},
	{	INLCR	,	"inclr"		},
	{	IGNCR	,	"igncr"		},
	{	ICRNL	,	"icrnl"		},
	{	IXON	,	"ixon"		},
	{	IXOFF   ,	"ixoff"		},
	{	0		,	NULL		}	};


struct flaginfo output_flags[] = {

	{	ONLCR	,	"onlcr"		},
	{	OLCUC   ,	"olcuc"		},
	{	OCRNL	,	"ocrnl"		},
	{	ONOCR	,	"onocr"		},
	{	ONLRET	,	"onlret"	},
	{	OFILL	,	"ofill"		},
	{	OFDEL	,	"ofdel"		},
	{	TABDLY	,	"tabs"		},
	{	0		,	NULL		}	};


struct flaginfo local_flags[] = {

	{	ISIG	,	"isig"		},
	{	ICANON	,	"icanon"	},
	{	ECHO	,	"echo"		},
	{	ECHOE	,	"echoe"		},
	{	ECHOK	,	"echok"		},
	{	0		,	NULL		}	};

struct flaginfo cc_ind[] = {

	{	VINTR	,	"intr"		},
	{	VERASE	,	"erase"		},
	{	VKILL	,	"kill"		},
	{	VEOF	,	"eof"		},
	{	VEOL	,	"eol"		},
	{	VSTART	,	"start"		},
	{	VSTOP	,	"stop"		},
	{	VSUSP	,	"susp"		},
	{	0		,	NULL		}	};

/*					
 * sttyl functions 
 */
void show_tty(struct termios info);
void show_flags( int theflag, struct flaginfo flagset[]);
int check_flags( char * param, int * theflag, struct flaginfo flagset[]);
int check_ccvals( char * param[], int isparam, char * thecc);