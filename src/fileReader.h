#ifndef FILE_READER
#define FILE_READER

#include "stdbool.h"


#define MAX_FILE_LINES 512
#define LINE_LENGTH 256

typedef struct {
    char lines[MAX_FILE_LINES][LINE_LENGTH];
    int lineNumber;
    int totalLines;
} LoadedFile;

// loading
LoadedFile readFile(char* filePath);

// reading
void fileSkip(LoadedFile* file); // skips line
char* fileNext(LoadedFile* file); // returns pointer to next char*
char* fileNextAlloc256(LoadedFile* file); // allocates 256 bytes and copies the next line to them
float fileNextF(LoadedFile* file); // parses next as float
int fileNextI(LoadedFile* file); // parses next as int
bool fileHasNext(LoadedFile* file); // is eof?



#endif