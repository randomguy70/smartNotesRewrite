#include "menu.h"

#include "colors.h"
#include "shapes.h"
#include "main.h"

#include <stdbool.h>
#include <keypadc.h>
#include <stdint.h>

int updateMenu(struct menu *menu);


void drawMenuBar(struct menuBar *menuBar)
{
	int spacing = GFX_LCD_WIDTH / NUM_MENU_BAR_ENTRIES;
	// testing
	int nameY = 229;
	int nameX;
	char *name;
	int pressedButton = getMenuBarPress();
	int selecterX, selecterY;
	
	// rectangle at bottom of screen
	gfx_SetColor(menuBarBackgroundColor);
	gfx_FillRectangle_NoClip(MENU_BAR_X, MENU_BAR_Y, MENU_BAR_WIDTH, MENU_BAR_HEIGHT);
	
	// color the top edge of the rectangle
	gfx_SetColor(menuBarEdgeColor);
	gfx_HorizLine_NoClip(MENU_BAR_X, MENU_BAR_Y, MENU_BAR_WIDTH);
	
	gfx_SetTextFGColor(black);
	
	// display option names
	for(int i = 0; i < NUM_MENU_BAR_ENTRIES; i++)
	{
		if(pressedButton == i)
		{
			selecterX = (i * spacing);
			selecterY = GFX_LCD_HEIGHT - MENU_BAR_HEIGHT + 1;
			gfx_SetColor(menuBarSelecterColor);
			chippedRectangle(selecterX, selecterY, spacing, MENU_BAR_HEIGHT - 1);
		}
		
		name = menuBar->menues[i].name;
		nameX = (i * spacing) + ((spacing - gfx_GetStringWidth(name)) / 2);
		gfx_PrintStringXY(name, nameX, nameY);
	}
}

enum programState runMenuBar(struct menuBar *menuBar, uint8_t menuIndex)
{
	int action;
	
	gfx_SetDrawBuffer();
	drawMenuBar(menuBar);
	drawMenu(&menuBar->menues[menuIndex], menuIndex);
	gfx_Blit(gfx_buffer);
	
	while(1)
	{
		action = updateMenu(&menuBar->menues[menuIndex]);
		
		
		if(action == 0)
		{
			return CANCEL;
		}
		else if(action == 1)
		{
			gfx_SetDrawBuffer();
			drawMenuBar(menuBar);
			drawMenu(&menuBar->menues[menuIndex], menuIndex);
			gfx_Blit(gfx_buffer);
		}
	}
}

void drawMenu(struct menu *menu, uint8_t menuIndex)
{
	const int height = (menu->numOptions >= MAX_MENU_ENTRIES_ON_SCRN) ? (MAX_MENU_HEIGHT) : (MENU_ENTRY_VERT_SPACING * menu->numOptions);
	int menuX = MENU_BAR_ENTRY_SPACING * menuIndex + ((MENU_BAR_ENTRY_SPACING - MENU_WIDTH) / 2);
	int menuY = GFX_LCD_HEIGHT - MENU_BAR_HEIGHT - height;
	
	// make sure the menu stays on the screen
	if(menuX + MENU_WIDTH >= GFX_LCD_WIDTH)
	{
		menuX = GFX_LCD_WIDTH - MENU_WIDTH;
	}
	else if(menuX < 0)
	{
		menuX = 0;
	}
	
	int menuEntryX = menuX + MENU_ENTRY_PADDING_LEFT;
	int menuEntryY = menuY + 1;
	int menuTextX = menuEntryX + MENU_TEXT_PADDING_LEFT;
	int menuTextY = menuEntryY + 2;
	outlinedRoundedRectangle(menuX, menuY, MENU_WIDTH, height, MENU_BORDER_RADIUS, menuWindowBackgroundColor, menuWindowOutlineColor);
	
	for(uint8_t i = menu->displayOffset, count = 0; count < MAX_MENU_ENTRIES_ON_SCRN && i < menu->numOptions; i++, count++)
	{
		if(i == menu->selected)
		{
			gfx_SetColor(menuWindowSelectorColor);
			chippedRectangle(menuEntryX, menuEntryY, MENU_ENTRY_WIDTH, MENU_ENTRY_HEIGHT);
			gfx_SetTextFGColor(menuWindowSelectedTextColor);
		}
		else
		{
			gfx_SetTextFGColor(menuWindowTextColor);
		}
		gfx_PrintStringXY(menu->options[i], menuTextX, menuTextY);
		menuEntryY += MENU_ENTRY_VERT_SPACING;
		menuTextY += MENU_ENTRY_VERT_SPACING;
	}
}

int updateMenu(struct menu *menu)
{
	kb_Scan();
	
	if(kb_IsDown(kb_KeyClear))
	{
		while(kb_IsDown(kb_KeyClear)) kb_Scan();
		return 0;
	}
	
	return 2;
}

bool menuBarWasPressed(void)
{
	if(kb_Data[1])
	{
		return true;
	}
	
	return false;
}

int getMenuBarPress(void)
{
	uint8_t buttonKeys[NUM_MENU_BAR_ENTRIES] = {kb_Yequ, kb_Window, kb_Zoom, kb_Trace, kb_Graph};
	
	if(!kb_Data[1])
	{
		return -1;
	}
	for(int i = 0; i < NUM_MENU_BAR_ENTRIES; i++)
	{
		if(kb_Data[1] & buttonKeys[i])
		{
			return i;
		}
	}
	
	return -1;
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
				.numOptions = 6,
				.options = 
				{
					"New",
					"Open",
					"Rename",
					"Hide",
					"Info",
					"Delete",
				}
			},
			{
				.name = "Edit",
				.numOptions = 7,
				.options = 
				{
					"Search",
					"Some",
					"Random",
					"Thing",
					"For",
					"No",
					"Reason",
				}
			}
		}
	};
	
	return &menuBar;
}