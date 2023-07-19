#include "main.h"

#include "shapes.h"
#include "colors.h"
#include "gfx/gfx.h"
#include "finder.h"
#include "editor.h"

#include <stdbool.h>
#include <stdint.h>
#include <graphx.h>
#include <fileioc.h>
#include <fontlibc.h>
#include <string.h>

bool init();
void cleanup();
void printSystemText(char *str);

struct finder finder;
struct editor editor;

enum programState programState = FINDER;

int main(void)
{
	if(init() == false) {return 0;}
	
	initFinder();
	initEditor();
	
	// generate files for testing
	char *aestheticNames[] = {"One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten", "Eleven", "Twelve", "Thirteen", "Fourteen"};
	char *names[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14"};
	char text[] = "The quick brown   fox      jumps over the lazy dog................................";
	char endOfFile = '\0';
	for(int i = 0; i < 14; i++)
	{
		uint8_t file = ti_Open(names[i], "w+");
		ti_Seek(0, SEEK_SET, file);
		ti_Write(FILE_DETECT_STRING, sizeof(FILE_DETECT_STRING) - 1, 1, file);
		ti_Write(aestheticNames[i], strlen(aestheticNames[i]), 1, file);
		ti_Seek(21, SEEK_SET, file);
		ti_Write(&endOfFile, 1, 1, file);
		ti_Write(text, 82, 1, file);
		ti_SetArchiveStatus(true, file);
		ti_Close(file);
	}
	
	while(programState != QUIT)
	{
		switch(programState)
		{
			case FINDER:
				programState = runFinder();
				break;
			case EDITOR:
				programState = runEditor();
				break;
			default:
				break;
		}
	}
	
	cleanup();
	return 0;
}

bool init()
{
	// gfx setup
	gfx_Begin();
	gfx_SetPalette(palette, sizeof(palette), myimages_palette_offset);
	gfx_SetTransparentColor(transparent);
	gfx_SetTextTransparentColor(transparent);
	gfx_SetTextBGColor(transparent);
	
	// fontlib setup
	fontlib_font_t *font_pack;
	char *font_pack_name = "DRSANS";
	fontlib_SetTransparency(true);
	font_pack = fontlib_GetFontByIndex(font_pack_name, 1);
	if (font_pack)
	{
		fontlib_SetFont(font_pack, 0);
	}
	else
	{
		printSystemText("Font file DRSANS not found.");
		return false;
	}
	
	return true;
}

void printSystemText(char *str)
{
	gfx_SetDraw(gfx_buffer);
	gfx_SetTextBGColor(transparent);
	gfx_SetTextFGColor(black);
	gfx_FillScreen(white);
	gfx_PrintStringXY(str, 4, 4);
}

void cleanup()
{
	gfx_End();
}