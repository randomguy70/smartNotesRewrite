#include "editor.h"

#include "file.h"
#include "main.h"
#include "menu.h"
#include "colors.h"
#include "finder.h"

#include "graphx.h"
#include "fileioc.h"
#include "keypadc.h"
#include "fontlibc.h"
#include "stdbool.h"

// DEBUG
#include "string.h"

void initEditor()
{
	editor.menuBar = loadEditorMenuBar();
	
	// wipe text buffer (XXX)
	for (int i = 0; i < MAX_DATA_SIZE + 1; i++)
	{
		editor.buffer[i] = 0;
	}
	
	// initialize some buffer metadata
	editor.bufferEnd = editor.buffer + MAX_DATA_SIZE - 1;
	editor.dataSize = 0;
	editor.file = NULL;
}

enum programState runEditor()
{
	editor.file = &finder.files[finder.fileOffset];
	loadFileData(editor.file);
	
	if(programState == QUIT)
	{
		return QUIT;
	}
	
	// DEBUG
	// drawEditor();
	gfx_SetDraw(gfx_buffer);
	gfx_FillScreen(white);
	gfx_SetTextFGColor(black);
	
	
	
	gfx_Blit(gfx_buffer);
	
	while(!(programState == QUIT))
	{
		kb_Scan();
		
		if(kb_IsDown(kb_KeyClear))
		{
			while(kb_IsDown(kb_KeyClear)) kb_Scan();
			return FINDER;
		}
	}
	
	return FINDER;
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
	
	// menuBar
	drawMenuBar(editor.menuBar, -1);
}

void drawEditorText()
{
	
}

void drawEditor()
{
	gfx_SetDrawBuffer();
	drawEditorBackground();
	drawEditorText();
	gfx_Blit(gfx_buffer);
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
			},
			{
				.name = "Settings",
				.numOptions = 0,
			},
			{
				.name = "Actions",
				.numOptions = 3,
				.options = 
				{
					"Go To End",
					"Erase",
					"Refresh"
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
				}
			},
		}
	};
	
	return &menuBar;
}

/*
char* editor_GetLineLen(char *readPos, int *lenBuffer)
{
	char* prevReadPos = readPos;
	
	// character length of a word
	int wordLen = 0;
	int lineLen = 0;
	// pixel width of a word
	int wordWidth = 0;
	int lineWidth = 0;
	
	while(readPos <= editor.bufferEnd)
	{
		readPos = editor_LoadWord(readPos, &wordLen, &wordWidth);
		if(lineWidth + wordWidth <= MAX_LINE_PIXEL_WIDTH)
		{
			lineLen += wordLen;
			lineWidth += wordWidth;
			
			
			// tack on a space after the word
			if(*readPos == ' ' && ((fontlib_GetGlyphWidth(*readPos) + lineWidth) <= MAX_LINE_PIXEL_WIDTH))
			{
				readPos++;
				lineWidth+= fontlib_GetGlyphWidth(*readPos);
				lineLen++;
			}
			// new line
			else if(*readPos == '\n')
			{
				readPos++;
				lineLen++;
				break;
			}
			// you shouldn't ever encounter a blank character...but if you do...
			else if(*readPos == '\0')
			{
				break;
			}
		}
	}
	
	return readPos;
}
*/

bool editor_LoadWord(char *readPos, int *lenBuffer, int *widthBuffer)
{
	char *startPos = readPos;
	int len = 0;
		
	// if we're before the cursor
	if(readPos >= editor.buffer && readPos < editor.cursorLeft)
	{
		// XXX I need to figure out how many of the smallest characters can fit on one line to optimize this...
		while(*readPos != '\n' && *readPos != ' ' && len < 200)
		{
			*widthBuffer += fontlib_GetGlyphWidth(*readPos);
			readPos++;
			len++;
			
			if(readPos >= editor.cursorLeft)
			{
				readPos = editor.cursorRight;
			}
		}
	}
	// if we're after the cursor
	else if(readPos >= editor.cursorRight && readPos < editor.bufferEnd)
	{
		while(*readPos != '\0' && *readPos != '\n' && *readPos != ' ' && len < 200)
		{
			readPos++;
			len++;
		}
		*widthBuffer = fontlib_GetStringWidthL(startPos, len);
	}
	// if we got a bad read pointer
	else
	{
		return false;
	}
	
	*lenBuffer = len;
	return true;
}