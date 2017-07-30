#pragma once

#include <SDL.h>
#include <boost/circular_buffer.hpp>
#include <boost/filesystem.hpp>
#include <boost/intrusive_ptr.hpp>
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
