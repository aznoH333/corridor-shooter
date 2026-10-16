#ifndef UI
#define UI
#include "raylib.h"

void uiBox(float x, float y, float width, float height, Color color);
void button(float x, float y, float width, float height, char* text, void (*function)());
void bar(float x, float y, float width, float value, float maxValue);


#endif