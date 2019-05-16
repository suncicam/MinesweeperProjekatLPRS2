#include <stdio.h>
#include <stdlib.h>

/* Defined variables */
#define EMPTY       0
#define BLACK       1
#define WHITE      -1
#define WIDTH       8
#define TABLE_SIZE 64

/* Functions used */

// Printing chess table
void print_table();
// Printing fields available to play
void print_available(int arr[], int size);
// Moving logic for all the figures
void move_pawn(int pos, int turn);
void move_rook(int pos, int turn);
void move_knight(int pos, int turn);
void move_bishop(int pos, int turn);
void move_queen(int pos, int turn);
void move_king(int pos, int turn);

// Checking if target is eatable
int eatable(int x, int turn);
/*
 Detecting specific corner with [X, Y] cordinate (-1, -1) bottom left,
                                                 (-1,  1) upper left,
                                                 ( 1, -1) bottom right,
                                                 ( 1,  1) upper right,
or we can detect specific side (-1,  0) left,
                               ( 1,  0) right
                               ( 0, -1) bottom,
                               ( 0,  1) upper.
*/
int edge_detection(int pos, int x, int y);
int cursor(int turn);

/* Custom structures used */

/* Globar variables */
// EMPTY  0    0
// PAWN   1   -1
// ROOK   5   -5
// KNIGHT 3   -3
// BISHOP 4   -4
// QUEEN  20  -20
// KING   100 -100

static int game = 0;            // win or lose
static int king_flag = 0;       // warning when the king is attacked, too hard !!!
static int player_turn = WHITE; // who's turn?
static int chess_table[TABLE_SIZE] = { -5, -3, -4, -100, -20, -4, -3, -5,
                                       -1, -1, -1,   -1,  -1, -1, -1, -1,
                                        0,  0,  0,    0,   0,  0,  0,  0,
                                        0,  0,  0,    0,   0,  0,  0,  0,
                                        0,  0,  0,    0,   0,  0,  0,  0,
                                        0,  0,  0,    0,   0,  0,  0,  0,
                                        1,  1,  1,    1,   1,  1,  1,  1,
                                        5,  3,  4,  100,  20,  4,  3,  5 };

int main() {

    print_table();
    move_knight(39, BLACK);

    return 0;
}

void move_king(int pos, int turn) {
    int available[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };

    int x, j = 0;

    if ( !edge_detection(pos, -1, 1) ) {
        x = pos - WIDTH - 1;
        if ( eatable(x, turn) || chess_table[x] == EMPTY )
            available[j++] = x;
    }

    if ( !edge_detection(pos, 0, 1) ) {
        x = pos - WIDTH;
        if ( eatable(x, turn) || chess_table[x] == EMPTY )
            available[j++] = x;
    }

    if ( !edge_detection(pos, 1, 1) ) {
        x = pos - WIDTH + 1;
        if ( eatable(x, turn) || chess_table[x] == EMPTY )
            available[j++] = x;
    }

    if ( !edge_detection(pos, -1, 0) ) {
        x = pos - 1;
        if ( eatable(x, turn) || chess_table[x] == EMPTY )
            available[j++] = x;
    }

    if ( !edge_detection(pos, 1, 0) ) {
        x = pos + 1;
        if ( eatable(x, turn) || chess_table[x] == EMPTY )
            available[j++] = x;
    }

    if ( !edge_detection(pos, -1, -1) ) {
        x = pos + WIDTH - 1;
        if ( eatable(x, turn) || chess_table[x] == EMPTY )
            available[j++] = x;
    }

    if ( !edge_detection(pos, 0, -1) ) {
        x = pos + WIDTH;
        if ( eatable(x, turn) || chess_table[x] == EMPTY )
            available[j++] = x;
    }

    if ( !edge_detection(pos, 1, -1) ) {
        x = pos + WIDTH + 1;
        if ( eatable(x, turn) || chess_table[x] == EMPTY )
            available[j++] = x;
    }

    print_available(available, 8);
}

void move_queen(int pos, int turn) {
    int available[29] = { -1, -1, -1, -1, -1, -1, -1,
                          -1, -1, -1, -1, -1, -1, -1,
                          -1, -1, -1, -1, -1, -1, -1,
                          -1, -1, -1, -1, -1, -1, -1,
                          -1 };

    int x, j = 0;

    // Up
    for ( int i = 1; i <= pos / WIDTH; i++ ) {
        x = pos - i * WIDTH;
        if ( chess_table[x] != EMPTY) {
            if ( eatable(x, turn) )
                available[j++] = x;
            break;
        } else {
            available[j++] = x;
        }
    }

    // Down
    for ( int i = 1; i < ( WIDTH - ( pos / WIDTH ) ); i++ ) {
        x = pos + i * WIDTH;
        if ( chess_table[x] != EMPTY ) {
            if ( eatable(x, turn) )
                available[j++] = x;
            break;
        } else {
            available[j++] = x;
        }
    }

    // Right
    for ( int i = 1; i < ( WIDTH - ( pos % WIDTH ) ); i++ )
    {
        x = pos + i;
        if ( chess_table[x] != EMPTY ) {
            if ( eatable(x, turn) )
                available[j++] = x;
            break;
        } else {
            available[j++] = x;
        }
    }
    // Left
    for ( int i = 1; i <= pos % WIDTH; i++ ) {
        x = pos - i;
        if ( chess_table[x] != EMPTY ) {
            if (  eatable(x, turn) )
                available[j++] = x;
            break;
        } else {
            available[j++] = x;
        }
    }

    // Diagonal, upper left
    if ( !edge_detection(pos, -1, 1) ) {
        for (int i = 1; i < WIDTH; i++) {
            x = pos - i*WIDTH - i;

            if ( eatable(x, turn) ) {
                available[j++] = x;
                break;
            }

            if ( chess_table[x] == EMPTY ) {
                available[j++] = x;
                if ( edge_detection(x, -1, 1) )
                    break;
                    continue;
            }

            break;
        }
    }

    // Diagonal, upper right
    if ( !edge_detection(pos, 1, 1) ) {
        for (int i = 1; i < WIDTH; i++) {
            x = pos - i*WIDTH + i;

            if ( eatable(x, turn) ) {
                available[j++] = x;
                break;
            }

            if ( chess_table[x] == EMPTY ) {
                available[j++] = x;
                if ( edge_detection(x, 1, 1) )
                    break;
                continue;
            }

            break;
        }
    }

    // Diagonal, bottom left
    if ( !edge_detection(pos, -1, -1) ) {
        for (int i = 1; i < WIDTH; i++) {
            x = pos + i*WIDTH - i;

            if ( eatable(x, turn) ) {
                available[j++] = x;
                break;
            }

            if ( chess_table[x] == EMPTY ) {
                available[j++] = x;
                if ( edge_detection(x, -1, -1) )
                    break;
                continue;
            }

            break;
        }
    }

    // Diagonal, bottom right
    if ( !edge_detection(pos, 1, -1) ) {
        for (int i = 1; i < WIDTH; i++) {
            x = pos + i*WIDTH + i;

            if ( eatable(x, turn) ) {
                available[j++] = x;
                break;
            }

            if ( chess_table[x] == EMPTY ) {
                available[j++] = x;
                if ( edge_detection(x, 1, -1) )
                    break;
                continue;
            }

            break;
        }
    }

    print_available(available, 29);
}

void move_bishop(int pos, int turn) {
    int available[14] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

    int x, j = 0;

    // Diagonal, upper left
    if ( !edge_detection(pos, -1, 1) ) {
        for (int i = 1; i < WIDTH; i++) {
            x = pos - i*WIDTH - i;

            if ( eatable(x, turn) ) {
                available[j++] = x;
                break;
            }

            if ( chess_table[x] == EMPTY ) {
                available[j++] = x;
                if ( edge_detection(x, -1, 1) )
                    break;
                continue;
            }

            break;
        }
    }

    // Diagonal, upper right
    if ( !edge_detection(pos, 1, 1) ) {
        for (int i = 1; i < WIDTH; i++) {
            x = pos - i*WIDTH + i;

            if ( eatable(x, turn) ) {
                available[j++] = x;
                break;
            }

            if ( chess_table[x] == EMPTY ) {
                available[j++] = x;
                if ( edge_detection(x, 1, 1) )
                    break;
                continue;
            }

            break;
        }
    }

    // Diagonal, bottom left
    if ( !edge_detection(pos, -1, -1) ) {
        for (int i = 1; i < WIDTH; i++) {
            x = pos + i*WIDTH - i;

            if ( eatable(x, turn) ) {
                available[j++] = x;
                break;
            }

            if ( chess_table[x] == EMPTY ) {
                available[j++] = x;
                if ( edge_detection(x, -1, -1) )
                    break;
                continue;
            }

            break;
        }
    }

    // Diagonal, bottom right
    if ( !edge_detection(pos, 1, -1) ) {
        for (int i = 1; i < WIDTH; i++) {
            x = pos + i*WIDTH + i;

            if ( eatable(x, turn) ) {
                available[j++] = x;
                break;
            }

            if ( chess_table[x] == EMPTY ) {
                available[j++] = x;
                if ( edge_detection(x, 1, -1) )
                    break;
                continue;
            }

            break;
        }
    }

    print_available(available, 14);
}

/* motherfucker */
void move_knight(int pos, int turn) {
    int available[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };

    int x, j = 0;

    switch (pos) {

        case 9:
            // upper left + 8 + 1
            if ( eatable(3, turn) || chess_table[3] == EMPTY )
                available[j++] = 3;
            if ( eatable(19, turn) || chess_table[19] == EMPTY )
                available[j++] = 19;
            if ( eatable(24, turn) || chess_table[24] == EMPTY )
                available[j++] = 24;
            if ( eatable(26, turn) || chess_table[26] == EMPTY )
                available[j++] = 26;
            break;
        case 49:
            // bottom left - 8 + 1
            if ( eatable(32, turn) || chess_table[32] == EMPTY )
                available[j++] = 32;
            if ( eatable(34, turn) || chess_table[34] == EMPTY )
                available[j++] = 34;
            if ( eatable(43, turn) || chess_table[43] == EMPTY )
                available[j++] = 43;
            if ( eatable(59, turn) || chess_table[59] == EMPTY )
                available[j++] = 59;
            break;
        case 14:
            // upper right + 8 - 1
            if ( eatable(4, turn) || chess_table[4] == EMPTY )
                available[j++] = 4;
            if ( eatable(20, turn) || chess_table[20] == EMPTY )
                available[j++] = 20;
            if ( eatable(29, turn) || chess_table[29] == EMPTY )
                available[j++] = 29;
            if ( eatable(31, turn) || chess_table[31] == EMPTY )
                available[j++] = 31;
            break;
        case 54:
            // bottom right - 8 - 1
            if ( eatable(37, turn) || chess_table[37] == EMPTY )
                available[j++] = 37;
            if ( eatable(39, turn) || chess_table[39] == EMPTY )
                available[j++] = 39;
            if ( eatable(44, turn) || chess_table[44] == EMPTY )
                available[j++] = 44;
            if ( eatable(60, turn) || chess_table[60] == EMPTY )
                available[j++] = 60;
            break;

        // >||
        case 17:
        case 25:
        case 33:
        case 41:
            if ( eatable(pos - 2*WIDTH + 1, turn) || chess_table[pos - 2*WIDTH + 1] == EMPTY )
                available[j++] = pos - 2*WIDTH + 1;
            if ( eatable(pos + 2*WIDTH + 1, turn) || chess_table[pos + 2*WIDTH + 1] == EMPTY )
                available[j++] = pos + 2*WIDTH + 1;
            if ( eatable(pos - 2*WIDTH - 1, turn) || chess_table[pos - 2*WIDTH - 1] == EMPTY )
                available[j++] = pos - 2*WIDTH - 1;
            if ( eatable(pos + 2*WIDTH - 1, turn) || chess_table[pos + 2*WIDTH - 1] == EMPTY )
                available[j++] = pos + 2*WIDTH - 1;
            if ( eatable(pos - WIDTH + 2, turn) || chess_table[pos - WIDTH + 2] == EMPTY )
                available[j++] = pos - WIDTH + 2;
            if ( eatable(pos + WIDTH + 2, turn) || chess_table[pos + WIDTH + 2] == EMPTY )
                available[j++] = pos + WIDTH + 2;
            break;

        // ||<
        case 23:
        case 31:
        case 39:
        case 47:
            if ( eatable(pos - 2*WIDTH - 1, turn) || chess_table[pos - 2*WIDTH - 1] == EMPTY )
                available[j++] = pos - 2*WIDTH - 1;
            if ( eatable(pos + 2*WIDTH - 1, turn) || chess_table[pos + 2*WIDTH - 1] == EMPTY )
                available[j++] = pos + 2*WIDTH - 1;
            if ( eatable(pos - WIDTH + 2, turn) || chess_table[pos - WIDTH + 2] == EMPTY )
                available[j++] = pos - WIDTH + 2;
            if ( eatable(pos + WIDTH + 2, turn) || chess_table[pos + WIDTH + 2] == EMPTY )
                available[j++] = pos + WIDTH + 2;
            break;

        // Upper -
        case 2:
        case 3:
        case 4:
        case 5:
            if ( eatable(pos + 2*WIDTH - 1, turn) || chess_table[pos + 2*WIDTH - 1] == EMPTY )
                available[j++] = pos + 2*WIDTH - 1;
            if ( eatable(pos + 2*WIDTH + 1, turn) || chess_table[pos + 2*WIDTH + 1] == EMPTY )
                available[j++] = pos + 2*WIDTH + 1;
            if ( eatable(pos + WIDTH - 2, turn) || chess_table[pos + WIDTH - 2] == EMPTY )
                available[j++] = pos + WIDTH - 2;
            if ( eatable(pos + WIDTH + 2, turn) || chess_table[pos + WIDTH + 2] == EMPTY )
                available[j++] = pos + WIDTH + 2;
            break;

        // Upper =
        case 10:
        case 11:
        case 12:
        case 13:
            if ( eatable(pos - WIDTH - 2, turn) || chess_table[pos - WIDTH - 2] == EMPTY )
                available[j++] = pos - WIDTH - 2;
            if ( eatable(pos - WIDTH + 2, turn) || chess_table[pos - WIDTH + 2] == EMPTY )
                available[j++] = pos - WIDTH + 2;
            if ( eatable(pos + 2*WIDTH - 1, turn) || chess_table[pos + 2*WIDTH - 1] == EMPTY )
                available[j++] = pos + 2*WIDTH - 1;
            if ( eatable(pos + 2*WIDTH + 1, turn) || chess_table[pos + 2*WIDTH + 1] == EMPTY )
                available[j++] = pos + 2*WIDTH + 1;
            if ( eatable(pos + WIDTH - 2, turn) || chess_table[pos + WIDTH - 2] == EMPTY )
                available[j++] = pos + WIDTH - 2;
            if ( eatable(pos + WIDTH + 2, turn) || chess_table[pos + WIDTH + 2] == EMPTY )
                available[j++] = pos + WIDTH + 2;
            break;

        // lower =
        case 50:
        case 51:
        case 52:
        case 53:
            if ( eatable(pos - WIDTH - 2, turn) || chess_table[pos - WIDTH - 2] == EMPTY )
                available[j++] = pos - WIDTH - 2;
            if ( eatable(pos - WIDTH + 2, turn) || chess_table[pos - WIDTH + 2] == EMPTY )
                available[j++] = pos - WIDTH + 2;
            if ( eatable(pos - 2*WIDTH - 1, turn) || chess_table[pos - 2*WIDTH - 1] == EMPTY )
                available[j++] = pos - 2*WIDTH - 1;
            if ( eatable(pos - 2*WIDTH + 1, turn) || chess_table[pos - 2*WIDTH + 1] == EMPTY )
                available[j++] = pos - 2*WIDTH + 1;
            if ( eatable(pos + WIDTH - 2, turn) || chess_table[pos + WIDTH - 2] == EMPTY )
                available[j++] = pos + WIDTH - 2;
            if ( eatable(pos + WIDTH + 2, turn) || chess_table[pos + WIDTH + 2] == EMPTY )
                available[j++] = pos + WIDTH + 2;
            break;

        // lower -
        case 58:
        case 59:
        case 60:
        case 61:
            if ( eatable(pos - 2*WIDTH - 1, turn) || chess_table[pos - 2*WIDTH - 1] == EMPTY )
                available[j++] = pos - 2*WIDTH - 1;
            if ( eatable(pos - 2*WIDTH + 1, turn) || chess_table[pos - 2*WIDTH + 1] == EMPTY )
                available[j++] = pos - 2*WIDTH + 1;
            if ( eatable(pos - WIDTH - 2, turn) || chess_table[pos - WIDTH - 2] == EMPTY )
                available[j++] = pos - WIDTH - 2;
            if ( eatable(pos - WIDTH + 2, turn) || chess_table[pos - WIDTH + 2] == EMPTY )
                available[j++] = pos - WIDTH + 2;
            break;

        // middle square, 4x4
        default:

            if ( !edge_detection(pos, -1, 1) ) {
                if ( eatable(pos - 2*WIDTH - 1, turn ) || chess_table[pos - 2*WIDTH - 1] == EMPTY )
                    available[j++] = pos - 2*WIDTH - 1;

                if ( eatable(pos - WIDTH - 2, turn ) || chess_table[pos - WIDTH - 2] == EMPTY )
                    available[j++] = pos - WIDTH - 2;
            }

            if ( !edge_detection(pos, -1, -1) ) {
                if ( eatable(pos + WIDTH - 2, turn ) || chess_table[pos + WIDTH - 2] == EMPTY )
                    available[j++] = pos + WIDTH - 2;

                if ( eatable(pos + 2*WIDTH - 1, turn ) || chess_table[pos + 2*WIDTH - 1] == EMPTY )
                    available[j++] = pos + 2*WIDTH - 1;
            }

            if ( !edge_detection(pos, 1, 1) ) {
                if ( eatable(pos - 2*WIDTH + 1, turn ) || chess_table[pos - 2*WIDTH + 1] == EMPTY )
                    available[j++] = pos - 2*WIDTH + 1;

                if ( eatable(pos - WIDTH + 2, turn ) || chess_table[pos - WIDTH + 2] == EMPTY )
                    available[j++] = pos - WIDTH + 2;
            }

            if ( !edge_detection(pos, 1, -1) ) {
                if ( eatable(pos + WIDTH + 2, turn ) || chess_table[pos + WIDTH - 2] == EMPTY )
                    available[j++] = pos + WIDTH + 2;

                if ( eatable(pos + 2*WIDTH + 1, turn ) || chess_table[pos + 2*WIDTH + 1] == EMPTY )
                    available[j++] = pos + 2*WIDTH + 1;
            }
            break;
    }

    print_available(available, 8);
}

void move_rook(int pos, int turn) {
    int available[14] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

    int x;
    int j = 0;

    // Up
    for ( int i = 1; i <= pos / WIDTH; i++ ) {
        x = pos - i * WIDTH;
        if ( chess_table[x] != EMPTY) {
            if ( eatable(x, turn) )
                available[j++] = x;
            break;
        } else {
            available[j++] = x;
        }
    }

    // Down
    for ( int i = 1; i < ( WIDTH - ( pos / WIDTH ) ); i++ ) {
        x = pos + i * WIDTH;
        if ( chess_table[x] != EMPTY ) {
            if ( eatable(x, turn) )
                available[j++] = x;
            break;
        } else {
            available[j++] = x;
        }
    }

    // Right
    for ( int i = 1; i < ( WIDTH - ( pos % WIDTH ) ); i++ )
    {
        x = pos + i;
        if ( chess_table[x] != EMPTY ) {
            if ( eatable(x, turn) )
                available[j++] = x;
            break;
        } else {
            available[j++] = x;
        }
    }
    // Left
    for ( int i = 1; i <= pos % WIDTH; i++ ) {
        x = pos - i;
        if ( chess_table[x] != EMPTY ) {
            if (  eatable(x, turn) )
                available[j++] = x;
            break;
        } else {
            available[j++] = x;
        }
    }

    print_available(available, 14);
}

void move_pawn(int pos, int turn) {
    int available[4] = { -1, -1, -1, -1 };

    int j = 0;

    if ( turn == BLACK ) {
        if ( !edge_detection(pos, 0, -1) ) {
            if ( chess_table[pos + WIDTH] == EMPTY ) {
                available[j++] = pos + WIDTH;

                if ( chess_table[pos + 2*WIDTH] == EMPTY && pos / WIDTH == 1 )
                    available[j++] = pos + 2*WIDTH;
            }
        }
    } else {
        if ( !edge_detection(pos, 0, 1) ) {
            if ( chess_table[pos - WIDTH] == EMPTY ) {
                available[j++] = pos - WIDTH;

                if ( chess_table[pos - 2*WIDTH] == EMPTY && pos / WIDTH == 7 )
                    available[j++] = pos - 2*WIDTH;
            }
        }
    }

    if ( !edge_detection(pos, -1, 0) )
        if ( eatable(pos + turn*WIDTH - 1, turn) )
            available[j++] = pos + turn*WIDTH - 1;
    if ( !edge_detection(pos,  1, 0) )
        if ( eatable(pos + turn*WIDTH + 1, turn) )
            available[j++] = pos + turn*WIDTH + 1;

    print_available(available, 4);
}

void print_table() {
	//system("clear");
	for ( int i = 0; i < TABLE_SIZE; i++ ) {
		if ( i % WIDTH == 0 ) {
			printf("\n");
		}

		printf(" %d", chess_table[i]);
	}

	printf("\n\n");
}

void print_available(int arr[], int size) {
    printf("Slobodna mesta: ");

    for ( int i = 0; i < size; i++ )
        if (arr[i] != -1)
            printf("%d ", arr[i]);

    printf("\n");
}

int eatable(int x, int turn) {
    if ( ( chess_table[x] < 0  && turn == WHITE ) || ( chess_table[x] > 0 && turn == BLACK ) )
        return 1;
    return 0;
}

int edge_detection(int pos, int x, int y) {
    if ( x == -1 )
        if ( pos % WIDTH == 0 )
            return 1;
    if ( x == 1 )
        if ( pos % WIDTH == 7 )
            return 1;
    if ( y == -1 )
        if ( pos / WIDTH == 7 )
            return 1;
    if ( y == 1 )
        if ( pos / WIDTH == 0 )
            return 1;

    return 0;
}
