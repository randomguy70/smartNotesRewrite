#ifndef FILE_H
#define FILE_H

#include "editor.h"

#include "stdbool.h"
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FILE_DETECT_STRING        "NOTES"
#define FILE_DETECT_STRING_LEN    5
#define MAX_FILES_LOADABLE        30
#define MAX_FILE_SIZE             MAX_DATA_SIZE + FILE_DETECT_STRING_LEN
#define FILE_DATA_OFFSET          FILE_DETECT_STRING_LEN

struct file
{
	char name[8];
	int size;
	int dataSize;
};

// loads the names and sizes of all detected note files (max number is 30)
uint8_t loadFiles(struct file files[MAX_FILES_LOADABLE]);

// loads the data from a file into the editor struct
void loadFileData(struct file *file);

uint8_t getNumFiles(void);
int toggleHiddenStatus(char* name);
bool isHidden(char* name);
void archiveAll(void);


#ifdef __cplusplus
}
#endif

#endif