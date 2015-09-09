/**
 * @file flyweights.hpp
 * @brief Various flyweight utilities. This file is deprecated.
 * @deprecated Will probably be removed later. Only the font
 * class still uses flyweights, everything else does not. Flyweights
 * do not play nicely with inheritance so that's unfortunate.
 * @author Raoul Wols
 */

#ifndef flyweights_hpp
#define flyweights_hpp

#include <boost/config.hpp>
#ifdef BOOST_MSVC
	#pragma warning (disable : 4520)
#endif
#include <boost/flyweight.hpp>
#include <boost/flyweight/key_value.hpp>
#ifdef BOOST_MSVC
	#pragma warning (default : 4520)
#endif

#endif