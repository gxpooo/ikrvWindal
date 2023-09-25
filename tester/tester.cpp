// tester.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "../ikrvWindal.h"

int main()
{
    SystemInfo si;

    int dlnum = si.DriveLetterNum();
    SystemInfo::DriveLetterInfo* dli = new SystemInfo::DriveLetterInfo[dlnum];
    si.LogicalDriveLetter(dli, dlnum);

    std::cout << "Desktop path:" << si.GetDesktopFolder() << "\r\n";

    for (unsigned x = 0; x < dlnum; x++) {
        std::cout << dli[x].Name << " " << dli[x].Volume << "\r\n";
        std::cout << dli[x].FileSystem << "\r\n";
        std::cout << dli[x].FreeSize << " GB\r\n";
        std::cout << dli[x].TotalSize << " GB\r\n";
    }
    unsigned fnum = 0;
    SystemInfo::FileInfo *fis =  si.ls(si.GetDesktopFolder().c_str(), fnum);


    for (unsigned x = 0; x < fnum; x++) {
        std::cout << "file:" << fis[x].name << " ," << fis[x].attrib << "\r\n";
    }

}

