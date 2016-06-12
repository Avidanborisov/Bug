/**************************
 * Menu.c
 *
 * This file is responsible for the behavior of the menu
 *
 *************************/

#include "Types.hpp"
#include "Console.hpp"
#include "Snake.hpp"
#include "Menu.hpp"
#include <string.hpp>
#include <system.hpp>

#define ENTER_KEY '\n'

#define ARRAY_SIZE(a)			(sizeof a / sizeof *a)
#define CENTER_POS(Snake, text)	(((Snake->range.x - String::length(text)) / 2) + 1)

Choice menu(SnakeData* Snake)
{
	const char* logo[] =
	{
        "          _______..__   __.      ___       __  ___  _______             ",
        "         /       ||  \\ |  |     /   \\     |  |/  / |   ____|          ",
        "        |   (----`|   \\|  |    /  ^  \\    |  '  /  |  |__             ",
        "         \\   \\    |  . `  |   /  /_\\  \\   |    <   |   __|          ",
        "     .----)   |   |  |\\   |  /  _____  \\  |  .  \\  |  |____          ",
        "     |_______/    |__| \\__| /__/     \\__\\ |__|\\__\\ |_______|       ",
        "                                                                        ",
        "                                                                        ",
        "                                                                        "
	};

	const char* menuText[] =
	{
		"PLAY",
        "SPEED: %d",
		"INSTRUCTIONS",
		"QUIT",
	};

    Color colors[] = { Console::Color::GREEN, Console::Color::CYAN, Console::Color::BLUE, Console::Color::BROWN };

	int key, j, i = 0, xPosition = CENTER_POS(Snake, *logo); /* center of the screen */
	Choice choice = 0;

	clearScreen();

	if (xPosition > 0 && (Snake->range.y + 1) >
		ARRAY_SIZE(logo) + ARRAY_SIZE(menuText)) /* only if there is enough space */
	{	for (i = 0; i < ARRAY_SIZE(logo); i++) /* print the logo */
		{
			setPosition(xPosition, i + 1);
            Console::print(logo[i], Console::Color::RED);
		}
	}

	xPosition = CENTER_POS(Snake, *menuText);
    Color color = colors[0];

	for (j = 0; j < ARRAY_SIZE(menuText); j++) /* print the menu options */
	{
		setPosition(xPosition, i + j + 2);
        Console::printf(color, menuText[j], Snake->speed);
        color = Console::Color::LIGHT_GREY;
	}

	while ((key = getKey()) != ENTER_KEY) /* while an option isn't selected, navigate through the menu */
	{
		/* Deselect the current selection */
        color = Console::Color::LIGHT_GREY;
		setPosition(xPosition, i + choice + 2);
        Console::printf(color, menuText[choice], Snake->speed);

		/* Get new menu choice (tolower() for CAPS LOCK compatibility) */
        if (key == DOWN)
		{
			choice = (choice + 1) % ARRAY_SIZE(menuText);
		}
        else if (key == UP)
		{
			choice = (choice + ARRAY_SIZE(menuText) - 1) % ARRAY_SIZE(menuText);
		}

		/* Select new menu choice */
        color = colors[choice];
        setPosition(xPosition, i + choice + 2);
        Console::printf(color, menuText[choice], Snake->speed);
	}

	return choice;
}

void play(SnakeData* Snake)
{
	initializeSnake(Snake);

	while (continuePlaying(Snake))
	{
		getDirection(Snake);
		moveSnake(Snake);
		checkFood(Snake);
		printScore(Snake->score, Snake->range);
		wait(Snake->speed);
	}

	gameOver(Snake);
	assignDefaultValues(Snake);
	deleteSnake(Snake->head);
}

void speed(SnakeData* Snake)
{
	const int MIN_SPEED = 1, MAX_SPEED = 10;
	int key;

	while ((key = getKey()) != ENTER_KEY) /* continue while an ENTER isn't pressed */
	{
		if (key == UP || key == DOWN)
		{
			int change = (key == UP) ? 1 : -1; /* UP to increment speed (+1), DOWN to decrement speed (-1) */

			if ((Snake->speed + change >= MIN_SPEED)
					&& (Snake->speed + change <= MAX_SPEED)) /* change speed only if it isn't beyond limits */
			{
				Snake->speed += change; /* change speed */
                Console::printf(Console::Color::CYAN, "\b\b%.2d", Snake->speed); /* print new speed */
			}
		}
	}
}

void instructions(SnakeData* Snake)
{
	const char* text[] =
	{
        "Welcome to Bug Snake!\n", "\n",
		"_GAMEPLAY_\n",
		"The purpose of this game is to move the snake, trying to eat\n",
		"food, and thus achieving more score.\n", "\n",
        "The snake should avoid hitting walls, or itself.\n",
        "Otherwise, the game is over.\n", "\n",
        "Use 'w', 's', 'a' and 'd' to move the snake up, down,\n",
        "left and right, respectively.\n", "\n",
		"There are two types of food:\n",
		"* Regular food - shown as ",
		"It gives you a point, and increases the length of the snake.\n",
		"* Special food - shown as ",
        "It gives you 5 points, and doesn't increase the length of the snake.\n"
	};

	PixelType example = REGULAR_FOOD;
	int xPosition, i;
	bool printFood = false;

	clearScreen();
    Color color = Console::Color::LIGHT_GREEN;

	if (biggestString(text, ARRAY_SIZE(text)) <= Snake->range.x
		&& ARRAY_SIZE(text) <= Snake->range.y) /* only if there is enough place to display
												the instructions, we display them */
	{
		for (i = 0; i < ARRAY_SIZE(text); i++)
		{
            if (printFood) /* if we need to show food examples, we show them. */
			{
				printFood = false;
				printPixel(example);
				example = SPECIAL_FOOD; /* second food example is special food */
                Console::print('.', Console::Color::CYAN);
			}
			else if (*text[i] == '_') /* if 'title' */
			{
                color = Color::RED;
			}
			else if (*text[i] == '*') /* if 'food example' */
			{
				printFood = true;
			}

			/* Print the current sentence */
			xPosition = CENTER_POS(Snake, text[i]);
			setPosition(xPosition, i + 2);
            Console::print(text[i], color);

            color = Color::CYAN;
		}
	}
	else /* not enough place */
	{
        Console::print("There isn't enough place to display the instructions!");
	}

	while (getKey() != ENTER_KEY); /* wait for ENTER to be pressed. */
}

void quit(SnakeData* Snake)
{
	resetConsole();
    System::exit();
}

void gameOver(SnakeData* Snake)
{
	const char* logo[] =
	{
		" _____                         _____                ",
		"|  __ \\                       |  _  |               ",
		"| |  \\/ __ _ _ __ ___   ___   | | | |_   _____ _ __ ",
		"| | __ / _` | '_ ` _ \\ / _ \\  | | | \\ \\ / / _ \\ '__|",
		"| |_\\ \\ (_| | | | | | |  __/  \\ \\_/ /\\ V /  __/ |   ",
		" \\____/\\__,_|_| |_| |_|\\___|   \\___/  \\_/ \\___|_|   "
	};
	const char* format[] = { "Your Score: %d\n", "Press any key to continue..." };

	int i, xPosition = CENTER_POS(Snake, *logo); /* center of the screen */

	clearScreen();

	if (xPosition > 0 && (Snake->range.y + 1) > ARRAY_SIZE(logo) + 1) /* only if there is enough space */
	{
		for (i = 0; i < ARRAY_SIZE(logo); i++) /* print the logo */
		{
			setPosition(xPosition, i + 1);
            Console::print(logo[i], Console::Color::CYAN);
		}
	}

	/* Print the score */
	xPosition = CENTER_POS(Snake, format[0]);
	setPosition(xPosition, i + 2);
    Console::printf(format[0], Snake->score, Console::Color::LIGHT_RED);

	/* Press any key to continue... */
	xPosition = CENTER_POS(Snake, format[1]);
	setPosition(xPosition, i + 3);
    Console::print(format[1], Console::Color::RED);

	/* Clear possible input from the snake, and wait a bit */
	clearInput();
	SleepMicrosec(1000);
	clearInput();

	/* Get a key to continue */
	getKey();
}

int biggestString(const char** strings, const int size)
{
	static int biggest = 0;
	static bool firstTime = true;
	int i, len;

	if (firstTime) /* since this function is called only once, there is
					no need to find the biggest string more than once */
	{
		for (i = 0; i < size; i++)
		{
            if ((len = String::length(strings[i])) > biggest)
			{
				biggest = len;
			}
		}

		firstTime = false;
	}

	return biggest;
}
