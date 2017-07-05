#include "Foundation/filesystem.hpp"
#include <iostream>

#ifdef BOOST_MSVC
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#elif __linux__
#include <limits.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#else

#error "Platform not supported."

#endif

namespace gintonic {

boost::filesystem::path get_executable_path()
{
#ifdef BOOST_MSVC

    char       buf[MAX_PATH];
    const auto numbytes = GetModuleFileName(nullptr, (LPSTR)buf, MAX_PATH);
    if (numbytes == 0)
    {
        return boost::filesystem::path();
    }
    else
    {
        boost::filesystem::path exec_path(buf);
        return exec_path.parent_path();
    }

#elif defined(__APPLE__)

    uint32_t bufsize = 0;
    char*    buf = nullptr;
    _NSGetExecutablePath(nullptr, &bufsize);
    buf = new char[bufsize];
    const auto result = _NSGetExecutablePath(buf, &bufsize);
    if (result == 0)
    {
        boost::filesystem::path exec_path(buf);
        delete[] buf;
        return exec_path.parent_path();
    }
    else
    {
        delete[] buf;
        return boost::filesystem::path(); // empty, silent failure
    }

#elif __linux__

    char  path[PATH_MAX];
    char  dest[PATH_MAX];
    pid_t pid = getpid();
    sprintf(path, "/proc/%d/exe", pid);
    if (readlink(path, dest, PATH_MAX) == -1)
    {
        return boost::filesystem::path(); // empty, silent failure
    }
    else
    {
        boost::filesystem::path exec_path(dest);
        return exec_path.parent_path();
    }

#else

#error "Platform not supported."

#endif
}

void initExecutablePath()
{
    boost::filesystem::current_path(get_executable_path());
}

std::string normalize(Path path)
{
    auto str = boost::filesystem::canonical(path).string();
    auto basePath = get_executable_path().string();
    std::replace(str.begin(), str.end(), '\\', '/');
    std::replace(basePath.begin(), basePath.end(), '\\', '/');
    const auto i = str.find(basePath);
    if (i != std::string::npos)
    {
        str.erase(i, basePath.length());
    }
    return str;
}

} // namespace gintonic
