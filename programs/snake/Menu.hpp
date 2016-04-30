/**************************
 * Menu.h
 *
 * This header file provides prototypes for all the menu functions being used.
 *
 *************************/

#ifndef MENU_H_
#define MENU_H_

typedef int Choice;

Choice menu(SnakeData* Snake);
void play(SnakeData* Snake);
void speed(SnakeData* Snake);
void instructions(SnakeData* Snake);
void quit(SnakeData* snake);

void gameOver(SnakeData* Snake);
int biggestString(const char** strings, const int size);

#endif /* MENU_H_ */
