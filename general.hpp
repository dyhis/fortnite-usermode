#pragma once

#define NOMINMAX
#define constant static inline
#define USE_VL_MACRO

#include <windows.h>
#include <winternl.h>
#include <variant>
#include <tlhelp32.h>
#include <ntstatus.h>
#include <d3d11.h>
#include <dwmapi.h>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <numbers>
#include <mutex>
#include <chrono>
#include <iostream>
#include <bit>
#include <type_traits>
#include <vector>
#include <fstream>

using namespace std;

//dependencies
#include "dep/ext/lazy.h"
#include "dep/shadow_syscall.hpp"
#include "dep/cacher.hpp"
#include "dep/Security/Encryption/oxorany_include.h"
#include "dep/imgui/imgui_impl_dx11.h"
#include "dep/imgui/imgui_impl_win32.h"
#include "dep/imgui/fonts/poppins.h"
#include "dep/sdk/vxlib.h"
#include "dep/crt.h"
#include "dep/imgui/imgui.h"
#include "dep/ui/widgets.h"
#include "dep/kernel/ntdef.hpp"
#include "dep/kernel/kernel.hpp"
#include "dep/input/input.hpp"
#include "dep/ui/keybinds.hpp"
#include "dep/nlohmann/json.hpp"
#include "dep/config/config.hpp"
#include "dep/safeguard.hpp"

#define BUGSPLAT_DATABASE L"FortRizz"
#define APPLICATION_NAME L"Fortnite"
#define APPLICATION_VERSION L"1.0.0"

//implementation
#include "impl/cheat/triggerbot/triggerbot.hpp"
#include "impl/cheat/visuals/visuals.hpp"
#include "impl/render/menu/menu.hpp"
#include "impl/render/render.hpp"
#include "impl/uengine/ext/math.h"

struct bone_data
{
	std::vector<math::ftransform> bones;
	math::ftransform component_to_world;

	bone_data() = default;

	explicit bone_data( std::vector<math::ftransform> b, math::ftransform ctw ) : bones( std::move( b ) ), component_to_world( ctw )
	{
	}
};

#include "impl/uengine/ext/game.hpp"
#include "impl/updater/updater.hpp"
#include "impl/uengine/uengine.hpp"
#include "impl/globals.hpp"

inline auto& g_kernel = c_kernel;
inline auto& g_render = c_render;
inline auto& g_triggerbot = c_triggerbot;

//updater
inline auto& g_engine = updater::engine;
inline auto& g_entities = updater::entities;
inline auto& g_enviroment = updater::environment;

inline auto& g_camera = c_uengine.s_camera;
inline auto& g_player = c_uengine.s_player;
inline auto& g_wscreen = c_uengine.s_wscreen;
















