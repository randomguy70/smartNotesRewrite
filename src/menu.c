#include "menu.h"

#include "colors.h"

#include <stdbool.h>
#include <keypadc.h>

void drawMenuBar(struct menuBar *menuBar)
{
	int spacing = GFX_LCD_WIDTH / NUM_MENU_BAR_ENTRIES;
	// testing
	int nameY = 229;
	int nameX;
	char *name;
	
	// rectangle at bottom of screen
	gfx_SetColor(menuBarBackgroundColor);
	gfx_FillRectangle_NoClip(MENU_BAR_X, MENU_BAR_Y, MENU_BAR_WIDTH, MENU_BAR_HEIGHT);
	
	// color the top edge of the rectangle
	gfx_SetColor(menuBarEdgeColor);
	gfx_HorizLine_NoClip(MENU_BAR_X, MENU_BAR_Y, MENU_BAR_WIDTH);
	
	// display option names
	for(int i = 0; i < NUM_MENU_BAR_ENTRIES; i++)
	{
		name = menuBar->menues[i].name;
		nameX = (i * spacing) + ((spacing - gfx_GetStringWidth(name)) / 2);
		gfx_PrintStringXY(name, nameX, nameY);
	}
}

struct menuBar *loadFinderMenuBar(void)
{
	static struct menuBar menuBar = 
	{
		.menues = 
		{
			{
				.name = "Exit",
				.numOptions = 0
			},
			{
				.name = "Help",
				.numOptions = 0,
			},
			{
				.name = "Settings",
				.numOptions = 0,
			},
			{
				.name = "File",
				.numOptions = 5,
				.options = 
				{
					"Open",
					"Rename",
					"Hide",
					"Info",
					"Delete",
				}
			},
			{
				.name = "Edit",
				.numOptions = 5,
				.options = 
				{
					"Search",
					"Something",
					"Another",
					"Random",
					"Thing"
				}
			}
		}
	};
	
	return &menuBar;
}

int checkForMenuBarRun(void)
{
	uint8_t buttonKeys[5] = {kb_Yequ, kb_Trace, kb_Zoom, kb_Window, kb_Yequ};
	
	if(!kb_Data[0])
	{
		return -1;
	}
	for(int i = 0; i < sizeof(buttonKeys); i++)
	{
		if(kb_Data[0] & buttonKeys[i])
		{
			return i;
		}
	}
	
	return -1;
}