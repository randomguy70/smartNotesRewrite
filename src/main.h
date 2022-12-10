#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * FINDER: request to run the finder
 * EDITOR: request to run the editor
 * SETTINGS: request to run settings
 * CANCEL: request to go back a scope (exit a menu, cancel a window, etc...)
 * QUIT: request to quit the program
**/
enum programState {FINDER, EDITOR, SETTINGS, CANCEL, QUIT};

extern enum programState programState;

#ifdef __cplusplus
}
#endif

#endif