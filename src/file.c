#include "file.h"

#include "finder.h"
#include "main.h"
#include "editor.h"
#include "ui.h"

#include <stdint.h>
#include <fileioc.h>
#include <string.h>
#include <stdbool.h>
#include <keypadc.h>
#include <debug.h>

uint8_t loadFiles(struct file files[MAX_FILES_LOADABLE])
{
	uint8_t numFiles = 0;
	char *osName = NULL;
	void *search_pos = NULL;
	uint8_t fileSlot;
	
	while ((osName = ti_Detect(&search_pos, FILE_DETECT_STRING)) != NULL)
	{
		if (numFiles > MAX_FILES_LOADABLE)
		{
			return numFiles;
		}
		
		// store the file's os name (the name the calculator uses to identify files)
		strcpy(files[numFiles].osName, osName);
		// store the aesthetic name (the name the user sees)
		fileSlot = ti_Open(files[numFiles].osName, "r");
		if(!fileSlot)
		{
			alert("Warning!", "Error loading file. Not going to load any more files.");
			break;
		}
		ti_Seek(AESTHETIC_FILE_NAME_POS, SEEK_SET, fileSlot);
		ti_Read(files[numFiles].aestheticName, (AESTHETIC_FILE_NAME_LEN - 1), 1, fileSlot);
		ti_Close(fileSlot);
		// don't forget the null byte after the aesthetic name string
		files[numFiles].aestheticName[AESTHETIC_FILE_NAME_LEN - 1] = '\0';
		
		dbg_printf("Loaded file %s\n", files[numFiles].aestheticName);
		
		numFiles++;
	}
	
	return numFiles;
}

void loadFileData(struct file *file)
{
	// check if it exists
	
	uint8_t slot = ti_Open(file->osName, "r");
	if(!slot)
	{
		programState = QUIT;
		return;
	}
	
	// get file size and make sure it doesn't exceed the maximum
	
	file->size = ti_GetSize(slot);
	file->dataSize = file->size - FILE_METADATA_SIZE;
	
	if(file->dataSize > MAX_FILE_SIZE || file->size < FILE_METADATA_SIZE)
	{	
		programState = QUIT;
		return;
	}
	
	editor.cursorLeft = editor.buffer;
	editor.cursorRight = editor.buffer + MAX_DATA_SIZE - file->dataSize;
	editor.startOfPage = editor.cursorRight;
	
	// copy file's data into the end of the editor text buffer
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
	if(!fileSlot || strlen(name) > 8)
	{
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

void archiveAllFiles(void)
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

bool createNotesFile(char *aestheticName)
{
	uint8_t slot, hiddenSlot;
	char osName[9] = {'\0'};
	char hiddenOsName[9] = {'\0'};
	uint8_t nullByte = 0;
	uint8_t creationAttempts = 0, maxCreationAttempts = 100;
	
	if(strlen(aestheticName) == 0)
	{
		return false;
	}
	
	for(int i = 1; i < 8; i++)
	{
		osName[i] = aestheticName[i];
		hiddenOsName[i] = aestheticName[i];
	}
	osName[0] = 65;
	hiddenOsName[0] = osName[0] ^ 64;
	
	while(creationAttempts < maxCreationAttempts)
	{
		// check if a file already exists with the same os name (hidden or not)
		slot = ti_Open(osName, "r");
		hiddenSlot = ti_Open(hiddenOsName, "r");
		ti_Close(slot);
		ti_Close(hiddenSlot);
		
		// if no file exists with the same os name...
		if(!slot && !hiddenSlot)
		{
			slot = ti_Open(osName, "w+");
			
			ti_Seek(FILE_DETECT_STRING_POS, SEEK_SET, slot);
			ti_Write(FILE_DETECT_STRING, sizeof(FILE_DETECT_STRING) - 1, 1, slot);
			ti_Seek(AESTHETIC_FILE_NAME_POS, SEEK_SET, slot);
			ti_Write(&nullByte, 1, AESTHETIC_FILE_NAME_LEN, slot);
			ti_Seek(AESTHETIC_FILE_NAME_POS, SEEK_SET, slot);
			ti_Write(aestheticName, strlen(aestheticName), 1, slot);
			ti_Write(&nullByte, 1, 1, slot);
			ti_Seek(AESTHETIC_FILE_NAME_POS + AESTHETIC_FILE_NAME_LEN, SEEK_SET, slot);
			ti_Write(&nullByte, 1, 1, slot);
			ti_Close(slot);
			
			return true;
		}
		// if a file with the same os name already exists, change the os name a tad
		else
		{
			osName[0]++;
			hiddenOsName[0] = osName[0] ^ 64;
			creationAttempts++;
		}
	}
	while(kb_AnyKey()) kb_Scan();
	while(!kb_AnyKey()) kb_Scan();
	return false;
}

bool askIfDeleteFile(void)
{
	char bodyString[95] = {"Do you really want to delete \""};
	strcat(bodyString, finder.files[finder.selectedFile].aestheticName);
	strcat(bodyString, "\"? Press enter to delete, and clear to cancel.");
	
	bool delete = alert("Warning!", bodyString);
	if(delete)
	{
		ti_Delete(finder.files[finder.selectedFile].osName);
		finder.reloadFiles = true;
		
		while(kb_IsDown(kb_KeyEnter)) kb_Scan();
		return true;
	}
	else
	{
		return false;
	}
}