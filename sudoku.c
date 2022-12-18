// LINUX
// gcc -Wall -Wextra -O3 sudoku.c -o sudoku && ./sudoku
// WINDOWS - https://mingw.osdn.io
// path %PATH%;C:\MinGW\bin
// "C:\Program Files\Notepad++\notepad++.exe"  C:\MinGW\sudoku.c
// gcc -o sudoku C:\MinGW\sudoku.c && sudoku.exe

// https://onlinegdb.com/n3JqjPJTU
// https://github.com/mentalmove/SudokuGenerator/branches/stale
// https://github.com/mislah/Sudoku/blob/main/sudoku.c
// https://www.geeksforgeeks.org/sudoku-backtracking-7
// https://developpaper.com/source-code-of-solving-sudoku-program-in-c-language/
// https://formatter.org/cpp-formatter

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

static unsigned long deep;


// Mix from 0-8
void mix(int buf[9]) {                    
  for (int i = 0; i < 9; i++) buf[i] = i;  // init with 0-8
  for (int i = 0; i < 9; i++) {            // use Fisherb Yates shuffle
    int rpos = rand() % 9;
    int temp = buf[i];
    buf[i] = buf[rpos];
    buf[rpos] = temp;
  }
}

// find an unassigned location in the grid
int find_empty_forward(int grid[9][9], int *row, int *col) {
  for (*row = 0; *row < 9; (*row)++)
    for (*col = 0; *col < 9; (*col)++)
      if (grid[*row][*col] == 0) return 1;
  return 0;
}

// find an unassigned location in the grid (reverse)
int find_empty_backward(int grid[9][9], int *row, int *col) {
  for (*row = 8; *row >= 0; (*row)--)
    for (*col = 8; *col >= 0; (*col)--)
      if (grid[*row][*col] == 0) return 1;
  return 0;
}

// Function to check if a number is safe to be placed in a particular location
int num_safe(int grid[9][9], int row, int col, int num) {
  deep++;
  for (int i = 0; i < 9; i++) 
    if (grid[row][i] == num || grid[i][col] == num) 
      return 0;
  int subgridRow = row / 3;
  int subgridCol = col / 3;
  for (int i = 0; i < 3; i++) 
    for (int j = 0; j < 3; j++) 
      if (grid[subgridRow * 3 + i][subgridCol * 3 + j] == num) 
        return 0;
  return 1;
}

// Recursive function to solve the sudoku
int solve_forward(int grid[9][9]) {
  int row, col;
  if (!find_empty_forward(grid, &row, &col)) return 1;
  for (int num = 1; num <= 9; num++)
    if (num_safe(grid, row, col, num)) {
      grid[row][col] = num;
      if (solve_forward(grid)) return 1;
      grid[row][col] = 0;
    }
  return 0;
}

// Recursive function to solve the sudoku (reverse)
int solve_backward(int grid[9][9]) {
  int row, col;
  if (!find_empty_backward(grid, &row, &col)) return 1;
  for (int num = 9; num > 0; num--) 
    if (num_safe(grid, row, col, num)) {
      grid[row][col] = num;
      if (solve_backward(grid)) return 1;
      grid[row][col] = 0;
    }
  return 0;
}

// compares two sudoku to check if they are equal:
int check_diff(int buf1[9][9], int buf2[9][9])
{
  for (int i = 0; i < 9; i++)
    for (int j = 0; j < 9; j++)
      if (buf1[i][j] != buf2[i][j])
        return 0; // buf1 and buf2 is not equal
  return 1; // buf1 and buf2 is equal 
}

// check the validity of sudoku 
int check_sudoku(int buf[9][9]) {
  for (int i = 0; i < 9; i++) {
    int block[9] = {0}, row[9]= {0}, col[9]= {0};
    for (int j = 0; j < 9; j++) {
      row[buf[i][j] - 1]++;
      col[buf[j][i] - 1]++;
      block[buf[(i / 3) * 3 + j / 3][(i % 3) * 3 + j % 3] - 1]++;
    }
    for (int k = 0; k < 9; k++) 
      if (row[k] > 1 || col[k] > 1 || block[k] > 1) 
        return 0; //the sudoku is invalid
  }
  return 1; //the sudoku is valid
}

int check_sudoku2(int buf[9][9]) {
  for (int i = 0; i < 9; i++) {
    int block[9] = {0}, row[9]= {0}, col[9]= {0};
    for (int j = 0; j < 9; j++) {
      row[buf[i][j] - 1]++;
      col[buf[j][i] - 1]++;
      block[buf[(i / 3) * 3 + j / 3][(i % 3) * 3 + j % 3] - 1]++;
    }
    for (int k = 0; k < 9; k++) 
      if (row[k] > 1 || col[k] > 1 || block[k] > 1) 
        return 0;
  }
  return 1;
}

// find a number in the line that is missing and legally possible only once
int find1(int sudoku[9][9]) {
  int hit_count, found, hit_col = 0, found_cnt = 0;
  int buf[2][9];
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
        printf("Hit on row %i, column %i with rule 1\n", row + 1, hit_col + 1);
      }
    }
  }
  return found_cnt;
}

// find a number in the column that is missing and legally possible only once
int find2(int sudoku[9][9]) {
  int hit_count, found, hit_row = 0, found_cnt = 0;
  int buf[2][9];
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
        printf("Hit on row %i, column %i with rule 2\n", hit_row + 1, col + 1);
      }
    }
  }
  return found_cnt;
}

// find a number in the group that is missing and legally possible only once
int find3(int sudoku[9][9]) {
  int hit_count, found, hit_row = 0, hit_col = 0, hit_elm = 0, found_cnt = 0,
                        row, col;
  int buf[2][9];
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
        printf("Hit on group %i, element %i with rule 3\n", grp + 1,
               hit_elm + 1);
      }
    }
  }
  return found_cnt;
}

// If on one position only on number posible we fond it!
int find4(int sudoku[9][9]) {
  int hit_num, hit_count = 0, found_cnt = 0;
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
          printf("Hit on row %i, column %i with rule 4\n", row + 1, col + 1);
          sudoku[row][col] = hit_num;
          found_cnt++;
        }
      }
    }
  }
  return found_cnt;
}



// function to print sudoku
#define RED "\x1B[31m"
#define NRM "\x1B[0m"
#ifdef __linux__
void printsudoku(int arr1[9][9], int arr2[9][9]) {
  static const char *border_row = "┏━━━┯━━━┯━━━┳━━━┯━━━┯━━━┳━━━┯━━━┯━━━┓\n";
  static const char *separator_row = "┠───┼───┼───╂───┼───┼───╂───┼───┼───┨\n";
  static const char *middle_border_row =
      "┣━━━┿━━━┿━━━╋━━━┿━━━┿━━━╋━━━┿━━━┿━━━┫\n";
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
void printsudoku(int arr1[9][9], int arr2[9][9]) {
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
  
 #define SUNR 18

  
int sudoku[30][9][9] =
 // sudoku 00 
    {{{0, 0, 0, 0, 0, 0, 0, 0, 0},  
      {0, 0, 0, 0, 0, 0, 0, 0, 0},  
      {0, 0, 0, 0, 0, 0, 0, 0, 0},  
      {0, 0, 0, 0, 0, 0, 0, 0, 0},  
      {0, 0, 0, 0, 0, 0, 0, 0, 0},  
      {0, 0, 0, 0, 0, 0, 0, 0, 0},  
      {0, 0, 0, 0, 0, 0, 0, 0, 0},  
      {0, 0, 0, 0, 0, 0, 0, 0, 0},  
      {0, 0, 0, 0, 0, 0, 0, 0, 0}}, 
  // sudoku 01
     {{0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0}},
     // sudoku 02
     {{2, 0, 7, 0, 3, 8, 1, 0, 0},
      {0, 8, 3, 0, 0, 0, 2, 7, 0},
      {0, 0, 0, 5, 2, 7, 3, 0, 8},
      {0, 0, 0, 8, 4, 2, 6, 0, 1},
      {0, 1, 2, 6, 7, 0, 4, 0, 3},
      {0, 0, 0, 3, 0, 0, 7, 0, 2},
      {0, 0, 0, 0, 5, 3, 9, 2, 0},
      {0, 0, 5, 2, 0, 0, 8, 1, 0},
      {4, 2, 0, 0, 8, 0, 5, 3, 0}},
     // sudoku 03
     {{0, 0, 5, 0, 0, 0, 6, 7, 1},
      {0, 0, 6, 0, 0, 4, 0, 0, 0},
      {0, 7, 0, 5, 0, 9, 0, 0, 2},
      {0, 0, 0, 7, 1, 6, 0, 0, 5},
      {4, 6, 0, 0, 0, 0, 0, 0, 3},
      {0, 0, 9, 0, 0, 3, 8, 0, 0},
      {7, 9, 8, 4, 0, 0, 0, 0, 0},
      {0, 2, 0, 9, 0, 0, 1, 3, 0},
      {1, 5, 3, 0, 0, 2, 0, 9, 4}},
     // sudoku 04
     {{0, 0, 0, 0, 0, 0, 0, 0, 1},
      {0, 0, 0, 8, 0, 0, 0, 0, 0},
      {3, 0, 0, 7, 6, 0, 0, 0, 0},
      {0, 0, 0, 2, 0, 8, 0, 5, 0},
      {0, 0, 0, 3, 0, 0, 2, 0, 0},
      {0, 8, 9, 0, 1, 0, 0, 0, 0},
      {0, 0, 0, 0, 4, 0, 3, 2, 0},
      {0, 0, 0, 0, 0, 9, 0, 0, 8},
      {0, 5, 0, 0, 0, 0, 0, 0, 0}},
     // sudoku 05
     {{9, 2, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 3, 8, 0, 0, 2},
      {0, 4, 0, 0, 0, 6, 0, 0, 0},
      {4, 8, 6, 7, 1, 2, 9, 5, 3},
      {0, 0, 2, 3, 4, 0, 8, 0, 6},
      {0, 0, 0, 6, 8, 0, 0, 0, 4},
      {8, 6, 4, 0, 5, 0, 1, 0, 0},
      {0, 5, 9, 0, 6, 0, 0, 8, 0},
      {0, 3, 7, 8, 0, 0, 0, 6, 0}},
     // sudoku 06
     {{3, 1, 6, 5, 7, 8, 4, 9, 2},
      {5, 2, 9, 1, 3, 4, 7, 6, 8},
      {4, 8, 7, 6, 2, 9, 5, 3, 1},
      {2, 6, 3, 0, 1, 5, 9, 8, 7},
      {9, 7, 4, 8, 6, 0, 1, 2, 5},
      {8, 5, 1, 7, 9, 2, 6, 4, 3},
      {1, 3, 8, 0, 4, 7, 2, 0, 6},
      {6, 9, 2, 3, 5, 1, 8, 7, 4},
      {7, 4, 5, 0, 8, 6, 3, 1, 0}},
     // sudoku 07
     {{3, 0, 6, 5, 0, 8, 4, 0, 0},
      {5, 2, 0, 0, 0, 0, 0, 0, 0},
      {0, 8, 7, 0, 0, 0, 0, 3, 1},
      {0, 0, 3, 0, 1, 0, 0, 8, 0},
      {9, 0, 0, 8, 6, 3, 0, 0, 5},
      {0, 5, 0, 0, 9, 0, 6, 0, 0},
      {1, 3, 0, 0, 0, 0, 2, 5, 0},
      {0, 0, 0, 0, 0, 0, 0, 7, 4},
      {0, 0, 5, 2, 0, 6, 3, 0, 0}},
     // sudoku 08
     {{0, 0, 5, 3, 0, 0, 0, 0, 0},
      {8, 0, 0, 0, 0, 0, 0, 2, 0},
      {0, 7, 0, 0, 1, 0, 5, 0, 0},
      {4, 0, 0, 0, 0, 5, 3, 0, 0},
      {0, 1, 0, 0, 7, 0, 0, 0, 6},
      {0, 0, 3, 2, 0, 0, 0, 8, 0},
      {0, 6, 0, 5, 0, 0, 0, 0, 9},
      {0, 0, 4, 0, 0, 0, 0, 3, 0},
      {0, 0, 0, 0, 0, 9, 7, 0, 0}},
     // sudoku 09
     {{0, 0, 0, 0, 0, 6, 0, 0, 0},
      {0, 5, 9, 0, 0, 0, 0, 0, 8},
      {2, 0, 0, 0, 0, 8, 0, 0, 0},
      {0, 4, 5, 0, 0, 0, 0, 0, 0},
      {0, 0, 3, 0, 0, 0, 0, 0, 0},
      {0, 0, 6, 0, 0, 3, 0, 5, 4},
      {0, 0, 0, 3, 2, 5, 0, 0, 6},
      {0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0}},
     // sudoku 10
     {{0, 0, 0, 0, 0, 5, 0, 8, 0},
      {0, 0, 0, 6, 0, 1, 0, 4, 3},
      {0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 1, 0, 5, 0, 0, 0, 0, 0},
      {0, 0, 0, 1, 0, 6, 0, 0, 0},
      {3, 0, 0, 0, 0, 0, 0, 0, 5},
      {5, 3, 0, 0, 0, 0, 0, 6, 1},
      {0, 0, 0, 0, 0, 0, 0, 0, 4},
      {0, 0, 0, 0, 0, 0, 0, 0, 0}},
     // sudoku 11
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
     // sudoku 13
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
      {1, 0, 6, 4, 0, 8, 0, 0, 2}},
     // sudoku 17
     {{0, 0, 0, 0, 0, 0, 0, 9, 0},
      {0, 0, 0, 0, 0, 4, 0, 0, 7},
      {0, 0, 0, 0, 6, 0, 0, 0, 3},
      {0, 5, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 8, 0, 0, 0, 5, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 2, 0},
      {1, 0, 0, 0, 8, 0, 0, 0, 0},
      {6, 0, 0, 2, 0, 0, 0, 0, 0},
      {0, 3, 0, 0, 0, 0, 0, 0, 0}},
     // sudoku 18
     {{0, 2, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 6, 0, 0, 0, 0, 3},
      {0, 7, 4, 0, 8, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 3, 0, 0, 2},
      {0, 8, 0, 0, 4, 0, 0, 1, 0},
      {6, 0, 0, 5, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 1, 0, 7, 8, 0},
      {5, 0, 0, 0, 0, 9, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 4, 0}},
     // sudoku 19
     {{0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 8, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 2, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 5, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 3, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0}},
     // sudoku 20
     {{0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 3, 0, 8, 5},
      {0, 0, 1, 0, 2, 0, 0, 0, 0},
      {0, 0, 0, 5, 0, 7, 0, 0, 0},
      {0, 0, 4, 0, 0, 0, 1, 0, 0},
      {0, 9, 0, 0, 0, 0, 0, 0, 0},
      {5, 0, 0, 0, 0, 0, 0, 7, 3},
      {0, 0, 2, 0, 1, 0, 0, 0, 0},
      {0, 0, 0, 0, 4, 0, 0, 0, 9}},
     // sudoku 21
     {{8, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 3, 6, 0, 0, 0, 0, 0},
      {0, 7, 0, 0, 9, 0, 2, 0, 0},
      {0, 5, 0, 0, 0, 7, 0, 0, 0},
      {0, 0, 0, 0, 4, 5, 7, 0, 0},
      {0, 0, 0, 1, 0, 0, 0, 3, 0},
      {0, 0, 1, 0, 0, 0, 0, 6, 8},
      {0, 0, 8, 5, 0, 0, 0, 1, 0},
      {0, 9, 0, 0, 0, 0, 4, 0, 0}}};

  // Init Rand
  srand(time(NULL));
  // Init
  deep = 0;
  for (int i = 0; i < 9 * 9; i++)
    sudokubuf1[i / 9][i % 9] = sudokubuf2[i / 9][i % 9] =
        sudoku[SUNR][i / 9][i % 9];
  // Simple solution attempt
 
  while (find1(sudokubuf2) || find2(sudokubuf2) || find3(sudokubuf2) || find4(sudokubuf2));
  while (find1(sudokubuf2) || find2(sudokubuf2) || find3(sudokubuf2) || find4(sudokubuf2));
  printsudoku(sudokubuf1, sudokubuf2);
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
   deep = 0;
  for (int i = 0; i < 9 * 9; i++)
    sudokubuf1[i / 9][i % 9] = sudokubuf2[i / 9][i % 9] =
        sudoku[SUNR][i / 9][i % 9];
 
  // Complete solution
  if (solve_forward(sudokubuf2))
    printsudoku(sudokubuf1, sudokubuf2);
  else
    printf("No solution exists\n");
  printf("Sudoku %i had %ld tests\n", SUNR, deep);
  
  // Init
  deep = 0;
  for (int i = 0; i < 9 * 9; i++)
    sudokubuf1[i / 9][i % 9] = sudokubuf2[i / 9][i % 9] =
        sudoku[SUNR][i / 9][i % 9];  
        
  
   // Complete solution (reverse)
  if (solve_backward(sudokubuf2))
    printsudoku(sudokubuf1, sudokubuf2);
  else
    printf("No solution exists\n");
  printf("Sudoku %i had %ld tests\n", SUNR, deep);
  
  
  return 0;
}
