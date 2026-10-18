#ifndef UI
#define UI

#include "raylib.h"
#include "gamesim.h"
#include "stdbool.h"

void uiBox(float x, float y, float width, float height, Color color);
void button(GameState* state, float x, float y, float width, float height, char* text, void (*function)(GameState* state));
bool boolButton(float x, float y, float width, float height, char* text); // same as button but returns true if clicked false if not. Doesn't accept function pointers.
void bar(float x, float y, float width, float value, float maxValue);


void insertNumberInString(char* string, int start, int end, int number);


#endif