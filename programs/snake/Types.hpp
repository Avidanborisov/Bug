/**************************
 * Types.c
 *
 * This file contains declarations of all data types being used in the program
 *
 *************************/

#ifndef TYPES_H_
#define TYPES_H_

/* Boolean type */

#include <console.hpp>
using Color = Console::Color;

typedef enum PixelType
{
	WALL,
	SNAKE_HEAD,
	SNAKE_BODY,
	REGULAR_FOOD,
	SPECIAL_FOOD
} PixelType;

typedef enum Direction
{
	LEFT = 'a',
	RIGHT = 'd',
	UP = 'w',
	DOWN = 's',
} Direction;

typedef struct Coord
{
	short x;
	short y;
} Coord;

typedef struct Element Element;
struct Element
{
	Coord coord;
	Element* next;
};

typedef struct SnakeData
{
	Direction direction;
	Element* head;
	Element* tail;
	int initialLength;
	int speed;
	Coord range;
	Coord food;
	bool specialFood;
	int score;
	bool returnMenu;
} SnakeData;

#endif /* TYPES_H_ */
