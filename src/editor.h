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
#define EDITOR_BODY_HEIGHT          (GFX_LCD_HEIGHT - (EDITOR_HEADER_BAR_HEIGHT + EDITOR_MENU_BAR_HEIGHT))
#define EDITOR_BODY_X               0
#define EDITOR_BODY_Y               EDITOR_HEADER_BAR_HEIGHT

#define MAX_LINES_ON_EDITOR_SCREEN  13
#define MAX_LINE_PIXEL_WIDTH        (GFX_LCD_WIDTH - 4) // we want 2 pixels padding on either side of the screen

/* Editor Split Buffer layout

            Buffer memory block
--------------------------------------------
||||||||||||||                       |||||||
--------------------------------------------
              ^                      ^
	       cursor left           cursor right
			(insert)      (data right after the cursor)
*/

struct editor
{
	bool redrawText, redrawAll;
	
	struct file *file;
	struct menuBar *menuBar;
	
	char buffer[MAX_DATA_SIZE + 1]; // +1 because of the EOF byte
	char *bufferEnd; // points to the last writable byte in the buffer that can store text
	unsigned int dataSize;
	char *cursorLeft, *cursorRight;
	char *startOfPage; // position of the first character on the screen
	
	char *linePointers[MAX_LINES_ON_EDITOR_SCREEN];
	int lineLengths[MAX_LINES_ON_EDITOR_SCREEN];
};
extern struct editor editor;


// run once at startup
void initEditor();

enum programState runEditor();

// draws the editor background and text (with buffering)
void drawEditor();
void drawEditorBackground();
void drawEditorText(void);

// returns a pointer to the editor menu bar struct
struct menuBar *loadEditorMenuBar();

// loads the lengths of the lines displayed onscreen
void editor_LoadUnwrappedScreen(char *startingPtr, int startingLine);

/** Calculates the character length and pixel width of a word. It does take into consideration the split buffer method I am using,
 * so I don't have to worry about that with editor_LoadLine.
 * @param readPos start of word
 * @param lenBuffer pointer to variable to store the character length
 * @param widthBuffer pointer to variable to store the pixel width
 * @param maxWidth the widest pixel length we'll calculate the word up to
 * @return Returns a pointer to the first byte AFTER the word, or NULL if there are no more words
 * */
char *editor_LoadWord(char *readPos, int *lenBuffer, int *widthBuffer, int maxWidth);

/** Calculates the character length of a line
 * @param readPos start of line
 * @param lenBuffer pointer to variable to hold the line width
 * @return returns a pointer to the next line, or NULL if there are no more lines
*/
char* editor_LoadLine(char *readPos, int *lenBuffer);

// returns the pointer to the start of the next line without word wrapping
// stores the line's length (in characters) in lenBuffer
char *editor_LoadUnwrappedLine(char *readPos, int *lenBuffer, int maxWidth);

// returns the pointer to the next character from the editor buffer,
// takes into account the split between the left and right sections.
// returns NULL if there are no more characters
char *getNextBufferChar(char *prev);

void drawLine(char *start, int len);

// scrolls down 1 line
// only used if settings specify word wrapping
// returns true if you can scroll down, false if you can't
bool editor_ScrollDown(void);

// scrolls down 1 line
// only used if settings specify no word wrapping
// returns true if you can scroll down, false if you can't
bool editor_ScrollDownUnwrapped(void);

#ifdef __cplusplus
}
#endif

#endif