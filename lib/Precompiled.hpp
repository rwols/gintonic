#pragma once

#include <SDL.h>
#include <boost/circular_buffer.hpp>
#include <boost/filesystem.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/signals2.hpp>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <glad/glad.h>
#include <imgui.h>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <new>
#include <set>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Math/SQT.hpp"
#include "Math/box2f.hpp"
#include "Math/box3f.hpp"
#include "Math/mat2f.hpp"
#include "Math/mat3f.hpp"
#include "Math/mat4f.hpp"
#include "Math/quatf.hpp"
#include "Math/vec2f.hpp"
#include "Math/vec3f.hpp"
#include "Math/vec4f.hpp"

#include "Foundation/Profiler.hpp"
#include "Foundation/exception.hpp"
#include "Foundation/filesystem.hpp"
#include "Foundation/portable_archive.hpp"
#include "Foundation/portable_iarchive.hpp"
#include "Foundation/portable_oarchive.hpp"
#include "Foundation/utilities.hpp"

#include "Graphics/Renderer.hpp"
