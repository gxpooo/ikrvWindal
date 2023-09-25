#include "pch.h"
#include "ikrvWindal.h"
#include <iostream>
#include <vector>
#include "memory.h"
#include <Shlobj.h>
#include<io.h>

#pragma comment(lib, "shell32.lib")

using namespace std;



SystemInfo::SystemInfo() {
    DriveNames = new std::vector<std::string>;
    Files = new vector<FileInfo>;
    char lpBuffer[100] = { 0 };
    GetLogicalDriveStringsA(100, lpBuffer); // 获取驱动器列表
    std::vector<std::string>* driveNames = (std::vector<std::string> *) DriveNames;
    char* p = lpBuffer;
    while (*p) {
        std::string driveName = p;
        if (GetDriveTypeA(driveName.c_str()) == DRIVE_FIXED) { // 仅处理固定磁盘驱动器
            driveNames->push_back(driveName);
        }
        p += driveName.length() + 1; // 指向下个驱动器
    }
}

SystemInfo::~SystemInfo() {
    delete (std::vector<std::string> *)DriveNames;
    DriveNames = 0;
    delete (vector<FileInfo> *)Files;
    Files = 0;
}

string SystemInfo::TCHAR2STRING(TCHAR* str)
{
    std::string strstr;
    try
    {
        int iLen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
        char* chRtn = new char[iLen * sizeof(char)];
        WideCharToMultiByte(CP_ACP, 0, str, -1, chRtn, iLen, NULL, NULL);

        strstr = chRtn;
        delete[] chRtn;
        chRtn = 0;
    }
    catch (std::exception e)
    {

    }
    return strstr;
}


int SystemInfo::DriveLetterNum() {
    return ((std::vector<std::string> *) DriveNames)->size();
}


int SystemInfo::LogicalDriveLetter(DriveLetterInfo* info, unsigned dlNum) {
    std::vector<std::string>* driveNames = (std::vector<std::string> *) DriveNames;

    for (int x = 0; x < driveNames->size(); x++) {
        char volumeNameBuffer[MAX_PATH] = { 0 }; // 卷名缓冲区
        char fileSystemNameBuffer[MAX_PATH] = { 0 }; // 文件系统名称缓冲区
        DWORD serialNumber = 0; // 扇区序列号
        DWORD maxComponentLength = 0; // 文件名大小写敏感标记
        DWORD fileSystemFlags = 0; // 文件系统标记
        info[x].Name = (*driveNames)[x];

        if (!GetVolumeInformationA((*driveNames)[x].c_str(), volumeNameBuffer, MAX_PATH, &serialNumber,
            &maxComponentLength, &fileSystemFlags, fileSystemNameBuffer, MAX_PATH)) {
            //std::cerr << "Failed to get information of drive " << driveName << std::endl;
            continue;
        }
        DWORDLONG totalSpace = 0; // 磁盘总空间
        DWORDLONG freeSpace = 0; // 磁盘可用空间
        DWORDLONG totalFreeSpace = 0; // 磁盘总可用空间
        if (!GetDiskFreeSpaceExA((*driveNames)[x].c_str(), (PULARGE_INTEGER)&freeSpace,
            (PULARGE_INTEGER)&totalSpace, (PULARGE_INTEGER)&totalFreeSpace)) {
            //std::cerr << "Failed to get disk space info of drive " << driveName << std::endl;
            continue;
        }
        info[x].FreeSize = (double)freeSpace / 1024 / 1024 / 1024;//GB
        info[x].TotalSize = (double)totalSpace / 1024 / 1024 / 1024;//GB
        info[x].Type = GetDriveTypeA(info[x].Name.c_str());
        info[x].Volume = volumeNameBuffer;
        info[x].FileSystem = fileSystemNameBuffer;
    }
    return 0;
}


string SystemInfo::GetDesktopFolder()
{
    TCHAR path[255];
    //获取当前用户的桌面路径
    SHGetSpecialFolderPath(0, path, CSIDL_DESKTOPDIRECTORY, 0);
    std::string rlt = TCHAR2STRING(path);
    return rlt;
}

string SystemInfo::GetUserFolder()
{
    TCHAR path[255];
    SHGetSpecialFolderPath(0, path, CSIDL_PROFILE, 0);
    std::string rlt = TCHAR2STRING(path);
    return rlt;
}


SystemInfo::FileInfo* SystemInfo::ls(const char* path, unsigned& num) {
    _finddata_t file;
    long lf;
    string Path = path;
    Path += "\\*.*";
    vector<FileInfo>* FileGrps = (vector<FileInfo> *)Files;
    num = 0;
    //_findfirst返回的是long型; long __cdecl _findfirst(const char *, struct _finddata_t *)
    if ((lf = _findfirst(Path.c_str(), &file)) == -1l)
        return 0;


    //int __cdecl _findnext(long, struct _finddata_t *);如果找到下个文件的名字成功的话就返回0,否则返回-1
    while (_findnext(lf, &file) == 0)
    {
        FileInfo fi = { file.name ,file.attrib };
        num += 1;
        FileGrps->push_back(fi);
        //cout << file.name;
        //if (file.attrib == _A_NORMAL)cout << " 普通文件 ";
        //else if (file.attrib == _A_RDONLY)cout << " 只读文件 ";
        //else if (file.attrib == _A_HIDDEN)cout << " 隐藏文件 ";
        //else if (file.attrib == _A_SYSTEM)cout << " 系统文件 ";
        //else if (file.attrib == _A_SUBDIR)cout << " 子目录 ";
        //else cout << " 存档文件 ";
        //cout << endl;
    }

    _findclose(lf);
    return &(*FileGrps)[0];
}


