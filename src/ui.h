#ifndef UI_H
#define UI_H

#include "colors.h"

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define INPUT_TEXT_BOX_HEIGHT 16
#define INPUT_TEXT_BOX_CURSOR_HEIGHT 10

// what type of characters we're typing in. Lowercase, Caps, math
enum textMode {MATH, UPPERCASE, LOWERCASE};

struct inputState
{
	enum textMode textMode;
	bool alphaPrev;
};

enum fontStyle {BOLD_LARGE, THIN_LARGE, BOLD_NORMAL, THIN_NORMAL, BOLD_SMALL, THIN_SMALL};

// filled rectangle with rounded corners
void roundedRectangle(int x, int y, int width, int height, int borderRadius);

// fills a rectangle & gives it a separate border color
void outlinedRoundedRectangle(int x, int y, int width, int height, int borderRadius, uint8_t bodyColor, uint8_t outlineColor);

// rounded window with outline and header
void window(int x, int y, int width, int height, int borderRadius, enum color headerColor, enum color bodyColor, enum color outlineColor, char *title);

// prints a window with the given title and body text
bool alert(char *title, char *body);

// filled rectangle with each corner pixel missing (approximation of rounded rectangle)
void chippedRectangle(int x, int y, int width, int height);

/**Draws a wrapped line of text within given parameters
 * @param str: start of the word
 * @param x: x coordinate of the text box
 * @param y: y coordinate of the text box
 * @param width: pixel width of the text box
 * @param height: pixel height of the text box
 * */ 
void drawWrappedText(char *str, int x, int y, int width, int height);

/**calculates the length and width of a wrapped text line from a NORMAL STRING (not the editor buffer)
 * @param str: start of the line
 * @param lenBuffer: buffer to hold the character length of the line
 * @param textBoxWidth: the pixel width limit of a line
 * @return a pointer to the next line, or NULL if there aren't any more lines
 * */ 
char *getStrLine(char *str, int *lenBuffer, int textBoxWidth);

/**Calculates the length and width of a word from a wrapped line of text in a NORMAL STRING (not the editor buffer)
 * @param str: start of the word
 * @param widthBuffer: buffer to hold the pixel width of the word
 * @param lenBuffer: buffer to hold the character length of the word
 * @param textBoxWidth: the pixel width limit of a line
 * @return a pointer to the next word
 * */ 
char *getStrWord(char *str, int *widthBuffer, int *lenBuffer);

/* creats a string from input keypresses
 returns 1 if successful, 0 if not
*/
bool inputString(char* buffer, uint8_t maxLength, bool restrictFirstChar, int boxX, int boxY, int boxWidth);

char getCharFromKeyPress(enum textMode mode, uint8_t keyPressed, bool allowNewLines);
void waitForAllKeysReleased(void);

#ifdef __cplusplus
}
#endif

#endif