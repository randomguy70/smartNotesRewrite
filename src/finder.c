#include "finder.h"
#include "shapes.h"
#include "colors.h"
#include "main.h"

#include <graphx.h>
#include <keypadc.h>
#include <stdint.h>

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
	
	for(uint8_t i = finder->fileOffset, count = 0; i < MAX_FILES_ON_SCREEN && i < finder->numFiles; i++, count++)
	{
		if(i == 0)
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
	finder->numFiles = loadFiles(finder->files);
	
	drawFinderBackground();
	drawFinderWindow(finder);
	
	while(!kb_AnyKey())
	{
		kb_Scan();
	}
	return QUIT;
}