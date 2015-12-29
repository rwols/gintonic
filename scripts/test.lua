-- Test script ...
-- See the file lib/luabindings.hpp
-- and the file lib/luabindings.cpp

foo = 4
bar = {}
bar[3] = "hi"
bar["key"] = "there"

vec2 = {}

function vec2:new(o)
	if o == nil then o = {x=0, y=0} end
	setmetatable(o, self)
	self.__index = self
	return o
end

function vec2:__tostring()
	return "(" .. self.x .. "," .. self.y .. ")"
end

function vec2:__unm()
	return vec2:new{x = -self.x, y = -self.y}
end

function vec2:__add( other )
	return vec2:new {x = (self.x + other.x), y = (self.y + other.y) }
end

function vec2:__sub( other )
	return vec2:new {x = (self.x - other.x), y = (self.y - other.y) }
end

function vec2:__mul (other)
	return vec2:new {x = (self.x * other.x), y = (self.y * other.y) }
end

function vec2:__eq (other)
	return self.x == other.x and self.y == other.y
end

function vec2:__lt (other)
	return self.x < other.x and self.y < other.y
end

function vec2:__le (other)
	return self.x <= other.x and self.y <= other.y
end

function vec2:scale(number)
	return vec2:new {x = (self.x * number), y = (self.y * number) }
end

function dot (lhs, rhs)
	return lhs.x * rhs.x + lhs.y * rhs.y
end

function mix (a_vec, b_vec, multiplier)
	return a_vec:scale(1-multiplier) + b_vec:scale(multiplier)
end

v = vec2:new {x=2,y=3}
w = vec2:new {x=3,y=1}
emptyvec = vec2:new(nil)

print("v: ", v)
print("w: ", w)
print("emptyvec: ", emptyvec)

sum = v + w

print("sum:", sum)

scaledsum = sum:scale(5)

mixer = mix(v, w, 0.5)

print(mixer)

print (scaledsum)

print(dot(v,w))

print(v < w)
print(w < v)
print(v <= w)
print (v == w)
print(emptyvec < w)
print(emptyvec < v)