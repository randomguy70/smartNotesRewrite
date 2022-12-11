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

bool init();
void cleanup();

struct finder finder;
struct editor editor;

enum programState programState = FINDER;

int main(void)
{
	if(init() == false) {return 0;}
	
	initFinder();
	
	// generate files for testing
	char *names[] = {"One", "Two", "Three","Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten", "Eleven", "Twelve", "Thirteen", "Fourteen", "Fifteen"};
	char text[] = "qwe rtyuiopa sdfghjkl zxcvbnhgjgfjghj,gfjkgjf 5678uyhjkgftyuihgj";
	
	for(int i = 0; i < 15; i++)
	{
		uint8_t file = ti_Open(names[i], "w+");
		ti_Write(FILE_DETECT_STRING, sizeof(FILE_DETECT_STRING) - 1, 1, file);
		ti_Write(text, 65, 1, file);
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
			default:
				break;
		}
	}
	
	cleanup();
	return 0;
}

bool init()
{
	gfx_Begin();
	gfx_SetPalette(palette, sizeof(palette), myimages_palette_offset);
	gfx_SetTransparentColor(transparent);
	gfx_SetTextTransparentColor(transparent);
	gfx_SetTextBGColor(transparent);
	
	return true;
}

void cleanup()
{
	gfx_End();
}