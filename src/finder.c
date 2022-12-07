#include "finder.h"

#include "file.h"
#include "shapes.h"
#include "colors.h"
#include "main.h"
#include "menu.h"

#include <graphx.h>
#include <keypadc.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/timers.h>

void drawFinderBackground(void);
void drawFinderWindow();
void displayFiles();
bool scrollUp();
bool scrollDown();

void drawFinderBackground(void)
{
	// fill screen
	gfx_SetColor(finderWindowBackgroundColor);
	gfx_FillRectangle_NoClip(0, 0, GFX_LCD_WIDTH, GFX_LCD_HEIGHT);
}

void drawFinderWindow()
{
	// draw window & title
	window(FINDER_WINDOW_X, FINDER_WINDOW_Y, FINDER_WINDOW_WIDTH, FINDER_WINDOW_HEIGHT, FINDER_WINDOW_BORDER_RADIUS, finderWindowHeaderColor, finderWindowBodyColor, finderWindowOutlineColor);
	char* finderWindowName = "Finder";
	gfx_SetTextFGColor(black);
	gfx_PrintStringXY(finderWindowName, GFX_LCD_WIDTH / 2 - gfx_GetStringWidth(finderWindowName) / 2, FINDER_WINDOW_Y + 5);
	
	displayFiles();
}

void displayFiles()
{
	int fileEntryX = FINDER_WINDOW_X + FILE_NAME_PADDING_LEFT;
	int fileEntryY = FINDER_WINDOW_Y + FINDER_WINDOW_HEADER_HEIGHT + (FILE_SPACING - 8) / 2;
	
	gfx_SetTextFGColor(black);
	
	for(uint8_t i = finder.fileOffset, count = 0; count < MAX_FILES_ON_SCREEN && i < finder.numFiles; i++, count++)
	{
		if(i == finder.selectedFile)
		{
			gfx_SetColor(finderSelectorColor);
			chippedRectangle(FINDER_WINDOW_X + FILE_ENTRY_PADDING_LEFT, FINDER_WINDOW_Y + FINDER_WINDOW_HEADER_HEIGHT + count * FILE_SPACING, FILE_ENTRY_WIDTH, FILE_SPACING);
			gfx_SetTextFGColor(white);
		}
		else
		{
			gfx_SetTextFGColor(black);
		}
		gfx_PrintStringXY(finder.files[i].name, fileEntryX, fileEntryY);
		fileEntryY += FILE_SPACING;
	}
}

void initFinder(void)
{
	finder.numFiles = loadFiles(finder.files);
	finder.fileOffset = 0;
	finder.selectedFile = 0;
	finder.selectedWasPressed = false;
	finder.lastScrollDir = noScrollDir;
	finder.lastScrollType = noScrollType;
	finder.timeSinceScroll = 0;
	finder.menuBar = loadFinderMenuBar();
}

bool scrollUp()
{
	// up arrow needs to be pressed, and the selector can't be on the first file
	if(kb_IsDown(kb_KeyUp) && (finder.selectedFile > 0))
	{
		return true;
	}
	
	return false;
}

bool scrollDown()
{
	// need the down key to be pressed, and NOT the up key!!!
	if(kb_IsDown(kb_KeyDown) && !kb_IsDown(kb_KeyUp))
	{
		// if there aren't too many files and the scrollbar isn't on the last one
		if(finder.selectedFile < 30 && finder.selectedFile < finder.numFiles - 1)
		{
			return true;
		}
	}
	
	return false;
}

void refreshAllFinderGraphics(void)
{
	gfx_SetDrawBuffer();
	drawFinderBackground();
	drawFinderWindow(&finder);
	drawMenuBar(finder.menuBar);
	gfx_Blit(gfx_buffer);
}

enum programState runFinder(void)
{
	float time;
	float prevTime = 0;
	float deltaTime;
	int fps;
	
	refreshAllFinderGraphics();
	
	bool refreshAll = false;
	bool refreshWindow = false;
	bool reloadFiles = false;
	bool refreshMenuBar = false;
	
	// reset timer
	timer_Set(1, 0);
	//Enable the timer while setting it to 32768 Hz and making it count up
	timer_Enable(1, TIMER_32K, TIMER_0INT, TIMER_UP);
	
	while(1)
	{
		// need to figure out optimized order of graphic refresh checks
		
		if(refreshAll == true)
		{
			refreshAll = false;
			refreshWindow = false;
			refreshMenuBar = false;
			refreshAllFinderGraphics(&finder);
		}
		else if(refreshWindow == true)
		{
			refreshWindow = false;
			
			gfx_SetDraw(gfx_buffer);
			drawFinderWindow(&finder);
			gfx_Blit(gfx_buffer);
		}
		if(reloadFiles == true)
		{
			reloadFiles = false;
			finder.numFiles = loadFiles(finder.files);
			
			gfx_SetDraw(gfx_buffer);
			drawFinderWindow(&finder);
			gfx_Blit(gfx_buffer);
		}
		if(refreshMenuBar == true)
		{
			refreshMenuBar = false;
			
			gfx_SetDrawBuffer();
			drawMenuBar(finder.menuBar);
			gfx_Blit(gfx_buffer);
		}
		
		// FPS meter (takes off more than 20 fps)
		
		time = (float)timer_GetSafe(1, TIMER_UP) / 32768;
		deltaTime = time - prevTime;
		prevTime = time;
		fps = (int)(1 / deltaTime);
		
		gfx_SetDrawBuffer();
		gfx_SetColor(black);
		gfx_FillRectangle_NoClip(0, 0, 50, 10);
		gfx_SetTextFGColor(white);
		gfx_SetTextXY(1, 1);
		gfx_PrintInt(fps, 3);
		gfx_BlitRectangle(gfx_buffer, 0, 0, 50, 10);
		
		kb_Scan();
		
		if(kb_IsDown(kb_KeyClear))
		{
			return QUIT;
		}
		
		// scrolling up
		if(scrollUp())
		{
			finder.selectedFile--;
			if(finder.selectedFile < finder.fileOffset)
			{
				finder.fileOffset--;
			}
			
			finder.lastScrollDir = scrollDirUp;
			refreshWindow = true;
			continue;
		}
		
		// scrolling down
		if(scrollDown())
		{
			finder.selectedFile++;
			if(finder.selectedFile >= finder.fileOffset + MAX_FILES_ON_SCREEN)
			{
				finder.fileOffset++;
			}
			
			refreshWindow = true;
			continue;
		}
		
		// check for menu bar press
		if(menuBarWasPressed())
		{
			int menuIndex = getMenuBarPress();
			if(menuIndex == -1)
			{
				continue;
			}
			// XXX add action requests for menu bar
			else if(menuIndex == 0 || menuIndex == 1 || menuIndex == 2)
			{
				break;
			}
			
			runMenuBar(finder.menuBar, menuIndex);
			refreshAll = true;
		}
	}
	
	return QUIT;
}