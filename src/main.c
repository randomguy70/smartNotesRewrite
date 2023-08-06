#include "main.h"

#include "ui.h"
#include "colors.h"
#include "gfx/gfx.h"
#include "finder.h"
#include "editor.h"
#include "fonts/fonts.h"

#include <stdbool.h>
#include <stdint.h>
#include <graphx.h>
#include <fileioc.h>
#include <fontlibc.h>
#include <string.h>
#include <keypadc.h>
#include <debug.h>

bool init();
void cleanup();
void generateTestingFiles(void);

struct finder finder;
struct editor editor;

enum programState programState = FINDER;

int main(void)
{
	if(init() == false) {return 0;}
	
	// generateTestingFiles();
	
	initFinder();
	initEditor();
	
	while(programState != QUIT)
	{
		switch(programState)
		{
			case FINDER:
				programState = runFinder();
				break;
			case EDITOR:
				dbg_printf("Running editor\n");
				programState = runEditor();
				break;
			default:
				break;
		}
	}
	
	cleanup();
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
	font_pack = fontlib_GetFontByIndex(font_pack_name, BOLD_LARGE);
	if (font_pack)
	{
		fontlib_SetFont(font_pack, 0);
	}
	else
	{
		alert("Warning!", "Error loading font. Check if DRSANS appvar exists.");
		gfx_End();
		return false;
	}
	
	return true;
}

void cleanup()
{
	gfx_End();
}

//////////////// testing functions ////////////////

void generateTestingFiles(void)
{
	char *aestheticNames[] = {"123456789012345", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten", "Eleven", "Twelve", "Thirteen", "Fourteen"};
	char *names[] = {"a", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N"};
	char text[] = "The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog. 1234567891234567891234567891234567891234567891234567891234567890123456789012345678901 2345678901234567890";
	text[2] = '\n';
	char nullBytes[16] = {'\0'};
	
	for(int i = 0; i < 14; i++)
	{
		uint8_t file = ti_Open(names[i], "w+");
		ti_Seek(FILE_DETECT_STRING_POS, SEEK_SET, file);
		ti_Write(FILE_DETECT_STRING, sizeof(FILE_DETECT_STRING) - 1, 1, file);
		
		ti_Seek(AESTHETIC_FILE_NAME_POS, SEEK_SET, file);
		ti_Write(nullBytes, 1, AESTHETIC_FILE_NAME_LEN, file);
		ti_Seek(AESTHETIC_FILE_NAME_POS, SEEK_SET, file);
		ti_Write(aestheticNames[i], strlen(aestheticNames[i]), 1, file);
		
		ti_Seek(FILE_DATA_POS, SEEK_SET, file);
		ti_Write(text, sizeof(text)-1, 1, file);
		ti_Write(nullBytes, 1, 1, file);
		ti_SetArchiveStatus(true, file);
		ti_Close(file);
	}
}