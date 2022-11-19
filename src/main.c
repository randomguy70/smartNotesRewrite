#include "main.h"

#include "shapes.h"
#include "colors.h"
#include "gfx/gfx.h"
#include "finder.h"

#include <stdbool.h>
#include <stdint.h>
#include <graphx.h>
#include <fileioc.h>

bool init();
void cleanup();

int main(void)
{
	if(init() == false) {return 0;}
	
	struct finder finder;
	enum programState state = FINDER;
	
	// generate files for testing
	char *names[] = {"One", "Two", "Three","Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten", "Eleven", "Twelve", "Thirteen", "Fourteen", "Fifteen"};
	
	for(int i = 0; i < 15; i++)
	{
		uint8_t file = ti_Open(names[i], "w+");
		ti_Write(FILE_DETECT_STRING, sizeof(FILE_DETECT_STRING) - 1, 1, file);
		ti_SetArchiveStatus(true, file);
		ti_Close(file);
	}
	
	while(1)
	{
		if(state == FINDER)
		{
			state = runFinder(&finder);
		}
		if(state == QUIT)
		{
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
	
	return true;
}

void cleanup()
{
	gfx_End();
}