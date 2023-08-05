#ifndef COLORS_H
#define COLORS_H

#ifdef __cplusplus
extern "C" {
#endif

// index of each color in the gfx pallett
// I arranged it like this so that I can edit the raw palett index of any color in the (future) settings
enum color
{
	// finder
	
	transparent,
	black,
	white,
	
	finderWindowHeaderColor = 3,
	finderWindowBodyColor,
	finderWindowOutlineColor,
	finderWindowBackgroundColor,
	finderSelectorColor,
	finderWindowTextColor,
	finderWindowSelectedTextColor,
	
	menuBarBackgroundColor,
	menuBarEdgeColor,
	menuBarSelecterColor,
	menuWindowBackgroundColor,
	menuWindowOutlineColor,
	menuWindowSelectorColor,
	menuWindowTextColor,
	menuWindowSelectedTextColor,
	finderHiddenFileColor,
	
	editorBackgroundColor = 30,
	editorHeaderBarBackgroundColor,
	editorHeaderBarBorderColor,
};

#ifdef __cplusplus
}
#endif

#endif