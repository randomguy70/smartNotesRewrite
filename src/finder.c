#include "finder.h"

#include "file.h"
#include "ui.h"
#include "colors.h"
#include "main.h"
#include "menu.h"

#include <graphx.h>
#include "fileioc.h"
#include <keypadc.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/timers.h>
#include <string.h>
#include <debug.h>

// regular functions
void drawFinderBackground(void);
void drawFinderWindow(void);
void displayFiles(void);
void dealWithScrollUp(void);
void dealWithScrollDown(void);

// menu functions
enum programState finderMenu_aboutWindow(void);
enum programState finderMenu_newFile(void);
enum programState finderMenu_Open(void);
enum programState finderMenu_Rename(void);
enum programState finderMenu_Delete(void);

enum programState runFinder(void)
{
	float time;
	float prevTime = 0;
	float deltaTime;
	int fps;
	
	redrawFinder();
	
	// reset, enable, and set timer to count up @ 32768 Hz
	timer_Set(1, 0);
	timer_Enable(1, TIMER_32K, TIMER_0INT, TIMER_UP);
	
	while(1)
	{
		updateFinder();
		
		// FPS meter (takes off a lot of FPS by itself)
		// XXX will make into a single function with static variables so it can be called anywhere
		{
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
		}
		
		kb_Scan();
		
		// quit
		if(kb_IsDown(kb_KeyClear))
		{
			return QUIT;
		}
		
		// open file
		if(kb_IsDown(kb_KeyEnter))
		{
			return EDITOR;
		}
		
		// delete file
		if(kb_IsDown(kb_KeyDel))
		{
			askIfDeleteFile();
		}
		
		dealWithScrollUp();
		dealWithScrollDown();
		
		if(menuBarWasPressed())
		{
			int activeIndex = getMenuBarIndex();
			enum programState state = runMenuBar(finder.menuBar, activeIndex);
			
			if(state == QUIT)          {return QUIT;}
			else if(state == EDITOR)   {return EDITOR;}
			else if(state == SETTINGS) {return SETTINGS;}
			else
			{
				finder.refreshAll = true;
				continue;
			}
		}
	}
	
	return QUIT;
}

void initFinder(void)
{
	finder.numFiles = loadFiles(finder.files);
	finder.fileOffset = 0;
	finder.selectedFile = 0;
	finder.selectedWasPressed = false;
	
	finder.refreshAll = false;
	finder.refreshWindow = false;
	finder.reloadFiles = false;
	finder.refreshMenuBar = false;
	
	finder.lastScrollDir = noScrollDir;
	finder.lastScrollType = noScrollType;
	finder.timeSinceScroll = 0;
	finder.menuBar = loadFinderMenuBar();
}

void updateFinder(void)
{
	if(finder.reloadFiles == true)
	{
		finder.reloadFiles = false;
		finder.numFiles = loadFiles(finder.files);
		
		finder.refreshWindow = true;
	}
	
	if(finder.refreshAll == true || (finder.refreshWindow == true && finder.refreshMenuBar == true))
	{
		finder.refreshAll = false;
		finder.refreshWindow = false;
		finder.refreshMenuBar = false;
		
		redrawFinder();
	}
	else if(finder.refreshWindow == true)
	{
		finder.refreshWindow = false;
		
		gfx_SetDraw(gfx_buffer);
		drawFinderWindow();
		gfx_Blit(gfx_buffer);
	}
	else if(finder.refreshMenuBar == true)
	{
		finder.refreshMenuBar = false;
		
		gfx_SetDrawBuffer();
		drawMenuBar(finder.menuBar, -1);
		gfx_Blit(gfx_buffer);
	}
}

void redrawFinder(void)
{
	gfx_SetDrawBuffer();
	drawFinderBackground();
	drawFinderWindow();
	drawMenuBar(finder.menuBar, -1);
	gfx_Blit(gfx_buffer);
}

void drawFinderBackground(void)
{
	// fill screen
	gfx_SetColor(finderWindowBackgroundColor);
	gfx_FillRectangle_NoClip(0, 0, GFX_LCD_WIDTH, GFX_LCD_HEIGHT);
}

void drawFinderWindow(void)
{
	// draw window & title
	window(FINDER_WINDOW_X, FINDER_WINDOW_Y, FINDER_WINDOW_WIDTH, FINDER_WINDOW_HEIGHT, FINDER_WINDOW_BORDER_RADIUS, finderWindowHeaderColor, finderWindowBodyColor, finderWindowOutlineColor, "Finder");
	displayFiles();
}

void displayFiles(void)
{
	int fileEntryX = FINDER_WINDOW_X + FILE_NAME_PADDING_LEFT;
	int fileEntryY = FINDER_WINDOW_Y + FINDER_WINDOW_HEADER_HEIGHT + (FILE_SPACING - 8) / 2;
	
	// make sure the scrollbar position isn't after all the files
	if(finder.selectedFile >= finder.numFiles)
	{
		finder.selectedFile = finder.numFiles - 1;
	}
	
	gfx_SetTextFGColor(black);
	
	for(uint8_t i = finder.fileOffset, count = 0; count < MAX_FILES_ON_SCREEN && i < finder.numFiles; i++, count++)
	{
		if(i == finder.selectedFile)
		{
			gfx_SetColor(finderSelectorColor);
			chippedRectangle(FINDER_WINDOW_X + FILE_ENTRY_PADDING_LEFT, FINDER_WINDOW_Y + FINDER_WINDOW_HEADER_HEIGHT + count * FILE_SPACING, FILE_NAME_SECTION_WIDTH, FILE_SPACING);
			gfx_SetTextFGColor(white);
		}
		else
		{
			gfx_SetTextFGColor(black);
		}
		gfx_PrintStringXY(finder.files[i].aestheticName, fileEntryX, fileEntryY);
		fileEntryY += FILE_SPACING;
	}
}

void dealWithScrollUp(void)
{
	// up arrow needs to be pressed, and the selector can't be on the first file
	if(kb_IsDown(kb_KeyUp) && (finder.selectedFile > 0))
	{
		finder.selectedFile--;
		if(finder.selectedFile < finder.fileOffset)
		{
			finder.fileOffset--;
		}
		
		finder.lastScrollDir = scrollDirUp;
		finder.refreshWindow = true;
	}
}

void dealWithScrollDown(void)
{
	// need the down key to be pressed, and NOT the up key!!!
	if(kb_IsDown(kb_KeyDown) && !kb_IsDown(kb_KeyUp))
	{
		// if there aren't too many files and the scrollbar isn't on the last one
		if(finder.selectedFile < 30 && finder.selectedFile < finder.numFiles - 1)
		{
			finder.selectedFile++;
			if(finder.selectedFile >= finder.fileOffset + MAX_FILES_ON_SCREEN)
			{
				finder.fileOffset++;
			}
			
			finder.refreshWindow = true;
		}
	}
}

struct menuBar *loadFinderMenuBar(void)
{
	static struct menuBar menuBar = 
	{
		.backgroundRefreshFunc = &redrawFinder,
		.menues = 
		{
			{
				.name = "About",
				.numOptions = 0,
				.funcPtrs = 
				{
					&finderMenu_aboutWindow
				}
			},
			{
				.name = "Help",
				.numOptions = 0,
				.funcPtrs = 
				{
					NULL
				}
			},
			{
				.name = "Settings",
				.numOptions = 0,
				.funcPtrs = 
				{
					NULL
				}
			},
			{
				.name = "File",
				.numOptions = 6,
				.options = 
				{
					"New File",
					"Open",
					"Rename",
					"(un)Hide",
					"File Info",
					"Delete",
				},
				.funcPtrs = 
				{
					&finderMenu_newFile,
					&finderMenu_Open,
					NULL,
					NULL,
					NULL,
					&finderMenu_Delete,
				}
			},
			{
				.name = "whatami",
				.numOptions = 0,
				.options = 
				{
				},
				.funcPtrs = 
				{
					NULL,
				}
			}
		},
	};
	
	return &menuBar;
}

/////////////// Finder Menu Functions ///////////////
// About
enum programState finderMenu_aboutWindow(void)
{
	int windowWidth = 150, windowHeight = 80;
	int windowX = GFX_LCD_WIDTH / 2 - windowWidth / 2;
	int windowY = GFX_LCD_HEIGHT / 2 - windowHeight / 2;
	
	gfx_SetDrawBuffer();
	window(windowX, windowY, windowWidth, windowHeight, 12, finderWindowHeaderColor, finderWindowBodyColor, finderWindowOutlineColor, "About");
	
	char str[150] = "SmartNotes CE V.1.0, created by JohnPaul Malloy (randomguy70). Enjoy! Compiled ";
	strcat(str, __DATE__);
	strcat(str, ", ");
	strcat(str, __TIME__);
	strcat(str, ".");
	
	drawWrappedText(str, windowX + 2, windowY + 13, windowWidth - 4, windowHeight - 13);
	
	gfx_BlitBuffer();
	
	while(!kb_IsDown(kb_KeyClear))
	{
		kb_Scan();
	}
	while(kb_IsDown(kb_KeyClear))
	{
		kb_Scan();
	}
	return CANCEL;
}

// Help

// Settings

// File
enum programState finderMenu_newFile(void)
{
	uint8_t result;
	
	int windowWidth = 200, windowHeight = 80;
	int windowX = GFX_LCD_WIDTH / 2 - windowWidth / 2;
	int windowY = GFX_LCD_HEIGHT / 2 - windowHeight / 2;
	
	int textBoxWidth = windowWidth - 50;
	int textBoxX = (GFX_LCD_WIDTH / 2 - textBoxWidth / 2);
	int textBoxY = windowY + 12 + 20;
	
	gfx_SetDrawBuffer();
	window(windowX, windowY, windowWidth, windowHeight, 12, finderWindowHeaderColor, finderWindowBodyColor, finderWindowOutlineColor, "New File");
	gfx_SetTextFGColor(finderWindowOutlineColor);
	gfx_PrintStringXY("Input the File's name", (GFX_LCD_WIDTH - gfx_GetStringWidth("Input the File's name")) / 2, windowY + 12 + 4);
	gfx_BlitBuffer();
	
	char aestheticName[AESTHETIC_FILE_NAME_LEN] = {'\0'};
	result = inputString(aestheticName, AESTHETIC_FILE_NAME_LEN - 1, true, textBoxX, textBoxY, textBoxWidth);
	
	if(result == 1)
	{
		createNotesFile(aestheticName);
		finder.reloadFiles = true;
	}
	
	return FINDER;
}

enum programState finderMenu_Open(void)
{
	return EDITOR;
}

enum programState finderMenu_Rename(void)
{
	uint8_t result;
	
	int windowWidth = 200, windowHeight = 80;
	int windowX = GFX_LCD_WIDTH / 2 - windowWidth / 2;
	int windowY = GFX_LCD_HEIGHT / 2 - windowHeight / 2;
	
	int textBoxWidth = windowWidth - 50;
	int textBoxX = (GFX_LCD_WIDTH / 2 - textBoxWidth / 2);
	int textBoxY = windowY + 12 + 20;
	
	gfx_SetDrawBuffer();
	window(windowX, windowY, windowWidth, windowHeight, 12, finderWindowHeaderColor, finderWindowBodyColor, finderWindowOutlineColor, "New File");
	gfx_SetTextFGColor(finderWindowOutlineColor);
	gfx_PrintStringXY("Input the File's name", (GFX_LCD_WIDTH - gfx_GetStringWidth("Input the File's name")) / 2, windowY + 12 + 4);
	gfx_BlitBuffer();
	
	char aestheticName[AESTHETIC_FILE_NAME_LEN] = {'\0'};
	result = inputString(aestheticName, AESTHETIC_FILE_NAME_LEN - 1, true, textBoxX, textBoxY, textBoxWidth);
	
	if(result == 1)
	{
		createNotesFile(aestheticName);
		finder.reloadFiles = true;
	}
	
	return FINDER;
}

enum programState finderMenu_Delete(void)
{
	askIfDeleteFile();
	return FINDER;
}
