/**************************
 * Console.h
 *
 * This header file provides prototypes for all the actual Snake Game functions.
 *
 *************************/

#ifndef SNAKE_H_
#define SNAKE_H_

void* safeAlloc(size_t size);
void assignDefaultValues(SnakeData* Snake);
void initializeSnake(SnakeData* Snake);
void drawMap(SnakeData* Snake);
void getDirection(SnakeData* Snake);
void moveSnake(SnakeData* Snake);
bool continuePlaying(SnakeData* Snake);
bool hitWall(SnakeData* Snake);
bool hitItself(SnakeData* Snake);
bool generateFood(Coord* food, Coord range);
void checkFood(SnakeData* Snake);
short randRange(short lowest, short highest);
void wait(int speed);
void deleteSnake(Element* head);

#endif /* SNAKE_H_ */
