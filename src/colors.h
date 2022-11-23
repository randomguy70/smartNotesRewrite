#ifndef COLORS_H
#define COLORS_H

#ifdef __cplusplus
extern "C" {
#endif

// index of each color in the gfx pallett
// I arranged it like this so that I can edit the raw palett index of any color in the (future) settings
enum color
{
	transparent,
	black,
	white,
	finderWindowHeaderColor,
	finderWindowBodyColor,
	finderWindowOutlineColor,
	finderWindowBackgroundColor,
	finderSelectorColor,
	menuBarBackgroundColor,
	menuBarEdgeColor,
	menuBarSelecterColor,
};

#ifdef __cplusplus
}
#endif

#endif