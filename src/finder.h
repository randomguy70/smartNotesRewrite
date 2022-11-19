#ifndef FINDER_H
#define FINDER_H

#include "file.h"
#include "stdbool.h"
#include "stdint.h"

#include <graphx.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_FILES_ON_SCREEN 9
#define FILE_SPACING  16
#define FILE_ENTRY_PADDING_LEFT 10
#define FILE_ENTRY_WIDTH 80

#define FINDER_WINDOW_BORDER_RADIUS  16
#define FINDER_WINDOW_HEADER_HEIGHT  FINDER_WINDOW_BORDER_RADIUS // a window header height is always equal to the border radius
#define FINDER_WINDOW_PADDING_BOTTOM 5
#define FINDER_WINDOW_WIDTH          250
#define FINDER_WINDOW_HEIGHT         ((FINDER_WINDOW_BORDER_RADIUS + FINDER_WINDOW_PADDING_BOTTOM) + (MAX_FILES_ON_SCREEN * FILE_SPACING)) 
#define FINDER_WINDOW_X              ((GFX_LCD_WIDTH - FINDER_WINDOW_WIDTH) / 2)
#define FINDER_WINDOW_Y              ((GFX_LCD_HEIGHT - FINDER_WINDOW_HEIGHT) / 2)

struct finder
{
	struct file files[MAX_FILES_LOADABLE];
	uint8_t numFiles;
	uint8_t numFilesOnScreen;
	uint8_t fileOffset;
	uint8_t selectedFile;
	bool selectedWasPressed;
};

enum programState runFinder(struct finder *finder);

#ifdef __cplusplus
}
#endif

#endif