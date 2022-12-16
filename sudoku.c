// LINUX
// gcc -Wall -Wextra -O3 sudoku.c -o sudoku && ./sudoku

// WINDOWS - https://mingw.osdn.io
// path %PATH%;C:\MinGW\bin
// "C:\Program Files\Notepad++\notepad++.exe"  C:\MinGW\sudoku.c
// gcc -o sudoku C:\MinGW\sudoku.c && sudoku.exe

// https://onlinegdb.com/DNA10nSCPU
// https://github.com/mentalmove/SudokuGenerator/branches/stale
// https://github.com/mislah/Sudoku/blob/main/sudoku.c
// https://www.geeksforgeeks.org/sudoku-backtracking-7
// https://developpaper.com/source-code-of-solving-sudoku-program-in-c-language/
// https://formatter.org/cpp-formatter
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static unsigned long deep = 0;

#define SUNR 8

// Mix from 0-8 use Fisherb Yates shuffle
void mix(int buf[9]) {
  for (int i = 0; i < 9; i++) buf[i] = i;
  for (int i = 0; i < 9; i++) {
    int rpos = rand() % 9;
    int temp = buf[i];
    buf[i] = buf[rpos];
    buf[rpos] = temp;
  }
}

// Function to find an unassigned location in the grid
int findUnassignedLocation(int grid[9][9], int *row, int *col) {
  for (*row = 0; *row < 9; (*row)++)
    for (*col = 0; *col < 9; (*col)++)
      if (grid[*row][*col] == 0) return 1;
  return 0;
}

// Function to check if a number is safe to be placed in a particular location
int num_safe(int grid[9][9], int row, int col, int num) {
  // Check if 'num' is not already placed in current row, current column and
  // current 3x3 box
  for (int i = 0; i < 9; i++)
    if (grid[row][i] == num || grid[i][col] == num) return 0;
  int startRow = row - row % 3;
  int startCol = col - col % 3;
  for (int i = startRow; i < startRow + 3; i++)
    for (int j = startCol; j < startCol + 3; j++)
      if (grid[i][j] == num) return 0;
  return 1;
}

// If on one position only on number posible we fond it!
int find1(int sudoku[9][9]) {
  int hit_num, hit_count=0, found_cnt = 0;
  int buf[3][9];
  mix(buf[0]);
  for (int rowmix = 0; rowmix < 9; rowmix++) {
    int row = buf[0][rowmix];
    mix(buf[1]);
    for (int colmix = 0; colmix < 9; colmix++) {
      int col = buf[1][colmix];
      hit_num = 0;
      hit_count = 0;
      if (!sudoku[row][col]) {
        mix(buf[2]);
        for (int nummix = 1; nummix < 10; nummix++) {
          int num = 1 + buf[2][nummix - 1];  // mixer from 0-8
          if (num_safe(sudoku, row, col, num)) {
            hit_num = num;
            hit_count++;
          }
        }
        if (hit_count == 1) {
          printf("Hit on row %i, column %i with rule 1\n", row + 1, col + 1);
          sudoku[row][col] = hit_num;
          found_cnt++;
        }
      }
    }
  }
  return found_cnt;
}

// find a number in the line that is missing and legally possible only once
int find2(int sudoku[9][9]) {
  int hit_count, found, hit_col=0, found_cnt = 0;
  int buf[3][9];
  mix(buf[0]);
  for (int nummix = 1; nummix < 10; nummix++) {
    int num = 1 + buf[0][nummix - 1];  // mixer from 0-8
    mix(buf[1]);
    for (int rowmix = 0; rowmix < 9; rowmix++) {
      int row = buf[1][rowmix];
      found = 0;
      hit_col = 0;
      hit_count = 0;
      // we check if the number we are looking for is already in the line
      for (int col = 0; col < 9; col++)
        if (sudoku[row][col] == num) found++;
      // Only if the line does not have the searched number
      if (found == 0)
        for (int col = 0; col < 9; col++)
          if (sudoku[row][col] == 0)
            if (num_safe(sudoku, row, col, num)) {
              hit_col = col;
              hit_count++;
            }
      // Only if the searched number is possible once
      if (hit_count == 1) {
        sudoku[row][hit_col] = num;
        found_cnt++;
        printf("Hit on row %i, column %i with rule 2\n", row + 1, hit_col + 1);
      }
    }
  }
  return found_cnt;
}

// find a number in the column that is missing and legally possible only once
int find3(int sudoku[9][9]) {
  int hit_count, found, hit_row=0, found_cnt = 0;
  int buf[3][9];
  mix(buf[0]);
  for (int nummix = 1; nummix < 10; nummix++) {
    int num = 1 + buf[0][nummix - 1];  // mixer from 0-8
    mix(buf[1]);
    for (int colmix = 0; colmix < 9; colmix++) {
      int col = buf[1][colmix];
      found = 0;
      hit_row = 0;
      hit_count = 0;
      // we check if the searched number is already in the column
      for (int row = 0; row < 9; row++)
        if (sudoku[row][col] == num) found++;
      // Only if the column does not have the searched number
      if (found == 0)
        for (int row = 0; row < 9; row++)
          if (sudoku[row][col] == 0)
            if (num_safe(sudoku, row, col, num)) {
              hit_row = row;
              hit_count++;
            }
      // Only if the searched number is possible once
      if (hit_count == 1) {
        sudoku[hit_row][col] = num;
        found_cnt++;
        printf("Hit on row %i, column %i with rule 3\n", hit_row + 1, col + 1);
      }
    }
  }
  return found_cnt;
}

// find a number in the group that is missing and legally possible only once
int find4(int sudoku[9][9]) {
  int hit_count, found, hit_row=0, hit_col=0, hit_elm=0, found_cnt = 0, row, col;
  int buf[3][9];
  mix(buf[0]);
  for (int nummix = 1; nummix < 10; nummix++) {
    int num = 1 + buf[0][nummix - 1];  // mixer from 0-8
    for (int grpmix = 0; grpmix < 9; grpmix++) {
      mix(buf[1]);
      int grp = buf[1][grpmix];
      found = 0;
      hit_count = 0;
      // we check if the number we are looking for is already in the column
      for (int elm = 0; elm < 9; elm++) {
        row = elm / 3 + grp / 3 * 3;
        col = elm % 3 + grp % 3 * 3;
        if (sudoku[row][col] == num) found++;
        // printf("Group %i, element %i = row %i, column %i\n",grp+1,
        // elm+1, row+1, col+1);
      }
      // Only if the group does not have the number you are looking for
      if (found == 0)
        for (int elm = 0; elm < 9; elm++) {
          row = elm / 3 + grp / 3 * 3;
          col = elm % 3 + grp % 3 * 3;
          if (sudoku[row][col] == 0)
            if (num_safe(sudoku, row, col, num)) {
              hit_row = row;
              hit_col = col;
              hit_elm = elm;
              hit_count++;
            }
        }
      // Only if the searched number is possible once
      if (hit_count == 1) {
        sudoku[hit_row][hit_col] = num;
        found_cnt++;
        printf("Hit on group %i, element %i with rule 4\n", grp + 1,
               hit_elm + 1);
      }
    }
  }
  return found_cnt;
}

// Recursive function to solve the sudoku
int solve(int grid[9][9]) {
  int row, col;
  deep++;
  // If there is no unassigned location, the puzzle is solved
  if (!findUnassignedLocation(grid, &row, &col)) return 1;
  // Try every number from 1 to 9
  for (int num = 1; num <= 9; num++) {
    // If it is safe to place 'num' at (row, col), do so and recursively try to
    // solve the rest of the puzzle
    if (num_safe(grid, row, col, num)) {
      grid[row][col] = num;
      if (solve(grid)) return 1;
      // If the puzzle cannot be solved, backtrack and unassign the location
      grid[row][col] = 0;
    }
  }
  return 0;
}


// function to print sudoku
#define RED "\x1B[31m"
#define NRM "\x1B[0m"
#ifdef __linux__
void print(int arr1[9][9], int arr2[9][9]) {
  static const char *border_row = "┏━━━┯━━━┯━━━┳━━━┯━━━┯━━━┳━━━┯━━━┯━━━┓\n";
  static const char *separator_row = "┠───┼───┼───╂───┼───┼───╂───┼───┼───┨\n";
  static const char *middle_border_row = "┣━━━┿━━━┿━━━╋━━━┿━━━┿━━━╋━━━┿━━━┿━━━┫\n";
  static const char *end_row = "┗━━━┷━━━┷━━━┻━━━┷━━━┷━━━┻━━━┷━━━┷━━━┛\n";
  printf("%s", border_row);
  for (int row = 0; row < 9; row++) {
    for (int col = 0; col < 9; col++) {
      printf("%s", (col % 3) ? "│" : "┃");
      if (arr1[row][col])
        printf(RED "%2d " NRM, arr1[row][col]);
      else if (arr2[row][col])
        printf("%2d ", arr2[row][col]);
      else
        printf("   ");
    }
    printf("┃\n");
    if (row % 3 != 2) printf("%s", separator_row);
    if (row == 2 || row == 5) printf("%s", middle_border_row);
  }
  printf("%s", end_row);
}
#else  //__linux__
void print(int arr1[9][9], int arr2[9][9]) {
#ifdef _WIN32
  system("Color");
#endif  //_WIN32
  for (int row = 0; row < 9; row++) {
    if (row % 3 == 0) printf("+------+------+------+\n");
    for (int col = 0; col < 9; col++) {
      if (col % 3 == 0) putchar('|');
      if (arr1[row][col])
        printf(RED "%2d" NRM, arr1[row][col]);
      else if (arr2[row][col])
        printf("%2d", arr2[row][col]);
      else
        printf(" -");
    }
    printf("|\n");
  }
  printf("+------+------+------+\n");
}
#endif  //__linux__

int main() {
  int sudokubuf1[9][9];
  int sudokubuf2[9][9];
  int sudoku[19][9][9] =
      // sudoku 00 with 392 recursion depth
      {{{0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0}}, 
       // sudoku 01 with 126776 recursion depth (no easy solution)
       {{0, 9, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 4, 6, 0},  
        {2, 0, 0, 0, 0, 0, 0, 0, 5},  
        {0, 7, 0, 8, 0, 4, 0, 1, 0},  
        {0, 3, 0, 0, 2, 0, 0, 0, 0},  
        {5, 0, 0, 0, 6, 0, 8, 0, 0},  
        {0, 1, 0, 7, 3, 0, 0, 2, 0},  
        {9, 0, 8, 0, 5, 0, 0, 0, 0},  
        {0, 0, 0, 2, 0, 0, 0, 0, 0}}, 
       // sudoku 02 with 914 recursion depth (no easy solution)
       {{2, 0, 7, 0, 3, 8, 1, 0, 0},  
        {0, 8, 3, 0, 0, 0, 2, 7, 0},  
        {0, 0, 0, 5, 2, 7, 3, 0, 8},  
        {0, 0, 0, 8, 4, 2, 6, 0, 1},  
        {0, 1, 2, 6, 7, 0, 4, 0, 3},  
        {0, 0, 0, 3, 0, 0, 7, 0, 2},  
        {0, 0, 0, 0, 5, 3, 9, 2, 0},  
        {0, 0, 5, 2, 0, 0, 8, 1, 0},  
        {4, 2, 0, 0, 8, 0, 5, 3, 0}}, 
       // sudoku 03 with 300 recursion depth (simple)
       {{0, 0, 5, 0, 0, 0, 6, 7, 1},  
        {0, 0, 6, 0, 0, 4, 0, 0, 0},  
        {0, 7, 0, 5, 0, 9, 0, 0, 2},  
        {0, 0, 0, 7, 1, 6, 0, 0, 5},  
        {4, 6, 0, 0, 0, 0, 0, 0, 3},  
        {0, 0, 9, 0, 0, 3, 8, 0, 0},  
        {7, 9, 8, 4, 0, 0, 0, 0, 0},  
        {0, 2, 0, 9, 0, 0, 1, 3, 0},  
        {1, 5, 3, 0, 0, 2, 0, 9, 4}}, 
       // sudoku 04 with 1953136 recursion depth (no easy solution)
       {{0, 0, 0, 0, 0, 0, 0, 0, 1},  
        {0, 0, 0, 8, 0, 0, 0, 0, 0},  
        {3, 0, 0, 7, 6, 0, 0, 0, 0},  
        {0, 0, 0, 2, 0, 8, 0, 5, 0},  
        {0, 0, 0, 3, 0, 0, 2, 0, 0},  
        {0, 8, 9, 0, 1, 0, 0, 0, 0},  
        {0, 0, 0, 0, 4, 0, 3, 2, 0},  
        {0, 0, 0, 0, 0, 9, 0, 0, 8},  
        {0, 5, 0, 0, 0, 0, 0, 0, 0}}, 
       // sudoku 05 with 2553 recursion depth (simple solution)
       {{9, 2, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 3, 8, 0, 0, 2},  
        {0, 4, 0, 0, 0, 6, 0, 0, 0},  
        {4, 8, 6, 7, 1, 2, 9, 5, 3},  
        {0, 0, 2, 3, 4, 0, 8, 0, 6},  
        {0, 0, 0, 6, 8, 0, 0, 0, 4},  
        {8, 6, 4, 0, 5, 0, 1, 0, 0},  
        {0, 5, 9, 0, 6, 0, 0, 8, 0},  
        {0, 3, 7, 8, 0, 0, 0, 6, 0}}, 
       // sudoku 06 with 82 recursion depth (simple)
       {{3, 1, 6, 5, 7, 8, 4, 9, 2},  
        {5, 2, 9, 1, 3, 4, 7, 6, 8},  
        {4, 8, 7, 6, 2, 9, 5, 3, 1},  
        {2, 6, 3, 0, 1, 5, 9, 8, 7},  
        {9, 7, 4, 8, 6, 0, 1, 2, 5},  
        {8, 5, 1, 7, 9, 2, 6, 4, 3},  
        {1, 3, 8, 0, 4, 7, 2, 0, 6},  
        {6, 9, 2, 3, 5, 1, 8, 7, 4},  
        {7, 4, 5, 0, 8, 6, 3, 1, 0}}, 
       // sudoku 07 with 1149 recursion depth (simple solution)
       {{3, 0, 6, 5, 0, 8, 4, 0, 0},  
        {5, 2, 0, 0, 0, 0, 0, 0, 0},  
        {0, 8, 7, 0, 0, 0, 0, 3, 1},  
        {0, 0, 3, 0, 1, 0, 0, 8, 0},  
        {9, 0, 0, 8, 6, 3, 0, 0, 5},  
        {0, 5, 0, 0, 9, 0, 6, 0, 0},  
        {1, 3, 0, 0, 0, 0, 2, 5, 0},  
        {0, 0, 0, 0, 0, 0, 0, 7, 4},  
        {0, 0, 5, 2, 0, 6, 3, 0, 0}}, 
       // sudoku 08 with 14578 recursion depth (no easy solution)
       {{0, 0, 5, 3, 0, 0, 0, 0, 0},  
        {8, 0, 0, 0, 0, 0, 0, 2, 0},  
        {0, 7, 0, 0, 1, 0, 5, 0, 0},  
        {4, 0, 0, 0, 0, 5, 3, 0, 0},  
        {0, 1, 0, 0, 7, 0, 0, 0, 6},  
        {0, 0, 3, 2, 0, 0, 0, 8, 0},  
        {0, 6, 0, 5, 0, 0, 0, 0, 9},  
        {0, 0, 4, 0, 0, 0, 0, 3, 0},  
        {0, 0, 0, 0, 0, 9, 7, 0, 0}}, 
       // sudoku 09 with 451243 recursion depth (no easy solution)
       {{0, 0, 0, 0, 0, 6, 0, 0, 0},  
        {0, 5, 9, 0, 0, 0, 0, 0, 8},  
        {2, 0, 0, 0, 0, 8, 0, 0, 0},  
        {0, 4, 5, 0, 0, 0, 0, 0, 0},  
        {0, 0, 3, 0, 0, 0, 0, 0, 0},  
        {0, 0, 6, 0, 0, 3, 0, 5, 4},  
        {0, 0, 0, 3, 2, 5, 0, 0, 6},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0}}, 
       // sudoku 10 with 3390563007 recursion depth (no solution!)
       {{0, 0, 0, 0, 0, 5, 0, 8, 0},  
        {0, 0, 0, 6, 0, 1, 0, 4, 3},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 1, 0, 5, 0, 0, 0, 0, 0},  
        {0, 0, 0, 1, 0, 6, 0, 0, 0},  
        {3, 0, 0, 0, 0, 0, 0, 0, 5},  
        {5, 3, 0, 0, 0, 0, 0, 6, 1},  
        {0, 0, 0, 0, 0, 0, 0, 0, 4},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0}}, 
       // sudoku 11 with 392 recursion depth (placeholder)
       {{0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 0, 0}}, 
       // sudoku 12
       {{0, 0, 0, 0, 0, 0, 0, 9, 0},
        {1, 9, 0, 4, 7, 0, 6, 0, 8},
        {0, 5, 2, 8, 1, 9, 4, 0, 7},
        {2, 0, 0, 0, 4, 8, 0, 0, 0},
        {0, 0, 9, 0, 0, 0, 5, 0, 0},
        {0, 0, 0, 7, 5, 0, 0, 0, 9},
        {9, 0, 7, 3, 6, 4, 1, 8, 0},
        {5, 0, 6, 0, 8, 1, 0, 7, 4},
        {0, 8, 0, 0, 0, 0, 0, 0, 0}},
       // sudoku 13 with 73414 tries (simple solution)
       {{0, 0, 0, 4, 0, 0, 2, 0, 1},  
        {9, 0, 0, 0, 0, 3, 8, 0, 0},  
        {0, 0, 0, 0, 0, 0, 0, 7, 5},  
        {0, 0, 5, 3, 0, 0, 0, 0, 0},  
        {4, 0, 1, 0, 9, 0, 0, 0, 0},  
        {0, 0, 0, 0, 5, 1, 0, 0, 3},  
        {0, 0, 0, 2, 0, 0, 7, 9, 0},  
        {0, 2, 0, 0, 0, 4, 5, 0, 0},  
        {0, 6, 0, 0, 0, 5, 0, 1, 0}}, 
       // sudoku 14
       {{3, 0, 6, 5, 0, 8, 4, 0, 0},
        {5, 2, 0, 0, 0, 0, 0, 0, 0},
        {0, 8, 7, 0, 0, 0, 0, 3, 1},
        {0, 0, 3, 0, 1, 0, 0, 8, 0},
        {9, 0, 0, 8, 6, 3, 0, 0, 5},
        {0, 5, 0, 0, 9, 0, 6, 0, 0},
        {1, 3, 0, 0, 0, 0, 2, 5, 0},
        {0, 0, 0, 0, 0, 0, 0, 7, 4},
        {0, 0, 5, 2, 0, 6, 3, 0, 0}},
       // sudoku 15
       {{0, 0, 9, 0, 0, 0, 0, 0, 0}, 
        {5, 3, 0, 0, 0, 0, 8, 0, 1}, 
        {0, 0, 2, 0, 6, 0, 7, 0, 0}, 
        {0, 2, 0, 6, 4, 0, 0, 0, 7}, 
        {0, 6, 8, 5, 0, 0, 1, 0, 0}, 
        {0, 0, 0, 0, 7, 0, 6, 0, 5}, 
        {0, 5, 0, 7, 0, 0, 0, 0, 9}, 
        {0, 0, 0, 0, 0, 0, 0, 0, 2}, 
        {0, 0, 0, 0, 9, 0, 4, 0, 0}},
       // sudoku 16
       {{0, 0, 0, 9, 1, 0, 7, 0, 5},   
        {0, 0, 0, 0, 0, 0, 0, 0, 0},   
        {4, 5, 0, 0, 0, 0, 0, 3, 0},   
        {7, 0, 3, 0, 0, 0, 0, 0, 0},   
        {0, 9, 2, 0, 0, 0, 6, 0, 0},   
        {0, 0, 0, 0, 2, 4, 0, 8, 0},   
        {0, 0, 0, 0, 0, 0, 4, 0, 8},   
        {0, 0, 0, 0, 0, 9, 0, 6, 0},   
        {1, 0, 6, 4, 0, 8, 0, 0, 2}}}; 

  // Init Rand
  srand(time(NULL));
  // Init
  for (int i = 0; i < 9*9; i++)  
      sudokubuf1[i/9][i%9] = sudokubuf2[i/9][i%9] = sudoku[SUNR][i/9][i%9];
  // Simple solution attempt
  while(find2(sudokubuf2)||find3(sudokubuf2)); 
  while(find2(sudokubuf2)||find3(sudokubuf2));
  while(find4(sudokubuf2)||find2(sudokubuf2)||find3(sudokubuf2));
  while(find1(sudokubuf2)||find2(sudokubuf2)||find3(sudokubuf2)||find4(sudokubuf2));
  while(find1(sudokubuf2)||find2(sudokubuf2)||find3(sudokubuf2)||find4(sudokubuf2));
  print(sudokubuf1, sudokubuf2);
  printf("Sudoku %i had %ld tests\n", SUNR, deep);
  int no_solution = 0, solutions = 0;
  for (int row = 0; row < 9; row++)
    for (int col = 0; col < 9; col++) {
      if (sudokubuf2[row][col] == 0) no_solution++;
      if (sudokubuf2[row][col] != sudokubuf1[row][col]) solutions++;
    }
  if (no_solution) {
    printf("No full solution found\n");
    if (solutions)
      printf("but %i new numbers found\n\n", solutions);
    else
      putchar('\n');
  } else
    putchar('\n');

  // Init
  for (int i = 0; i < 9*9; i++)  
      sudokubuf1[i/9][i%9] = sudokubuf2[i/9][i%9] = sudoku[SUNR][i/9][i%9];
    
  // Complete solution
  deep = 0;
  if (solve(sudokubuf2))
    print(sudokubuf1, sudokubuf2);
  else
    printf("No solution exists\n");
  printf("Sudoku %i had %ld tests\n", SUNR, deep);
  return 0;
}
