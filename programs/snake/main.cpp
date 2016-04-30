/**************************
 *  main.c
 *
 *	This file contains the main function which executes everything else.
 *
 *************************/

#include "Types.hpp"
#include "Console.hpp"
#include "Snake.hpp"
#include "Menu.hpp"

int main(void)
{
	SnakeData Snake;
	void (*execute[])(SnakeData*) =	{ play, speed, instructions, quit }; /* functions of menu options */
	Choice choice;

	prepareConsole();
	assignDefaultValues(&Snake);

	while (true)
	{
		choice = menu(&Snake);
		execute[choice](&Snake);
	}

	return 0;
}
