#ifndef WINDOWSINCLUDES_H
#define WINDOWSINCLUDES_H

#define NTDDI_VERSION NTDDI_WIN7
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#define WINVER _WIN32_WINNT_WIN7

#pragma comment(lib, "mincore_downlevel.lib")
#include <windows.h>
#include <psapi.h>

#endif // WINDOWSINCLUDES_H
