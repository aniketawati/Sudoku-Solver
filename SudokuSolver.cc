/*
 ******************************************************************************
 *
 *  fileName    :   SudokuSolver.cc
 *
 *  Author      :   Aniket Awati <an.aaasss@gmail.com>
 *  Modified By :   Aditya Shevade <aditya.shevade@gmail.com>
 *
 *  Version     :   2.0.0
 *  Created     :   10/11/2009
 *  Modified    :   12/06/2011
 *
 *  Description :   This class solves SuDoKu puzzles. It receives a 2D array
 *                  during initialization (via constructor). The empty cells
 *                  in the puzzle can have any random value. The class will
 *                  ignore them while reading the input.
 *
 *                  The constructor itself calls the initialization methods
 *                  followed by the solvers and finally writes the input array
 *                  (using a reference call - does not return anything).
 *
 *  License     :   This program is free software: you can redistribute it and/or modify
 *                  it under the terms of the GNU General Public License as published by
 *                  the Free Software Foundation, either version 3 of the License, or
 *                  (at your option) any later version.
 *
 *                  This program is distributed in the hope that it will be useful,
 *                  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *                  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *                  GNU General Public License for more details.
 *
 *                  You should have received a copy of the GNU General Public License
 *                  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Changelog   :
 *      12/06/2011  :   Added license.
 *                      Cleaned up the code.
 *
 ******************************************************************************
 */

#include <iostream>
#include <cstdio>
#include <cstring>

using namespace std;

class SudokuSolver {

    public:

        int     NUM;                    // Constant 9
        int     ROW;                    // Constant 3
        int     COL;                    // Constant 3
        int     BLK;                    // Constant 3
        int     problemMatrix [9][9];   // Main problem matrix
        bool    tagRow [9][9];          // Row tags
        bool    tagCol [9][9];          // Column tags
        bool    tagBlk [9][9];          // Block tags

    SudokuSolver (int (&inputMatrix) [9][9]) {
        NUM = 9;
        ROW = 3;
        COL = 3;
        BLK = 3;
        initTag ();
        initPuzzle ();
        for (int j = 0; j < NUM; j++)
            for (int k = 0; k < NUM; k++) {
                if (inputMatrix[j][k] < 1 || inputMatrix[j][k] > NUM)
                    continue;
                else
                    problemMatrix [j][k] = inputMatrix [j][k];
            }
        fillTags();
        if (solvePuzzle ()) {
            for (int j = 0; j < NUM; j++)
                for (int k = 0; k < NUM; k++)
                    inputMatrix [j][k] = problemMatrix [j][k];
        }
        else {
            cerr << "Error: Puzzle cannot  be solved." << endl;
            exit (1);
        }
    }

    /*  This function initializes all the tags to false (as in empty). The tags
     *  are later used to determine if a number is valid in a row, column or
     *  block. Whenever the number is filled, the corresponding tags are set.
     */
    void initTag (void) {
        for (int j = 0; j < NUM; j++) {
            for (int k = 0; k < NUM; k++) {
                tagRow[j][k] = false;
                tagCol[j][k] = false;
                tagBlk[j][k] = false;
            }
        }
    }

    /*  This function makes all the cells of the main problem matrix zero.
     *  It is not really required if the input to the class already has
     *  zeroes in place instead of empty / garbage cells.
     */
    void initPuzzle (void) {
        for (int j = 0; j < NUM; j++) {
            for (int k = 0; k < NUM; k++) {
                problemMatrix   [j][k]  = 0;
            }
        }
    }

    /*  After reading the input puzzle, call this function. This function
     *  goes through the problem matrix and calls the assignTag method that
     *  sets the proper tags to true.
     */
    void fillTags (void) {
        for (int i = 0; i < NUM; i++)
            for (int j = 0; j < NUM; j++)
                assignTag (i, j, problemMatrix[i][j]);
    }

    /*  This function checks if the input value 'c' can be placed inside the
     *  problem matrix at the [i][j] position. The way this works is, the tags
     *  of the cells that are already filled are set to true. If the value 'c'
     *  is being written at a place [i][j] where it has no conflicts with the
     *  row, column or the block then it is valid and method returns true.
     */
    bool checkValid (int i, int j, int c) {
        c--;
        if (tagRow[i][c] == true)
            return false;
        if (tagCol[c][j] == true)
            return false;
        if (tagBlk[(i/BLK)*BLK+j/BLK][c] == true)
            return false;
        return true;
    }

    /*  This method is used to set the tags to true. Once it's determined that
     *  a value is valid (using the checkValid method, this method should be
     *  called. It will set the proper tags for the next checking.
     */
    void assignTag (int i, int j, int n) {
        if (n == 0)
            return;
        n--;
        tagRow[i][n] = true;
        tagCol[n][j] = true;
        tagBlk[(i/BLK)*BLK+j/BLK][n] = true;
    }

    /*  This method is used for the backtracking. Once the logical solving is
     *  done the class goes into backtracking mode. It sets a random valid value
     *  and tries to solve the puzzle. If it gives an error, the solver needs to
     *  go back (backtrack). This also means it needs to reset the tags which
     *  were set during the initial backtrack assumption.
     */
    void resetTag (int i, int j, int n) {
        if (n == 0)
            return;
        n--;
        tagRow[i][n] = false;
        tagCol[n][j] = false;
        tagBlk[(i/BLK)*BLK+j/BLK][n] = false;
    }

    /*  This method is just a placeholder for different solving mechanisms. The
     *  puzzles can have various difficulties and solver may need more methods.
     *  Those algorithms would be called through this method. Mainly for future
     *  use.
     */
    bool solvePuzzle(void) {
        solveLogical ();
        //printPuzzle();
        return solveBacktrack (0, 0, problemMatrix);
    }

    /*  This method tries to solve the puzzle logically. It's a simple set of
     *  implication rules. It goes on checking in each block, row and then
     *  column to check if there are any singles (cells where only one value
     *  can be put). If it finds such cells then it assigns the value.
     *
     *  The method is in an infinite loop. Until it does not have any value to
     *  assign to any cell it stays in the loop otherwise it breaks out.
     */
    void solveLogical (void) {
        int i, j, k, l = 0, row = 0, col = 0, cnt = 0;
        bool flagNotSingle = false; // Used to check if the cell is indeed a singles cell.
        bool flagValueSet = false;  // Is set if we made an assignment in the while loop.
        while (true) {
            flagValueSet = false; // Assume we make no assignment.
            for (k = 0; k < NUM; k++) { // Check all the 9 blocks for singles.
                for (l = 1; l < (NUM+1); l++) {
                    cnt = 0;
                    flagNotSingle = false; // Assume we have no singles.
                    for (i = (k/COL)*COL; i < ((k/COL)*COL + COL); i++) {
                        for (j = (k%COL)*ROW; j < ((k%COL)*ROW + ROW); j++) {
                            if (problemMatrix [i][j] > 0) // Skip the cells that are already filled.
                                continue;
                            if (checkValid (i,j,l)) { // If empty cell found, check if current value is valid.
                                row = i;
                                col = j;
                                cnt ++;
                            }
                            if (cnt > 1) { // Make sure it was a singles value and not doubles or more.
                                flagNotSingle = true; // If more than one valid value was found, break.
                                break;
                            }
                        }
                        if (flagNotSingle) // More than one valid value found, break.
                            break;
                    }
                    if (cnt == 1) { // If any singles value was found then assign it.
                        problemMatrix [row][col] = l;
                        assignTag (row, col, l); // Then set tags.
                        flagValueSet = true; // Set the flag that assignment was made.
                    }
                }
            }

            for (i = 0; i < NUM; i++) { // Check all columns.
                for (l = 1; l < NUM; l++) {
                    cnt = 0;
                    for (j = 0; j < NUM; j++) {
                        if (problemMatrix [i][j] > 0) // Skip cells that are already filled.
                            continue;
                        if (checkValid (i, j, l)) { // If empty cell found, check if current value is valid.
                            row = i;
                            col = j;
                            cnt++;
                        }
                        if (cnt > 1) { // Make sure it was a singles value and not doubles or more.
                            flagNotSingle = true; // If more than one valid value was found, break.
                            break;
                        }
                    }
                    if (cnt == 1) { // If any singles value was found then assign it.
                        problemMatrix [row][col] = l;
                        assignTag (row, col, l); // Then set tags.
                        flagValueSet = true; // Set the flag that assignment was made.
                    }
                }
            }

            for (i = 0; i < NUM; i++) { // Check all rows.
                for (l = 1; l < NUM; l++) {
                    cnt = 0;
                    for (j = 0; j < NUM; j++) {
                        if (problemMatrix [j][i] > 0) // Skip cells that are already filled.
                            continue;
                        if (checkValid (j, i, l)) { // If empty cell found, check if current value is valid.
                            row = j;
                            col = i;
                            cnt++;
                        }
                        if (cnt > 1) { // Make sure it was a singles value and not doubles or more.
                            flagNotSingle = true; // If more than one valid value was found, break.
                            break;
                        }
                    }
                    if (cnt == 1) { // If any singles value was found then assign it.
                        problemMatrix [row][col] = l;
                        assignTag (row, col, l); // Then set tags.
                        flagValueSet = true; // Set the flag that assignment was made.
                    }
                }
            }
            if(!flagValueSet) // If assignment was not made during an iteration, break out of logical solve.
                break;
        }
    }

    /*  This method is called after the logical solver. This method can be
     *  directly called to solve the puzzles also but that has a problem.
     *
     *  Some puzzles are designed such that backtrack solvers take a long
     *  time to solve them. (Google for more information). The logical solver
     *  removed some of the issues from that and makes solving faster.
     *
     *  For simple puzzles, logical solve will give the result and this method
     *  will just exit on initial check.
     *
     *  The method is simple, it is similar to logical solve but instead of
     *  checking if the value is singles or not, it only checks if the value
     *  is valid, if it is then the solver assumes that to be the correct value
     *  and advances, if it encounters an error, it goes back to last modified
     *  cell and checks for another valid value. If there is none it goes back
     *  another step and so on till all the cells are filled.
     */
    bool solveBacktrack (int i, int j, int proMat [9][9]) {
        if (i == NUM) { // This function goes checking row after row.
            i = 0;
            if (++j == NUM)   // Once a row is finished, go to next row.
                return true; // If final cell is reached, puzzle is solved.
        }
        if (proMat[i][j] > 0)  // Skip filled cells
            return solveBacktrack (i + 1, j, proMat); // Check next row of current column.

        for (int val = 1; val <= NUM; val++) { // Empty cell found. Check from 1 to 9.
            if (checkValid(i, j, val)) { // If it is legal (check description of legal) then fill the cell.
                proMat[i][j] = val;
                assignTag (i, j, val); // Also set the tags for checking next value.
                if (solveBacktrack (i+1,j,proMat)) // And call the solve function again.
                    return true; // If it returns true that means the puzzle is solved.
                else // If not then we need to reset the tags and backtrack.
                    resetTag (i, j, proMat[i][j]);
            }
        }
        resetTag (i, j, proMat[i][j]); // Reset tags and backtrack.
        proMat[i][j] = 0; // Also reset the value for next iteration.
        return false;
    }

    /*  This method is used to print the problem matrix to console. Mainly used
     *  for debugging purpose.
     */
    void printPuzzle (void) {
        cout << "-------------------------------------------------------" << endl;
        for (int row = 0; row < NUM; row++) {
            cout << "| ";
            for (int col = 0; col < NUM; col++) {
                cout << " *" << problemMatrix[row][col] << "* ";
                if (!((col+1) % BLK))
                    cout << " | ";
            }
            if (!((row+1) % BLK))
                cout << endl << "-------------------------------------------------------" << endl;
            else
                cout << endl;
        }
    }

};

