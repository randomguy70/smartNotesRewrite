#include "finder.h"

#include "file.h"
#include "shapes.h"
#include "colors.h"
#include "main.h"

#include <graphx.h>
#include <keypadc.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/timers.h>

void drawFinderBackground(void);
void drawFinderWindow(struct finder *finder);
void displayFiles(struct finder *finder);
bool canScrollUp(struct finder *finder);
bool canScrollDown(struct finder *finder);

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

void initFinder(struct finder *finder)
{
	finder->fileOffset = 0;
	finder->selectedFile = 0;
	finder->selectedWasPressed = false;
	finder->numFiles = loadFiles(finder->files);
	finder->prevDown = false;
	finder->prevUp = false;
	finder->timeSinceScrollUp = 0;
	finder->timeSinceScrollDown = 0;
}

bool canScrollUp(struct finder *finder)
{
	if(!(finder->selectedFile > 0))
	{
		return false;
	}
	
	if(!finder->prevUp)
	{
		return true;
	}
	else if(finder->timeSinceScrollUp >= MIN_TIME_BTWN_CONTINOUS_SCROLL)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool canScrollDown(struct finder *finder)
{
	if(!(finder->selectedFile < 30 && finder->selectedFile < finder->numFiles - 1))
	{
		return false;
	}
	
	if(!finder->prevDown)
	{
		return true;
	}
	else if(finder->timeSinceScrollDown >= MIN_TIME_BTWN_CONTINOUS_SCROLL)
	{
		return true;
	}
	else
	{
		return false;
	}
}

enum programState runFinder(struct finder *finder)
{
	float time = 0;
	float prevTime = 0;
	float deltaTime = 0;
	
	initFinder(finder);
	
	gfx_SetDraw(gfx_buffer);
	drawFinderBackground();
	drawFinderWindow(finder);
	gfx_SwapDraw();
	
	// prevent weird background on swapDraw
	gfx_Blit(gfx_screen);
	
	bool refreshAll = false;
	bool refreshWindow = false;
	bool reloadFiles = false;
	
	// reset timer
	timer_Set(1, 0);
	//Enable the timer while setting it to 32768 Hz and making it count up
	timer_Enable(1, TIMER_32K, TIMER_0INT, TIMER_UP);
	
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
		
		time = (float)timer_GetSafe(1, TIMER_UP) / 32768;
		deltaTime = time - prevTime;
		prevTime = time;
		
		kb_Scan();
		
		if(kb_IsDown(kb_KeyClear))
		{
			return QUIT;
		}
		else if(kb_IsDown(kb_KeyUp) && canScrollUp(finder))
		{
			finder->selectedFile--;
			if(finder->selectedFile < finder->fileOffset)
			{
				finder->fileOffset--;
			}
			finder->prevUp = true;
			finder->timeSinceScrollUp = 0;
			
			refreshWindow = true;
			continue;
		}
		else
		{
			finder->timeSinceScrollUp += deltaTime;
			finder->prevUp = false;
		}
		
		if(kb_IsDown(kb_KeyDown) && canScrollDown(finder))
		{
			finder->selectedFile++;
			if(finder->selectedFile >= finder->fileOffset + MAX_FILES_ON_SCREEN)
			{
				finder->fileOffset++;
			}
			
			finder->timeSinceScrollDown = 0;
			refreshWindow = true;
			continue;
		}
		else
		{
			finder->timeSinceScrollDown += 1;
			finder->prevDown = false;
		}
	}
	
	return QUIT;
}