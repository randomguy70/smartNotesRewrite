#include "editor.h"

#include "main.h"
#include "menu.h"
#include "colors.h"
#include "finder.h"
#include "file.h"

#include "graphx.h"
#include "fileioc.h"
#include "keypadc.h"

enum programState runEditor()
{
	editor.file = &finder.files[finder.fileOffset];
	loadFileData(editor.file);
	
	initEditor();
	drawEditor();
	
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

void initEditor()
{
	editor.menuBar = loadEditorMenuBar();
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