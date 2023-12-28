#include "editor.h"

#include "file.h"
#include "main.h"
#include "menu.h"
#include "colors.h"
#include "finder.h"

#include <graphx.h>
#include <fileioc.h>
#include <keypadc.h>
#include <fontlibc.h>
#include <stdbool.h>
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
		editor.lineLengths[i] = 0;
		editor.linePointers[i] = NULL;
	}
	
	// initialize some buffer metadata
	editor.bufferEnd = editor.buffer + MAX_DATA_SIZE - 1;
	editor.dataSize = 0;
	editor.file = NULL;
	
	editor.redrawText = false;
	editor.redrawAll = false;
}

// wipes the editor and loads the menu bar
void initEditor()
{
	editor.menuBar = loadEditorMenuBar();
}

enum programState runEditor()
{
	wipeEditor();
	
	editor.file = &finder.files[finder.selectedFile];
	loadFileData(editor.file);
	
	editor.lineBeforeScreen = editor.cursorRight;
	editor_LoadUnwrappedScreen(editor.cursorRight, 0);
	
	drawEditor();
	
	while(programState == EDITOR)
	{
		if(editor.redrawText)
		{
			editor.redrawText = false;
			gfx_SetDrawBuffer();
			drawEditorBackground();
			drawEditorText();
			gfx_BlitBuffer();
		}
		
		kb_Scan();
		
		if(kb_IsDown(kb_KeyClear))
		{
			while(kb_IsDown(kb_KeyClear)) kb_Scan();
			return FINDER;
		}
		
		if(kb_IsDown(kb_KeyDown))
		{
			editor_ScrollDownUnwrapped();
			editor.redrawText = true;
			// while(kb_IsDown(kb_KeyDown)) kb_Scan();
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
	for(int i = 0, y = 20; i < MAX_LINES_ON_EDITOR_SCREEN; i++, y+= 15)
	{
		fontlib_SetCursorPosition(2, y);
		drawLine(editor.linePointers[i], editor.lineLengths[i]);
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
	//  || (readPos >= editor.cursorRight && readPos <= editor.bufferEnd))
	// first, we read words from the left of the cursor
	if(readPos >= editor.buffer && readPos < editor.cursorLeft)
	{
		// when we hit a Null Byte or New Line Code, return a pointer to it
		// treat spaces as individual words
		while(*readPos != '\0' && *readPos !='\n' && readPos < editor.cursorLeft)
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
			
			if(leftOfCursor && readPos >= editor.cursorLeft)
			{
				dbg_printf("switching from left to right side of buffer\n");
				leftOfCursor = false;
				readPos = editor.cursorRight;
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
	if(readPos >= editor.cursorLeft && readPos < editor.cursorRight)
	{
		readPos = editor.cursorRight;
	}
	
	dbg_printf("loaded word. Len: %d, width: %d, ascii: %s\n", *lenBuffer, *widthBuffer, readPos - *lenBuffer);
	return readPos;
}

char* editor_LoadLine(char *readPos, int *lenBuffer)
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

char *editor_LoadUnwrappedLine(char *readPos, int *lenBuffer, int maxWidth)
{
	*lenBuffer = 0;
	int width = 0;
	
	while((readPos != NULL) && (readPos < editor.bufferEnd))
	{
		// if we hit a newline code, return the next character after the code
		if(*readPos == '\n')
		{
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

char *getNextBufferChar(char *prev)
{
	prev++;
	if(prev >= editor.bufferEnd)
	{
		return NULL;
	}
	if((prev >= editor.cursorLeft) && (prev < editor.cursorRight))
	{
		{
			return editor.cursorRight;
		}
	}
	return prev;
}

void drawLine(char *start, int len)
{
	for(int i = 0; i < len; i++)
	{
		fontlib_DrawGlyph(*start);
		start = getNextBufferChar(start);
	}
}

bool editor_ScrollDown(void)
{
	int newLineLen;
	char *newLine = editor_LoadLine(editor.linePointers[MAX_LINES_ON_EDITOR_SCREEN - 1], &newLineLen);
	if(newLine != NULL)
	{
		for(int i = 0; i < MAX_LINES_ON_EDITOR_SCREEN - 1; i++)
		{
			editor.linePointers[i] = editor.linePointers[i + 1];
			editor.lineLengths[i] = editor.lineLengths[i + 1];
		}
		editor.linePointers[MAX_LINES_ON_EDITOR_SCREEN - 1] = newLine;
		editor.lineLengths[MAX_LINES_ON_EDITOR_SCREEN - 1] = newLineLen;
		
		return true;
	}
	else
	{
		return false;
	}
}

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
	
	editor.lineBeforeScreen = editor.linePointers[0];
	for(int i = 0; i < (MAX_LINES_ON_EDITOR_SCREEN - 1); i++)
	{
		editor.linePointers[i] = editor.linePointers[i + 1];
		editor.lineLengths[i] = editor.lineLengths[i + 1];
	}
	
	// XXX refactor later (one if else statement)
	// make the pointer to the new line at the bottom of the screen equal to the previous line plus the previous line's length
	// BUT, take into account the buffer gap
	if(editor.linePointers[MAX_LINES_ON_EDITOR_SCREEN - 2] < editor.cursorLeft)
	{
		// in the case that the cursor is on the bottom line, we jump over the buffer gap
		if(editor.cursorLeft - editor.linePointers[MAX_LINES_ON_EDITOR_SCREEN - 2] < editor.lineLengths[MAX_LINES_ON_EDITOR_SCREEN - 2])
		{
			newLine = editor.cursorRight + editor.lineLengths[MAX_LINES_ON_EDITOR_SCREEN - 2] - (editor.cursorLeft - editor.linePointers[MAX_LINES_ON_EDITOR_SCREEN - 2]);
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

// XXX change it to only load lines starting from the starting line
void editor_LoadUnwrappedScreen(char *startingPtr, int startingLine)
{
	char *curLinePtr = startingPtr;
	int length;
	
	for(int i = startingLine; ((i < MAX_LINES_ON_EDITOR_SCREEN) && (curLinePtr != NULL)); i++)
	{
		editor.linePointers[i] = curLinePtr;
		curLinePtr = editor_LoadUnwrappedLine(editor.linePointers[i], &length, MAX_LINE_PIXEL_WIDTH);
		editor.lineLengths[i] = length;
	}
}

