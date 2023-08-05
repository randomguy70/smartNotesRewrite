#include "ui.h"
#include "colors.h"
#include "keypress.h"

#include <graphx.h>
#include <debug.h>
#include <stdbool.h>
#include <ti/getcsc.h>
#include <keypadc.h>
#include <sys/timers.h>

void roundedRectangle(int x, int y, int width, int height, int borderRadius)
{
	// top left corner
	gfx_FillCircle_NoClip(x + borderRadius, y + borderRadius, borderRadius);
	
	// top right corner
	gfx_FillCircle_NoClip(x + width - 1 - borderRadius, y + borderRadius, borderRadius);
	
	// bottom left corner
	gfx_FillCircle_NoClip(x + borderRadius, y + height - 1 - borderRadius, borderRadius);
	
	// bottom right corner
	gfx_FillCircle_NoClip(x + width - 1 - borderRadius, y + height - 1 - borderRadius, borderRadius);
	
	// Middle (body) rectangle
	gfx_FillRectangle_NoClip(x + borderRadius, y, width - 2 * borderRadius, height);
	
	// left side
	gfx_FillRectangle_NoClip(x, y + borderRadius, borderRadius, height - 2 * borderRadius);
	
	// right side
	gfx_FillRectangle_NoClip(x + width - borderRadius, y + borderRadius, borderRadius, height - 2 * borderRadius);
}

void outlinedRoundedRectangle(int x, int y, int width, int height, int borderRadius, uint8_t bodyColor, uint8_t outlineColor)
{
	//---- body corners ----------
	gfx_SetColor(bodyColor);
	
	// top left corner
	gfx_FillCircle_NoClip(x + borderRadius, y + borderRadius, borderRadius);
	
	// top right corner
	gfx_FillCircle_NoClip(x + width - 1 - borderRadius, y + borderRadius, borderRadius);
	
	// bottom left corner
	gfx_FillCircle_NoClip(x + borderRadius, y + height - 1 - borderRadius, borderRadius);
	
	// bottom right corner
	gfx_FillCircle_NoClip(x + width - 1 - borderRadius, y + height - 1 - borderRadius, borderRadius);
	
	//---- outline ----------
	
	gfx_SetColor(outlineColor);
	
	// top left corner
	gfx_Circle_NoClip(x + borderRadius, y + borderRadius, borderRadius);
	
	// top right corner
	gfx_Circle_NoClip(x + width - 1 - borderRadius, y + borderRadius, borderRadius);
	
	// bottom left corner
	gfx_Circle_NoClip(x + borderRadius, y + height - 1 - borderRadius, borderRadius);
	
	// bottom right corner
	gfx_Circle_NoClip(x + width - 1 - borderRadius, y + height - 1 - borderRadius, borderRadius);
	
	// top edge
	gfx_HorizLine_NoClip(x + borderRadius, y, width - 2 * borderRadius);
	
	// bottom edge
	gfx_HorizLine_NoClip(x + borderRadius, y + height - 1, width - 2 * borderRadius);
	
	// left edge
	gfx_VertLine_NoClip(x, y + borderRadius, height - 2 * borderRadius);
	
	// right edge
	gfx_VertLine_NoClip(x + width - 1, y + borderRadius, height - 2 * borderRadius);
	
	// -------- body --------
	
	gfx_SetColor(bodyColor);
	// Middle (body) rectangle
	gfx_FillRectangle_NoClip(x + borderRadius, y + 1, width - 2 * borderRadius, height - 2);
	
	// left side
	gfx_FillRectangle_NoClip(x + 1, y + borderRadius, borderRadius - 1, height - 2 * borderRadius);
	
	// right side
	gfx_FillRectangle_NoClip(x + width - borderRadius - 1, y + borderRadius, borderRadius, height - 2 * borderRadius);
}

void window(int x, int y, int width, int height, int borderRadius, enum color headerColor, enum color bodyColor, enum color outlineColor, char *title)
{
	// -------------------------------
	// corners
	gfx_SetColor(headerColor);
	
	// top left corner
	gfx_FillCircle_NoClip(x + borderRadius, y + borderRadius, borderRadius);
	
	// top right corner
	gfx_FillCircle_NoClip(x + width - 1 - borderRadius, y + borderRadius, borderRadius);
	
	gfx_SetColor(bodyColor);
	
	// bottom left corner
	gfx_FillCircle_NoClip(x + borderRadius, y + height - 1 - borderRadius, borderRadius);
	
	// bottom right corner
	gfx_FillCircle_NoClip(x + width - 1 - borderRadius, y + height - 1 - borderRadius, borderRadius);
	
	// -------------------------------
	// corner outlines
	
	gfx_SetColor(outlineColor);
	
	// top left corner
	gfx_Circle_NoClip(x + borderRadius, y + borderRadius, borderRadius);
	
	// top right corner
	gfx_Circle_NoClip(x + width - 1 - borderRadius, y + borderRadius, borderRadius);
	
	// bottom left corner
	gfx_Circle_NoClip(x + borderRadius, y + height - 1 - borderRadius, borderRadius);
	
	// bottom right corner
	gfx_Circle_NoClip(x + width - 1 - borderRadius, y + height - 1 - borderRadius, borderRadius);
	
	// edge outlines
	
	// left edge
	gfx_VertLine_NoClip(x, y + borderRadius, height - 2 * borderRadius);
	
	// right edge
	gfx_VertLine_NoClip(x + width - 1, y + borderRadius, height - 2 * borderRadius);
	
	// top edge
	gfx_HorizLine_NoClip(x + borderRadius, y, width - 2 * borderRadius);
	
	// bottom edge
	gfx_HorizLine_NoClip(x + borderRadius, y + height - 1, width - 2 * borderRadius);
	
	// -------------------------------
	// header body
	gfx_SetColor(headerColor);
	
	gfx_FillRectangle_NoClip(x + borderRadius, y + 1, width- 2 * borderRadius, borderRadius);
	
	// -------------------------------
	// body
	gfx_SetColor(bodyColor);
	
	// body rectangle (middle)
	gfx_FillRectangle_NoClip(x + borderRadius, y + borderRadius, width - 2 * borderRadius, height - borderRadius - 1);
	
	// left rectangle
	gfx_FillRectangle_NoClip(x + 1, y + borderRadius, borderRadius - 1, height - 2 * borderRadius);
	
	// right rectangle
	gfx_FillRectangle_NoClip(x + width - 1 - borderRadius, y + borderRadius, borderRadius, height - 2 * borderRadius);

	// -------------------------------
	// title text
	int titleWidth = gfx_GetStringWidth(title);
	int titleX = (x + (width - titleWidth) / 2);
	int titleY = y + (borderRadius / 2) - 3;
	gfx_SetTextXY(titleX, titleY);
	gfx_SetTextFGColor(black);
	gfx_PrintString(title);
}

void chippedRectangle(int x, int y, int width, int height)
{
	// body
	gfx_FillRectangle_NoClip(x + 1, y + 1, width - 2, height - 2);
	
	// top edge
	gfx_HorizLine_NoClip(x + 1, y, width - 2);
	
	// bottom edge
	gfx_HorizLine_NoClip(x + 1, y + height - 1, width - 2);
	
	// left edge
	gfx_VertLine_NoClip(x, y + 1, height - 2);
	
	// right edge
	gfx_VertLine_NoClip(x + width - 1, y + 1, height - 2);
}

void drawWrappedText(char *str, int x, int y, int width, int height)
{
	int cursorY = y;
	char *nextLine = str;
	int lineLen;
	int i;
	
	gfx_SetTextFGColor(black);
	gfx_SetTextXY(x, y);
	
	while ((cursorY < y + height - 9) && (str != NULL))
	{
		nextLine = getStrLine(str, &lineLen, width);
		dbg_printf("Line len: %d", lineLen);
		for(i = 0; i < lineLen; i++)
		{
			gfx_PrintChar(str[i]);
		}
		str = nextLine;
		cursorY += 10;
		gfx_SetTextXY(x, cursorY);
	}
}

char *getStrWord(char *str, int *widthBuffer, int *lenBuffer)
{
	(*widthBuffer) = 0;
	(*lenBuffer )= 0;
	int len = 0, width = 0;
	
	if(*str == ' ')
	{
		*widthBuffer = gfx_GetCharWidth(' ');
		*lenBuffer = 1;
		return ++str;
	}
	
	while((*str != '\0') && (*str != ' '))
	{
		width += gfx_GetCharWidth(*str);
		len++;
		str++;
	}
	
	(*widthBuffer) = width;
	(*lenBuffer) = len;
	
	return str;
}

char *getStrLine(char *str, int *lenBuffer, int textBoxWidth)
{
	char *prevStr = str;
	int wordWidth, wordLen, lineLen = 0, lineWidth = 0;
	*lenBuffer = 0;
	
	while(*str != '\0' && str != NULL)
	{
		str = getStrWord(str, &wordWidth, &wordLen);
		if(lineWidth + wordWidth <= textBoxWidth)
		{
			lineWidth += wordWidth;
			lineLen += wordLen;
			prevStr = str;
		}
		else if(lineLen == 0 && wordWidth > textBoxWidth)
		{
			while(lineWidth + (int)gfx_GetCharWidth(*prevStr) <= textBoxWidth)
			{
				lineWidth +=  gfx_GetCharWidth(*prevStr);
				lineLen++;
				prevStr++;
			}
			*lenBuffer = lineLen;
			return prevStr;
		}
		else
		{
			*lenBuffer = lineLen;
			return prevStr;
		}
	}
	
	(*lenBuffer) = lineLen;
	return NULL;
}

uint8_t inputString(char* buffer, uint8_t maxLength, bool restrictFirstChar, int boxX, int boxY, int boxWidth)
{
	uint8_t strLen = 0;
	char character = '\0';
	enum textMode textMode = UPPERCASE;
	sk_key_t keyPressed;
	bool redrawGraphics = true;
	bool deletePrev = false;
	float time;
	float cursorBlinkTime = .5, lastCursorChangeTime;
	bool showCursor = true;
	
	// wipe the buffer (always pays off to be sure)
	for(int i = 0; i < maxLength + 1; i++)
	{
		buffer[i] = '\0';
	}
	
	// make sure nothing is being pressed
	while(kb_AnyKey())
	{
		kb_Scan();
	}
	
	// reset, enable, and set timer to count up @ 32768 Hz
	timer_Set(1, 0);
	timer_Enable(1, TIMER_32K, TIMER_0INT, TIMER_UP);
	time = (float)timer_GetSafe(1, TIMER_UP) / 32768;
	lastCursorChangeTime = time;
	
	while(true)
	{
		// ------- graphics -------
		if(redrawGraphics)
		{
			redrawGraphics = false;
			gfx_SetDrawBuffer();
			
			// blue outline
			gfx_SetColor(finderSelectorColor);
			chippedRectangle(boxX, boxY, boxWidth, INPUT_TEXT_BOX_HEIGHT);
			gfx_Rectangle_NoClip(boxX + 1, boxY + 1, boxWidth - 2, INPUT_TEXT_BOX_HEIGHT - 2);
			
			// background box
			gfx_SetColor(white);
			gfx_FillRectangle_NoClip(boxX + 2, boxY + 2, boxWidth - 4, INPUT_TEXT_BOX_HEIGHT - 4);
			
			// text
			gfx_SetTextFGColor(black);
			gfx_PrintStringXY(buffer, boxX + 4, boxY + 5);
			
			// cursor
			if(showCursor)
			{
				gfx_SetColor(finderSelectorColor);
				gfx_VertLine_NoClip(boxX + 4 + gfx_GetStringWidth(buffer), boxY + 3, 10);
				gfx_VertLine_NoClip(boxX + 4 + gfx_GetStringWidth(buffer) + 1, boxY + 3, 10);
			}
			
			gfx_BlitBuffer();
		}
		
		// update
		kb_Scan();
		time = (float)timer_GetSafe(1, TIMER_UP) / 32768;
		
		// show/hide cursor
		if(time - lastCursorChangeTime >= cursorBlinkTime)
		{
			lastCursorChangeTime = time;
			
			if(showCursor)
			{
				showCursor = false;
				redrawGraphics = true;
			}
			else
			{
				showCursor = true;
				redrawGraphics = true;
			}
		}
		
		// return
		if (kb_IsDown(kb_KeyEnter) && strLen > 0)
		{
			while(kb_IsDown(kb_KeyEnter)) kb_Scan();
			return 1;
		}
		
		// quit
		else if (kb_IsDown(kb_KeyClear))
		{
			while(kb_IsDown(kb_KeyClear)) kb_Scan();
			return 0;
		}
		
		// delete character
		if(kb_IsDown(kb_KeyDel) && !(deletePrev))
		{
			deletePrev = true;
			
			if(strLen > 0)
			{
				buffer[strLen - 1] = '\0';
				strLen--;
			}
			redrawGraphics = true;
			continue;
		}
		else if(!kb_IsDown(kb_KeyDel))
		{
			deletePrev = false;
		}
		
		// input character
		keyPressed = getSingleCSCKey();
		if(keyPressed && strLen < maxLength && keyPressed != sk_Del && keyPressed)
		{
			character = inputChar(textMode, keyPressed);
			if(character != '\0')
			{
				if(strLen == 0 && restrictFirstChar == true)
				{
					if((character >= 'A' && character <= 'Z') || (character >= 'a' && character <= 'z') || (character >= '0' && character <= '9'))
					{
						buffer[strLen] = character;
						strLen++;
						
						showCursor = true;
						lastCursorChangeTime = time;
					}
				}
				else
				{
					buffer[strLen] = character;
					strLen++;
					
					showCursor = true;
					lastCursorChangeTime = time;
				}
			}
			
			redrawGraphics = true;
		}
	}
	
	return 0;
}

char inputChar(enum textMode mode, uint8_t keyPressed)
{
	static const unsigned char numsDat[] =
	{
		0x0, 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0, 
		0x0, 0x0 , 0x2B, 0x2D, 0x2A, 0x2F, 0x5E, 0x0, 
		0x0, 0x2D, 0x33, 0x36, 0x39, 0x29, 0x0 , 0x0, 
		0x0, 0x2E, 0X32, 0x35, 0x38, 0x28, 0x0 , 0x0, 
		0x0, 0x30, 0x31, 0x34, 0x37, 0x2C, 0x0 , 0x0, 
		0x0, 0x0 , 0x1a, 0x0 , 0x0 , 0x0 , 0x0 , 0x0, 
	};
	static const unsigned char UPPERCASEDat[] =
	{
		0x0, 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 
		0x0, 0x0 , 0x22, 0x57, 0x52, 0x4D, 0x48, 0x0 , 
		0x0, 0x3F, 0x5B, 0x56, 0x51, 0x4C, 0x47, 0x0 , 
		0x0, 0x3A, 0X5A, 0x55, 0x50, 0x4B, 0x46, 0x43, 
		0x0, 0x20, 0x59, 0x54, 0x4F, 0x4A, 0x45, 0x42, 
		0x0, 0x0 , 0x58, 0x53, 0x4E, 0x49, 0x44, 0x41, 
	};
	static const unsigned char lowerCaseDat[] =
	{
		0x0, 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 0x0 , 
		0x0, 0x22, 0x22, 0x77, 0x72, 0x6d, 0x68, 0x0 , 
		0x0, 0x3f, 0x0 , 0x76, 0x71, 0x6c, 0x67, 0x0 , 
		0x0, 0x3a, 0X7a, 0x75, 0x70, 0x6b, 0x66, 0x63, 
		0x0, 0x20, 0x79, 0x74, 0x6f, 0x6a, 0x65, 0x62, 
		0x0, 0x0 , 0x78, 0x73, 0x6e, 0x69, 0x64, 0x61, 
	};
	
	char character = '\0';
	
	if (mode == NUMBERS)
	{
		character = numsDat[keyPressed];
		return character;
	}
	if (mode == UPPERCASE)
	{
		character = UPPERCASEDat[keyPressed];
		return character;
	}
	if (mode == LOWERCASE)
	{
		character = lowerCaseDat[keyPressed];
		return character;
	}
	
	return character;
}