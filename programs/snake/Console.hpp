/**************************
 * Console.h
 *
 * This header file provides prototypes for all the console functions used.
 *
 *************************/

#ifndef CONSOLE_H_
#define CONSOLE_H_

#include "console.hpp"

extern const int ENTER_KEY;
void setPosition(short x, short y);
void clearScreen(void);
void cursorBlinking(bool flag);
void changeBufferingState(bool unbuffered);
void clearInput(void);
int getKey(void);
bool keyPressed(void);
void SleepMicrosec(int microsec);
void printPixel(PixelType type);
void setPixel(Coord coord, PixelType type);
void setColor(Color color);
void removePixel(Coord coord);
void printCopyright(const char* message, Coord consoleSize);
void printScore(int score, Coord consoleSize);
Coord getConsoleSize(void);
void prepareConsole(void);
void resetConsole(void);

#endif /* CONSOLE_H_ */
