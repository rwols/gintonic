#include <gintonic/Foundation/getExecutablePath.hpp>
#include <iostream>

#ifdef BOOST_MSVC
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#elif defined(__linux__)
#include <limits.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#else
#error "Platform not supported."
#endif

using namespace gintonic;

FilePath getExecutablePath()
{

#ifdef BOOST_MSVC

    char buf[MAX_PATH];
    const auto numbytes = GetModuleFileName(nullptr, (LPSTR)buf, MAX_PATH);
    if (numbytes == 0)
    {
        return FilePath();
    }
    else
    {
        FilePath exePath(buf);
        return exePath.parent_path();
    }

#elif defined(__APPLE__)

    uint32_t bufsize = 0;
    char* buf = nullptr;
    _NSGetExecutablePath(nullptr, &bufsize);
    buf = new char[bufsize];
    const auto result = _NSGetExecutablePath(buf, &bufsize);
    if (result == 0)
    {
        FilePath exePath(buf);
        delete[] buf;
        return exePath.parent_path();
    }
    else
    {
        delete[] buf;
        return FilePath(); // empty, silent failure
    }

#elif __linux__

    char path[PATH_MAX];
    char dest[PATH_MAX];
    pid_t pid = getpid();
    sprintf(path, "/proc/%d/exe", pid);
    if (readlink(path, dest, PATH_MAX) == -1)
    {
        return FilePath(); // empty, silent failure
    }
    else
    {
        FilePath exePath(dest);
        return exePath.parent_path();
    }

#else
#error "Platform not supported."
#endif
}
