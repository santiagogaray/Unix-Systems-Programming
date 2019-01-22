
/**
 **	Game constants
 **/
#define	BLANK		' '
#define	DFL_SYMBOL	'*'
#define	TOP_ROW		5
#define	BOT_ROW 	20
#define	LEFT_EDGE	10
#define	RIGHT_EDGE	70
#define	X_INIT		10			/* starting col		*/
#define	Y_INIT		10			/* starting row		*/
#define	TICKS_PER_SEC	50		/* affects speed	*/
#define NUM_BALLS		3		/* balls to play */
#define MAX_TICKS		6		/* variance of ball movement */
#define SLOW_ADD		2		/* slowness of ball movement */

/**
 ** Paddle contants
 **/
#define P_INIT_TOP		10		/* initial top position */
#define P_INIT_BOT		15		/* initial bottom position */
#define P_COL			70		/* horizontal position */
#define P_SYMBOL		'#'		/* symbol used to display paddle*/
#define P_UP			-1		/* paddle movement states */
#define P_DOWN			1
#define P_STEADY		0


/**
 **	the ball
 **/
struct ppball {
		int	y_pos, x_pos	, 
			y_ttm, x_ttm	,
			y_ttg, x_ttg	,
			y_dir, x_dir	;
		char	symbol		;

	} ;

/**
 **	the paddle
 **/
struct pppaddle {
		int		top,	
				bot,	
				col,	
				dir;	
		char	symbol;	

	} ;
