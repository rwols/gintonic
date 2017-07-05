/**
 * @file filesystem.hpp
 * @brief Various filesystem utilities. Include this file
 * instead of <boost/filesystem.hpp>.
 * @author Raoul Wols
 */

#pragma once

#include <boost/filesystem.hpp>
#include <boost/serialization/split_free.hpp>

#include <string>

namespace gintonic {

using Path = boost::filesystem::path;

/**
 * @brief Get the filepath of the executable, cross-platform.
 * @return The filepath of the executable.
 */
Path get_executable_path();

/**
 * @brief Call this function in main before doing something path or asset
 *        related.
 *
 * @detail This function will change the current process's working directory
 * to the path of the executable.
 */
void initExecutablePath();

std::string normalize(Path path);

} // namespace gintonic

//!@cond

BOOST_SERIALIZATION_SPLIT_FREE(gintonic::Path)

namespace boost {
namespace serialization {

template <class Archive>
void save(Archive& ar, const gintonic::Path& path, unsigned int version)
{
    ar& gintonic::normalize(path);
}

template <class Archive>
void load(Archive& ar, gintonic::Path& path, unsigned int version)
{
    std::string filePathAsString;
    ar&         filePathAsString;
    path = std::move(filePathAsString);
}

} // namespace serialization
} // namespace boost

//!@endcond
