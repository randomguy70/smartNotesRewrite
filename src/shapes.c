#include "shapes.h"
#include "colors.h"

#include <graphx.h>

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

void roundedRectangleOutlined(int x, int y, int width, int height, int borderRadius, uint8_t bodyColor, uint8_t outlineColor)
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

void window(int x, int y, int width, int height, int borderRadius, enum color headerColor, enum color bodyColor, enum color outlineColor)
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