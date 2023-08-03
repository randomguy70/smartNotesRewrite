#ifndef FILE_H
#define FILE_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FILE_DETECT_STRING        "NOTES"
#define FILE_DETECT_STRING_POS    0
#define FILE_DETECT_STRING_LEN    5

#define AESTHETIC_FILE_NAME_POS   (FILE_DETECT_STRING_POS + FILE_DETECT_STRING_LEN) // 5
#define AESTHETIC_FILE_NAME_LEN   17

#define FILE_DATA_POS             (AESTHETIC_FILE_NAME_POS + AESTHETIC_FILE_NAME_LEN) // 22

#define FILE_EOF_LEN              1

#define FILE_METADATA_SIZE        (FILE_DETECT_STRING_LEN + AESTHETIC_FILE_NAME_LEN + FILE_EOF_LEN) // 23

#define MAX_DATA_SIZE             (3000)

#define MAX_FILE_SIZE             (FILE_DETECT_STRING_LEN + AESTHETIC_FILE_NAME_LEN + MAX_DATA_SIZE + 1)
#define MAX_FILES_LOADABLE        30

/*
----------------- FILE LAYOUT ----------------
| Offset | Length | Purpose                  |
----------------------------------------------
| 0      | 5      | file detect string       |
| 5      | 16     | aesthetic file name      |
| 21     | n      | file data n bytes long   |
| n + 21 | 1      | NULL byte signifying EOF |
----------------------------------------------
*/

struct file
{
	char aestheticName[AESTHETIC_FILE_NAME_LEN];
	char osName[9];
	int size;
	int dataSize;
};

// loads the names and sizes of all detected note files (max number is 30)
// ** used in the Finder
uint8_t loadFiles(struct file files[MAX_FILES_LOADABLE]);

// loads the data from a file into the editor struct
// ** used for the editor
void loadFileData(struct file *file);

uint8_t getNumFiles(void);
int toggleHiddenStatus(char* name);
bool isHidden(char* name);
void archiveAllFiles(void); // archives all the user's note files (soon it will archive all of SmartNotes' appvars too)

#ifdef __cplusplus
}
#endif

#endif