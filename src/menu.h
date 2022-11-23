#ifndef MENU_H
#define MENU_H

#include <stdbool.h>
#include <graphx.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NUM_MENU_BAR_ENTRIES    5
#define MENU_BAR_HEIGHT         15
#define MENU_BAR_WIDTH          GFX_LCD_WIDTH
#define MENU_BAR_X              0
#define MENU_BAR_Y              GFX_LCD_HEIGHT - MENU_BAR_HEIGHT

struct menu
{
	char *name;
	char *options[10];
	uint8_t numOptions;
	void *funcPtrs[];
};

struct menuBar
{
	struct menu menues[5];
};

// does what it says
void drawMenuBar(struct menuBar *menuBar);

// returns a pointer to static menubar struct
struct menuBar *loadFinderMenuBar(void);

// returns -1 if failure, otherwise returns the index of the menu bar option to open
int checkForMenuBarRun(void);

#ifdef __cplusplus
}
#endif

#endif