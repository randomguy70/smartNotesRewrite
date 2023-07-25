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
	
	// XXX DEBUG
	drawEditor();
	int lineLen;
	editor_GetLineLen(editor.cursorRight, &lineLen);
	gfx_SetDraw(gfx_screen);
	fontlib_SetCursorPosition(2, 25);
	for(int i = 0; i < lineLen; i++)
	{
		fontlib_DrawGlyph(*(editor.cursorRight + i));
	}
	dbg_printf("Line Length: %d\n", lineLen);
	dbg_printf("Cursor Right: %p\n", editor.cursorRight);
	
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
				.name = "Settings",
				.numOptions = 0,
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

char* editor_GetLineLen(char *readPos, int *lenBuffer)
{
	int lineLen = 0, lineWidth = 0;
	int wordLen = 0, wordWidth = 0;
	
	// while you don't hit a newline code or the end of the file and you can fit another word on the line...keep adding
	while(1)
	{
		readPos = editor_LoadWord(readPos, &wordLen, &wordWidth);
		
		// if the word fits on the line
		if(lineWidth + wordWidth <= MAX_LINE_PIXEL_WIDTH)
		{
			lineLen += wordLen;
			lineWidth += wordWidth;
			
			// if the word ended with a space
			if(*readPos == ' ')
			{
				// if the space can fit on the line
				if((fontlib_GetGlyphWidth(' ') + lineWidth) <= MAX_LINE_PIXEL_WIDTH)
				{
					readPos++;
					lineWidth += fontlib_GetGlyphWidth(' ');
					lineLen++;
				}
				// if the space can't fit on the line
				else
				{
					break;
				}
			}
			
			// if we hit a new line code ('\n')
			else if(*readPos == '\n')
			{
				readPos++;
				
				break;
			}
			
			// if we hit the end of the buffer
			else if(*readPos == '\0')
			{
				*lenBuffer = lineLen;
				return NULL;
			}
		}
		
		// if it's the first word on the line and it's longer than a full line, it needs to be trimmed
		else if(lineLen == 0)
		{
			do
			{
				lineWidth += fontlib_GetGlyphWidth(*readPos++);
				lineLen++;
			} while (lineWidth < MAX_LINE_PIXEL_WIDTH);
		}
		
		// if there are too many words on the line to fit another, end the line
		else
		{
			break;
		}
	}
	
	*lenBuffer = lineLen;
	
	return readPos;
}

char *editor_LoadWord(char *readPos, int *lenBuffer, int *widthBuffer)
{
	char *startPos = readPos;
	int len = 0;
	
	// if we're before the cursor (a blinking line showing where we're inserting text)
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
		while(*readPos != '\0' && *readPos != '\n' && *readPos != ' ' && len < 100)
		{
			readPos++;
			len++;
		}
		*widthBuffer = fontlib_GetStringWidthL(startPos, len);
	}
	// if we got a bad read pointer for a parameter
	else
	{
		return NULL;
	}
	
	*lenBuffer = len;
	
	// make sure the reading pointer we return isn't in between the two two buffer sections
	// that way, when we're loading a line, we don't have to worry about where we are in the buffer
	if(readPos >= editor.cursorLeft && readPos < editor.cursorRight)
	{
		readPos = editor.cursorRight;
	}
	dbg_printf("read pos after word is %p\n", readPos);
	return readPos;
}