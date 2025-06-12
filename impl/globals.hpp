#pragma once

using json = nlohmann::json;

static int preset_index = 0;
static int preset2_index = 0;
static int aimbone = 0;

struct globals
{
	inline static int screen_width = GetSystemMetrics(0);
	inline static int screen_height = GetSystemMetrics(1);
	inline static int screen_width_center = screen_width / 2;
	inline static int screen_height_center = screen_height / 2;
};

struct aimbot
{
	constant float fov = 100;
	constant float smoothing = 5;
	constant int hitbox = 0;
};

struct settings
{
	struct aimbot
	{
		constant bool enabled = true;

		constant bool weapon_configs = false;

		constant bool prediction = true;
		constant bool dynamic_fov = true;
		constant bool visible_check = true;
		constant bool knocked_check = true;
		constant bool render_fov = true;

		constant int hotkey = VK_RBUTTON;

		constant float refresh_rate = 144;

		struct weapon_data
		{
			int hitbox = 0;
			float fov = 100;
			float smoothing = 5;
		};

		constant weapon_data
			basic,
			shotgun,
			sniper,
			smg,
			dmr,
			pistol,
			rifle;
	};

	struct triggerbot
	{
		constant bool enabled = false;
		constant bool always_on = true;
		constant int hotkey = VK_RBUTTON;
		constant float post_delay = 0;
		constant float maximum_distance = 0;
		constant float pre_delay = 0;
		constant bool shotgun_only = true;
	};

	struct radar
	{
		constant bool distance = true;
		constant float size = 350;
		constant bool enabled = false;
		constant int type = 0;
		constant ImVec2 radar_position{ 200, 200 };
	};

	struct environment
	{
		constant bool enabled = true;

		constant bool items = true;

		constant bool common = true;
		constant bool uncommon = true;
		constant bool rare = true;
		constant bool epic = true;
		constant bool legendary = true;
		constant bool mythic = true;

		constant bool chests = true;
		constant bool ammo_containers = true;

		constant float maximum_distance = 100.f;
	};

	struct visuals
	{
		constant bool enabled = true;

		struct skeleton
		{
			constant bool enabled = true;
			constant int type = 1;
			constant float thickness = 1.f;
			constant bool outlined = true;
		};

		struct box
		{
			constant bool enabled = true;
			constant bool filled = true;
			constant bool outlined = true;
			constant float thickness = 1.f;
		};

		constant bool indicators = true;
		constant bool distance = true;
		constant bool team = true;
		constant bool weapon = true;
		constant bool weapon_rarity = true;
		constant bool platform = true;
		constant bool username = true;
		constant float font_size = 20; // tragic asf couldnt add font size 💔

		constant ImColor visible_color = { 0, 255, 255 }, invisible_color = { 255, 0, 0 };

		constant ImColor skeleton_color_visible = { 0, 255, 255 };
		constant ImColor skeleton_color_invisible = { 255, 0, 0 };

		constant ImColor text_color_visible = { 255, 255, 255 };
		constant ImColor text_color_invisible = { 255, 0, 0 };

		constant ImColor fov_color = { 255, 255, 255 };

		constant ImColor knocked_color = { 255, 255, 0 };
	};

	struct misc
	{
		constant bool watermark = true;
		constant bool vsync = true;
		constant bool fps_counter = true;

		constant bool spectator_list = true;
		constant float spectator_list_opacity = 255.f;
		constant ImVec2 spectator_list_pos = ImVec2(50, 50);
	};

	constant void save_config(const std::string& filename)
	{
		config cfg;

		// Aimbot
		{
			cfg.set("a_enabled", settings::aimbot::enabled);
			cfg.set("a_weapon_configs", settings::aimbot::weapon_configs);
			cfg.set("a_prediction", settings::aimbot::prediction);
			cfg.set("a_visible_check", settings::aimbot::visible_check);
			cfg.set("a_knocked_check", settings::aimbot::knocked_check);
			cfg.set("a_render_fov", settings::aimbot::render_fov);
			cfg.set("a_hotkey", settings::aimbot::hotkey);
			cfg.set("a_refresh_rate", settings::aimbot::refresh_rate);

			auto save_weapon = [&](const std::string& prefix, const settings::aimbot::weapon_data& w) {
				cfg.set(prefix + "_hitbox", w.hitbox);
				cfg.set(prefix + "_fov", w.fov);
				cfg.set(prefix + "_smoothing", w.smoothing);
				};

			save_weapon("a_basic", settings::aimbot::basic);
			save_weapon("a_shotgun", settings::aimbot::shotgun);
			save_weapon("a_sniper", settings::aimbot::sniper);
			save_weapon("a_smg", settings::aimbot::smg);
			save_weapon("a_dmr", settings::aimbot::dmr);
			save_weapon("a_pistol", settings::aimbot::pistol);
			save_weapon("a_rifle", settings::aimbot::rifle);
		}

		// Triggerbot
		{
			cfg.set("t_enabled", settings::triggerbot::enabled);
			cfg.set("t_always_on", settings::triggerbot::always_on);
			cfg.set("t_hotkey", settings::triggerbot::hotkey);
			cfg.set("t_post_delay", settings::triggerbot::post_delay);
			cfg.set("t_maximum_distance", settings::triggerbot::maximum_distance);
			cfg.set("t_pre_delay", settings::triggerbot::pre_delay);
			cfg.set("t_shotgun_only", settings::triggerbot::shotgun_only);
		}

		// Radar
		{
			cfg.set("r_enabled", settings::radar::enabled);
			cfg.set("r_type", settings::radar::type);
			cfg.set("r_distance", settings::radar::distance);
			cfg.set("r_size", settings::radar::size);
			cfg.set("r_pos", settings::radar::radar_position);
		}

		// Environment
		{
			cfg.set("e_enabled", settings::environment::enabled);
			cfg.set("e_items", settings::environment::items);
			cfg.set("e_common", settings::environment::common);
			cfg.set("e_uncommon", settings::environment::uncommon);
			cfg.set("e_rare", settings::environment::rare);
			cfg.set("e_epic", settings::environment::epic);
			cfg.set("e_legendary", settings::environment::legendary);
			cfg.set("e_mythic", settings::environment::mythic);
			cfg.set("e_chests", settings::environment::chests);
			cfg.set("e_ammo", settings::environment::ammo_containers);
			cfg.set("e_max_dist", settings::environment::maximum_distance);
		}

		// Visuals
		{
			cfg.set("v_enabled", settings::visuals::enabled);

			cfg.set("v_box_enabled", settings::visuals::box::enabled);
			cfg.set("v_box_outlined", settings::visuals::box::outlined);
			cfg.set("v_box_filled", settings::visuals::box::filled);
			cfg.set("v_box_thickness", settings::visuals::box::thickness);

			cfg.set("v_skeleton_enabled", settings::visuals::skeleton::enabled);
			cfg.set("v_skeleton_outlined", settings::visuals::skeleton::outlined);
			cfg.set("v_skeleton_type", settings::visuals::skeleton::type);
			cfg.set("v_skeleton_thickness", settings::visuals::skeleton::thickness);

			cfg.set("v_distance", settings::visuals::distance);
			cfg.set("v_indicators", settings::visuals::indicators);
			cfg.set("v_platform", settings::visuals::platform);
			cfg.set("v_username", settings::visuals::username);
			cfg.set("v_team", settings::visuals::team);
			cfg.set("v_weapon", settings::visuals::weapon);
			cfg.set("v_weapon_rarity", settings::visuals::weapon_rarity);

			cfg.set("v_font_size", settings::visuals::font_size);

			cfg.set("v_color_visible", settings::visuals::visible_color);
			cfg.set("v_color_invisible", settings::visuals::invisible_color);
			cfg.set("v_color_skeleton_visible", settings::visuals::skeleton_color_visible);
			cfg.set("v_color_skeleton_invisible", settings::visuals::skeleton_color_invisible);
			cfg.set("v_color_text_visible", settings::visuals::text_color_visible);
			cfg.set("v_color_text_invisible", settings::visuals::text_color_invisible);
			cfg.set("v_color_fov", settings::visuals::fov_color);
			cfg.set("v_color_knocked", settings::visuals::knocked_color);
		}

		// Misc
		{
			cfg.set("m_watermark", settings::misc::watermark);
			cfg.set("m_vsync", settings::misc::vsync);
			cfg.set("m_fps_counter", settings::misc::fps_counter);
			cfg.set("m_spec_list", settings::misc::spectator_list);
			cfg.set("m_spec_opacity", settings::misc::spectator_list_opacity);
			cfg.set("m_spec_pos", settings::misc::spectator_list_pos);
		}

		cfg.save_to_file(filename);
	}

	constant void load_config(const std::string& filename = "config.json")
	{
		config cfg;
		if (!cfg.load_from_file(filename)) return;

		// Aimbot
		auto load_weapon = [&](const std::string& prefix, settings::aimbot::weapon_data& w) {
			if (auto val = cfg.get<int>(prefix + "_hitbox")) w.hitbox = *val;
			if (auto val = cfg.get<float>(prefix + "_fov")) w.fov = *val;
			if (auto val = cfg.get<float>(prefix + "_smoothing")) w.smoothing = *val;
			};

		if (auto val = cfg.get<bool>("a_enabled")) settings::aimbot::enabled = *val;
		if (auto val = cfg.get<bool>("a_weapon_configs")) settings::aimbot::weapon_configs = *val;
		if (auto val = cfg.get<bool>("a_prediction")) settings::aimbot::prediction = *val;
		if (auto val = cfg.get<bool>("a_visible_check")) settings::aimbot::visible_check = *val;
		if (auto val = cfg.get<bool>("a_knocked_check")) settings::aimbot::knocked_check = *val;
		if (auto val = cfg.get<bool>("a_render_fov")) settings::aimbot::render_fov = *val;
		if (auto val = cfg.get<int>("a_hotkey")) settings::aimbot::hotkey = *val;
		if (auto val = cfg.get<float>("a_refresh_rate")) settings::aimbot::refresh_rate = *val;

		load_weapon("a_basic", settings::aimbot::basic);
		load_weapon("a_shotgun", settings::aimbot::shotgun);
		load_weapon("a_sniper", settings::aimbot::sniper);
		load_weapon("a_smg", settings::aimbot::smg);
		load_weapon("a_dmr", settings::aimbot::dmr);
		load_weapon("a_pistol", settings::aimbot::pistol);
		load_weapon("a_rifle", settings::aimbot::rifle);

		// Triggerbot
		if (auto val = cfg.get<bool>("t_enabled")) settings::triggerbot::enabled = *val;
		if (auto val = cfg.get<bool>("t_always_on")) settings::triggerbot::always_on = *val;
		if (auto val = cfg.get<int>("t_hotkey")) settings::triggerbot::hotkey = *val;
		if (auto val = cfg.get<float>("t_post_delay")) settings::triggerbot::post_delay = *val;
		if (auto val = cfg.get<float>("t_maximum_distance")) settings::triggerbot::maximum_distance = *val;
		if (auto val = cfg.get<float>("t_pre_delay")) settings::triggerbot::pre_delay = *val;
		if (auto val = cfg.get<bool>("t_shotgun_only")) settings::triggerbot::shotgun_only = *val;

		// Radar
		if (auto val = cfg.get<bool>("r_enabled")) settings::radar::enabled = *val;
		if (auto val = cfg.get<int>("r_type")) settings::radar::type = *val;
		if (auto val = cfg.get<bool>("r_distance")) settings::radar::distance = *val;
		if (auto val = cfg.get<float>("r_size")) settings::radar::size = *val;
		if (auto val = cfg.get<ImVec2>("r_pos")) settings::radar::radar_position = *val;

		// Environment
		if (auto val = cfg.get<bool>("e_enabled")) settings::environment::enabled = *val;
		if (auto val = cfg.get<bool>("e_items")) settings::environment::items = *val;
		if (auto val = cfg.get<bool>("e_common")) settings::environment::common = *val;
		if (auto val = cfg.get<bool>("e_uncommon")) settings::environment::uncommon = *val;
		if (auto val = cfg.get<bool>("e_rare")) settings::environment::rare = *val;
		if (auto val = cfg.get<bool>("e_epic")) settings::environment::epic = *val;
		if (auto val = cfg.get<bool>("e_legendary")) settings::environment::legendary = *val;
		if (auto val = cfg.get<bool>("e_mythic")) settings::environment::mythic = *val;
		if (auto val = cfg.get<bool>("e_chests")) settings::environment::chests = *val;
		if (auto val = cfg.get<bool>("e_ammo")) settings::environment::ammo_containers = *val;
		if (auto val = cfg.get<float>("e_max_dist")) settings::environment::maximum_distance = *val;

		// Visuals
		if (auto val = cfg.get<bool>("v_enabled")) settings::visuals::enabled = *val;

		// 🔄 Box
		if (auto val = cfg.get<bool>("v_box_enabled")) settings::visuals::box::enabled = *val;
		if (auto val = cfg.get<bool>("v_box_outlined")) settings::visuals::box::outlined = *val;
		if (auto val = cfg.get<bool>("v_box_filled")) settings::visuals::box::filled = *val;
		if (auto val = cfg.get<float>("v_box_thickness")) settings::visuals::box::thickness = *val;

		// 🔄 Skeleton
		if (auto val = cfg.get<bool>("v_skeleton_enabled")) settings::visuals::skeleton::enabled = *val;
		if (auto val = cfg.get<bool>("v_skeleton_outlined")) settings::visuals::skeleton::outlined = *val;
		if (auto val = cfg.get<int>("v_skeleton_type")) settings::visuals::skeleton::type = *val;
		if (auto val = cfg.get<float>("v_skeleton_thickness")) settings::visuals::skeleton::thickness = *val;

		if (auto val = cfg.get<bool>("v_distance")) settings::visuals::distance = *val;
		if (auto val = cfg.get<bool>("v_indicators")) settings::visuals::indicators = *val;
		if (auto val = cfg.get<bool>("v_platform")) settings::visuals::platform = *val;
		if (auto val = cfg.get<bool>("v_username")) settings::visuals::username = *val;
		if (auto val = cfg.get<bool>("v_team")) settings::visuals::team = *val;
		if (auto val = cfg.get<bool>("v_weapon")) settings::visuals::weapon = *val;
		if (auto val = cfg.get<bool>("v_weapon_rarity")) settings::visuals::weapon_rarity = *val;

		if (auto val = cfg.get<float>("v_font_size")) settings::visuals::font_size = *val;

		if (auto val = cfg.get<ImColor>("v_color_visible")) settings::visuals::visible_color = *val;
		if (auto val = cfg.get<ImColor>("v_color_invisible")) settings::visuals::invisible_color = *val;
		if (auto val = cfg.get<ImColor>("v_color_skeleton_visible")) settings::visuals::skeleton_color_visible = *val;
		if (auto val = cfg.get<ImColor>("v_color_skeleton_invisible")) settings::visuals::skeleton_color_invisible = *val;
		if (auto val = cfg.get<ImColor>("v_color_text_visible")) settings::visuals::text_color_visible = *val;
		if (auto val = cfg.get<ImColor>("v_color_text_invisible")) settings::visuals::text_color_invisible = *val;
		if (auto val = cfg.get<ImColor>("v_color_fov")) settings::visuals::fov_color = *val;
		if (auto val = cfg.get<ImColor>("v_color_knocked")) settings::visuals::knocked_color = *val;

		// Misc
		if (auto val = cfg.get<bool>("m_watermark")) settings::misc::watermark = *val;
		if (auto val = cfg.get<bool>("m_vsync")) settings::misc::vsync = *val;
		if (auto val = cfg.get<bool>("m_fps_counter")) settings::misc::fps_counter = *val;
		if (auto val = cfg.get<bool>("m_spec_list")) settings::misc::spectator_list = *val;
		if (auto val = cfg.get<float>("m_spec_opacity")) settings::misc::spectator_list_opacity = *val;
		if (auto val = cfg.get<ImVec2>("m_spec_pos")) settings::misc::spectator_list_pos = *val;
	}
};

struct offsets
{
	static constexpr int gworld = 0x18540340; // GWorld
	static constexpr int location_pointer = 0x170; // GWorld
	static constexpr int game_instance = 0x248; // OwningGameInstance
	static constexpr int game_state = 0x1d0; // GameState
	static constexpr int server_world_time = 0x1A8; // ServerWorldTimeSecondsDelta
	static constexpr int local_player = 0x38; // LocalPlayer
	static constexpr int player_controller = 0x30;  // PlayerController
	static constexpr int acknowledged_pawn = 0x350; // AcknowledgedPawn
	static constexpr int player_state = 0x2b0; // PlayerState
	static constexpr int team_index = 0x1271; // TeamIndex
	static constexpr int current_weapon = 0xaa8; // CurrentWeapon
	static constexpr int weapon_data = 0x558; // WeaponData
	static constexpr int projectile_speed = 0x2264;
	static constexpr int projectile_gravity = 0x2268;
	static constexpr int targeted_fort_pawn = 0x1990; // TargetedFortPawn
	static constexpr int last_update_velocity = 0x378; // TargetedFortPawn

	static constexpr int player_array = 0x2c0; // PlayerArray
	static constexpr int pawn_private = 0x320; // PawnPrivate
	static constexpr int skeletal_mesh = 0x328; // Mesh
	static constexpr int character_movement = skeletal_mesh + 0x8; // CharacterMovement
	static constexpr int bone_array = 0x5E8; // ComponentSpaceTransformsArray
	static constexpr int component_to_world = 0x1E0; // ComponentToWorld
	static constexpr int bIsDying = 0x728; // BIsDying Deadcheck
	static constexpr int bIsDbno = 0x98a; // bIsDbno downed check
	static constexpr int levels = 0x1e8; // Levels
	static constexpr int actors = 0x48; // Actors
	static constexpr int root_component = 0x1B0; // RootComponent
	static constexpr int relative_location = 0x138; // RootComponent
};
