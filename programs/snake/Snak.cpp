/**************************
 * Console.c
 *
 * This file is responsible for the actual Snake gameplay.
 *
 *************************/

#include "Types.hpp"
#include "Console.hpp"
#include "Snake.hpp"
#include <system.hpp>

#define ENTER_KEY '\n'

void* safeAlloc(size_t size)
{
    void* p = new char[size];

    if (p == nullptr)
	{
        Console::printf("Memory allocation error!\n");
        System::exit();
	}

	return p;
}

void assignDefaultValues(SnakeData* Snake)
{
	static bool firstSpeed = true;

	if (firstSpeed) /* We don't want the speed to change each time we return to the menu.. */
	{
		Snake->speed = 5;
		firstSpeed = false;
	}

	Snake->initialLength = 3;
	Snake->direction = RIGHT;
	Snake->score = 0;
	Snake->range = getConsoleSize();
	Snake->returnMenu = false;
}

void initializeSnake(SnakeData* Snake)
{
	int i;

	clearScreen();
	drawMap(Snake);

    printCopyright("Made by Avidan", Snake->range); /* Yep, made by me */

    Element* element = Snake->head = new Element;

	for (i = 1; i <= Snake->initialLength; i++)
	{
		/* Assign initial coordinates for the snake's elements */
		element->coord.x = ((Snake->range.x) / 2) - i;
		element->coord.y = Snake->range.y / 2;

		if (i < Snake->initialLength) /* while we don't reach the 'tale' (last element) of the snake */
		{
            element->next = new Element;
			element = element->next;
		}
		else /* we reached the 'tale' */
		{
			element->next = NULL;
		}
	}

	Snake->tail = element; /* last element */

	Snake->specialFood = generateFood(&Snake->food, Snake->range);
}

void drawMap(SnakeData* Snake)
{
	Coord wall = { 1, 1 };

	for (; wall.x <= Snake->range.x; wall.x++) /* print top wall */
	{
		setPixel(wall, WALL);
	}

	for (wall.x--; wall.y <= Snake->range.y; wall.y++) /* print right wall */
	{
		setPixel(wall, WALL);
	}

	for (wall.x = 1; wall.x <= Snake->range.x; wall.x++) /* print bottom wall */
	{
		setPixel(wall, WALL);
	}

	for (wall.x = 1, wall.y = 1; wall.y <= Snake->range.y; wall.y++) /* print left wall */
	{
		setPixel(wall, WALL);
	}
}

void getDirection(SnakeData* Snake)
{
	int ch;

	if (keyPressed())
	{
		ch = getKey();
        int key = ch; /* Added for CAPS LOCK compatibility */

		/* Change direction (only if the new direction is different, and not opposite) */
		if ((Snake->direction == UP || Snake->direction == DOWN)
				&& (key == LEFT || key == RIGHT))
		{
            Snake->direction = (Direction)key;
		}
		else if ((Snake->direction == LEFT || Snake->direction == RIGHT)
				 && (key == UP || key == DOWN))
		{
            Snake->direction = (Direction)key;
		}

		else if (key == ENTER_KEY) /* return to the menu if ENTER was pressed */
		{
		    Snake->returnMenu = true;
		}
	}
}

void moveSnake(SnakeData* Snake)
{
	Element* element = Snake->head->next;
	Coord next, last = Snake->head->coord;

	/* Move the head according to the direction */
	switch (Snake->direction)
	{
		case LEFT:
		{
			Snake->head->coord.x--;
			break;
		}

		case RIGHT:
		{
			Snake->head->coord.x++;
			break;
		}

		case UP:
		{
			Snake->head->coord.y--;
			break;
		}

		case DOWN:
		{
			Snake->head->coord.y++;
			break;
		}
	}

	/* Display the head of the snake */
	setPixel(Snake->head->coord, SNAKE_HEAD);

	/* Move and display the body of the snake */
	while (element != NULL)
	{
		next = element->coord;
		element->coord = last;
		last = next;

		setPixel(element->coord, SNAKE_BODY);

		element = element->next;
	}

	/* Remove the old 'tail' of the snake */
	removePixel(last);
}

bool continuePlaying(SnakeData* Snake)
{
	if (hitWall(Snake) || hitItself(Snake) || Snake->returnMenu)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool hitWall(SnakeData* Snake)
{
	if (Snake->head->coord.x <= 1 || Snake->head->coord.y <= 1
		|| Snake->head->coord.x >= Snake->range.x
		|| Snake->head->coord.y > Snake->range.y) /* if the coordinate of the snake is beyond it's range, it means that the snake hit a wall */
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool hitItself(SnakeData* Snake)
{
	Element* element = Snake->head->next;

	while (element != NULL)
	{
		/* Loop through all the elements, and check if their coordinates are the same as the head's one */
		if (Snake->head->coord.x == element->coord.x
				&& Snake->head->coord.y == element->coord.y)
		{
			return true;
		}

		element = element->next;
	}

	return false;
}

bool generateFood(Coord* food, Coord range)
{
	/* Generate random location (within range) for the food */
	food->x = randRange(2, range.x - 1);
	food->y = randRange(2, range.y - 1);

	if (randRange(1, 7) == 7) /* just some random value */
	{
		return true; /* special food */
	}
	else
	{
		return false; /* regular food */
	}
}

void checkFood(SnakeData* Snake)
{
	if (Snake->head->coord.x == Snake->food.x
			&& Snake->head->coord.y == Snake->food.y) /* If the snake reached the food */
	{
		if (Snake->specialFood) /* if the food was special */
		{
			Snake->score += 5; /* only increase score */
		}

		else /* regular.. */
		{
			/* Increase the size of the snake */
            Snake->tail->next = new Element;
			Snake->tail->next->coord = Snake->tail->coord;
			Snake->tail = Snake->tail->next;
			Snake->tail->next = NULL;

			Snake->score++; /* increase score */
		}

		/* Generate new food */
		Snake->specialFood = generateFood(&Snake->food, Snake->range);
	}

	setPixel(Snake->food, Snake->specialFood ? SPECIAL_FOOD : REGULAR_FOOD); /* display the food */
}

static unsigned long int next = 1;

int rand(void) // RAND_MAX assumed to be 32767
{
    next = next * 1103515245 + 12345;
    return (unsigned int)(next/65536) % 32768;
}

short randRange(short lowest, short highest)
{
	static bool randomize = true;

	if (randomize)
	{
//		srand(time(NULL));
		randomize = false;
	}

	return (rand() % (highest - lowest + 1) + lowest);
}

void wait(int speed)
{
	const int MAX_SPEED = 10;
	const int MIN_WAIT = 25;
	const int SPEED_MULTIPLIER = 20;

	SleepMicrosec(MIN_WAIT + (MAX_SPEED - speed) * SPEED_MULTIPLIER);
}

void deleteSnake(Element* head)
{
	Element* temp;

	while (head != NULL)
	{
		temp = head->next;
//		free(head);
		head = temp;
	}
}
