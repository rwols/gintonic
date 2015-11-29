#ifndef luabindings_hpp
#define luabindings_hpp

#include "selene/selene.h"

namespace gintonic {

sel::State get_lua_state();

void luatest();

} // namespace gintonic

#endif