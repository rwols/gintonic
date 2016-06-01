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

/**
 * @brief Get the filepath of the executable, cross-platform.
 * @return The filepath of the executable.
 */
boost::filesystem::path get_executable_path();

// typedef boost::error_info<struct tag_filesystem_path_err,boost::filesystem::path> errinfo_path;

} // namespace gintonic

//!@cond

BOOST_SERIALIZATION_SPLIT_FREE(boost::filesystem::path)

namespace boost {
namespace serialization {

template<class Archive>
void save(Archive& ar, const filesystem::path& p, unsigned int version) 
{
	ar & p.string();
}
template<class Archive>
void load(Archive& ar, filesystem::path& lFilepath, unsigned int version) 
{
	std::string lFilepathAsString;
	ar & lFilepathAsString;
	lFilepath = lFilepathAsString;
}

} // namespace serialization
} // namespace boost

//!@endcond
