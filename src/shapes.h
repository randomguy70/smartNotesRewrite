#ifndef SHAPES_H
#define SHAPES_H

#include "colors.h"

#ifdef __cplusplus
extern "C" {
#endif

// filled rectangle with rounded corners
void roundedRectangle(int x, int y, int width, int height, int borderRadius);

// rounded window with outline and header
void window(int x, int y, int width, int height, int borderRadius, enum color headerColor, enum color bodyColor, enum color outlineColor);

// filled rectangle with each corner pixel missing (for small rounded rectangle simulation)
void chippedRectangle(int x, int y, int width, int height);

#ifdef __cplusplus
}
#endif

#endif