#include <stdio.h>
#include <stdlib.h>

typedef enum State { 
    STATE_UNKNOWN,
    STATE_BOMB,
    STATE_REWARD
} State;

typedef struct Puzzle {
    State** grid;
    int dimension;
    int* row_amounts;
    int* column_amounts;
} Puzzle;

// Prints the puzzle grid, with 0 representing an unknown square, 1 a bomb and 2 a reward
void print_grid(State** grid, int dimension) {
    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++) {
            printf("%d ", grid[i][j]);
        }
        printf("\n");
    }
}

// Check every row and column to chck for valid number of bombs
int is_valid_grid(Puzzle* puzzle) {
    int bomb_count, unknown_count, bomb_total;

    // Columns
    for (int i = 0; i < puzzle->dimension; i++) {
        bomb_total = puzzle->column_amounts[i];
        unknown_count = 0;
        bomb_count = 0;

        for (int j = 0; j < puzzle->dimension; j++) {
            if (puzzle->grid[j][i] == STATE_UNKNOWN) {
                unknown_count++;
            } else if (puzzle->grid[j][i] == STATE_BOMB) {
                bomb_count++;
            }
        }

        if (unknown_count == 0 && bomb_count != bomb_total) {
            return 0;
        } else if (bomb_count > bomb_total || bomb_count + unknown_count < bomb_total) {
            return 0;
        }
    }

    // Rows
    for (int i = 0; i < puzzle->dimension; i++) {
        bomb_total = puzzle->row_amounts[i];
        unknown_count = 0;
        bomb_count = 0;

        for (int j = 0; j < puzzle->dimension; j++) {
            if (puzzle->grid[puzzle->dimension - 1 - i][j] == STATE_UNKNOWN) {
                unknown_count++;
            } else if (puzzle->grid[puzzle->dimension - 1 - i][j] == STATE_BOMB) {
                bomb_count++;
            }
        }

        if (unknown_count == 0 && bomb_count != bomb_total) {
            return 0;
        } else if (bomb_count > bomb_total || bomb_count + unknown_count < bomb_total) {
            return 0;
        }
    }

    return 1;
}

// Checks if the are unknown squares in the grid
int is_fully_discovered(Puzzle* puzzle) {
    int i = 0;
    int j;

    while (i < puzzle->dimension) {
        j = 0;

        while (j < puzzle->dimension) {

            if (puzzle->grid[i][j] == STATE_UNKNOWN) {
                return 0;
            }

            j++;
        }

        i++;
    }

    return 1;
}

// Recursive function for backtracking
int solve_recursive(Puzzle* puzzle, int index) {
    if (is_valid_grid(puzzle)) {
        if (is_fully_discovered(puzzle)) {
            return 1;
        } else {
            int row = index / puzzle->dimension;
            int col = index % puzzle->dimension;

            if (puzzle->grid[row][col] == STATE_UNKNOWN) {
                // Try placing a reward on current square
                puzzle->grid[row][col] = STATE_REWARD;
                if (solve_recursive(puzzle, index + 1)) {
                    return 1;
                }

                // If that failed, try placing a bomb
                puzzle->grid[row][col] = STATE_BOMB;
                if (solve_recursive(puzzle, index + 1)) {
                    return 1;
                }

                // If both failed, reset square and return false
                puzzle->grid[row][col] = STATE_UNKNOWN;
                return 0;
            } else {
                // If this square has been determined before, go tho the next square
                return solve_recursive(puzzle, index + 1);
            }
        }
    } else {
        return 0;
    }
}

// Fills in the puzzle grid until every square has been determined
void solve(Puzzle* puzzle, int index) {
    int row = index / puzzle->dimension;
    int col = index % puzzle->dimension;

    if (puzzle->grid[row][col] == STATE_UNKNOWN) {
        puzzle->grid[row][row] = STATE_REWARD;
        if (solve_recursive(puzzle, index + 1)) {
            return;
        }

        puzzle->grid[row][col] = STATE_BOMB;
        if (solve_recursive(puzzle, index + 1)) {
            return;
        }

    } else {
        solve(puzzle, index + 1);
    }
}

int main(int argc, char** argv) {
    if (argc < 2 || argc % 2 != 1) {
        printf("Usage: ./voltorb <bomb counts of columns> <bomb counts of rows>\n");
        return 1;
    }

	int dimension = (argc - 1) / 2;

    State** grid = malloc(dimension * sizeof(int*));

    for (int i = 0; i < dimension; i++) {
        grid[i] = malloc(dimension * sizeof(int));

        for (int j = 0; j < dimension; j++) {
            grid[i][j] = STATE_UNKNOWN;
        }
    }

    // Read arguments into arrays
    int* column_amounts = malloc(sizeof(int) * dimension);
    int* row_amounts = malloc(sizeof(int) * dimension);

    for (int i = 1; i < dimension + 1; i++) {
        int next_col = atoi(argv[i]);
        int next_row = atoi(argv[i + dimension]);

        if (next_col == 0) {
            for (int j = 0; j < dimension; j++) {
                grid[j][i - 1] = STATE_REWARD;
            }
        }

        if (next_row == 0) {
            for (int j = 0; j < dimension; j++) {
                grid[dimension - i][j] = STATE_REWARD;
            }
        }

        column_amounts[i - 1] = next_col;
        row_amounts[i - 1] = next_row;
    }

    Puzzle puzzle;
    puzzle.grid = grid;
    puzzle.dimension = dimension;
    puzzle.row_amounts = row_amounts;
    puzzle.column_amounts = column_amounts;

    solve(&puzzle, 0);
    print_grid(puzzle.grid, puzzle.dimension);

    free(row_amounts);
    free(column_amounts);

    for (int i = 0; i < dimension; i++) {
        free(grid[i]);
    }

    free(grid);

    return 0;
}
