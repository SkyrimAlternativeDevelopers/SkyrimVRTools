#include "DirUtils.h"
#include <stdio.h>
#include <WinBase.h>

std::string GetCurrentWorkingDir(void) 
{
	char buff[FILENAME_MAX];
	GetCurrentDirectory(FILENAME_MAX, buff);
	std::string current_working_dir(buff);
	return current_working_dir;
}