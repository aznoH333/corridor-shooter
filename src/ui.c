#include "ui.h"
#include "utils.h"




#define LINE_THICKNESS 10

void uiBox(float x, float y, float width, float height, Color color) {

    // draw outline
    rect(
        x - LINE_THICKNESS, 
        y - LINE_THICKNESS, 
        width + LINE_THICKNESS * 2, 
        height + LINE_THICKNESS * 2, 
        WHITE
    );

    // draw rect
    rect(x, y, width, height, color);

}




void button(GameState* state, float x, float y, float width, float height, char* text, void (*function)(GameState* state)) {
    if (boolButton(x, y, width, height, text)) {
        function(state);
    }
}


bool boolButton(float x, float y, float width, float height, char* text) {
    Vector2 mousePos = getMousePosition();


    bool isHovered = 
        mousePos.x >= x && mousePos.x <= x + width &&
        mousePos.y >= y && mousePos.y <= y + height;


    Color col = BLUE;

    if (isHovered) {
        col = RED;
    }


    uiBox(x, y, width, height, col);
    drawText(text, x + 10, y + 10, 50, WHITE);

    return isHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}




#define BAR_HEIGHT 30

void bar(float x, float y, float width, float value, float maxValue) {
    // holding bars
    rect(
        x, 
        y, 
        LINE_THICKNESS, 
        BAR_HEIGHT, 
        WHITE
    );

    rect(
        x + width, 
        y, 
        LINE_THICKNESS, 
        BAR_HEIGHT, 
        WHITE
    );


    float barStart = x + LINE_THICKNESS * 2;
    float barEnd = x + width - LINE_THICKNESS;
    float maxWidth = width - LINE_THICKNESS * 3;
    
    // inner bar
    rect(
        barStart, 
        y + BAR_HEIGHT * 0.5 - LINE_THICKNESS * 0.5, 
        maxWidth, 
        LINE_THICKNESS, 
        GRAY
    );


    // indicator bar
    float resolvedWidth = max(min(value, maxValue), 0) / maxValue * maxWidth;
    rect(
        barStart, 
        y, 
        resolvedWidth, 
        BAR_HEIGHT, 
        WHITE
    );
}


// i know std can do this easily with sprintf but i wanted to write my own for no reason
void insertNumberInString(char* string, int start, int end, int number) {
    
    int acc = number;
    for (int i = end - start; i >= 0; --i) {
        char c = 48;        
       
        
        if (acc > 0) {
            int mod = acc % 10;
            c += mod;
            acc = (acc - mod) / 10;
        }

        string[start + i] = c;
    }

}

