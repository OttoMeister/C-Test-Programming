// LINUX
// gcc -o sudoku sudoku.c && ./sudoku

// WINDOWS - https://mingw.osdn.io
// path %PATH%;C:\MinGW\bin
// "C:\Program Files\Notepad++\notepad++.exe"  C:\MinGW\sudoku.c
// gcc -o sudoku C:\MinGW\sudoku.c && sudoku.exe

// https://onlinegdb.com/LplrpeWWI
// https://www.geeksforgeeks.org/sudoku-backtracking-7
// https://developpaper.com/source-code-of-solving-sudoku-program-in-c-language/
// https://formatter.org/cpp-formatter

#include <stdio.h>
#include <stdlib.h>
static unsigned long long deep = 0;

#define RED "\x1B[31m"
#define NRM "\x1B[0m"

// Checks whether it will be legal to assign num to the given row, col
int isSafe(int grid[9][9], int row, int col, int num) {
  // Check if we find the same num in the similar row
  for (int i = 0; i < 9; i++) if (grid[row][i] == num) return 0;
  // Check if we find the same num in the similar column
  for (int i = 0; i < 9; i++) if (grid[i][col] == num) return 0;
  // Check if we find the same num in the particular 3*3 matrix
  int startRow = row - row % 3, startCol = col - col % 3;
  for (int i = 0; i < 3; i++) for (int ii = 0; ii < 3; ii++)
      if (grid[i + startRow][ii + startCol] == num) return 0;
  return 1;
}

// Takes a partially filled-in grid and attempts to assign
// values to all unassigned locations in such a way to meet
// the requirements for Sudoku solution (non-duplication
// across rows, columns, and boxes)
int solveSudoku(int grid[9][9], int row, int col) {
  deep++;
  // Check if we have reached the 8thbrow and 9th column (0
  // indexed matrix) we are returning true to avoid further
  // backtracking
  if (row == 8 && col == 9) return 1;
  // Check if column value  becomes 9 ,we move to next row
  // and column start from 0
  if (col == 9) { row++; col = 0; }
  // Check if the current position of the grid already
  // contains value >0, we iterate for next column
  if (grid[row][col] > 0) return solveSudoku(grid, row, col + 1);
  for (int num = 1; num <= 9; num++) {
    // Check if it is safe to place the num (1-9) in the
    // given row ,col  ->we move to next column
    if (isSafe(grid, row, col, num)) {
      // Assigning the num in the current (row,col) position
      // of the grid and assuming our assigned num in the
      // position is correct
      grid[row][col] = num;
      // Checking for next possibility with next column
      if (solveSudoku(grid, row, col + 1)) return 1;
    }
    // Removing the assigned num ,since our assumption was
    // wrong , and we go for next assumption with diff num
    // value
    grid[row][col] = 0;
  }
  return 0;
}

// function to print grid
void print(int arr1[9][9], int arr2[9][9]) {
#ifdef _WIN32
  system("Color");
#endif  // Windows
  for (int i = 0; i < 9; i++) {
    if (i % 3 == 0) printf("+------+------+------+\n");
    for (int ii = 0; ii < 9; ii++) {
      if (ii % 3 == 0) putchar('|');
      if (arr1[i][ii]) printf(RED "%2d" NRM, arr2[i][ii]);
      else printf("%2d", arr2[i][ii]);
    }
   printf("|\n");
  }
  printf("+------+------+------+\n");
}

int main() {
#define SUNR 4
  int start[9][9];
  int grids[12][9][9] =
// sudoku 00 mit 392 Rekursionstiefe
      {{{0, 0, 0, 0, 0, 0, 0, 0, 0},   // Zeile 1
        {0, 0, 0, 0, 0, 0, 0, 0, 0},   // Zeile 2
        {0, 0, 0, 0, 0, 0, 0, 0, 0},   // Zeile 3
        {0, 0, 0, 0, 0, 0, 0, 0, 0},   // Zeile 4
        {0, 0, 0, 0, 0, 0, 0, 0, 0},   // Zeile 5
        {0, 0, 0, 0, 0, 0, 0, 0, 0},   // Zeile 6
        {0, 0, 0, 0, 0, 0, 0, 0, 0},   // Zeile 7
        {0, 0, 0, 0, 0, 0, 0, 0, 0},   // Zeile 8
        {0, 0, 0, 0, 0, 0, 0, 0, 0}},  // Zeile 9
// sudoku 01 mit 126776 Rekursionstiefe
       {{0, 9, 0, 0, 0, 0, 0, 0, 0},   // Zeile 1
        {0, 0, 0, 0, 0, 0, 4, 6, 0},   // Zeile 2
        {2, 0, 0, 0, 0, 0, 0, 0, 5},   // Zeile 3
        {0, 7, 0, 8, 0, 4, 0, 1, 0},   // Zeile 4
        {0, 3, 0, 0, 2, 0, 0, 0, 0},   // Zeile 5
        {5, 0, 0, 0, 6, 0, 8, 0, 0},   // Zeile 6
        {0, 1, 0, 7, 3, 0, 0, 2, 0},   // Zeile 7
        {9, 0, 8, 0, 5, 0, 0, 0, 0},   // Zeile 8
        {0, 0, 0, 2, 0, 0, 0, 0, 0}},  // Zeile 9
// sudoku 02 mit 914 Rekursionstiefe
       {{2, 0, 7, 0, 3, 8, 1, 0, 0},   // Zeile 1
        {0, 8, 3, 0, 0, 0, 2, 7, 0},   // Zeile 2
        {0, 0, 0, 5, 2, 7, 3, 0, 8},   // Zeile 3
        {0, 0, 0, 8, 4, 2, 6, 0, 1},   // Zeile 4
        {0, 1, 2, 6, 7, 0, 4, 0, 3},   // Zeile 5
        {0, 0, 0, 3, 0, 0, 7, 0, 2},   // Zeile 6
        {0, 0, 0, 0, 5, 3, 9, 2, 0},   // Zeile 7
        {0, 0, 5, 2, 0, 0, 8, 1, 0},   // Zeile 8
        {4, 2, 0, 0, 8, 0, 5, 3, 0}},  // Zeile 9
// sudoku 03 mit 300 Rekursionstiefe
       {{0, 0, 5, 0, 0, 0, 6, 7, 1},   // Zeile 1
        {0, 0, 6, 0, 0, 4, 0, 0, 0},   // Zeile 2
        {0, 7, 0, 5, 0, 9, 0, 0, 2},   // Zeile 3
        {0, 0, 0, 7, 1, 6, 0, 0, 5},   // Zeile 4
        {4, 6, 0, 0, 0, 0, 0, 0, 3},   // Zeile 5
        {0, 0, 9, 0, 0, 3, 8, 0, 0},   // Zeile 6
        {7, 9, 8, 4, 0, 0, 0, 0, 0},   // Zeile 7
        {0, 2, 0, 9, 0, 0, 1, 3, 0},   // Zeile 8
        {1, 5, 3, 0, 0, 2, 0, 9, 4}},  // Zeile 9
// sudoku 04 mit 1953136 Rekursionstiefe
       {{0, 0, 0, 0, 0, 0, 0, 0, 1},   // Zeile 1
        {0, 0, 0, 8, 0, 0, 0, 0, 0},   // Zeile 2
        {3, 0, 0, 7, 6, 0, 0, 0, 0},   // Zeile 3
        {0, 0, 0, 2, 0, 8, 0, 5, 0},   // Zeile 4
        {0, 0, 0, 3, 0, 0, 2, 0, 0},   // Zeile 5
        {0, 8, 9, 0, 1, 0, 0, 0, 0},   // Zeile 6
        {0, 0, 0, 0, 4, 0, 3, 2, 0},   // Zeile 7
        {0, 0, 0, 0, 0, 9, 0, 0, 8},   // Zeile 8
        {0, 5, 0, 0, 0, 0, 0, 0, 0}},  // Zeile 9
// sudoku 05 mit 2553 Rekursionstiefe
       {{9, 2, 0, 0, 0, 0, 0, 0, 0},   // Zeile 1
        {0, 0, 0, 0, 3, 8, 0, 0, 2},   // Zeile 2
        {0, 4, 0, 0, 0, 6, 0, 0, 0},   // Zeile 3
        {4, 8, 6, 7, 1, 2, 9, 5, 3},   // Zeile 4
        {0, 0, 2, 3, 4, 0, 8, 0, 6},   // Zeile 5
        {0, 0, 0, 6, 8, 0, 0, 0, 4},   // Zeile 6
        {8, 6, 4, 0, 5, 0, 1, 0, 0},   // Zeile 7
        {0, 5, 9, 0, 6, 0, 0, 8, 0},   // Zeile 8
        {0, 3, 7, 8, 0, 0, 0, 6, 0}},  // Zeile 9
// sudoku 06 mit 82 Rekursionstiefe
       {{3, 1, 6, 5, 7, 8, 4, 9, 2},   // Zeile 1
        {5, 2, 9, 1, 3, 4, 7, 6, 8},   // Zeile 2
        {4, 8, 7, 6, 2, 9, 5, 3, 1},   // Zeile 3
        {2, 6, 3, 0, 1, 5, 9, 8, 7},   // Zeile 4
        {9, 7, 4, 8, 6, 0, 1, 2, 5},   // Zeile 5
        {8, 5, 1, 7, 9, 2, 6, 4, 3},   // Zeile 6
        {1, 3, 8, 0, 4, 7, 2, 0, 6},   // Zeile 7
        {6, 9, 2, 3, 5, 1, 8, 7, 4},   // Zeile 8
        {7, 4, 5, 0, 8, 6, 3, 1, 0}},  // Zeile 9
// sudoku 07 mit 1149 Rekursionstiefe
       {{3, 0, 6, 5, 0, 8, 4, 0, 0},   // Zeile 1
        {5, 2, 0, 0, 0, 0, 0, 0, 0},   // Zeile 2
        {0, 8, 7, 0, 0, 0, 0, 3, 1},   // Zeile 3
        {0, 0, 3, 0, 1, 0, 0, 8, 0},   // Zeile 4
        {9, 0, 0, 8, 6, 3, 0, 0, 5},   // Zeile 5
        {0, 5, 0, 0, 9, 0, 6, 0, 0},   // Zeile 6
        {1, 3, 0, 0, 0, 0, 2, 5, 0},   // Zeile 7
        {0, 0, 0, 0, 0, 0, 0, 7, 4},   // Zeile 8
        {0, 0, 5, 2, 0, 6, 3, 0, 0}},  // Zeile 9
// sudoku 08 mit 14578 Rekursionstiefe
       {{0, 0, 5, 3, 0, 0, 0, 0, 0},   // Zeile 1
        {8, 0, 0, 0, 0, 0, 0, 2, 0},   // Zeile 2
        {0, 7, 0, 0, 1, 0, 5, 0, 0},   // Zeile 3
        {4, 0, 0, 0, 0, 5, 3, 0, 0},   // Zeile 4
        {0, 1, 0, 0, 7, 0, 0, 0, 6},   // Zeile 5
        {0, 0, 3, 2, 0, 0, 0, 8, 0},   // Zeile 6
        {0, 6, 0, 5, 0, 0, 0, 0, 9},   // Zeile 7
        {0, 0, 4, 0, 0, 0, 0, 3, 0},   // Zeile 8
        {0, 0, 0, 0, 0, 9, 7, 0, 0}},  // Zeile 9
// sudoku 09 mit 451243 Rekursionstiefe
       {{0, 0, 0, 0, 0, 6, 0, 0, 0},   // Zeile 1
        {0, 5, 9, 0, 0, 0, 0, 0, 8},   // Zeile 2
        {2, 0, 0, 0, 0, 8, 0, 0, 0},   // Zeile 3
        {0, 4, 5, 0, 0, 0, 0, 0, 0},   // Zeile 4
        {0, 0, 3, 0, 0, 0, 0, 0, 0},   // Zeile 5
        {0, 0, 6, 0, 0, 3, 0, 5, 4},   // Zeile 6
        {0, 0, 0, 3, 2, 5, 0, 0, 6},   // Zeile 7
        {0, 0, 0, 0, 0, 0, 0, 0, 0},   // Zeile 8
        {0, 0, 0, 0, 0, 0, 0, 0, 0}},  // Zeile 9
// sudoku 10 mit  3390563007 Rekursionstiefe (keine LC6sung)
       {{0, 0, 0, 0, 0, 5, 0, 8, 0},    // Zeile 1
        {0, 0, 0, 6, 0, 1, 0, 4, 3},    // Zeile 2
        {0, 0, 0, 0, 0, 0, 0, 0, 0},    // Zeile 3
        {0, 1, 0, 5, 0, 0, 0, 0, 0},    // Zeile 4
        {0, 0, 0, 1, 0, 6, 0, 0, 0},    // Zeile 5
        {3, 0, 0, 0, 0, 0, 0, 0, 5},    // Zeile 6
        {5, 3, 0, 0, 0, 0, 0, 6, 1},    // Zeile 7
        {0, 0, 0, 0, 0, 0, 0, 0, 4},    // Zeile 8
        {0, 0, 0, 0, 0, 0, 0, 0, 0}},   // Zeile 9
// sudoku 11 mit 392 Rekursionstiefe
       {{0, 0, 0, 0, 0, 0, 0, 0, 0},    // Zeile 1
        {0, 0, 0, 0, 0, 0, 0, 0, 0},    // Zeile 2
        {0, 0, 0, 0, 0, 0, 0, 0, 0},    // Zeile 3
        {0, 0, 0, 0, 0, 0, 0, 0, 0},    // Zeile 4
        {0, 0, 0, 0, 0, 0, 0, 0, 0},    // Zeile 5
        {0, 0, 0, 0, 0, 0, 0, 0, 0},    // Zeile 6
        {0, 0, 0, 0, 0, 0, 0, 0, 0},    // Zeile 7
        {0, 0, 0, 0, 0, 0, 0, 0, 0},    // Zeile 8
        {0, 0, 0, 0, 0, 0, 0, 0, 0}}};  // Zeile 9

  for (int i = 0; i < 9; i++) for (int ii = 0; ii < 9; ii++) 
    start[i][ii] = grids[SUNR][i][ii];
  if (solveSudoku(grids[SUNR], 0, 0) == 1) print(start, grids[SUNR]);
  else printf("No solution exists\n");
  printf("Sudoku %i hat %lld Rekursionstiefe\n", SUNR, deep);
  return 0;
}



