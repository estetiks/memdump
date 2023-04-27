#pragma once
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>
#include <fstream>
#include <string>
#include <sstream>

//Names of functions
int get_list_proc();//To het a list of working processes, pid, ppid

void dump_process_memory(DWORD, std::string);//To dump the virtual memory of process





//realising
int get_list_proc() {
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    DWORD dwPriorityClass;

    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        std::cout << "Error: CreateToolhelp32Snapshot (of processes)" << std::endl;
        return 1;
    }

    // Set the size of the structure before using it.
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Retrieve information about the first process and exit if unsuccessful
    if (!Process32First(hProcessSnap, &pe32)) {
        std::cout << "Error: Process32First" << std::endl;
        CloseHandle(hProcessSnap);  // clean the snapshot object
        return 1;
    }

    // Now walk the snapshot of processes, and display information about each process in turn
    do {
        std::cout << "------------------------------------------------------------" << std::endl;
        std::wcout << L"PROCESS NAME: " << pe32.szExeFile << std::endl;
        std::wcout << L"PID: " << pe32.th32ProcessID << std::endl;
        std::wcout << L"PARENT PID: " << pe32.th32ParentProcessID << std::endl;
        std::cout << "------------------------------------------------------------" << std::endl;
    } while (Process32Next(hProcessSnap, &pe32));

    // clean the snapshot object
    CloseHandle(hProcessSnap);
    return 0;

}

void dump_process_memory(DWORD pid, std::string path)
{

    HANDLE hProcess;
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hProcess == NULL)
    {
        std::cerr << "Error to create handle" << std::endl;
        return;
    }

    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    DWORD_PTR minAddr = (DWORD_PTR)systemInfo.lpMinimumApplicationAddress;
    DWORD_PTR maxAddr = (DWORD_PTR)systemInfo.lpMaximumApplicationAddress;
    DWORD_PTR addr = minAddr;


    
    std::string filename = path + "Dump_" + std::to_string((int)pid) + ".dmp";

    std::ofstream file(filename, std::ios::out | std::ios::binary | std::ios::app);
    if (!file)
    {
        std::cerr << "Error to create dumpfile " << GetLastError() << std::endl;
        CloseHandle(hProcess);
        return;
    }

    int nPrivBlock = 0;

    while (addr < maxAddr)
    {
        MEMORY_BASIC_INFORMATION memInfo;
        SIZE_T bytesReturned = VirtualQueryEx(hProcess, (LPCVOID)addr, &memInfo, sizeof(memInfo));
        if (bytesReturned == 0)
        {
            std::cerr << "Error to read memory process" << std::endl;
            CloseHandle(hProcess);
            file.close();
            return;
        }

        if (memInfo.State == MEM_COMMIT && memInfo.Protect != PAGE_NOACCESS)
        {
            BYTE* buffer = new BYTE[memInfo.RegionSize];
            SIZE_T bytesRead = 0;
            BOOL result = ReadProcessMemory(hProcess, memInfo.BaseAddress, buffer, memInfo.RegionSize, &bytesRead);
            if (!result || bytesRead != memInfo.RegionSize)
            {
                nPrivBlock++;
                delete[] buffer;
                addr += memInfo.RegionSize;
                continue;
            }
            file.write(reinterpret_cast<const char*>(buffer), bytesRead);
            delete[] buffer;
        }
        addr += memInfo.RegionSize;
    }

    file.close();
    CloseHandle(hProcess);
    std::cout << "Finded " + std::to_string(nPrivBlock) + " private blocks" << std::endl;
    std::cout << "Success! " << filename << std::endl;
}
