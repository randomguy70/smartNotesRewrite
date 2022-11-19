#ifndef FILE_H
#define FILE_H

#include "stdbool.h"
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FILE_DETECT_STRING  "NOTES"
#define MAX_FILES_LOADABLE  30

struct file
{
	char name[8];
	int size;
};

uint8_t loadFiles(struct file files[MAX_FILES_LOADABLE]);
uint8_t getNumFiles(void);
int toggleHiddenStatus(char* name);
bool isHidden(char* name);
void archiveAll(void);


#ifdef __cplusplus
}
#endif

#endif