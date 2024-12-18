#include "editor.h"

#include "file.h"
#include "main.h"
#include "menu.h"
#include "colors.h"
#include "finder.h"
#include "keypress.h"

#include <graphx.h>
#include <fileioc.h>
#include <keypadc.h>
#include <fontlibc.h>
#include <stdbool.h>
#include <ti/getcsc.h>
#include <debug.h>

// does what it says
void wipeEditor();

// erases every variable in the editor
void wipeEditor()
{
	// wipe text buffer (XXX)
	for (int i = 0; i < MAX_DATA_SIZE + 1; i++)
	{
		editor.buffer[i] = 0;
	}
	
	for(int i = 0; i < MAX_LINES_ON_EDITOR_SCREEN; i++)
	{
		editor.linePointers[i] = NULL;
		editor.lineTerminated[i] = false;
	}
	
	// initialize some buffer metadata
	editor.bufferEnd = editor.buffer + MAX_DATA_SIZE - 1;
	editor.dataSize = 0;
	editor.file = NULL;
	editor.lineOffset = 0;
	editor.redrawText = false;
	editor.redrawAll = false;
	editor.cursorInsert = NULL;
	editor.afterCursor = NULL;
	editor.cursorRow = 0;
	editor.cursorCol = 0;
	editor.desiredCol = 0;
	
	editor.textMode = LOWERCASE;
}

// loads the menu bar
void initEditor()
{
	editor.menuBar = loadEditorMenuBar();
}

enum programState runEditor()
{
	wipeEditor();
	
	editor.file = &finder.files[finder.selectedFile];
	loadFileData(editor.file);
	
	editor_LoadUnwrappedScreen(editor.afterCursor, 0);
	
	drawEditor();
	
	while(programState == EDITOR)
	{
		if(editor.redrawText == true)
		{
			editor.redrawText = false;
			
			gfx_SetDrawBuffer();
			drawEditorBackground();
			drawEditorText();
			drawEditorCursor();
			gfx_SwapDraw();
		}
		
		kb_Scan();
		
		if(kb_IsDown(kb_KeyClear))
		{
			while(kb_IsDown(kb_KeyClear)) kb_Scan();
			return FINDER;
		}
		
		// moving cursor 
		
		else if(kb_IsDown(kb_KeyRight))
		{
			moveCursorRight();
			editor.redrawText = true;
		}
		else if(kb_IsDown(kb_KeyLeft))
		{
			moveCursorLeft();
			editor.redrawText = true;
		}
		else if(kb_IsDown(kb_KeyUp))
		{
			moveCursorUp();
			editor.redrawText = true;
		}
		else if(kb_IsDown(kb_KeyDown))
		{
			moveCursorDown();
			editor.redrawText = true;
		}
		
		// text insertion
		
		sk_key_t keyPressed = getSingleCSCKey();
		char inputChar = '\0';
		if(keyPressed)
		{
			inputChar = getCharFromKeyPress(editor.textMode, keyPressed);
			bool successfulInput = inputCharacter(inputChar);
		}
	}
	
	return programState;
}

void drawEditor()
{
	gfx_SetDrawBuffer();
	drawEditorBackground();
	drawMenuBar(editor.menuBar, -1);
	drawEditorText();
	drawEditorCursor();
	gfx_BlitBuffer();
}

void drawEditorBackground()
{	
	// header bar
	gfx_SetColor(editorBackgroundColor);
	gfx_FillRectangle_NoClip(EDITOR_HEADER_BAR_X, EDITOR_HEADER_BAR_Y, EDITOR_HEADER_BAR_WIDTH, EDITOR_HEADER_BAR_HEIGHT - 1);
	gfx_SetColor(editorHeaderBarBorderColor);
	gfx_HorizLine_NoClip(EDITOR_HEADER_BAR_X, EDITOR_HEADER_BAR_Y + EDITOR_HEADER_BAR_HEIGHT - 1, EDITOR_HEADER_BAR_WIDTH);
	
	// body
	gfx_SetColor(editorBackgroundColor);
	gfx_FillRectangle_NoClip(EDITOR_BODY_X, EDITOR_BODY_Y, EDITOR_HEADER_BAR_WIDTH, EDITOR_BODY_HEIGHT);
}

void drawEditorText()
{
	fontlib_SetForegroundColor(black);
	for(int i = 0, y = EDITOR_FIRST_LINE_Y; i < MAX_LINES_ON_EDITOR_SCREEN; i++, y+= EDITOR_LINE_VERT_SPACING)
	{
		fontlib_SetCursorPosition(2, y);
		drawLine(editor.linePointers[i], editor.lineLengths[editor.lineOffset + i]);
	}
}

struct menuBar *loadEditorMenuBar()
{
	static struct menuBar menuBar = 
	{
		.menues = 
		{
			{
				.name = "Home",
				.numOptions = 0,
				.funcPtrs = 
				{
					NULL,
				}
			},
			{
				.name = "Actions",
				.numOptions = 3,
				.options = 
				{
					"Go To End",
					"Erase",
					"Refresh"
				},
				.funcPtrs = 
				{
					NULL,
					NULL,
					NULL,
				}
			},
			{
				.name = "Settings",
				.numOptions = 0,
				.funcPtrs = 
				{
					NULL,
				}
			},
			{
				.name = "Edit",
				.numOptions = 5,
				.options = 
				{
					"Undo",
					"Redo",
					"Paste",
					"Find",
					"Replace",
				},
				.funcPtrs = 
				{
					NULL,
					NULL,
					NULL,
					NULL,
					NULL,
				}
			},
			{
				.name = "File",
				.numOptions = 7,
				.options = 
				{
					"Save",
					"Close",
					"Rename",
					"Clone",
					"Encrypt",
					"Info",
					"Delete",
				},
				.funcPtrs = 
				{
					NULL,
					NULL,
					NULL,
					NULL,
					NULL,
					NULL,
					NULL,
				}
			},
		}
	};
	
	return &menuBar;
}

char *editor_LoadWord(char *readPos, int *lenBuffer, int *widthBuffer, int maxWidth)
{
	bool leftOfCursor;
	char *prevReadPos;
	int len = 0, width = 0;
	
	// see editor.h line 27 for the buffer layout
	//  || (readPos >= editor.afterCursor && readPos <= editor.bufferEnd))
	// first, we read words from the left of the cursor
	if(readPos >= editor.buffer && readPos < editor.cursorInsert)
	{
		// when we hit a Null Byte or New Line Code, return a pointer to it
		// treat spaces as individual words
		while(*readPos != '\0' && *readPos !='\n' && readPos < editor.cursorInsert)
		{
			if(*readPos == ' ')
			{
				if(len == 0)
				{
					width = fontlib_GetGlyphWidth(' ');
					len = 1;
					readPos++;
				}
				break;
			}
			
			width += fontlib_GetGlyphWidth(*readPos);
			prevReadPos = readPos;
			readPos++;
			len++;
			
			// once a word gets too long, go back a character and return
			if(width > maxWidth && len > 1)
			{
				// readPos = prevReadPos;
				width -= fontlib_GetGlyphWidth(*readPos);
				len--;
				break;
			}
			
			if(leftOfCursor && readPos >= editor.cursorInsert)
			{
				dbg_printf("switching from left to right side of buffer\n");
				leftOfCursor = false;
				readPos = editor.afterCursor;
			}
		}
	}
	
	// if we got a bad read pointer for a parameter
	else
	{
		*widthBuffer = 0;
		*lenBuffer = 0;
		return NULL;
	}
	
	*lenBuffer = len;
	*widthBuffer = width;
	
	// double check the reading pointer we return isn't in between the two two buffer sections
	if(readPos >= editor.cursorInsert && readPos < editor.afterCursor)
	{
		readPos = editor.afterCursor;
	}
	
	return readPos;
}

char* editor_LoadWrappedLine(char *readPos, int *lenBuffer)
{
	char *prevReadPos;
	int lineLen = 0, lineWidth = 0;
	int wordLen = 0, wordWidth = 0;
	
	while(1)
	{
		prevReadPos = readPos;
		readPos = editor_LoadWord(readPos, &wordLen, &wordWidth, MAX_LINE_PIXEL_WIDTH);
		
		// we only get Null if we passed an invalid pointer to editor_LoadWord
		if(readPos == NULL)
		{
			*lenBuffer = 0;
			return NULL;
		}
		
		// if the word fits on the line
		if(lineWidth + wordWidth <= MAX_LINE_PIXEL_WIDTH)
		{
			lineLen += wordLen;
			lineWidth += wordWidth;
			
			// // if the word ended with a space
			// if(*readPos == ' ')
			// {
			// 	// if the space can fit on the line
			// 	if((fontlib_GetGlyphWidth(' ') + lineWidth) <= MAX_LINE_PIXEL_WIDTH)
			// 	{
			// 		readPos++;
			// 		lineWidth += fontlib_GetGlyphWidth(' ');
			// 		lineLen++;
			// 	}
			// 	// if the space can't fit on the line
			// 	else
			// 	{
			// 		break;
			// 	}
			// }
			
			// if we hit a new line code
			if(*readPos == '\n')
			{
				readPos++;
				break;
			}
			
			// if we hit the end of the buffer
			if(readPos >= editor.bufferEnd)
			{
				*lenBuffer = lineLen;
				dbg_printf("Finished reading out buffer\n");
				return NULL;
			}
			
			// if the word was too long for a line and simply had to be trimmed
			// else
			// {
			// 	lineLen = wordLen;
			// 	break;
			// }
			
		}
		
		// XXX perhaps will add a statement for if a word is really long...but for now loadWord cuts words off at the width of the screen
		// so I guess I'm good for now?
		
		// if there are too many words on the line to fit another, end the line
		else
		{
			// go back to the last word
			readPos = prevReadPos;
			break;
		}
	}
	
	*lenBuffer = lineLen;
	
	return readPos;
}

char *editor_LoadUnwrappedLine(char *readPos, int maxWidth, int *lenBuffer, bool *lineTerminated)
{
	*lenBuffer = 0;
	*lineTerminated = false;
	int width = 0;
	
	while((readPos != NULL) && (readPos < editor.bufferEnd))
	{
		// if we hit a newline code, return the next character after the code
		if(*readPos == '\n')
		{
			*lineTerminated = true;
			return getNextBufferChar(readPos);
		}
		
		// if the character fits on the line, add it
		else if(width + fontlib_GetGlyphWidth(*readPos) <= maxWidth)
		{
			width += fontlib_GetGlyphWidth(*readPos);
			(*lenBuffer)++;
			readPos = getNextBufferChar(readPos);
		}
		// if the character doesn't fit on the line, return it as the start of the next line
		else
		{
			return readPos;
		}
	}
	
	dbg_printf("Read to end of file. Last character was: %c\n", *(readPos - 1));
	return NULL;
}

char *getNextLinePtrFromLoadedLine(char *start, int length)
{
	if(start >= editor.afterCursor)
	{
		return start + length;
	}
	else
	{
		if(editor.cursorInsert - start >= length)
		{
			return start + length;
		}
		else
		{
			return editor.afterCursor + (length - (editor.cursorInsert - start));
		}
	}
}

char *getNextBufferChar(char *cur)
{
	cur++;
	if(cur >= editor.bufferEnd)
	{
		return NULL;
	}
	else if((cur >= editor.cursorInsert) && (cur < editor.afterCursor))
	{
		return editor.afterCursor;
	}
	else
	{
		return cur;
	}
}

char *getPrevBufferChar(char *cur)
{
	cur--;
	if(cur < editor.buffer)
	{
		return NULL;
	}
	if((cur >= editor.cursorInsert) && (cur < editor.afterCursor))
	{
		return editor.cursorInsert;
	}
	return cur;
}

void drawLine(char *start, int len)
{
	int i = 0;
	while((i < len) && (*start != '\n'))
	{
		fontlib_DrawGlyph(*start);
		start = getNextBufferChar(start);
		i++;
	}
}

/*
bool editor_ScrollDownWrapped(void)
{
	char *lastLine = editor.linePointers[MAX_LINES_ON_EDITOR_SCREEN -1];
	int lastLineLen = editor.lineLengths[editor.lineOffset + MAX_LINES_ON_EDITOR_SCREEN - 1];
	char *newLine = lastLine + lastLineLen;
	int newLineLen;
	
	// if there aren't any more lines to load
	if((lastLine == NULL) || (newLine >= editor.bufferEnd))
	{
		return false;
	}
	
	// since we can load another line, shift all the onscreen line pointers up one
	for(int i = 0; i < MAX_LINES_ON_EDITOR_SCREEN - 1; i++)
	{
		editor.linePointers[i] = editor.linePointers[i + 1];
		editor.lineLengths[i] = editor.lineLengths[i + 1];
	}
	
	// increment the line offset
	editor.lineOffset++;
	
	// set the pointer of the new line and load its length
	editor.linePointers[MAX_LINES_ON_EDITOR_SCREEN - 1] = newLine;
	// XXX change to load wrapped line
	editor_LoadUnwrappedLine(newLine, &newLineLen, MAX_LINE_PIXEL_WIDTH);
	editor.lineLengths[editor.lineOffset + MAX_LINES_ON_EDITOR_SCREEN - 1] = newLineLen;
	
	// success!
	return true;
}
*/
/*
bool editor_ScrollDownUnwrapped(void)
{
	int newLineLen;
	char *newLine;
	
	// if all the text in the file doesn't add up to a full screen, then we obviously can't scroll down
	if(editor.linePointers[MAX_LINES_ON_EDITOR_SCREEN - 1] == NULL)
	{
		return false;
	}
	
	// if we're on the last line, we can't scroll down any more
	if(editor.linePointers[MAX_LINES_ON_EDITOR_SCREEN - 1] + editor.lineLengths[MAX_LINES_ON_EDITOR_SCREEN - 1] >= editor.bufferEnd)
	{
		return false;
	}
	
	for(int i = 0; i < (MAX_LINES_ON_EDITOR_SCREEN - 1); i++)
	{
		editor.linePointers[i] = editor.linePointers[i + 1];
		editor.lineLengths[i] = editor.lineLengths[i + 1];
	}
	
	// XXX refactor later (one if else statement)
	// make the pointer to the new line at the bottom of the screen equal to the previous line plus the previous line's length
	// BUT, take into account the buffer gap
	if(editor.linePointers[MAX_LINES_ON_EDITOR_SCREEN - 2] < editor.cursorInsert)
	{
		// in the case that the cursor is on the bottom line, we jump over the buffer gap
		if(editor.cursorInsert - editor.linePointers[MAX_LINES_ON_EDITOR_SCREEN - 2] < editor.lineLengths[MAX_LINES_ON_EDITOR_SCREEN - 2])
		{
			newLine = editor.afterCursor + editor.lineLengths[MAX_LINES_ON_EDITOR_SCREEN - 2] - (editor.cursorInsert - editor.linePointers[MAX_LINES_ON_EDITOR_SCREEN - 2]);
		}
		else
		{
			newLine = editor.linePointers[MAX_LINES_ON_EDITOR_SCREEN - 2] + editor.lineLengths[MAX_LINES_ON_EDITOR_SCREEN - 2];
		}
	}
	else
	{
		newLine = editor.linePointers[MAX_LINES_ON_EDITOR_SCREEN - 2] + editor.lineLengths[MAX_LINES_ON_EDITOR_SCREEN - 2];
	}
	
	editor_LoadUnwrappedLine(newLine, &newLineLen, MAX_LINE_PIXEL_WIDTH);
	
	editor.linePointers[MAX_LINES_ON_EDITOR_SCREEN - 1] = newLine;
	editor.lineLengths[MAX_LINES_ON_EDITOR_SCREEN - 1] = newLineLen;
	return true;
}
*/

// XXX take into account split buffer when cursorCol > 0 and is on bottom line and we're scrolling down :P
bool editor_ScrollDownUnwrapped(void)
{
	char *lastLine = editor.linePointers[MAX_LINES_ON_EDITOR_SCREEN - 1];
	int lastLineLen = editor.lineLengths[editor.lineOffset + MAX_LINES_ON_EDITOR_SCREEN - 1];
	char *newLine = getNextLinePtrFromLoadedLine(lastLine, lastLineLen);
	int newLineLen;
	bool lineTerminated;
	
	// if there aren't any more lines to load
	if((lastLine == NULL) || newLine >= editor.bufferEnd)
	{
		return false;
	}
	
	// since we can load another line, shift all the onscreen line pointers up one
	for(int i = 0; i < MAX_LINES_ON_EDITOR_SCREEN - 1; i++)
	{
		editor.linePointers[i] = editor.linePointers[i + 1];
	}
	
	// increment the line offset
	editor.lineOffset++;
	
	// set the pointer of the new line and load its length
	editor.linePointers[MAX_LINES_ON_EDITOR_SCREEN - 1] = newLine;
	editor_LoadUnwrappedLine(newLine, MAX_LINE_PIXEL_WIDTH, &newLineLen, &lineTerminated);
	editor.lineLengths[editor.lineOffset + MAX_LINES_ON_EDITOR_SCREEN - 1] = newLineLen;

	return true;
}

bool editor_ScrollUpUnwrapped(void)
{
	// if we can't scroll up, too bad so sad
	if(editor.lineOffset < 1)
	{
		return false;
	}
	
	// shift all the line pointers down one
	for(int i = (MAX_LINES_ON_EDITOR_SCREEN - 1); i >= 1; i--)
	{
		editor.linePointers[i] = editor.linePointers[i - 1];
	}
	
	// decrement the line offset
	editor.lineOffset--;
	
	// to find the pointer to the line we are scrolling up to, 
	// subtract its stored length from the pointer to the line right below it (MAKE SURE TO FACTOR IN THE SPLIT BUFFER and line pointers when the cursor column = 0)
	if(editor.cursorCol == 0)
	{
		editor.linePointers[0] = editor.cursorInsert - editor.lineLengths[editor.lineOffset];
	}
	else
	{
		editor.linePointers[0] = editor.linePointers[1] - editor.lineLengths[editor.lineOffset];
	}
	
	// we scrolled up, so success!
	return true;
}

void editor_LoadUnwrappedScreen(char *startingPtr, int startingLine)
{
	int length;
	bool isTerminated;
	
	for(int i = startingLine, lineIndex = editor.lineOffset; (i < MAX_LINES_ON_EDITOR_SCREEN); i++, lineIndex++)
	{
		if(startingPtr == NULL)
		{
			editor.linePointers[i] = NULL;
			editor.lineTerminated[i] = false;
			editor.lineLengths[lineIndex] = 0;
		}
		else
		{
			editor.linePointers[i] = startingPtr;
			startingPtr = editor_LoadUnwrappedLine(editor.linePointers[i], MAX_LINE_PIXEL_WIDTH, &length, &isTerminated);
			editor.lineLengths[lineIndex] = length;
		}
	}
}


void drawEditorCursor(void)
{
	int y = EDITOR_FIRST_LINE_Y + (editor.cursorRow * EDITOR_LINE_VERT_SPACING);
	int x = getCursorX();
	
	gfx_SetColor(cursorColor);
	gfx_VertLine_NoClip(x, y, EDITOR_LINE_VERT_SPACING);
	gfx_VertLine_NoClip(x + 1, y, EDITOR_LINE_VERT_SPACING);
}

int getCursorX(void)
{
	char *linePtr = editor.linePointers[editor.cursorRow];
	
	// we have a padding of 2 pixels on each side of the screen
	// but the cursor should be slightly more to the left side, so make it 1 pixel
	int x = 1;
	
	for(int i = 0; i < editor.cursorCol; i++)
	{
		x += fontlib_GetGlyphWidth(*linePtr);
		linePtr = getNextBufferChar(linePtr);
	}
	
	return x;
}

bool moveCursorLeft(void)
{
	// check if moving the cursor left makes us scroll up
	if(editor.cursorRow == 0 && editor.cursorCol == 0)
	{
		if(editor_ScrollUpUnwrapped() == true)
		{
			editor.cursorRow++;
		}
		else
		{
			return false;
		}
	}
	
	// if we're not at the far left side of the line
	if(editor.cursorCol > 0)
	{
		editor.cursorInsert--;
		editor.afterCursor--;
		editor.cursorCol--;
		*(editor.afterCursor) = *(editor.cursorInsert);
		*(editor.cursorInsert) = '\0';
	}
	// if we're going to have to move up a line
	else if(editor.cursorCol == 0)
	{
		// if the line above ends with a newline, don't count the newline when calculating the cursor column
		if(*(editor.cursorInsert - 1) == '\n')
		{
			editor.cursorCol = editor.lineLengths[editor.lineOffset + editor.cursorRow - 1];
			editor.cursorRow--;
			editor.afterCursor--;
			*(editor.afterCursor) = '\n';
			editor.cursorInsert--;
			*(editor.cursorInsert) = '\0';
		}
		else
		{
			editor.cursorCol = editor.lineLengths[editor.lineOffset + editor.cursorRow - 1];
			editor.cursorRow--;
		}
	}
	
	// if we moved the cursor to the far left side of any line, we need to update the pointer to the beginning of the line
	// to point to the right side of the split buffer
	if(editor.cursorCol == 0)
	{
		dbg_printf("updated line %d's pointer to %p\n", editor.lineOffset + editor.cursorRow, editor.afterCursor);
		editor.linePointers[editor.cursorRow] = editor.afterCursor;
	}
	
	editor.desiredCol = editor.cursorCol;
	dbg_printf("Cursor Pos - Row: %d, Col: %d, Insert: %p, AfterCursor: %p, LinePointer: %p\n", editor.cursorRow, editor.cursorCol, editor.cursorInsert, editor.afterCursor, editor.linePointers[editor.cursorRow]);
	
	return true;
}

bool moveCursorRight(void)
{
	// if the cursor is already at the end of the file, it can't move farther
	if(editor.afterCursor >= editor.bufferEnd)
	{
		return false;
	}
	
	// check if moving the cursor right makes us scroll down
	// first check if we're on the last line...
	if(editor.cursorRow == (MAX_LINES_ON_EDITOR_SCREEN - 1))
	{
		// then, check if the cursor is at the end of the visible line (excluding newline codes)
		if((*(editor.afterCursor) == '\n') || (editor.cursorCol == editor.lineLengths[editor.lineOffset + editor.cursorRow]))
		{
			// XXX delete the return true (it's just for debugging purposes)
			if(editor_ScrollDownUnwrapped() == true)
			{
				editor.cursorCol = 0;
				if(*(editor.afterCursor) == '\n')
				{
					*editor.cursorInsert = '\n';
					editor.cursorInsert++;
					*editor.afterCursor = '\0';
					*editor.afterCursor++;
				}
			}
			else
			{
				return false;
			}
		}
	}
	
	// if we have to update the pointer to the beginning of the line since we'll move the first character to the left side of the split buffer
	// NOTE: the pointer is wrong until we have actually moved the first character into the left side of the split buffer
	if(editor.cursorCol == 0)
	{
		editor.linePointers[editor.cursorRow] = editor.cursorInsert;
	}
	
	// if the line being moved through ends on a newline code, we have to move the code to the left side of the split buffer
	if(*(editor.afterCursor) == '\n')
	{
		editor.cursorCol = 0;
		editor.cursorRow++;
		*(editor.cursorInsert) = *(editor.afterCursor);
		editor.cursorInsert++;
		*(editor.afterCursor) = '\0';
		editor.afterCursor++;
	}
	// if the cursor needs to move down a line without updating the split buffer
	else if(editor.cursorCol == editor.lineLengths[editor.lineOffset + editor.cursorRow])
	{
		editor.cursorCol = 0;
		editor.cursorRow++;
	}
	else
	{
		*(editor.cursorInsert) = *(editor.afterCursor);
		editor.cursorInsert++;
		*(editor.afterCursor) = '\0';
		editor.afterCursor++;
		editor.cursorCol++;
	}
	
	editor.desiredCol = editor.cursorCol;
	
	dbg_printf("Cursor Pos - Row: %d, Col: %d, Insert: %p, AfterCursor: %p, LinePointer: %p\n", editor.cursorRow, editor.cursorCol, editor.cursorInsert, editor.afterCursor, editor.linePointers[editor.cursorRow]);
	
	return true;
}

bool moveCursorUp(void)
{
	// if we're at the very beginning of the file, we can't move the cursor up
	if((editor.cursorRow <= 0) && (editor.lineOffset <= 0) && (editor.cursorCol <= 0))
	{
		return false;
	}
	
	// if we're at the top of the screen, try scrolling up 1 line
	if(editor.cursorRow == 0)
	{
		bool scrollUpHuh = editor_ScrollUpUnwrapped();
		if(!scrollUpHuh)
		{
			dbg_printf("scrolling up failed. Moving cursor to the start of the line instead\n");
		}
		else
		{
			dbg_printf("scrolling up worked\n");
			editor.cursorRow++;
		}
	}
	
	int newCol;
	int newRow;
	int newLineLen;
	int dataShiftSize;
	
	// this if-else block calculates the new cursor position and how much data we need to shift to the right
	
	// if our cursor is still on the first line (which means we couldn't scroll up), then move the cursor to the start of the line (standard behavior)
	if(editor.cursorRow == 0)
	{
		dbg_printf("Moving to start of line\n");
		newCol = 0;
		newRow = 0;
		editor.desiredCol = 0;
		dataShiftSize = editor.cursorCol;
	}
	// if we're actually going to move up a line
	else
	{
		dbg_printf("Moving up a line\n");
		newCol = editor.desiredCol;
		newRow = editor.cursorRow - 1;
		newLineLen = editor.lineLengths[editor.lineOffset + newRow];
				
		// if we have to move the cursor left to fit on a smaller line
		if(newLineLen <= newCol)
		{
			if(*(editor.linePointers[newRow] + newLineLen) == '\n')
			{
				newCol = newLineLen - 1;
			}
			else
			{
				newCol = newLineLen;
			}
		}
		
		dataShiftSize = editor.cursorInsert - (editor.linePointers[newRow] + newCol);
		
		// update the current line's pointer, if necessary, to make it point to the right side of the split buffer
		if(editor.cursorCol > 0)
		{
			editor.linePointers[editor.cursorRow] = editor.afterCursor - editor.cursorCol;
		}
	}
	
	dbg_printf("Going to shift %d bytes\n", dataShiftSize);
	
	// now that we know how much data to shift and all that stuff, actually shift it!
	for(int i = 0; i < dataShiftSize; i++)
	{
		editor.afterCursor--;
		editor.cursorInsert--;
		*(editor.afterCursor) = *(editor.cursorInsert);
		*(editor.cursorInsert) = '\0';
	}
	
	editor.cursorCol = newCol;
	editor.cursorRow = newRow;
	
	if(newCol == 0)
	{
		editor.linePointers[newRow] = editor.afterCursor;
	}
	
	dbg_printf("Editor data:\nCursor Insert: %p, After Cursor %p, curLine pointer %p\nCursorCol %d, CursorRow %d, Desired Col %d\n", editor.cursorInsert, editor.afterCursor, editor.linePointers[editor.cursorRow], editor.cursorCol, editor.cursorRow, editor.desiredCol);
	
	return true;
}

// XXX maybe don't redraw everything when you move the cursor - just redraw the characters on either side of the cursor
bool moveCursorDown(void)
{
	if(editor.afterCursor == editor.bufferEnd)
	{
		return false;
	}
	
	// if we're at the bottom of the screen, try scrolling down 1 line
	if(editor.cursorRow == (MAX_LINES_ON_EDITOR_SCREEN - 1))
	{
		bool scrollDownHuh = editor_ScrollDownUnwrapped();
		dbg_printf("scrolled down\n");
		if(!scrollDownHuh)
		{
			dbg_printf("scrolling down failed\n");
			return false;
		}
		else
		{
			dbg_printf("scrolling down worked\n");
			editor.cursorRow--;
		}
	}
	
	dbg_printf("Before editor data:\nCursor Insert: %p, After Cursor %p, curLine pointer %p\nCursorCol %d, CursorRow %d\n", editor.cursorInsert, editor.afterCursor, editor.linePointers[editor.cursorRow], editor.cursorCol, editor.cursorRow);
	
	// if we're already on the last line, then move the cursor to the end of the current line
	if(editor.linePointers[editor.cursorRow + 1] == NULL)
	{
		int length = editor.lineLengths[editor.lineOffset + editor.cursorRow] - editor.cursorCol;
		// got to update those line pointers...
		if(length > 0)
		{
			editor.linePointers[editor.cursorRow] = editor.cursorInsert;
		}
		
		if( *(editor.linePointers[editor.cursorRow] + length - 1) == '\n')
		{
			length--;
		}
		for(int i = 0; i < length; i++)
		{
			*(editor.cursorInsert) = *(editor.afterCursor);
			editor.cursorInsert++;
			editor.afterCursor++;
		}
		editor.cursorCol += length;
		return true;
	}
	
	int newCursorCol = editor.lineLengths[editor.lineOffset + editor.cursorRow + 1];
	if(*(editor.linePointers[editor.cursorRow + 1] + newCursorCol - 1) == '\n')
	{
		newCursorCol--;		
	}
	if(newCursorCol > editor.desiredCol)
	{
		newCursorCol = editor.desiredCol;
	}
	
	// shift the buffer data left until the beginning of the next line
	int shiftSize = (editor.linePointers[editor.cursorRow + 1] + newCursorCol) - editor.afterCursor;
	
	for(int i = 0; i < shiftSize; i++)
	{
		*(editor.cursorInsert) = *(editor.afterCursor);
		editor.cursorInsert++;
		*(editor.afterCursor) = '\0';
		editor.afterCursor++;
	}
	// update either the next line or the previous line's pointer to point to the left side of the buffer
	// depending on which line's beginning we skipped over
	if(editor.cursorCol > 0)
	{
		editor.linePointers[editor.cursorRow + 1] = editor.cursorInsert - newCursorCol;
	}
	else
	{
		editor.linePointers[editor.cursorRow] = (editor.cursorInsert - shiftSize);
	}
	
	dbg_printf("After editor data:\nCursor Insert: %p, After Cursor %p, curLine pointer %p\nCursorCol %d, CursorRow %d, Desired Col %d\n", editor.cursorInsert, editor.afterCursor, editor.linePointers[editor.cursorRow], editor.cursorCol, editor.cursorRow, editor.desiredCol);
	editor.cursorRow++;
	editor.cursorCol = newCursorCol;
	
	return true;
}

bool inputCharacter(char charIn)
{
	if((editor.dataSize >= MAX_DATA_SIZE) || (charIn == '\0'))
	{
		return false;
	}
	
	// if the file was hitherto empty, initialize the first line pointer and length
	
	if(editor.dataSize == 0)
	{
		editor.linePointers[0] = editor.buffer;
		editor.lineLengths[0] = 1;
	}
	
	*editor.cursorInsert = charIn;
	editor.cursorInsert++;
	editor.dataSize++;
	
	
	
	editor_LoadUnwrappedScreen(editor.linePointers[editor.cursorRow], editor.cursorRow);
	editor.redrawText = true;
	//  check if the character fits on the current line, 
	// if it doesn't, we'll need to shift all the lines below to make room
	
	
	// XXX maybe more
}