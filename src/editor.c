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

// calculates the character lengths of 10 lines of text
// and stores the pointers to the lines in the editor struct
void editor_LoadScreen(char *startOfPage);
void wipeEditor();

enum programState runEditor()
{
	wipeEditor();
	
	editor.file = &finder.files[finder.selectedFile];
	loadFileData(editor.file);
	
	editor_LoadScreen(editor.cursorRight);
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
			editor_ScrollDown();
			editor.redrawText = true;
			while(kb_IsDown(kb_KeyDown)) kb_Scan();
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
		fontlib_DrawStringL(editor.linePointers[i], editor.lineLengths[i]);
		// dbg_printf("Line #%d pointer: %p, length: %d\n", i, editor.linePointers[i], editor.lineLengths[i]);
	}
	
	// dbg_printf("End of the buffer: %p", editor.bufferEnd);
}

// wipes the editor and loads the menu bar
void initEditor()
{
	editor.menuBar = loadEditorMenuBar();
	wipeEditor();
}

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
	if((readPos >= editor.buffer && readPos < editor.cursorLeft) || (readPos >= editor.cursorRight && readPos <= editor.bufferEnd))
	{
		if(readPos < editor.cursorLeft)
		{
			leftOfCursor = true;
		}
		
		// when we hit a Null Byte or New Line Code, return a pointer to it
		// treat spaces as individual words
		while(*readPos != '\0' && *readPos != '\n' && readPos <= editor.bufferEnd)
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
			if(width > maxWidth && len > 0)
			{
				readPos = prevReadPos;
				width -= fontlib_GetGlyphWidth(*readPos);
				len--;
				break;
			}
			
			if(leftOfCursor && readPos >= editor.cursorLeft)
			{
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
		
		dbg_printf("New line on screen. Ptr: %p, Len: %d\n", newLine, newLineLen);
		
		return true;
	}
	else
	{
		return false;
	}
}

void editor_LoadScreen(char *startOfPage)
{
	int lineNum = 0, lineLen = 0;
	char *linePtr = startOfPage;
	
	// we already know where the first line starts...
	editor.linePointers[0] = startOfPage;
	linePtr = editor_LoadLine(startOfPage, &lineLen);
	editor.lineLengths[0] = lineLen;
	
	lineNum++;
	
	while(lineNum < MAX_LINES_ON_EDITOR_SCREEN)
	{
		editor.linePointers[lineNum] = linePtr;
		linePtr = editor_LoadLine(linePtr, &lineLen);
		if(linePtr == NULL)
		{
			return;
		}
		editor.lineLengths[lineNum] = lineLen;
		
		lineNum++;		
	}
}