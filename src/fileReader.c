#include "fileReader.h"
#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "dirent.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Loading#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LoadedFile readFile(char* filePath) {
    LoadedFile file = {
        .lines = {0},
        .lineNumber = 0,
        .totalLines = 0
    };

    // init file
    FILE* cf = fopen(filePath, "r");

    if (cf == NULL) {
        printf("Invalid file path %s \n", filePath);
        return;
    }

    // read file
    int nextIndex = 0;
    while(nextIndex < MAX_FILE_LINES) {
        char* buffer = fgets(file.lines[nextIndex], LINE_LENGTH, cf);

        // exit if eof
        if (buffer == NULL) {
            break;
        }

        // trim new line
        for (int i = 0; i < MAX_FILE_LINES; ++i) {
            char c = buffer[i];

            if (c == '\n' || c == 0) {
                buffer[i] = 0;
                break;
            }
        }


        // inc counter
        nextIndex++;
    }
    file.totalLines = nextIndex;


    // close and output
    fclose(cf);
    return file;

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#Reading#
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void fileSkip(LoadedFile* file) {
    file->lineNumber++;
}


char* fileNext(LoadedFile* file) {
    return file->lines[file->lineNumber++];
}


char* fileNextAlloc256(LoadedFile* file) {
    char* output = malloc(256);
    char* next = fileNext(file);

    for (int i = 0; i < 256; ++i) {
        output[i] = next[i];
    }

    return output;
}



float fileNextF(LoadedFile* file) {
    char* line = fileNext(file);
    
    // find . index
    int dotIndex = -1;
    int preDotLength = 0;
    int postDotLength = 0;
    int sign = 1;
    int readingIndexStart = 0;

    if (line[0] == '-') {
        sign = -1;
        readingIndexStart = 1;
    }

    for (int i = readingIndexStart; i < LINE_LENGTH; ++i) {
        char c = line[i];

        // check if dot
        if (c == '.') {
            dotIndex = i;
            preDotLength = i;
        }

        // reached end of line
        if (c == 0) {
            if (dotIndex == -1) {
                preDotLength = i;
            } else {
                postDotLength = i - preDotLength - 1;
            }
            break;
        }

    }

    // parse predot
    float preDot = 0;
    for (int i = readingIndexStart; i < preDotLength; ++i) {
        preDot += pow(10, preDotLength - i - 1) * (line[i] - '0');
    }

    // parse post dot
    float postDot = 0;
    if (dotIndex != -1) {
        for (int i = 0; i < postDotLength; ++i) {
            postDot += pow(10, - i - 1) * (line[dotIndex + i + 1] - '0');
        } 
    }
    
    
    return (preDot + postDot) * sign;
}

int fileNextI(LoadedFile* file) {
    return fileNextF(file);
}

bool fileHasNext(LoadedFile* file) {
    return file->lineNumber < file->totalLines;
}
