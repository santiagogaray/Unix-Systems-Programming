#include	<curses.h>
#include	<signal.h>
#include	<stdlib.h>

/*
 *	pong.c	
 *			Game based on ball bouncing around a court and
 *			a paddle moved by the user.
 *
 *	Goal:	try to keep the ball in play as much time
 *			as possible
 *
 *	user input:
 *		 	k slow down x component
 *		 	l speed up x component
 *			Q quit
 *
 */

#include	"pong.h"

struct ppball the_ball ;
struct pppaddle the_paddle ;

int	balls_left = NUM_BALLS - 1;

/**
 **	the main loop
 **		setups game and handles paddle movement
 **/

main()
{
	int	c;
	/* init states */
	set_up();

	while ( ( c = getchar()) != 'Q' ){

		if ( c == 'k' && the_paddle.top > TOP_ROW )
			the_paddle.dir = P_UP;
		if ( c == 'm' && the_paddle.bot < BOT_ROW )
			the_paddle.dir = P_DOWN;
	}

	/* put back to normal	*/
	set_ticker( 0 );
	endwin();		
}

/*	
 *	set_up()
 *	init ball and paddle states and displays the scene
 *
 */
set_up()
{
	char szballs_left[32];	/* displays ball left */

	void	on_tick();		/* the signal handler */

	signal( SIGINT , SIG_IGN );

	srand( (unsigned)time( NULL )  );	/* reset random number generator */

	/* the ball init */
	the_ball.y_pos = Y_INIT;
	the_ball.x_pos = X_INIT;
	the_ball.y_ttg = the_ball.y_ttm = rand() % MAX_TICKS + SLOW_ADD;
	the_ball.x_ttg = the_ball.x_ttm = rand() % MAX_TICKS + SLOW_ADD;
	the_ball.y_dir = 1  ;
	the_ball.x_dir = 1  ;
	the_ball.symbol = DFL_SYMBOL ;

	/* the paddle init */
	the_paddle.top = P_INIT_TOP;
	the_paddle.bot = P_INIT_BOT;
	the_paddle.col = P_COL;
	the_paddle.dir = P_STEADY;
	the_paddle.symbol = P_SYMBOL;


	initscr();
	noecho();
	crmode();


	/* draw the court */
	display_court();

	/* draw the ball */
	put_a_char( the_ball.y_pos, the_ball.x_pos, the_ball.symbol  );

	/* draw the paddle */
	put_col_chars( the_paddle.top, the_paddle.bot,
				   the_paddle.col, the_paddle.symbol );

	/* balls left */
	sprintf( szballs_left, "Balls left: %d", balls_left );
	move(LINES-2, 10);
	addstr( szballs_left );

	/* show all */
	refresh();			
	
	signal( SIGALRM, on_tick );			/* start handling signals  */
	set_ticker( 1000 / TICKS_PER_SEC );	/* send millisecs per tick */
}

/*
 *	display_court()
 *		Displays boundaries of court with
 *		specified symbols
 */
display_court()
{
	put_row_chars( LEFT_EDGE-1, RIGHT_EDGE, TOP_ROW-1 , '-' );
	put_row_chars( LEFT_EDGE-1, RIGHT_EDGE, BOT_ROW+1 , '-' );
	put_col_chars( TOP_ROW, BOT_ROW, LEFT_EDGE-1 , '|' );

}

/*
 *	on_tick()
 *		Calls the tick-driven objects' (the_ball and the_paddle)
 *		movement functions. Checks if the_ball bounces or if
 *		it's out of play.
 */
void
on_tick()
{
	signal( SIGALRM , SIG_IGN );   /* ignore signals for now */

	paddle_move();							  /* draw paddle */

	if ( ball_move() ) {						/* draw ball */
						  /* check for bounce or out of play */
		if( bounce_or_lose( &the_ball, &the_paddle ) ) {
			if ( balls_left-- )
				set_up();	/* reset game and play next ball */
			else {
				move(12, 40);
				addstr ("Game Over");		 	  /* is over */
				move(LINES-1,0);
				refresh();
				exit (0);
			}
		}
	}

	move(23,70);
	refresh();									 /* show all */

	signal( SIGALRM, on_tick );
}

/*
 *	paddle_move()
 *		Checks if the the paddle was moved,
 *		displays the paddle in new position
 *		and updates paddle states.
 */
paddle_move()
{
	if ( the_paddle.dir == P_STEADY )	/* no change */
		return;

	if ( the_paddle.dir == P_UP ) {		/* move it up 1 row */
		put_a_char( the_paddle.bot, the_paddle.col, BLANK );
		put_a_char( the_paddle.top-1, the_paddle.col,
					the_paddle.symbol );
	}

	if ( the_paddle.dir == P_DOWN ) {	/* move it down 1 row */
		put_a_char( the_paddle.top, the_paddle.col, BLANK );
		put_a_char( the_paddle.bot+1, the_paddle.col,
					the_paddle.symbol );
	}

	the_paddle.top += the_paddle.dir;	/* update paddle states */
	the_paddle.bot += the_paddle.dir;
	the_paddle.dir = P_STEADY;

}

/*
 *	ball_move()
 *		Cehcks if the ball position changes in
 *		current signal call , updates
 *		states and displays ball.
 */
ball_move()
{
	int	y_cur, x_cur, moved;	/* store previous location */

	y_cur = the_ball.y_pos ;
	x_cur = the_ball.x_pos ;
	moved = 0 ;

	if ( --the_ball.y_ttg == 0 ) {			/* is time for an y move? */
		the_ball.y_pos += the_ball.y_dir ;	/* move	*/
		the_ball.y_ttg = the_ball.y_ttm  ;	/* reset*/
		moved = 1;
	}

	if ( --the_ball.x_ttg == 0 ) {			/* is time for an x move? */
		the_ball.x_pos += the_ball.x_dir ;	/* move	*/
		the_ball.x_ttg = the_ball.x_ttm  ;	/* reset*/
		moved = 1;
	}

	if ( moved ) {							/* draw the ball */
		put_a_char( y_cur, x_cur, BLANK );
		put_a_char( the_ball.y_pos, the_ball.x_pos, 
					the_ball.symbol  );

	}

	return moved;
}

/*
 *	bounce_or_lose( struct ppball * bp, struct pppaddle * pp )
 *		Bounces the ball around the court,
 *		when reaches the right edge, checkes if
 *		the paddle is present and returns 1
 *		if not.
 */
bounce_or_lose(bp, pp)
struct ppball *bp; struct pppaddle *pp;
{
	int	lose = 0 ;

	if ( bp->y_pos == TOP_ROW )
		bp->y_dir = 1;

	if ( bp->y_pos == BOT_ROW )
		bp->y_dir = -1;

	if ( bp->x_pos == LEFT_EDGE )
		bp->x_dir = 1;
	
	if ( bp->x_pos == RIGHT_EDGE-1 ) {
			if ( bp->y_pos >= pp->top && bp->y_pos <= pp->bot ) {
				bp->x_dir = -1;
				bp->y_ttg = bp->y_ttm = rand() % MAX_TICKS + SLOW_ADD;
				bp->x_ttg = bp->x_ttm = rand() % MAX_TICKS + SLOW_ADD;
			}
	}

	if ( bp->x_pos == RIGHT_EDGE + 8 || 
		 bp->y_pos == LINES - 1 ||
		 bp->y_pos == 1 )
		lose = 1;

	return lose;
}

/*
 *	put_a_char(int x, int y, char c)
 *		Places a character at a specified location
 */
put_a_char( y, x , c )
char c;
{
	move(y,x);
	addch(c);
}

/*
 *	put_col_chars(int top, int bot, int col, char s)
 *		Places a column of characters from top to
 *		bottom rows at a specific column
 */
put_col_chars ( top, bot, col, s)
char s;
{
	int row;
	for( row=top; row <= bot; row++)
		put_a_char(row, col, s);

}

/*
 *	put_col_chars(int left, int right, int row, char s)
 *		Places a row of characters from left to
 *		right columns at a specific row
 */
put_row_chars ( left, right, row, s)
char s;
{
	int col;
	for( col=left; col <= right; col++)
		put_a_char(row, col, s);

}


