/**
 * @file filesystem.hpp
 * @brief Various filesystem utilities. Include this file
 * instead of <boost/filesystem.hpp>.
 * @author Raoul Wols
 */

#pragma once

#include <boost/filesystem/path.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_free.hpp>
#include <string>

#include <gintonic/nsbegin.hpp>

/**
 * @brief A path class.
 */
using FilePath = boost::filesystem::path;

template <class Archive>
void save(Archive& ar, const FilePath& p, unsigned int version)
{
    const auto str = p.string();
    ar << ::boost::serialization::make_nvp("path", str);
}
template <class Archive>
void load(Archive& ar, FilePath& p, unsigned int version)
{
    std::string str;
    ar >> ::boost::serialization::make_nvp("path", str);
    p = str;
}

#include <gintonic/nsend.hpp>

BOOST_SERIALIZATION_SPLIT_FREE(NAMESPACE_GINTONIC::FilePath)
