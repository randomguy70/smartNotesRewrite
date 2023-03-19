#include "editor.h"

#include "main.h"
#include "menu.h"
#include "colors.h"
#include "finder.h"
#include "file.h"

#include "graphx.h"
#include "fileioc.h"
#include "keypadc.h"
#include "fontlibc.h"

// DEBUG
#include "string.h"

void initEditor()
{
	editor.menuBar = loadEditorMenuBar();
	
	// wipe text buffer (XXX)
	for (int i = 0; i < MAX_DATA_SIZE; i++)
	{
		editor.buffer[i] = 0;
	}
	
	// keep track of memory
	
	editor.bufferEnd = editor.buffer + MAX_DATA_SIZE - 1;
	editor.cursorPos = 0;
	editor.dataSize = 0;
	editor.file = NULL;
}

enum programState runEditor()
{
	editor.file = &finder.files[finder.fileOffset];
	loadFileData(editor.file);
	
	// DEBUG
	char str[100] = {'\0'};
	strcpy(str, "This is a sentence");
	strcpy(editor.buffer, str);
	editor.dataSize = strlen(str);
	
	if(programState == QUIT)
	{
		return QUIT;
	}
	
	// DEBUG
	// drawEditor();
	gfx_SetDraw(gfx_buffer);
	gfx_FillScreen(white);
	gfx_SetTextFGColor(black);
	
	gfx_HorizLine(0, 10, fontlib_GetStringWidth("Four"));
	fontlib_SetCursorPosition(0, 11);
	fontlib_DrawString("Four");
	
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

void editor_GetLineLen(char *readPos, int *lenBuffer)
{
	
}

void editor_LoadWord(char *readPos, int *lenBuffer, int *widthBuffer)
{
	int len = 0;
	int width = 0;
	bool beforeCursor;
	char character;
	
	// make sure readPos is within the text buffer
	
	if (readPos < editor.buffer || readPos > editor.bufferEnd)
	{
		return;
	}
	
	// check if we have read past the cursor yet
	
	if (readPos < editor.buffer + editor.cursorPos)
	{
		beforeCursor = true;
	}
	else
	{
		beforeCursor = false;
	}
	
	while (readPos <= editor.bufferEnd)
	{
		character = *readPos;
		
		if (character != ' ' && character != '\n' && character != '\0')
		{
			readPos++;
			len++;
			width += gfx_GetCharWidth(character);
		}
		else
		{
			break;
		}
		
		if (beforeCursor && readPos >= editor.buffer + editor.cursorPos)
		{
			beforeCursor = false;
			readPos = (editor.bufferEnd - editor.dataSize + editor.cursorPos) + 1;
		}
	}
	
	*lenBuffer = len;
	*widthBuffer = width;
	
	return;
}