#include "utils.h"
#include "stdbool.h"
#include "string.h"
#include "stdio.h"

// -------------------------------------------------------------------------------------
// Hashmap
// -------------------------------------------------------------------------------------
unsigned long hashString(char* name) {
	const unsigned long FNV_OFFSET = 0xCBF29CE484222325ULL;
	const unsigned long FNV_PRIME  = 0x100000001B3ULL;

	unsigned long hash = FNV_OFFSET;

	int i = 0;

	while (name[i]) {
		char value = name[i];
		
		
		// use fnv hash
		for (int shift = 24; shift >= 0; shift -= 8) {
			unsigned char byte = (unsigned char)((value >> shift) & 0xFF);


			hash ^= byte;
			hash *= FNV_PRIME;
		}

		++i;
	}

	return hash % HASH_MAP_SIZE;
}


// -------------------------------------------------------------------------------------
// File utils
// -------------------------------------------------------------------------------------
void doForEachFileInFolder(char* directoryPath, void (*func)(char* directoryPath, int, struct dirent*)) {
	struct dirent* directoryEntry;

	DIR* dir = opendir(directoryPath);

	if (dir == NULL) {
		printf("Could not open requested directory [%s]\n", directoryPath);
		return;
	}
	

	for (int i = 0; (directoryEntry = readdir(dir)) != NULL; i++) {
		func(directoryPath, i, directoryEntry);
	}

	closedir(dir);
}

void getFullFilePathAndName(char* directoryPath, char* fullPathTarget, char* nameTarget, struct dirent* entry) {

	// build texture path
	snprintf(fullPathTarget, 500, "%s%s", directoryPath, entry->d_name);
		
	// build identifier
	int copyLength = strlen(entry->d_name)-4;
	strncpy(nameTarget, entry->d_name, 255);
	nameTarget[copyLength] = 0;

}

// -------------------------------------------------------------------------------------
// Math
// -------------------------------------------------------------------------------------
float min(float a, float b) {
	return ((a < b) * a) + ((a >= b) * b);
}

float max(float a, float b) {
	return ((a > b) * a) + ((a <= b) * b);
}

float boolToSign(bool input) {
	return (input * 2) - 1;
}


#define RandomFloatPrecision 1000
float randomFloat(float min, float max) {
	int randomValue = GetRandomValue(1, RandomFloatPrecision);
	float normalized = ((float) randomValue) / RandomFloatPrecision;

	return (normalized * (max - min)) + min;
}


bool randomChance(float chance) {
	return randomFloat(0.0f, 1.0f) < chance;
}


float interpolate(float a, float b, float w){
	float mn = min(a, b);
	float mx = max(a, b);
	float distance = mx - mn;

	return distance * w + mn;

}

//------------------------------------------------------------------------------------
// Asset loading
//------------------------------------------------------------------------------------
GenHashmapDefinition(Texture2D, TextureMap) 
GenHashmapDefinition(Sound, SoundMap)
GenHashmapDefinition(Music, MusicMap)
TextureMap textures = {0};
SoundMap sounds = {0};
MusicMap music = {0};


// TODO : THIS IS COPY PASTED GARGBAGE
// Maybe get rid of the do for each file function and just process these sequentialy
void loadSprite(char* directoryPath, int index, struct dirent* entry) {
	if (index < 2) {
		return;
	}
	
	char fullPath[500];	
	char identifier[500];
	
	getFullFilePathAndName(directoryPath, fullPath, identifier, entry);

	printf("Loading sprite [%s] from path [%s] \n", identifier, fullPath);

	HashMapPut(textures, identifier, LoadTexture(fullPath)); 
}

void loadSound(char* directoryPath, int index, struct dirent* entry) {
	if (index < 2) {
		return;
	}
	
	char fullPath[500];	
	char identifier[500];
	
	getFullFilePathAndName(directoryPath, fullPath, identifier, entry);

	printf("Loading sound [%s] from path [%s] \n", identifier, fullPath);

	HashMapPut(sounds, identifier, LoadSound(fullPath)); 
}

void loadMusic(char* directoryPath, int index, struct dirent* entry) {
	if (index < 2) {
		return;
	}
	
	char fullPath[500];	
	char identifier[500];
	
	getFullFilePathAndName(directoryPath, fullPath, identifier, entry);

	printf("Loading music [%s] from path [%s] \n", identifier, fullPath);

	HashMapPut(music, identifier, LoadMusicStream(fullPath)); 
}

void loadAssets() {
	doForEachFileInFolder("./resources/textures/", &loadSprite);
	doForEachFileInFolder("./resources/sounds/", &loadSound);
	doForEachFileInFolder("./resources/music/", &loadMusic);
}


Texture2D* getSprite(char* spriteName) {
	return &HashMapGet(textures, spriteName);
}

Sound* getSound(char* soundName) {
	return &HashMapGet(sounds, soundName);
}

Music* getMusic(char* trackName) {
	return &HashMapGet(music, trackName);
}

// -------------------------------------------------------------------------------------
// Audio
// -------------------------------------------------------------------------------------
void playSound(char* soundName, float pitch, float volume) {
	Sound* sound = getSound(soundName);

	SetSoundVolume(*sound, volume);
    SetSoundPitch(*sound, pitch); 

	PlaySound(*sound);
	
}


// -------------------------------------------------------------------------------------
// Music
// -------------------------------------------------------------------------------------
Music* currentMusic = 0;

void playMusic(char* trackName, float volume) {
	printf("Playing music %s. Volume %f\n", trackName, volume);

	Music* track = getMusic(trackName);

	SetMusicVolume(*track, volume);
	SeekMusicStream(*track, 0);
	PlayMusicStream(*track);
	currentMusic = track;

}


void stopMusic() {
	if (currentMusic == 0) {
		return;
	}

	StopMusicStream(*currentMusic);

	currentMusic = 0;
}


// -------------------------------------------------------------------------------------
// Texture drawing
// -------------------------------------------------------------------------------------
#define MAX_TEXTURE_LAYERS 8
#define MAX_TEXTURES_PER_LAYER 1024

typedef struct {
	Texture2D* texture;
	Rectangle source;
	Rectangle destination;
	Vector2 origin;
	float rotation;
	Color color;
} DrawArguments;

void drawSpriteFromArguments(DrawArguments* arguments) {
	DrawTexturePro(
		*arguments->texture,
		arguments->source,
		arguments->destination,
		arguments->origin,
		arguments->rotation,
		arguments->color
	);
}


GenArrayDefinition(DrawArguments, MAX_TEXTURES_PER_LAYER,DrawArgumentArray)
DrawArgumentArray drawQueue[MAX_TEXTURE_LAYERS] = {0};


void sprMain(char* spriteName, float x, float y, float width, float height, bool flipX, bool flipY, float rotation, Color color, int layer) {
	DrawArgumentArray* target = &drawQueue[layer];
	
	Texture2D* texture = getSprite(spriteName); 

	DrawArguments args = {
		texture, 
		(Rectangle){0, 0, texture->width * boolToSign(!flipX), texture->height * boolToSign(!flipY)}, 
		(Rectangle){x, y, texture->width * width, texture->height * height},
		(Vector2) {texture->width / 2.0f * width, texture->height / 2.0f * height},
		rotation / PI * 180, 
		color
	};

	ArrayPush((*target), args);
}


void spr(char* spriteName, float x, float y, int layer) {
	sprMain(spriteName, x, y, 1.0f, 1.0f, false, false, 0.0f, WHITE, layer);
}

void sprFRC(char* spriteName, float x, float y, bool flipX, bool flipY, float rotation, Color color, int layer) {
	sprMain(spriteName, x, y, 1.0f, 1.0f, flipX, flipY, rotation, color, layer);
}



// -------------------------------------------------------------------------------------
// Text
// -------------------------------------------------------------------------------------
typedef struct {
	const char *text; 
	int posX; 
	int posY;
	int fontSize;
	Color color;
} TextArguments;

#define MAX_TEXT_DRAWS_PER_FRAME 64
GenArrayDefinition(TextArguments, MAX_TEXT_DRAWS_PER_FRAME, TextArgumentsArray);
TextArgumentsArray textQueue = {0};

void drawText(char* text, float x, float y, float size, Color color){
	ArrayPush(textQueue, ((TextArguments){
		text,
		x,
		y,
		size,
		color
	}));
}

// -------------------------------------------------------------------------------------
// Rendering
// -------------------------------------------------------------------------------------
Camera camera3d = {0};
int worldWidth;
int worldHeight;
int windowWidth;
int windowHeight;
bool toggleFullscreen = false;
RenderTexture2D renderTexture;
float scaleFactor;
float renderWidth;
float renderHeight;
Shader shader;
float gutterXWidth;
float gutterYHeight;
Mesh planeMesh;
Material planeMaterial;


void recalculateRenderingValues() {
	
	float xRatio = (float) windowWidth / worldWidth;
	float yRatio = (float) windowHeight / worldHeight;

	scaleFactor = min(yRatio, xRatio);
	renderWidth = renderTexture.texture.width * scaleFactor;
	renderHeight = renderTexture.texture.height * scaleFactor;
	
	gutterXWidth = (windowWidth - renderWidth) / 2.0f;
	gutterYHeight = (windowHeight - renderHeight) / 2.0f;

}

// Initializes a new window. Wraps a few raylib functions to handle screen scaling.
void InitTextureWindow(int newWindowWidth, int newWindowHeight, int newWorldWidth, int newWorldHeight, char* windowTitle) {
	worldWidth = newWorldWidth;
	worldHeight = newWorldHeight;
	windowWidth = newWindowWidth;
	windowHeight = newWindowHeight;

    camera3d.position = (Vector3){ 0.0f, 10.0f, 10.0f }; // Camera position
    camera3d.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera3d.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera3d.fovy = 45.0f;                                // Camera field-of-view Y
    camera3d.projection = CAMERA_PERSPECTIVE;             // Camera projection type
	
	InitWindow(windowWidth, windowHeight, windowTitle);
	InitAudioDevice();

	renderTexture = LoadRenderTexture(worldWidth, worldHeight);


	recalculateRenderingValues();
	
	// load assets
	loadAssets();

    // load mesh
    planeMesh = GenMeshPlane(1.0f, 1.0f, 1, 1);
    planeMaterial = LoadMaterialDefault();
    printf("got here1\n");
    SetMaterialTexture(&planeMaterial, MATERIAL_MAP_DIFFUSE, LoadTexture("./resources/textures/debug.png"));

	shader = LoadShader(0, 0);
}


void RenderQueued2D() {
    // draw queued sprites
	for (int layerIndex = 0; layerIndex < MAX_TEXTURE_LAYERS; layerIndex++) {
		DrawArgumentArray* layer = &drawQueue[layerIndex];
		for (unsigned int i = 0; i < layer->count; i++) {
			DrawArguments* args = &ArrayGet((*layer), i);

			drawSpriteFromArguments(args);

		}

		layer->count = 0;
	}
	
	// render text
	for (unsigned int i = 0; i < textQueue.count; i++) {
		TextArguments* args = &ArrayGet(textQueue, i);

		DrawText(args->text, args->posX, args->posY, args->fontSize, args->color);
	}

	textQueue.count = 0;
}


void RenderToScreen() {
	BeginDrawing();
    ClearBackground(BLACK);
	BeginShaderMode(shader);

	DrawTexturePro(renderTexture.texture,
		(Rectangle){ 0, 0, (float)renderTexture.texture.width, (float)-renderTexture.texture.height },
		(Rectangle){ windowWidth / 2.0, windowHeight / 2.0, renderWidth, renderHeight },
		(Vector2){ renderWidth / 2.0 , renderHeight / 2.0 }, 0, WHITE);
	
	EndShaderMode();

	EndDrawing();
	
	// update music
	if (currentMusic != 0) {
		UpdateMusicStream(*currentMusic);
	}

	// This nonsense is here to make sure that wayland updates the window properly after toggling fullscreen
	if (toggleFullscreen) {
		toggleFullscreen = false;
		SetWindowSize(1,1);
		SetWindowSize(windowWidth, windowHeight);
	}
}

// render in 2d mode
void Render() {
	// render to texture
	BeginTextureMode(renderTexture);
	ClearBackground(BLACK);	
	
	RenderQueued2D();
	EndTextureMode();

	// render to screen
	RenderToScreen();
	
}

void Begin3DMode() {
	BeginTextureMode(renderTexture);
	ClearBackground(BLACK);	

    BeginMode3D(camera3d);
}

void End3DMode() {
    EndMode3D();
    // render2d objects (gui and text)
	RenderQueued2D();

	EndTextureMode();

    RenderToScreen();
}


// changes the current window resolution
// if fullscreen is true changes to monitor resolution instead and switches fullscreen on
void SwitchResolution(int width, int height, bool fullscreen) {
	

	int targetResolutionWidth = width;
	int targetResolutionHeight = height;

	if (fullscreen) {
		int monitor = GetCurrentMonitor();
		targetResolutionWidth = GetMonitorWidth(monitor);
		targetResolutionHeight = GetMonitorHeight(monitor);
	}

	if (windowWidth == targetResolutionWidth && windowHeight == targetResolutionHeight && fullscreen == IsWindowFullscreen()) {
		return; // exit early if resolution is the same
	}

	// change resolution
	printf("changing resolution from [%d, %d] to [%d, %d]\n", windowWidth, windowHeight, targetResolutionWidth, targetResolutionHeight);
	SetWindowSize(targetResolutionWidth, targetResolutionHeight);
	
	// update rendering variables
	windowWidth = targetResolutionWidth;
	windowHeight = targetResolutionHeight;
	recalculateRenderingValues();

	if ((fullscreen && !IsWindowFullscreen()) || (!fullscreen && IsWindowFullscreen())) {
		ToggleFullscreen();
		SetWindowSize(targetResolutionWidth, targetResolutionHeight);
		toggleFullscreen = true;	
	}

}


void UseShader(char* vertexPath, char* fragmentPath){
	
	printf("Using shader [vertex : %s] [fragment : %s]\n", vertexPath, fragmentPath);	
	shader = LoadShader(vertexPath, fragmentPath);

}

// -------------------------------------------------------------------------------------
// 3D Rendering utils
// -------------------------------------------------------------------------------------
void plane(char* spriteName, float x, float y, float z, float width, float height, float yaw, float pitch, float roll) {
    Matrix matrix = MatrixIdentity();

    matrix = MatrixMultiply(matrix, MatrixScale(width, 1.0f, height));
    matrix = MatrixMultiply(matrix, MatrixRotateXYZ((Vector3){pitch, yaw, roll}));
    matrix = MatrixMultiply(matrix, MatrixTranslate(x, y, z));
    // printf("t %f, %f, %f \n", yaw, pitch, roll);

    //MatrixTranslate(x, y, z);
    //MatrixRotateXYZ(pitch, yaw, roll);
    //MatrixScale(width, height, 1.0f);

    //SetMaterialTexture(&planeMaterial, MATERIAL_MAP_DIFFUSE, *getSprite(spriteName));

    DrawMesh(planeMesh, planeMaterial, matrix);
}



// -------------------------------------------------------------------------------------
// Input
// -------------------------------------------------------------------------------------
Vector2 getMousePosition() {
	int x = GetMouseX();
	int y = GetMouseY();


	return (Vector2){(x - gutterXWidth) / scaleFactor, (y - gutterYHeight) / scaleFactor};	
}

