#pragma once

#include <gintonic/Foundation/FilePath.hpp>

#include <gintonic/nsbegin.hpp>

/**
 * @brief Get the filepath of the executable, cross-platform.
 * @return The filepath of the executable.
 */
FilePath getExecutablePath();

#include <gintonic/nsend.hpp>
