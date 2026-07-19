#ifndef _GUTILS
#define _GUTILS

#include "dirent.h"
#include "stddef.h"
#include "stdio.h"
#include "raylib.h"
#include "raymath.h"


// -------------------------------------------------------------------------------------
// ArrayHandle
// -------------------------------------------------------------------------------------
#define GenArrayDefinition(type, maxElements, arrayStructName) typedef struct { \
	type elements[maxElements]; \
	unsigned int count; \
} arrayStructName;

#define ArrayNew(arrayStructName) (arrayStructName) {.elements = {0}, .count = 0 }
#define ArrayPush(array, element) array.elements[array.count++] = element
#define ArrayGet(array, index) array.elements[index]

// -------------------------------------------------------------------------------------
// Hashmap
// -------------------------------------------------------------------------------------
unsigned long hashString(char* name);

#define HASH_MAP_SIZE 512
#define GenHashmapDefinition(type, hashMapStructName) typedef struct { 	\
	type elements[HASH_MAP_SIZE];										\
} hashMapStructName;

#define HashMapPut(hashmap, key, value) hashmap.elements[hashString(key)] = value
#define HashMapGet(hashmap, key) 		hashmap.elements[hashString(key)]


// -------------------------------------------------------------------------------------
// File utils
// -------------------------------------------------------------------------------------
// Calls (func) for each file in a directory. The first parameter of (func) is the current directory path the second is the index of the file inside the folder, the third is a reference to that directory entry
void doForEachFileInFolder(char* directoryPath, void (*func)(char*, int, struct dirent*)); 


// -------------------------------------------------------------------------------------
// Math
// -------------------------------------------------------------------------------------
float min(float a, float b);
float max(float a, float b);
float boolToSign(bool input);
float randomFloat(float min, float max);
bool randomChance(float chance);
float interpolate(float a, float b, float w);


// -------------------------------------------------------------------------------------
// Rendering
// -------------------------------------------------------------------------------------

// Initializes a new window. Wraps a few raylib functions to handle screen scaling.
void InitTextureWindow(int newWindowWidth, int newWindowHeight, int newWorldWidth, int newWorldHeight, char* windowTitle);
// Renders all the qued sprites to the screen
void Render();
// renders in 3d mode
void Begin3DMode();
void End3DMode();



// Changes the current window resolution
// If fullscreen is true changes to monitor resolution instead and switches fullscreen on
void SwitchResolution(int width, int height, bool fullscreen);
// Loads a shader into memory and sets it as active. If you want to use the default shader just pass 0 instead of vertexPath or fragmentPath.
void UseShader(char* vertexPath, char* fragmentPath);
// Loads a shader to use only for the 3d world rendering
void Use3DShader(char* vertexPath, char* fragmentPath);

// -------------------------------------------------------------------------------------
// Texture drawing
// -------------------------------------------------------------------------------------
void sprMain(char* spriteName, float x, float y, float width, float height, bool flipX, bool flipY, float rotation, Color color, int layer);
void spr(char* spriteName, float x, float y, int layer);
void sprFRC(char* spriteName, float x, float y, bool flipX, bool flipY, float rotation, Color color, int layer);

// -------------------------------------------------------------------------------------
// 3D Rendering utils
// -------------------------------------------------------------------------------------
void plane(char* spriteName, float x, float y, float z, float width, float height, float yaw, float pitch, float roll);
void billboard(char* spriteName, float x, float y, float z, float scale);
void setCamera(float x, float y, float z, float rotationHorizontal, float rotationVertical);

// -------------------------------------------------------------------------------------
// Text
// -------------------------------------------------------------------------------------
void drawText(char* text, float x, float y, float size, Color color);

// -------------------------------------------------------------------------------------
// Audio
// -------------------------------------------------------------------------------------
void playSound(char* soundName, float pitch, float volume);
void playMusic(char* trackName, float volume);
void stopMusic();


// -------------------------------------------------------------------------------------
// Input
// -------------------------------------------------------------------------------------
Vector2 getMousePosition();


#endif
