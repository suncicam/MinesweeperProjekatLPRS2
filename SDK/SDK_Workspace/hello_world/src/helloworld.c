#include <stdio.h>

#include "xio.h"
#include "bitmap.h"
#include "platform.h"
#include "xparameters.h"
#include "xil_exception.h"
#include "vga_periph_mem.h"

#define UP     0b01000000
#define DOWN   0b00000100
#define LEFT   0b00100000
#define RIGHT  0b00001000

#define SW0 0b00000001

#define SIZE   8
#define WIDTH  8

#define WHITE   1
#define BLACK  -1

typedef enum { NOTHING_PRESSED, SOMETHING_PRESSED } btn_state_t;

/* States of pieces */

enum Piece { DEAD = 0, PAWN = 1, ROOK = 5, KNIGHT = 3, BISHOP = 4, QUEEN = 20, KING = 100 };


/* Custom structures */

/* Coordinates of pieces or squares */
typedef struct point_st {
    int x, y;
} POINT;

/* self explanatory */
typedef struct chess_piece_st {
    POINT point;
    int piece; 
    int color; 
} PIECE;

/* self explanatory */
typedef struct square_st {
    POINT point;
    int color;
    PIECE* piece; // if there is piece on this square, this pointer will point to it else NULL
} SQUARE;


/* Global variables */

/* BLACK player 16 pieces */
static PIECE black[WIDTH<<1] = {};

/* WHITE player 16 pieces */
static PIECE white[WIDTH<<1] = {};

/* 8x8 board matrix */
static SQUARE board[WIDTH][WIDTH] = {};

/* array of posible moves for selected piece */
static POINT playable[27]; // why 27 u ask ?
						   // because it's a theoretical limmit of posible queen moves,
						   // and all other pieces can fit inside 27 moves radius

/* current turn BLACK or WHITE */
static int player_turn = BLACK;

/* if king is dead it's game over */
static int king_is_dead_long_live_the_king = 0;

/* take back flag */
static int flag = 0;

/* Functions */

/* 
	You'd never guess, but this function is used to draw a cursor on screen

	startX, endX, startY, endY - exact pixels where cursor should be drawn
	batman 0 - eraseing cursor 
	batman 1 - draws bright green cursor, current selection
	batman 2 - draws bright blue cursor, posible locations to be played
 */
void draw_cursor(int startX, int startY, int endX, int endY, int batman) {
    unsigned short RGB;
    int x, y, i;

    if (batman == 1)
        RGB = 0x38;

    if (batman == 0) {
        if (((startX-40)/30 + startY/30 ) & 1)
            RGB = 0x163;
        else
            RGB = 0x1F5;
    }

	if (batman == 2)
		RGB = 0x3F;


	// upper edge of cursor square
	for (x = startX; x < endX; x++) {
		for (y = startY; y < startY + 2; y++) {
			i = y * 320 + x;
			VGA_PERIPH_MEM_mWriteMemory(
					XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ i * 4, RGB);
		}
	}

	// bottom edge of cursor square
	for (x = startX; x < endX; x++) {
		for (y = endY - 2; y < endY; y++) {
			i = y * 320 + x;
			VGA_PERIPH_MEM_mWriteMemory(
					XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ i * 4, RGB);
		}
	}

	// left edge of cursor square
	for (x = startX; x < startX + 2; x++) {
		for (y = startY; y < endY; y++) {
			i = y * 320 + x;
			VGA_PERIPH_MEM_mWriteMemory(
					XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ i * 4, RGB);
		}
	}

	// right edge of cursor square
	for (x = endX - 2; x < endX; x++) {
		for (y = startY; y < endY; y++) {
			i = y * 320 + x;
			VGA_PERIPH_MEM_mWriteMemory(
					XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ i * 4, RGB);
		}
	}
}


/* functions used to reset POINT playable array */
void reset_playable() {
	int i;

	for (i = 0; i < 28; i++) {
		playable[i].x = playable[i].y = -1;
	}
}


/* 
	when POINT playable is filled with posible moves, 
	we use this function to mark moves on map with the bright blue color 
*/
void mark_playable() {
	int i, startX, startY, endX, endY;

	for (i = 0; i < 28; i++) {
		// if playble X or Y is equal to -1, it means that we are out of legal moves,
		// so there is no point going further down the playable array
		if (playable[i].x == -1)
			break;

		startX = 40 + playable[i].x*30;
		startY = playable[i].y*30;
		endX = startX + 30;
		endY = startY + 30;

		draw_cursor(startX, startY, endX, endY, 2);
	}
}


/*  
	helper function used to check if the specific square is empty or occupied by a friend or foe
 */
int eatable(POINT pos) {
	if (board[pos.y][pos.x].piece == NULL)
		return 0;	// empty square

	if (board[pos.y][pos.x].piece->color == player_turn)
		return -1; // friendly fire
	else
		return  1; // kill on sight
}

/* 
	calculate posible king moves and fills in playable array
	king - position of king
*/
void move_king(PIECE king) {
    int k = 1;
    POINT pos;

    playable[0].x = king.point.x;
    playable[0].y = king.point.y;

    // is UP move legal ?
    pos.x = king.point.x;

    if (king.point.y != 0) {
        pos.y = king.point.y - 1;

        if ( eatable(pos) != -1 ) {
            playable[k].x = pos.x;
            playable[k].y = pos.y;
            k++;
        }
    }

    // is DOWN move legal ?
    if (king.point.y != WIDTH-1) {
        pos.y = king.point.y + 1;

        if ( eatable(pos) != -1 ) {
            playable[k].x = pos.x;
            playable[k].y = pos.y;
            k++;
        }
    }

    // is LEFT move legal ?
    pos.y = king.point.y;

    if (king.point.x != 0) {
        pos.x = king.point.x - 1;

        if ( eatable(pos) != -1 ) {
            playable[k].x = pos.x;
            playable[k].y = pos.y;
            k++;
        }
    }

    // is RIGHT move legal ?
    if (king.point.x != WIDTH-1) {
        pos.x = king.point.x + 1;

        if ( eatable(pos) != -1 ) {
            playable[k].x = pos.x;
            playable[k].y = pos.y;
            k++;
        }
    }

    // is UP_LEFT move legal ?
    if (king.point.y != 0 && king.point.x != 0) {
        pos.x = king.point.x - 1;
        pos.y = king.point.y - 1;

        if ( eatable(pos) != -1 ) {
            playable[k].x = pos.x;
            playable[k].y = pos.y;
            k++;
        }
    }

    // is UP_RIGHT move legal ?
    if (king.point.y != 0 && king.point.x != WIDTH-1) {
        pos.x = king.point.x + 1;
        pos.y = king.point.y - 1;

        if ( eatable(pos) != -1 ) {
            playable[k].x = pos.x;
            playable[k].y = pos.y;
            k++;
        }
    }

    // is BOTTOM_LEFT move legal ?
    if (king.point.y != WIDTH-1 && king.point.x != 0) {
        pos.x = king.point.x - 1;
        pos.y = king.point.y + 1;

        if ( eatable(pos) != -1 ) {
            playable[k].x = pos.x;
            playable[k].y = pos.y;
            k++;
        }
    }

    // is BOTTOM_RIGHT move legal ?
    if (king.point.y != WIDTH-1 && king.point.x != WIDTH-1) {
        pos.x = king.point.x + 1;
        pos.y = king.point.y + 1;

        if ( eatable(pos) != -1 ) {
            playable[k].x = pos.x;
            playable[k].y = pos.y;
            k++;
        }
    }
}

/* 
	calculate posible queen moves and fills in playable array
	queen - position of queen
*/
void move_queen(PIECE queen) {
    int x, i, j, k = 1;
    POINT pos;

    playable[0].x = queen.point.x;
    playable[0].y = queen.point.y;

    pos.x = queen.point.x; // X is fixed for UP/DOWN
    // are UP moves legal?
    for (i = (queen.point.y - 1); i >= 0; i--) {
        pos.y = i;

        x = eatable(pos);

        if ( x == -1 ) // u shall not pass! (friendly fire)
            break;

        playable[k].x = pos.x;
        playable[k].y = pos.y;
        k++;

        if ( x == 1 ) // u shall not pass! (foe alert)
            break;
    }

    // are BOTTOM moves legal?
    for (i = (queen.point.y + 1); i < WIDTH; i++) {
        pos.y = i;

        x = eatable(pos);

        if ( x == -1 ) // naisla je na figuru svoje boje pa ne sme da jede
            break;

        // dodavanje u playable i povecavanje brojaca
        playable[k].x = pos.x;
        playable[k].y = pos.y;
        k++;

        if ( x == 1 ) // sme da jede ali ne sme dalje
            break;
    }

  
    pos.y = queen.point.y; // Y is fixed for LEFT/RIGHT
    // are LEFT moves legal ?
    for (i = (queen.point.x - 1); i >= 0; i--) {
        pos.x = i;

        x = eatable(pos); 

        if ( x == -1 ) // u shall not pass! (friendly fire)
            break;

        playable[k].x = pos.x;
        playable[k].y = pos.y;
        k++;

        if ( x == 1 ) // u shall not pass! (foe alert)
            break;
    }

    // are RIGHT moves legal ?
    for (i = (queen.point.x + 1); i < WIDTH; i++) {
        pos.x = i;

        x = eatable(pos);

        if ( x == -1 ) // u shall not pass! (friendly fire)
            break;

        // dodavanje u playable i povecavanje brojaca
        playable[k].x = pos.x;
        playable[k].y = pos.y;
        k++;

        if ( x == 1 ) // u shall not pass! (foe alert)
            break;
    }

    // are UP_LEFT moves legal ?
    for (i = (queen.point.x - 1), j = (queen.point.y - 1); i >= 0 && j >= 0; i--, j--) {
        pos.x = i;
        pos.y = j;

        x = eatable(pos);

        if ( x == -1 ) // u shall not pass! (friendly fire)
            break;

        playable[k].x = pos.x;
        playable[k].y = pos.y;
        k++;

        if ( x == 1 ) // u shall not pass! (foe alert)
            break;
    }

    // are UP_RIGHT moves legal ?
    for (i = (queen.point.x + 1), j = (queen.point.y - 1); i < WIDTH && j >= 0; i++, j--) {
        pos.x = i;
        pos.y = j;

        x = eatable(pos);

        if ( x == -1 ) // u shall not pass! (friendly fire)
            break;

        playable[k].x = pos.x;
        playable[k].y = pos.y;
        k++;

        if ( x == 1 ) // u shall not pass! (foe alert)
            break;
    }

    // are BOTTOM_LEFT moves legal ?
    for (i = (queen.point.x - 1), j = (queen.point.y + 1); i >= 0 && j < WIDTH; i--, j++) {
        pos.x = i;
        pos.y = j;

        x = eatable(pos);

        if ( x == -1 ) // u shall not pass! (friendly fire)
            break;

        playable[k].x = pos.x;
        playable[k].y = pos.y;
        k++;

        if ( x == 1 ) // u shall not pass! (foe alert)
            break;
    }

     // are BOTTOM_RIGHT moves legal ?
    for (i = (queen.point.x + 1), j = (queen.point.y + 1); i < WIDTH && j < WIDTH; i++, j++) {
        pos.x = i;
        pos.y = j;

        x = eatable(pos);

        if ( x == -1 ) // u shall not pass! (friendly fire)
            break;

        playable[k].x = pos.x;
        playable[k].y = pos.y;
        k++;

        if ( x == 1 ) // u shall not pass! (foe alert)
            break;
    }
}


/* 
	calculate posible bishop moves and fills in playable array
	bishop - position of bishop
*/
void move_bishop(PIECE bishop) {
    int x, i, j, k = 1;
    POINT pos;

    playable[0].x = bishop.point.x;
    playable[0].y = bishop.point.y;

    // are UP_lEFT moves legal ?
    for (i = (bishop.point.x - 1), j = (bishop.point.y - 1); i >= 0 && j >= 0; i--, j--) {
        pos.x = i;
        pos.y = j;

        x = eatable(pos);

        if ( x == -1 ) // u shall not pass! (friendly fire)
            break;

        playable[k].x = pos.x;
        playable[k].y = pos.y;
        k++;

        if ( x == 1 ) // u shall not pass! (foe alert)
            break;
    }

    // are UP_RIGHT moves legal ?
    for (i = (bishop.point.x + 1), j = (bishop.point.y - 1); i < WIDTH && j >= 0; i++, j--) {
        pos.x = i;
        pos.y = j;

        x = eatable(pos);

        if ( x == -1 ) // u shall not pass! (friendly fire)
            break;

        playable[k].x = pos.x;
        playable[k].y = pos.y;
        k++;

        if ( x == 1 ) // u shall not pass! (foe alert)
            break;
    }

    // are BOTTOM_lEFT moves legal ?
    for (i = (bishop.point.x - 1), j = (bishop.point.y + 1); i >= 0 && j < WIDTH; i--, j++) {
        pos.x = i;
        pos.y = j;

        x = eatable(pos);

        if ( x == -1 ) // u shall not pass! (friendly fire)
            break;

        playable[k].x = pos.x;
        playable[k].y = pos.y;
        k++;

        if ( x == 1 )  // u shall not pass! (foe alert)
            break;
    }

    // are BOTTOM_RIGHT moves legal ?
    for (i = (bishop.point.x + 1), j = (bishop.point.y + 1); i < WIDTH && j < WIDTH; i++, j++) {
        pos.x = i;
        pos.y = j;

        x = eatable(pos);

        if ( x == -1 ) // u shall not pass! (friendly fire)
            break;

        playable[k].x = pos.x;
        playable[k].y = pos.y;
        k++;

        if ( x == 1 ) // u shall not pass! (foe alert)
            break;
    }
}

/* 
	calculate posible knight moves and fills in playable array
	knight - position of knight
*/
void move_knight(PIECE knight){
	int k = 1;
    POINT pos;

    playable[0].x = knight.point.x;
    playable[0].y = knight.point.y;

	// is UP by 2 LEFT by 1 move legal ?
	pos.y = knight.point.y - 2;
	if (knight.point.y > 1 && knight.point.x > 0) {
		pos.x = knight.point.x - 1;
		if ( eatable(pos) != -1 ) {
			playable[k].x = pos.x;
			playable[k].y = pos.y;
			k++;
		}
	}

	// is UP by 2 RIGHT by 1 move legal ?
	if (knight.point.y > 1 && knight.point.x < 7) {
		pos.x = knight.point.x + 1;
		if ( eatable(pos) != -1 ) {
			playable[k].x = pos.x;
			playable[k].y = pos.y;
			k++;
		}
	}

	// is UP by 1 LEFT by 2 move legal ?
	pos.y = knight.point.y - 1;
	if (knight.point.y > 0 && knight.point.x > 1) {
		pos.x = knight.point.x - 2;
		if ( eatable(pos) != -1 ) {
			playable[k].x = pos.x;
			playable[k].y = pos.y;
			k++;
		}
	}

	// is UP by 1 RIGHT by 2 move legal ?
	if (knight.point.y > 0 && knight.point.x < 6) {
		pos.x = knight.point.x + 2;
		if ( eatable(pos) != -1 ) {
			playable[k].x = pos.x;
			playable[k].y = pos.y;
			k++;
		}
	}

	// is BOTTOM by 1 LEFT by 2 move legal ?
	pos.y = knight.point.y + 1;
	if (knight.point.y < 7 && knight.point.x > 1) {
		pos.x = knight.point.x - 2;
		if ( eatable(pos) != -1 ) {
			playable[k].x = pos.x;
			playable[k].y = pos.y;
			k++;
		}
	}

	// is BOTTOM by 1 RIGHT by 2 move legal ?
	if (knight.point.y < 7 && knight.point.x < 6) {
		pos.x = knight.point.x + 2;
		if ( eatable(pos) != -1 ) {
			playable[k].x = pos.x;
			playable[k].y = pos.y;
			k++;
		}
	}

	// is BOTTOM by 2 LEFT by 1 move legal ?
	pos.y = knight.point.y + 2;
	if (knight.point.y < 6 && knight.point.x > 0) {
		pos.x = knight.point.x - 1;
		if ( eatable(pos) != -1 ) {
			playable[k].x = pos.x;
			playable[k].y = pos.y;
			k++;
		}
	}

	// is BOTTOM by 2 RIGHT by 1 move legal ?
	if (knight.point.y < 6 && knight.point.x < 7) {
		pos.x = knight.point.x + 1;
		if ( eatable(pos) != -1 ) {
			playable[k].x = pos.x;
			playable[k].y = pos.y;
			k++;
		}
	}
}

/* 
	calculate posible rook moves and fills in playable array
	rook - position of rook
*/
void move_rook(PIECE rook) {
    int x, i, k = 1;
    POINT pos;

    playable[0].x = rook.point.x;
    playable[0].y = rook.point.y;

    pos.x = rook.point.x; // X is fixed for UP/DOWN

    // are UP moves legal ?
    for (i = (rook.point.y - 1); i >= 0; i--) {
        pos.y = i;

        x = eatable(pos);

        if ( x == -1 ) // u shall not pass! (friendly fire)
            break;

        playable[k].x = pos.x;
        playable[k].y = pos.y;
        k++;

        if ( x == 1 ) // u shall not pass! (foe alert)
            break;
    }

	// are BOTTOM moves legal ?
    for (i = (rook.point.y + 1); i < WIDTH; i++) {
        pos.y = i;

        x = eatable(pos);

        if ( x == -1 ) // u shall not pass! (friendly fire)
            break;

        playable[k].x = pos.x;
        playable[k].y = pos.y;
        k++;

        if ( x == 1 ) // u shall not pass! (foe alert)
            break;
    }

    pos.y = rook.point.y; // Y is fixed for LEFT/RIGHT

    // are LEFT moves legal ?
    for (i = (rook.point.x - 1); i >= 0; i--) {
        pos.x = i;

        x = eatable(pos);

        if ( x == -1 ) // u shall not pass! (friendly fire)
            break;

        playable[k].x = pos.x;
        playable[k].y = pos.y;
        k++;

        if ( x == 1 ) // u shall not pass! (foe alert)
            break;
    }

    // are RIGHT moves legal ?
    for (i = (rook.point.x + 1); i < WIDTH; i++) {
        pos.x = i;

        x = eatable(pos);

        if ( x == -1 ) // u shall not pass! (friendly fire)
            break;

        playable[k].x = pos.x;
        playable[k].y = pos.y;
        k++;

        if ( x == 1 ) // u shall not pass! (foe alert)
            break;
    }
}

/* 
	calculate posible pawn moves and fills in playable array,
	2 sets of identical functionalities, because pawn is the only piece
	who moves different based on color/side

	pawn - position of pawn
*/
void move_pawn(PIECE pawn) {
    int k = 1;
    POINT pos;

    playable[0].x = pawn.point.x;
    playable[0].y = pawn.point.y;

    if (player_turn == WHITE) {

        if (pawn.point.y != 0) {
            pos.y = pawn.point.y - 1;

            if ( pawn.point.x != 0 ) {
                pos.x = pawn.point.x - 1;
                if ( eatable(pos) == 1 ) {
                    playable[k].x = pos.x;
                    playable[k].y = pos.y;
                    k++;
                }
            }

            if ( pawn.point.x != 7 ) {
                pos.x = pawn.point.x + 1;
                if ( eatable(pos) == 1) {
                    playable[k].x = pos.x;
                    playable[k].y = pos.y;
                    k++;
                }
            }

            pos.x = pawn.point.x;
            if ( eatable(pos) == 0 ) {
                playable[k].x = pos.x;
                playable[k].y = pos.y;
                k++;

                pos.y--;
                if (pawn.point.y == 6) {
                    if ( eatable(pos) == 0 ) {
                        playable[k].x = pos.x;
                        playable[k].y = pos.y;
                        k++;
                    }
                }
            }
        }
    } else {
        if (pawn.point.y != 7) {
            pos.y = pawn.point.y + 1;

            if ( pawn.point.x != 0 ) {
                pos.x = pawn.point.x - 1;
                if ( eatable(pos) == 1 ) {
                    playable[k].x = pos.x;
                    playable[k].y = pos.y;
                    k++;
                }
            }

            if ( pawn.point.x != 7 ) {
                pos.x = pawn.point.x + 1;
                if ( eatable(pos) == 1) {
                    playable[k].x = pos.x;
                    playable[k].y = pos.y;
                    k++;
                }
            }

            pos.x = pawn.point.x;
            if ( eatable(pos) == 0 ) {
                playable[k].x = pos.x;
                playable[k].y = pos.y;
                k++;

                pos.y++;
                if (pawn.point.y == 1) {
                    if ( eatable(pos) == 0 ) {
                        playable[k].x = pos.x;
                        playable[k].y = pos.y;
                        k++;
                    }
                }
            }
        }
    }
}

/*
	
	piece - 
 */
PIECE for_whom_the_bell_tolls(PIECE piece) {
    reset_playable();

    switch (piece.piece) {
        case PAWN:
            move_pawn(piece);
            break;
        case ROOK:
            move_rook(piece);
            break;
        case KNIGHT:
            move_knight(piece);
            break;
        case BISHOP:
            move_bishop(piece);
            break;
        case QUEEN:
            move_queen(piece);
            break;
        case KING:
            move_king(piece);
            break;
    }

    mark_playable();

    return piece;
}

/*
	moves piece from one point to the other, and clears it history from the previous
 */
void swap(POINT from, POINT to) {
    if (board[to.y][to.x].piece != NULL) {

    	if(board[to.y][to.x].piece->piece==KING){
    		if(board[to.y][to.x].piece->color==BLACK)
    			king_is_dead_long_live_the_king = 1; // WHITE WON
    		else
    			king_is_dead_long_live_the_king = 2; // BLACK WON
    	}

        board[to.y][to.x].piece->piece = DEAD; // got killed brah
    }

    board[to.y][to.x].piece = board[from.y][from.x].piece;

    board[to.y][to.x].piece->point.x = to.x;
    board[to.y][to.x].piece->point.y = to.y;

    board[from.y][from.x].piece = NULL;
}

/*
	setup board on the game start
 */
void setup_board(SQUARE board[][WIDTH], PIECE black[], PIECE white[]) {
	int x, y;
    for (y = 0; y < WIDTH; y++) {
        for (x = 0; x < WIDTH; x++) {
            board[y][x].point.x = x;
            board[y][x].point.y = y;
            board[y][x].piece = NULL;


            if (y == 0)
                board[y][x].piece = (PIECE*)(black + x);
            if (y == 1)
                board[y][x].piece = (PIECE*)(black + WIDTH + x);
            if (y == 6)
                board[y][x].piece = (PIECE*)(white + WIDTH + x);
            if (y == 7)
                board[y][x].piece = (PIECE*)(white + x);

            if (((x+y) & 1) == 0)
                board[y][x].color = WHITE;
            else
                board[y][x].color = BLACK;
        }
    }
}

/*
	fills in black & white pieces array on start
	order isn't random:
						r k b k q b k r
						p p p p p p p p 
 */
void setup_players(PIECE black[], PIECE white[]) {
    // PAWNS Black & White
	int i;

    for (i = 0; i < WIDTH; i++) {
        black[WIDTH+i].point.x = i;
        black[WIDTH+i].point.y = 1;
        black[WIDTH+i].piece = PAWN;
        black[WIDTH+i].color = BLACK;

        white[WIDTH+i].point.x = i;
        white[WIDTH+i].point.y = 6;
        white[WIDTH+i].piece = PAWN;
        white[WIDTH+i].color = WHITE;
    }

    // ROOKS's Black & White

    black[0].point.x = 0;
    black[0].point.y = 0;
    black[0].piece = ROOK;
    black[0].color = BLACK;

    white[0].point.x = 0;
    white[0].point.y = 7;
    white[0].piece = ROOK;
    white[0].color = WHITE;

    black[7].point.x = 7;
    black[7].point.y = 0;
    black[7].piece = ROOK;
    black[7].color = BLACK;

    white[7].point.x = 7;
    white[7].point.y = 7;
    white[7].piece = ROOK;
    white[7].color = WHITE;

    // KNIGHT's Black & White

    black[1].point.x = 1;
    black[1].point.y = 0;
    black[1].piece = KNIGHT;
    black[1].color = BLACK;

    white[1].point.x = 1;
    white[1].point.y = 7;
    white[1].piece = KNIGHT;
    white[1].color = WHITE;

    black[6].point.x = 6;
    black[6].point.y = 0;
    black[6].piece = KNIGHT;
    black[6].color = BLACK;

    white[6].point.x = 6;
    white[6].point.y = 7;
    white[6].piece = KNIGHT;
    white[6].color = WHITE;

    // BISHOP's Black & White

    black[2].point.x = 2;
    black[2].point.y = 0;
    black[2].piece = BISHOP;
    black[2].color = BLACK;

    white[2].point.x = 2;
    white[2].point.y = 7;
    white[2].piece = BISHOP;
    white[2].color = WHITE;

    black[5].point.x = 5;
    black[5].point.y = 0;
    black[5].piece = BISHOP;
    black[5].color = BLACK;

    white[5].point.x = 5;
    white[5].point.y = 7;
    white[5].piece = BISHOP;
    white[5].color = WHITE;

    // QUEEN's Black & White

    black[4].point.x = 4;
    black[4].point.y = 0;
    black[4].piece = KING;
    black[4].color = BLACK;

    white[4].point.x = 4;
    white[4].point.y = 7;
    white[4].piece = KING;
    white[4].color = WHITE;

    // KING's Black & White

    black[3].point.x = 3;
    black[3].point.y = 0;
    black[3].piece = QUEEN;
    black[3].color = BLACK;

    white[3].point.x = 3;
    white[3].point.y = 7;
    white[3].piece = QUEEN;
    white[3].color = WHITE;
}

/*
	function used to draw a piece on screen

	in  - exact starting pixel in mega-map
	out - position of square on board to be written on, these are indexes not exact pixels
 */
void draw_piece(POINT in, POINT out) {
    int R, G, B, size = 30;
    unsigned short x, y, RGB, tmp = 0;
    int iy, ix, ii, ox, oy, oi;

	for (y = 0; y < size; y++) {
		for (x = 0; x < size; x++) {
			ox = ( 40 + (out.x * size) ) + x;
			oy = ( out.y * size ) + y;
			oi = oy * 320 + ox;

			ix = in.x + x;
			iy = in.y + y;
			ii = iy * bitmap.width + ix;

			// CONVERTS RGB_565 to RGB_333
			// ----------------------------------------------------------------------------------------------------------------------------------------------------

			tmp = ( (unsigned short)bitmap.pixel_data[ii * bitmap.bytes_per_pixel + 1] << 8 ) | (unsigned short)bitmap.pixel_data[ii * bitmap.bytes_per_pixel + 0];

			R =   ((tmp >> 0)  & 0x1f) >> 2;

			G =   ((tmp >> 5)  & 0x3f) >> 3;

			B =   ((tmp >> 11) & 0x1f) >> 2;

			B <<= 6;
			G <<= 3;
			RGB = R | G | B;

			// ----------------------------------------------------------------------------------------------------------------------------------------------------

			VGA_PERIPH_MEM_mWriteMemory(
					XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ oi * 4, RGB);
        }
    }
}


/*
	draws empty squares
 */
void draw_field(POINT out, int color) {
    unsigned short RGB; // beton verzija "crna" ili "bela"
    int x, y;
    int ox, oy, oi;

    size_t size = 30;

    RGB = (color == BLACK ? 0x163 : 0x1F5);

	for (y = 0; y < size; y++) {
		for (x = 0; x < size; x++) {
			ox = ( 40 + ( out.x * size) ) + x;
			oy = ( out.y * size ) + y;
			oi = oy * 320 + ox;

			VGA_PERIPH_MEM_mWriteMemory(
					XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ oi * 4, RGB);
        }
    }
}

/*
	You'd never guess

	bord - board
*/
void draw_board(SQUARE board[][WIDTH]) {
    POINT in, out;
    int x, y;

    out.x = out.y = 0;

    for (y = 0; y < WIDTH; y++, out.y++) {
    	out.x = 0;
        for (x = 0; x < WIDTH; x++, out.x++) {
            if (board[y][x].piece != NULL)  {

                switch (board[y][x].piece->piece) {

                    case PAWN:

                        in.x = 0;

                        if ( board[y][x].color == WHITE && board[y][x].piece->color == WHITE ) {
                            in.y = 0;
                            draw_piece(in, out);
                        }
                        else if ( board[y][x].color == WHITE && board[y][x].piece->color == BLACK ) {
                            in.y = 30;
                            draw_piece(in, out);
                        }
                        else if ( board[y][x].color == BLACK && board[y][x].piece->color == WHITE ) {
                            in.y = 60;
                            draw_piece(in, out);
                        }
                        else {
                            in.y = 90;
                            draw_piece(in, out);
                        }
                        break;

                    case ROOK:

                        in.x = 30;

                        if ( board[y][x].color == WHITE && board[y][x].piece->color == WHITE ) {
                            in.y = 0;
                            draw_piece(in, out);
                        }
                        else if ( board[y][x].color == WHITE && board[y][x].piece->color == BLACK ) {
                            in.y = 30;
                            draw_piece(in, out);
                        }
                        else if ( board[y][x].color == BLACK && board[y][x].piece->color == WHITE ) {
                            in.y = 60;
                            draw_piece(in, out);
                        }
                        else {
                            in.y = 90;
                            draw_piece(in, out);
                        }
                        break;

                    case KNIGHT:

                        in.x = 60;

                        if ( board[y][x].color == WHITE && board[y][x].piece->color == WHITE ) {
                            in.y = 0;
                            draw_piece(in, out);
                        }
                        else if ( board[y][x].color == WHITE && board[y][x].piece->color == BLACK ) {
                            in.y = 30;
                            draw_piece(in, out);
                        }
                        else if ( board[y][x].color == BLACK && board[y][x].piece->color == WHITE ) {
                            in.y = 60;
                            draw_piece(in, out);
                        }
                        else {
                            in.y = 90;
                            draw_piece(in, out);
                        }
                        break;

                    case BISHOP:

                        in.x = 90;

                        if ( board[y][x].color == WHITE && board[y][x].piece->color == WHITE ) {
                            in.y = 0;
                            draw_piece(in, out);
                        }
                        else if ( board[y][x].color == WHITE && board[y][x].piece->color == BLACK ) {
                            in.y = 30;
                            draw_piece(in, out);
                        }
                        else if ( board[y][x].color == BLACK && board[y][x].piece->color == WHITE ) {
                            in.y = 60;
                            draw_piece(in, out);
                        }
                        else {
                            in.y = 90;
                            draw_piece(in, out);
                        }
                        break;

                    case QUEEN:

                        in.x = 120;

                        if ( board[y][x].color == WHITE && board[y][x].piece->color == WHITE ) {
                            in.y = 0;
                            draw_piece(in, out);
                        }
                        else if ( board[y][x].color == WHITE && board[y][x].piece->color == BLACK ) {
                            in.y = 30;
                            draw_piece(in, out);
                        }
                        else if ( board[y][x].color == BLACK && board[y][x].piece->color == WHITE ) {
                            in.y = 60;
                            draw_piece(in, out);
                        }
                        else {
                            in.y = 90;
                            draw_piece(in, out);
                        }
                        break;

                    case KING:

                        in.x = 150;

                        if ( board[y][x].color == WHITE && board[y][x].piece->color == WHITE ) {
                            in.y = 0;
                            draw_piece(in, out);
                        }
                        else if ( board[y][x].color == WHITE && board[y][x].piece->color == BLACK ) {
                            in.y = 30;
                            draw_piece(in, out);
                        }
                        else if ( board[y][x].color == BLACK && board[y][x].piece->color == WHITE ) {
                            in.y = 60;
                            draw_piece(in, out);
                        }
                        else {
                            in.y = 90;
                            draw_piece(in, out);
                        }
                        break;
                    default:
                        break;
                }
            } else {
                if (board[y][x].color == WHITE)
                    draw_field(out, WHITE);
                else
                    draw_field(out, BLACK);
            }
        }
    }
}

/*
	gray - black or white piece array
	
	when u press the switch you will iterate trough the gray array
 */
PIECE select(PIECE gray[]) {
	int startX, startY, endX, endY, cnt, i = 0;

	btn_state_t btn_state = NOTHING_PRESSED;

	// skipping the dead pieces
	while(gray[i].piece == DEAD) i++;

	startX=gray[i].point.x*30+40;
	startY=gray[i].point.y*30;
	endX=startX+30;
	endY=startY+30;
	draw_cursor(startX, startY, endX, endY, 1);

	while((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & SW0) == 0) {
		if (btn_state == NOTHING_PRESSED) {
			btn_state = SOMETHING_PRESSED;

			if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & DOWN) == 0) {
				draw_cursor(startX, startY, endX, endY, 0);

				if (i > 7) {

					i -= 8;

					// skipping the dead pieces
					while(gray[i].piece==DEAD) {

						if (i == 0)
							i = 15;
						else
							i--;
					}

					startX = gray[i].point.x * 30 + 40;
					startY = gray[i].point.y * 30;
					endX=startX+30;
					endY=startY+30;
				}

				draw_cursor(startX, startY, endX, endY, 1);
			}
			else if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & UP) == 0) {
				draw_cursor(startX, startY, endX, endY, 0);

				if (i < 8) {

					i += 8;

					// skipping the dead pieces
					while(gray[i].piece==DEAD) {

						if (i == 16)
							i = 0;
						else
							i++;
					}

					startX = gray[i].point.x * 30 + 40;
					startY = gray[i].point.y * 30;
					endX=startX+30;
					endY=startY+30;
				}

				draw_cursor(startX, startY, endX, endY, 1);
			}
			else if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & RIGHT) == 0) {
				draw_cursor(startX, startY, endX, endY, 0);

				if(i < 16) {

					i++;

					// skipping the dead pieces
					while(gray[i].piece == DEAD || i == 16) {

						if (i == 16)
							i = 0;
						else
							i++;
					}

					startX = gray[i].point.x * 30 + 40;
					startY = gray[i].point.y * 30;
					endX=startX+30;
					endY=startY+30;
				}

				draw_cursor(startX, startY, endX, endY, 1);
			}
			else if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & LEFT) == 0) {
				draw_cursor(startX, startY, endX, endY, 0);

				if(i > 0) {

					i--;

					// skipping the dead pieces
					while(gray[i].piece==DEAD) {

						if (i == 0)
							i = 15;
						else
							i--;
					}

				} else {
					i = 15;

					// skipping the dead pieces
					while(gray[i].piece==DEAD) {

						if (i == 0)
							i = 15;
						else
							i--;
					}
				}

				startX = gray[i].point.x * 30 + 40;
				startY = gray[i].point.y * 30;
				endX=startX+30;
				endY=startY+30;

				draw_cursor(startX, startY, endX, endY, 1);
			}
		}


		for(cnt=0; cnt < 1500000; cnt++) {
			VGA_PERIPH_MEM_mWriteMemory(
					XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ 0 * 4, 0x0);
		}

		btn_state = NOTHING_PRESSED;
	}

	return gray[i];
}

/*
	almost all previous function are used here, this is core game logic
 */
void play_playable(PIECE gray[]) {
	int startX, startY, endX, endY, cnt, i = 0;
	PIECE piece;

	btn_state_t btn_state = NOTHING_PRESSED;

	flag = 0;

	reset_playable();

	draw_board(board);
	piece = for_whom_the_bell_tolls( select( gray ));

	startX=piece.point.x*30+40;
	startY=piece.point.y*30;
	endX=startX+30;
	endY=startY+30;

	draw_cursor(startX, startY, endX, endY, 1);

	while((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & SW0) != 0) {
		if (btn_state == NOTHING_PRESSED) {
			btn_state = SOMETHING_PRESSED;


			if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & RIGHT) == 0) {
				draw_cursor(startX, startY, endX, endY, 2);

				if(i < 28) {

					i++;

					while (playable[i].x == -1 && i < 28) {
						i++;

						if (i == 28)
							i = 0;
					}

					startX = playable[i].x * 30 + 40;
					startY = playable[i].y * 30;
					endX=startX+30;
					endY=startY+30;
				}

				draw_cursor(startX, startY, endX, endY, 1);
			}
			else if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & LEFT) == 0) {
				draw_cursor(startX, startY, endX, endY, 2);

				if(i >= 0) {

					i--;

					if (i == -1)
						i = 27;

					while(playable[i].x == -1 && i >= 0) {
						i--;

						if (i == -1)
							i = 27;
					}

					startX = playable[i].x * 30 + 40;
					startY = playable[i].y * 30;
					endX=startX+30;
					endY=startY+30;
				}

				draw_cursor(startX, startY, endX, endY, 1);
			}
		}

		for(cnt = 0; cnt < 1500000; cnt++) {
			VGA_PERIPH_MEM_mWriteMemory(
					XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ 0 * 4, 0x0);
		}

		btn_state = NOTHING_PRESSED;
	}

	if(piece.point.x == playable[i].x && piece.point.y == playable[i].y) //
		flag = 1;
	else
		swap(piece.point, playable[i]);
}

/*
 	 Victory celebration
 */

void victory(PIECE gray[]) {
	int i, j;

	for(i=0; i<8; i++) {
		for (j=0; j<8; j++) {
			board[i][j].piece = NULL;
		}
	}

	/*
	gray[0].piece = PAWN;
	gray[0].point.x = 1;
	gray[0].point.y = 3;
	board[3][1].piece = &gray[0];

	gray[1].piece = PAWN;
	gray[0].point.x = 1;
	gray[0].point.y = 4;
	board[4][1].piece = &gray[1];

	gray[2].piece = PAWN;
	gray[0].point.x = 1;
	gray[0].point.y = 5;
	board[5][1].piece = &gray[2];

	gray[3].piece = PAWN;
	gray[0].point.x = 2;
	gray[0].point.y = 6;
	board[6][2].piece = &gray[3];

	gray[4].piece = PAWN;
	gray[0].point.x = 3;
	gray[0].point.y = 5;
	board[5][3].piece = &gray[4];

	gray[5].piece = KING;
	gray[0].point.x = 3;
	gray[0].point.y = 4;
	board[4][3].piece = &gray[5];

	gray[6].piece = QUEEN;
	gray[0].point.x = 4;
	gray[0].point.y = 4;
	board[4][4].piece = &gray[6];

	gray[7].piece = PAWN;
	gray[0].point.x = 4;
	gray[0].point.y = 5;
	board[5][4].piece = &gray[7];

	gray[8].piece = PAWN;
	gray[0].point.x = 5;
	gray[0].point.y = 6;
	board[6][5].piece = &gray[8];

	gray[9].piece = PAWN;
	gray[0].point.x = 6;
	gray[0].point.y = 3;
	board[3][6].piece = &gray[9];

	gray[10].piece = PAWN;
	gray[0].point.x = 6;
	gray[0].point.y = 4;
	board[4][6].piece = &gray[10];

	gray[11].piece = PAWN;
	gray[0].point.x = 6;
	gray[0].point.y = 5;
	board[5][6].piece = &gray[11];
	*/

	gray[0].piece = PAWN;
	gray[0].point.x = 2;
	gray[0].point.y = 1;
	board[1][2].piece = &gray[0];

	gray[1].piece = PAWN;
	gray[0].point.x = 2;
	gray[0].point.y = 2;
	board[2][2].piece = &gray[1];

	gray[2].piece = PAWN;
	gray[0].point.x = 2;
	gray[0].point.y = 3;
	board[3][2].piece = &gray[2];

	gray[3].piece = PAWN;
	gray[0].point.x = 2;
	gray[0].point.y = 4;
	board[4][2].piece = &gray[3];

	gray[4].piece = PAWN;
	gray[0].point.x = 2;
	gray[0].point.y = 5;
	board[5][2].piece = &gray[4];

	gray[5].piece = PAWN;
	gray[0].point.x = 2;
	gray[0].point.y = 6;
	board[6][2].piece = &gray[5];

	gray[6].piece = PAWN;
	gray[0].point.x = 5;
	gray[0].point.y = 1;
	board[1][5].piece = &gray[6];

	gray[7].piece = PAWN;
	gray[0].point.x = 5;
	gray[0].point.y = 2;
	board[2][5].piece = &gray[7];

	gray[8].piece = PAWN;
	gray[0].point.x = 5;
	gray[0].point.y = 3;
	board[3][5].piece = &gray[8];

	gray[9].piece = PAWN;
	gray[0].point.x = 5;
	gray[0].point.y = 4;
	board[4][5].piece = &gray[9];

	gray[10].piece = PAWN;
	gray[0].point.x = 5;
	gray[0].point.y = 5;
	board[5][5].piece = &gray[10];

	gray[11].piece = PAWN;
	gray[0].point.x = 5;
	gray[0].point.y = 6;
	board[6][5].piece = &gray[11];

	gray[12].piece = PAWN;
	gray[0].point.x = 3;
	gray[0].point.y = 1;
	board[1][3].piece = &gray[12];

	gray[13].piece = PAWN;
	gray[0].point.x = 4;
	gray[0].point.y = 1;
	board[1][4].piece = &gray[13];

}

/*
	Game logic
 */
void game() {
	int x, y, i;

	// Mihajlo Solda's gay background
	for (x = 0; x < 320; x++) {
		for (y = 0; y < 240; y++) {
			i = y * 320 + x;
			VGA_PERIPH_MEM_mWriteMemory(
					XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF + i * 4, 0x0);
		}
	}

    setup_players(black, white);
    setup_board(board, black, white);

    draw_board(board);

    while (!king_is_dead_long_live_the_king) {
    	player_turn = WHITE;

    	do {
    		play_playable(white);
    	} while (flag);

		draw_board(board);

		if(king_is_dead_long_live_the_king)	break;

		player_turn = BLACK;

		do {
			play_playable(black);
		} while (flag);

		draw_board(board);
    }

    if (king_is_dead_long_live_the_king == 1)
    	victory(white);
    else
    	victory(black);

    draw_board(board);
}

/*-----------------------------------MAIN--------------------------------------*/

int main() {

	init_platform();

	VGA_PERIPH_MEM_mWriteMemory(
			XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x00, 0x0); // direct mode   0
	VGA_PERIPH_MEM_mWriteMemory(
			XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x04, 0x3); // display_mode  1
	VGA_PERIPH_MEM_mWriteMemory(
			XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x08, 0x0); // show frame      2
	VGA_PERIPH_MEM_mWriteMemory(
			XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x0C, 0xff); // font size       3
	VGA_PERIPH_MEM_mWriteMemory(
			XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x10, 0xFFFFFF); // foreground 4
	VGA_PERIPH_MEM_mWriteMemory(
			XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x14, 0x0000FF); // background color 5
	VGA_PERIPH_MEM_mWriteMemory(
			XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x18, 0xFF0000); // frame color      6
	VGA_PERIPH_MEM_mWriteMemory(
			XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x20, 1);

	game();

	cleanup_platform();

	return 0;
}

/*-----------------------------------MAIN--------------------------------------*/
