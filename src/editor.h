#ifndef EDITOR_H
#define EDITOR_H

#include "menu.h"
#include "main.h"
#include "file.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_DATA_SIZE               8000 /* 8kb is probably more than enough. However, you can increase it if you want to risk a crash :P */

#define EDITOR_HEADER_BAR_WIDTH     GFX_LCD_WIDTH
#define EDITOR_HEADER_BAR_HEIGHT    16
#define EDITOR_HEADER_BAR_X         0
#define EDITOR_HEADER_BAR_Y         0

#define EDITOR_MENU_BAR_HEIGHT      16

#define EDITOR_BODY_WIDTH           GFX_LCD_WIDTH
#define EDITOR_BODY_HEIGHT          GFX_LCD_HEIGHT - (EDITOR_HEADER_BAR_HEIGHT + EDITOR_MENU_BAR_HEIGHT)
#define EDITOR_BODY_X               0
#define EDITOR_BODY_Y               EDITOR_HEADER_BAR_HEIGHT

struct editor
{
	char buffer1[MAX_DATA_SIZE];
	char buffer2[MAX_DATA_SIZE];
	
	unsigned int cursorPos;
	
	struct file *file;
	struct menuBar *menuBar;
};

extern struct editor editor;

void initEditor();
struct menuBar *loadEditorMenuBar();

enum programState runEditor();

// draws the editor background and text (with buffering)
void drawEditor();

void drawEditorBackground();
void drawEditorText();

#ifdef __cplusplus
}
#endif

#endif