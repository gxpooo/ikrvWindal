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
    GetLogicalDriveStringsA(100, lpBuffer); // ��ȡ�������б�
    std::vector<std::string>* driveNames = (std::vector<std::string> *) DriveNames;
    char* p = lpBuffer;
    while (*p) {
        std::string driveName = p;
        if (GetDriveTypeA(driveName.c_str()) == DRIVE_FIXED) { // ������̶�����������
            driveNames->push_back(driveName);
        }
        p += driveName.length() + 1; // ָ���¸�������
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
        char volumeNameBuffer[MAX_PATH] = { 0 }; // ����������
        char fileSystemNameBuffer[MAX_PATH] = { 0 }; // �ļ�ϵͳ���ƻ�����
        DWORD serialNumber = 0; // �������к�
        DWORD maxComponentLength = 0; // �ļ�����Сд���б��
        DWORD fileSystemFlags = 0; // �ļ�ϵͳ���
        info[x].Name = (*driveNames)[x];

        if (!GetVolumeInformationA((*driveNames)[x].c_str(), volumeNameBuffer, MAX_PATH, &serialNumber,
            &maxComponentLength, &fileSystemFlags, fileSystemNameBuffer, MAX_PATH)) {
            //std::cerr << "Failed to get information of drive " << driveName << std::endl;
            continue;
        }
        DWORDLONG totalSpace = 0; // �����ܿռ�
        DWORDLONG freeSpace = 0; // ���̿��ÿռ�
        DWORDLONG totalFreeSpace = 0; // �����ܿ��ÿռ�
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
    //��ȡ��ǰ�û�������·��
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
    //_findfirst���ص���long��; long __cdecl _findfirst(const char *, struct _finddata_t *)
    if ((lf = _findfirst(Path.c_str(), &file)) == -1l)
        return 0;


    //int __cdecl _findnext(long, struct _finddata_t *);����ҵ��¸��ļ������ֳɹ��Ļ��ͷ���0,���򷵻�-1
    while (_findnext(lf, &file) == 0)
    {
        FileInfo fi = { file.name ,file.attrib };
        num += 1;
        FileGrps->push_back(fi);
        //cout << file.name;
        //if (file.attrib == _A_NORMAL)cout << " ��ͨ�ļ� ";
        //else if (file.attrib == _A_RDONLY)cout << " ֻ���ļ� ";
        //else if (file.attrib == _A_HIDDEN)cout << " �����ļ� ";
        //else if (file.attrib == _A_SYSTEM)cout << " ϵͳ�ļ� ";
        //else if (file.attrib == _A_SUBDIR)cout << " ��Ŀ¼ ";
        //else cout << " �浵�ļ� ";
        //cout << endl;
    }

    _findclose(lf);
    return &(*FileGrps)[0];
}


