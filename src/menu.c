#include "menu.h"

#include "finder.h"
#include "colors.h"
#include "shapes.h"
#include "main.h"

#include <stdbool.h>
#include <keypadc.h>
#include <stdint.h>
#include <debug.h>

void drawMenuBar(struct menuBar *menuBar, int activeIndex)
{
	int spacing = GFX_LCD_WIDTH / NUM_MENU_BAR_ENTRIES;
	// testing
	int nameY = 229;
	int nameX;
	char *name;
	int pressedButton = getMenuBarIndex();
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
		if(pressedButton == i || activeIndex == i)
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

enum programState runMenuBar(struct menuBar *menuBar, uint8_t activeIndex)
{
	// simplify things by having a pointer to the current menu that's being scrolled through
	struct menu *menu = &menuBar->menues[activeIndex];
	
	bool redrawBackground = false;
	bool redrawForeground = false;
	
	gfx_SetDrawBuffer();
	drawMenuBar(menuBar, activeIndex);
	drawMenu(menu, activeIndex);
	gfx_Blit(gfx_buffer);
	
	// if the menu button pressed has no menu options, it points to single function/action, not a menu
	// in this case, just run the first function in its function array
	if(menu->numOptions == 0)
	{
		if(menu->funcPtrs[0] != NULL)
		{
			return (menu->funcPtrs[0])();
		}
		else
		{
			while(kb_Data[1])
			{
				kb_Scan();
			}
			return CANCEL;
		}
	}
	
	while(1)
	{
		// graphics
		
		if(redrawBackground)
		{
			redrawBackground = false;
			menuBar->backgroundRefreshFunc();
		}
		if(redrawForeground)
		{
			redrawForeground = false;
			
			gfx_SetDrawBuffer();
			drawMenuBar(menuBar, activeIndex);
			drawMenu(menu, activeIndex);
			gfx_Blit(gfx_buffer);
		}
		
		// keypresses
		
		kb_Scan();
		
		// if the user hits enter, run the function for that option
		if(kb_IsDown(kb_KeyEnter) || kb_IsDown(kb_Key2nd))
		{
			while(kb_IsDown(kb_KeyEnter) || kb_IsDown(kb_Key2nd))
			{
				kb_Scan();
			}
			
			if(menu->funcPtrs[menu->selected] != NULL)
			{
				return (menu->funcPtrs[menu->selected])();
			}
			else
			{
				return CANCEL;
			}
		}
		
		// quit menu
		else if(kb_IsDown(kb_KeyClear))
		{
			while(kb_IsDown(kb_KeyClear))
			{
				kb_Scan();
			}
			return CANCEL;
		}
		
		// scroll up
		else if(kb_IsDown(kb_KeyUp) && menu->selected > 0)
		{
			menu->selected--;
			if(menu->selected < menu->displayOffset)
			{
				menu->displayOffset--;
			}
			
			redrawForeground = true;
		}
		
		// scroll down
		else if(kb_IsDown(kb_KeyDown) && menuBar->menues[activeIndex].selected < menu->numOptions - 1)
		{
			menu->selected++;
			if(menu->selected >= menu->displayOffset + MAX_MENU_ENTRIES_ON_SCRN)
			{
				menu->displayOffset++;
			}
			
			redrawForeground = true;
		}
		
		// switch 1 menu left
		else if(kb_IsDown(kb_KeyLeft) && activeIndex > 0)
		{
			if(menuBar->menues[activeIndex - 1].numOptions > 0)
			{
				activeIndex--;
				menu = &menuBar->menues[activeIndex];
				
				redrawBackground = true;
				redrawForeground = true;
			}
		}
		
		// switch 1 menu right
		else if(kb_IsDown(kb_KeyRight) && activeIndex + 1 < NUM_MENU_BAR_ENTRIES)
		{
			if(menuBar->menues[activeIndex + 1].numOptions > 0)
			{
				activeIndex++;
				menu = &menuBar->menues[activeIndex];
				
				redrawBackground = true;
				redrawForeground = true;
			}
			
			continue;
		}
		
		// jump to menu with button press
		else if(menuBarWasPressed())
		{
			int newIndex = getMenuBarIndex();
			if(newIndex != -1 && newIndex != activeIndex && menuBar->menues[newIndex].numOptions > 0)
			{
				activeIndex = newIndex;
				menu = &menuBar->menues[newIndex];
				
				redrawBackground = true;
				redrawForeground = true;
			}
		}
	}
}

void drawMenu(struct menu *menu, uint8_t activeIndex)
{
	const int height = (menu->numOptions >= MAX_MENU_ENTRIES_ON_SCRN) ? (MAX_MENU_HEIGHT) : (MENU_ENTRY_VERT_SPACING * menu->numOptions);
	int menuX = MENU_BAR_ENTRY_SPACING * activeIndex + ((MENU_BAR_ENTRY_SPACING - MENU_WIDTH) / 2);
	int menuY = GFX_LCD_HEIGHT - MENU_BAR_HEIGHT - height - 1;
	
	// make sure that the menu has at least 1 option to choose from
	if(menu->numOptions == 0)
	{
		return;
	}
	
	// make sure the menu stays on the screen (with 1 pixel margin)
	if(menuX + MENU_WIDTH >= GFX_LCD_WIDTH - 1)
	{
		menuX = GFX_LCD_WIDTH - MENU_WIDTH - 1;
	}
	else if(menuX < 1)
	{
		menuX = 1;
	}
	
	int menuEntryX = menuX + MENU_ENTRY_PADDING_LEFT;
	int menuEntryY = menuY + 1 + MENU_PADDING_TOP;
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

bool menuBarWasPressed(void)
{
	return kb_Data[1] ? true : false;
}

int getMenuBarIndex(void)
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