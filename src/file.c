#include "file.h"

#include "finder.h"
#include "main.h"

#include <stdint.h>
#include <fileioc.h>
#include <string.h>

uint8_t loadFiles(struct file files[MAX_FILES_LOADABLE])
{
	uint8_t numFiles = 0;
	char *namePtr = NULL;
	void *search_pos = NULL;
	
	while ((namePtr = ti_Detect(&search_pos, FILE_DETECT_STRING)) != NULL)
	{
		if (numFiles > 30)
		{
			return numFiles;
		}
		
		strcpy(files[numFiles].name, namePtr);
		
		// XXX in the future I will display the 16 digit aesthetic filename
		
		// if(isHidden(files[numFiles].view_name))
		// {
		// 	files[numFiles].view_name[0] ^= 64;
		// 	files[numFiles].isHidden = true;
		// }
		// else
		// {
		// 	files[numFiles].isHidden = false;
		// }
		
		numFiles++;
		
	}
	
	return numFiles;
}

void loadFileData(struct file *file)
{
	// check if it exists
	
	uint8_t slot = ti_Open(file->name, "r");
	if(!slot)
	{
		programState = QUIT;
		return;
	}
	
	// get file size and make sure it doesn't exceed the maximum
	
	file->size = ti_GetSize(slot);
	file->dataSize = file->size - FILE_METADATA_SIZE;
	if(file->size > MAX_FILE_SIZE || file->size < FILE_DETECT_STRING_LEN)
	{
		programState = QUIT;
		return;
	}
	
	editor.cursorLeft = 0;
	editor.cursorRight = editor.buffer + MAX_DATA_SIZE - file->dataSize;
	editor.startOfPage = editor.cursorRight;
	
	// copy file into the end of the editor text buffer
	
	ti_Seek(FILE_DATA_POS, SEEK_SET, slot);
	ti_Read(editor.cursorRight, file->dataSize, 1, slot);
	ti_Close(slot);
}

uint8_t getNumFiles(void)
{
	uint8_t numFiles = 0;
	void *pos = NULL;
	
	while (ti_Detect(&pos, FILE_DETECT_STRING) != NULL)
	{
		numFiles++;
	}
	
	return numFiles;
}

int toggleHiddenStatus(char* name)
{
	ti_var_t fileSlot = ti_Open(name, "r");
	if(!fileSlot || strlen(name) > 8) {
		ti_Close(fileSlot);
		return -1;
	}
	
	char temp[10] = {0};
	ti_GetName(temp, fileSlot);
	temp[0] ^= 64;
	ti_Close(fileSlot);
	ti_Rename(name, temp);
	
	return isHidden(temp);
}

bool isHidden(char* name)
{
	return (name[0])<(65);
}

void archiveAll(void)
{
	void *searchPtr = NULL;
	char *namePtr = NULL;
	ti_var_t fileSlot;
	
	while ((namePtr = ti_Detect(&searchPtr, FILE_DETECT_STRING)) != NULL)
	{
		fileSlot = ti_Open(namePtr, "r");

		if(!fileSlot)
		{
			return;
		}
		
		if (ti_SetArchiveStatus(true, fileSlot))
		{
			ti_Close(fileSlot);
		}
		else
		{
			return;
		}
	}
}