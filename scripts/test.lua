-- test script to make sure
-- that selene + lua work
-- See the file lib/luabindings.hpp
-- and the file lib/luabindings.cpp

foo = 4
bar = {}
bar[3] = "hi"
bar["key"] = "there"

-- Attempt to create a "vector" "class".
vec2 = {}
vec2.mt = {}
setmetatable(vec2, vec2.mt)