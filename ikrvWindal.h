#pragma once


#include <Windows.h>
#include <string>


#define CLASSEXPORT		__declspec(dllexport)

class CLASSEXPORT SystemInfo {
private:
	void* DriveNames;
	void* Files;
public:
	typedef struct _DriveLetter {
		unsigned TotalSize;
		unsigned FreeSize;
		std::string FileSystem;
		std::string Volume;
		std::string Name;
		int Type;
	}DriveLetterInfo;

	typedef struct _Fileinfo {
		std::string name;
		unsigned    attrib;
	}FileInfo;

	SystemInfo();
	~SystemInfo();

	int DriveLetterNum();
	int LogicalDriveLetter(DriveLetterInfo*, unsigned);

	std::string GetDesktopFolder();
	std::string GetUserFolder();


	FileInfo* ls(const char* path, unsigned& num);

private:
	std::string TCHAR2STRING(TCHAR* str);
	
};

