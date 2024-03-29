#ifndef MENU_H
#define MENU_H

#include "main.h"

#include <stdbool.h>
#include <graphx.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// max number of options in a single menu bubble
#define MAX_MENU_OPTIONS          10

// number of menue buttons stored in a menu bar
#define NUM_MENU_BAR_ENTRIES      5
#define MENU_BAR_ENTRY_SPACING    (GFX_LCD_WIDTH / NUM_MENU_BAR_ENTRIES)
#define MENU_BAR_HEIGHT           16
#define MENU_BAR_WIDTH            GFX_LCD_WIDTH
#define MENU_BAR_X                0
#define MENU_BAR_Y                (GFX_LCD_HEIGHT - MENU_BAR_HEIGHT)

#define MENU_BORDER_RADIUS        16
#define MENU_HORIZ_SPACING        (GFX_LCD_WIDTH / NUM_MENU_BAR_ENTRIES)
#define MENU_ENTRY_VERT_SPACING   12
#define MAX_MENU_ENTRIES_ON_SCRN  7
#define MENU_PADDING_TOP          1
#define MENU_PADDING_BOTTOM       1
#define MENU_WIDTH                100
#define MAX_MENU_HEIGHT           (MENU_ENTRY_VERT_SPACING * MAX_MENU_ENTRIES_ON_SCRN) + 2 + (MENU_PADDING_TOP + MENU_PADDING_BOTTOM)
#define MENU_ENTRY_PADDING_LEFT   (MENU_BORDER_RADIUS - 2)
#define MENU_ENTRY_PADDING_RIGHT  (MENU_BORDER_RADIUS - 2)
#define MENU_TEXT_PADDING_LEFT    5
#define MENU_ENTRY_WIDTH          (MENU_WIDTH - MENU_ENTRY_PADDING_LEFT - MENU_ENTRY_PADDING_RIGHT)
#define MENU_ENTRY_HEIGHT         MENU_ENTRY_VERT_SPACING
#define MENU_X                    ((GFX_LCD_WIDTH - MENU_WIDTH) / 2)

// single menu (a list of options)
// clicking on an option runs the function specific to that option
struct menu
{
	char *name;
	char options[MAX_MENU_OPTIONS][10];
	uint8_t numOptions;
	uint8_t displayOffset;
	uint8_t selected;
	
	// array of function pointers returning a programState and taking no parameters
	enum programState (*funcPtrs[MAX_MENU_OPTIONS]) (void);
};

/**
 * A collection of 5 menues / Actions
 * Ex: About,  Help,   Settings,  File,    Edit
 *       ^       ^        ^         ^        ^
 *       |       |        |         |        |
 *    Action   Action   Menu      Menu     Menu
 * 
 * Clicking on an action runs a function immediately and returns, whereas clicking on a menu allows the user to scroll through more options
*/

  struct menuBar
{
	struct menu menues[5];
	void (*backgroundRefreshFunc)(void);
};

   /***
 * @param menuBar     Pointer to a menu bar struct returned from a menu bar loader function
 * @param activeIndex Index of menu being run. If no menu is being run, pass -1 instead. This is used to color the button under an active menu.
*/
void drawMenuBar(struct menuBar *menuBar, int activeIndex);

// runs a menu bar and allows for switching between all 5 menu bar options
enum programState runMenuBar(struct menuBar *menuBar, uint8_t activeIndex);

// returns -1 if failure, otherwise returns the index of the menu bar option to open
int getMenuBarIndex(void);

bool menuBarWasPressed(void);

void drawMenu(struct menu *menu, uint8_t activeIndex);

#ifdef __cplusplus
}
#endif

#endif