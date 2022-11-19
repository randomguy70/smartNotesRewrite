#include "finder.h"

#include "file.h"
#include "shapes.h"
#include "colors.h"
#include "main.h"

#include <graphx.h>
#include <keypadc.h>
#include <stdint.h>
#include <stdbool.h>

void drawFinderBackground(void);
void drawFinderWindow(struct finder *finder);
void displayFiles(struct finder *finder);

enum programState runFinder(struct finder *finder);

void drawFinderBackground(void)
{
	// fill screen
	gfx_SetColor(finderWindowBackgroundColor);
	gfx_FillRectangle_NoClip(0, 0, GFX_LCD_WIDTH, GFX_LCD_HEIGHT);
}

void drawFinderWindow(struct finder *finder)
{
	// draw window & title
	window(FINDER_WINDOW_X, FINDER_WINDOW_Y, FINDER_WINDOW_WIDTH, FINDER_WINDOW_HEIGHT, FINDER_WINDOW_BORDER_RADIUS, finderWindowHeaderColor, finderWindowBodyColor, finderWindowOutlineColor);
	char* finderWindowName = "Finder";
	gfx_SetTextFGColor(black);
	gfx_PrintStringXY(finderWindowName, GFX_LCD_WIDTH / 2 - gfx_GetStringWidth(finderWindowName) / 2, FINDER_WINDOW_Y + 5);
	
	displayFiles(finder);
}

void displayFiles(struct finder *finder)
{
	int fileEntryX = FINDER_WINDOW_X + FILE_NAME_PADDING_LEFT;
	int fileEntryY = FINDER_WINDOW_Y + FINDER_WINDOW_HEADER_HEIGHT + (FILE_SPACING - 8) / 2;
	
	gfx_SetTextFGColor(black);
	
	for(uint8_t i = finder->fileOffset, count = 0; count < MAX_FILES_ON_SCREEN && i < finder->numFiles; i++, count++)
	{
		if(i == finder->selectedFile)
		{
			gfx_SetColor(finderSelectorColor);
			chippedRectangle(FINDER_WINDOW_X + FILE_ENTRY_PADDING_LEFT, FINDER_WINDOW_Y + FINDER_WINDOW_HEADER_HEIGHT + count * FILE_SPACING, FILE_ENTRY_WIDTH, FILE_SPACING);
			gfx_SetTextFGColor(white);
		}
		else
		{
			gfx_SetTextFGColor(black);
		}
		gfx_PrintStringXY(finder->files[i].name, fileEntryX, fileEntryY);
		fileEntryY += FILE_SPACING;
	}
}

enum programState runFinder(struct finder *finder)
{
	finder->fileOffset = 0;
	finder->selectedFile = 0;
	finder->selectedWasPressed = false;
	finder->numFiles = loadFiles(finder->files);
	
	gfx_SetDraw(gfx_buffer);
	drawFinderBackground();
	drawFinderWindow(finder);
	gfx_SwapDraw();
	
	bool refreshAll = false;
	bool refreshWindow = false;
	bool reloadFiles = false;
	
	gfx_Blit(gfx_screen);
	
	while(1)
	{
		if(refreshAll == true)
		{
			gfx_SetDraw(gfx_buffer);
			drawFinderBackground();
			drawFinderWindow(finder);
			gfx_SwapDraw();
			
			refreshAll = false;
			refreshWindow = false;
		}
		else if(refreshWindow == true)
		{
			gfx_SetDraw(gfx_buffer);
			drawFinderWindow(finder);
			gfx_SwapDraw();
			
			refreshWindow = false;
		}
		if(reloadFiles == true)
		{
			finder->numFiles = loadFiles(finder->files);
		}
	
		kb_Scan();
		
		if(kb_IsDown(kb_KeyClear))
		{
			return QUIT;
		}
		else if(kb_IsDown(kb_KeyUp) && finder->selectedFile > 0)
		{
			finder->selectedFile--;
			if(finder->selectedFile < finder->fileOffset)
			{
				finder->fileOffset--;
			}
			
			refreshWindow = true;
			continue;
		}
		else if(kb_IsDown(kb_KeyDown) && finder->selectedFile < 30 && finder->selectedFile < finder->numFiles - 1)
		{
			finder->selectedFile++;
			if(finder->selectedFile >= finder->fileOffset + MAX_FILES_ON_SCREEN)
			{
				finder->fileOffset++;
			}
			
			refreshWindow = true;
			continue;
		}
		
	}
	return QUIT;
}