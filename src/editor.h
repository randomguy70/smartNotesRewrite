#ifndef EDITOR_H
#define EDITOR_H

#include "file.h"

#include "menu.h"
#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EDITOR_HEADER_BAR_WIDTH     GFX_LCD_WIDTH
#define EDITOR_HEADER_BAR_HEIGHT    16
#define EDITOR_HEADER_BAR_X         0
#define EDITOR_HEADER_BAR_Y         0

#define EDITOR_MENU_BAR_HEIGHT      16

#define EDITOR_BODY_WIDTH           GFX_LCD_WIDTH
#define EDITOR_BODY_HEIGHT          GFX_LCD_HEIGHT - (EDITOR_HEADER_BAR_HEIGHT + EDITOR_MENU_BAR_HEIGHT)
#define EDITOR_BODY_X               0
#define EDITOR_BODY_Y               EDITOR_HEADER_BAR_HEIGHT

#define MAX_LINE_PIXEL_WIDTH        (GFX_LCD_WIDTH - 2)

/* Editor Buffer layout

              File
--------------------------------------
||||||||||||||                 |||||||
--------------------------------------
              ^                ^
	       cursor left   cursor right
            (insert)
*/

extern struct editor editor;
struct editor
{
	char buffer[MAX_DATA_SIZE + 1]; // +1 because of the EOF byte
	unsigned int dataSize;
	char* bufferEnd;
	char* cursorLeft, *cursorRight;
	char* startOfPage; // position of the first character on the screen
	
	struct file *file;
	struct menuBar *menuBar;
};

// run once at startup
void initEditor();

enum programState runEditor();

// draws the editor background and text (with buffering)
void drawEditor();

void drawEditorBackground();
void drawEditorText();

// returns a pointer to the editor menu bar struct
struct menuBar *loadEditorMenuBar();

/** Calculates the character length and pixel width of a word
 * @param readPos start of word
 * @param lenBuffer pointer to variable to store the character length
 * @param widthBuffer pointer to variable to store the pixel width
 * @return Returns a pointer to the first byte after the word
 * */
bool editor_LoadWord(char *readPos, int *lenBuffer, int *widthBuffer);

/** Calculates the character length of a line
 * @param readPos start of line
 * @param lenBuffer pointer to variable to hold the line width
 * @return returns a pointer to the next line, or NULL if there are no more lines
*/
char* editor_GetLineLen(char *readPos, int *lenBuffer);

#ifdef __cplusplus
}
#endif

#endif