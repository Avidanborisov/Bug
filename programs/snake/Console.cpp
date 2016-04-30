/**************************
 * Console.c
 *
 * This file is responsible for all the 'lower' console behavior.
 *
 *************************/

#include "Types.hpp"
#include <console.hpp>
#include <system.hpp>

const int ENTER_KEY = '\n';

void changeBufferingState(bool unbuffered) { }

void clearInput(void) { }

int getKey(void) {
    return Console::getChar();
}

bool keyPressed(void) {
    return Console::hasInput();
}

void SleepMicrosec(int microsec)
{
    System::sleep(microsec);
}

void cursorBlinking(bool flag) {
    Console::dontPrint(flag);
}

void clearScreen(void)
{
    Console::clear();
}

void setPosition(short x, short y)
{
    Console::setPosition(x, y);
}

void setColor(Color color)
{

}

Coord getConsoleSize(void)
{
    return { 77, 20 };
}

void prepareConsole(void)
{
    clearScreen();
    changeBufferingState(true);
    cursorBlinking(true);
}

void resetConsole(void)
{
    changeBufferingState(false);
    cursorBlinking(false);
//    setColor(RESET_COLOR);
    clearScreen();
}

void printPixel(PixelType type)
{
	static const struct
	{
		const int pixel;
		const Color color;
	} pixels[] =
	{
        { '#', Console::Color::WHITE },			/* WALL			*/
        { '0', Console::Color::YELLOW },		/* SNAKE_HEAD	*/
        { 'O', Console::Color::BROWN },	/* SNAKE_BODY	*/
        { '+', Console::Color::RED },			/* REGULAR_FOOD	*/
        { '$', Console::Color::GREEN }			/* SPECIAL_FOOD	*/
	};

    Console::printf(pixels[type].color, "%c", pixels[type].pixel);
}

void setPixel(Coord coord, PixelType type)
{
	setPosition(coord.x, coord.y);
	printPixel(type);
}

void removePixel(Coord coord)
{
	setPosition(coord.x, coord.y);
    Console::print(' ');
}

void printScore(int score, Coord consoleSize)
{
	setPosition(1, consoleSize.y + 2); /* bottom left of the screen */
    Console::printf(Console::Color::CYAN, "Score: %d", score);
}

void printCopyright(const char* message, Coord consoleSize)
{
    setPosition(consoleSize.x - String::length(message) + 1, consoleSize.y + 2); /* bottom right of the screen */
    Console::print(message, Console::Color::BLUE);
	setPosition(1, 1); /* a weird bug will happen otherwise.. :S */
}
