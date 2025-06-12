#include "general.hpp"

void updater::engine_t::update()
{
	this->world = g_kernel.read_memory<uintptr_t>(g_kernel.target.process_base + offsets::gworld);
	this->location_pointer = g_kernel.read_memory<uintptr_t>(world + 0x170);
	this->rotation_pointer = g_kernel.read_memory<uintptr_t>(world + 0x180);
	this->game_instance = g_kernel.read_memory<uintptr_t>(world + offsets::game_instance);
	this->game_state = g_kernel.read_memory<uintptr_t>(world + offsets::game_state);
	this->local_player = g_kernel.read_memory<uintptr_t>(g_kernel.read_memory<uintptr_t>(game_instance + offsets::local_player));
	this->player_controller = g_kernel.read_memory<uintptr_t>(local_player + offsets::player_controller);
	this->acknowledged_pawn = g_kernel.read_memory<uintptr_t>(player_controller + offsets::acknowledged_pawn);
	this->player_state = g_kernel.read_memory<uintptr_t>(player_controller + offsets::player_state);
	this->server_world_time = g_kernel.read_memory<float>(game_state + offsets::server_world_time);
	this->team_index = g_kernel.read_memory<int>(player_state + offsets::team_index);
}

void updater::engine_t::tick()
{
	auto determine_weapon_type = [](const std::string& weapon_name) -> weapon_type {
		if (weapon_name.find("Shotgun") != std::string::npos) return weapon_type::shotgun;
		if (weapon_name.find("Sniper") != std::string::npos || weapon_name == "Hunting Rifle") return weapon_type::sniper;
		if (weapon_name.find("Assault") != std::string::npos || weapon_name.find("AR") != std::string::npos) return weapon_type::rifle;
		if (weapon_name.find("DMR") != std::string::npos) return weapon_type::dmr;
		if (weapon_name.find("SMG") != std::string::npos || weapon_name.find("Submachine") != std::string::npos || weapon_name.find("Drum") != std::string::npos) return weapon_type::smg;
		if (weapon_name.find("Pistol") != std::string::npos) return weapon_type::pistol;
		if (weapon_name.find("Pickaxe") != std::string::npos || weapon_name.find("Harvesting Tool") != std::string::npos) return weapon_type::harvesting_tool;
		return weapon_type::unknown;
		};

	this->current_weapon = g_player.get_weapon(acknowledged_pawn);
	this->current_weapon_type = determine_weapon_type(current_weapon);

	if (settings::aimbot::weapon_configs)
	{
		if (current_weapon_type == weapon_type::shotgun)
		{
			aimbot::fov = settings::aimbot::shotgun.fov;
			aimbot::hitbox = settings::aimbot::shotgun.hitbox;
			aimbot::smoothing = settings::aimbot::shotgun.smoothing;
		}
		else if (current_weapon_type == weapon_type::sniper)
		{
			aimbot::fov = settings::aimbot::sniper.fov;
			aimbot::hitbox = settings::aimbot::sniper.hitbox;
			aimbot::smoothing = settings::aimbot::sniper.smoothing;
		}
		else if (current_weapon_type == weapon_type::dmr)
		{
			aimbot::fov = settings::aimbot::dmr.fov;
			aimbot::hitbox = settings::aimbot::dmr.hitbox;
			aimbot::smoothing = settings::aimbot::dmr.smoothing;
		}
		else if (current_weapon_type == weapon_type::rifle)
		{
			aimbot::fov = settings::aimbot::rifle.fov;
			aimbot::hitbox = settings::aimbot::rifle.hitbox;
			aimbot::smoothing = settings::aimbot::rifle.smoothing;
		}
		else if (current_weapon_type == weapon_type::smg)
		{
			aimbot::fov = settings::aimbot::smg.fov;
			aimbot::hitbox = settings::aimbot::smg.hitbox;
			aimbot::smoothing = settings::aimbot::smg.smoothing;
		}
		else if (current_weapon_type == weapon_type::pistol)
		{
			aimbot::fov = settings::aimbot::pistol.fov;
			aimbot::hitbox = settings::aimbot::pistol.hitbox;
			aimbot::smoothing = settings::aimbot::pistol.smoothing;
		}
		else
		{
			aimbot::fov = settings::aimbot::basic.fov;
			aimbot::hitbox = settings::aimbot::basic.hitbox;
			aimbot::smoothing = settings::aimbot::basic.smoothing;
		}
	}
	else
	{
		aimbot::fov = settings::aimbot::basic.fov;
		aimbot::hitbox = settings::aimbot::basic.hitbox;
		aimbot::smoothing = settings::aimbot::basic.smoothing;
	}

	auto weapon_address = g_kernel.read_memory<uintptr_t>(acknowledged_pawn + offsets::current_weapon);
	projectile_gravity = g_kernel.read_memory<float>(weapon_address + offsets::projectile_gravity);

	projectile_speed = g_kernel.read_memory<float>(weapon_address + offsets::projectile_speed);
	if (!projectile_speed)
	{
		projectile_speed = g_kernel.read_memory<float>(weapon_address + 0x1d50);
		if (!projectile_speed)
		{
			projectile_speed = g_kernel.read_memory<float>(weapon_address + 0x2008);
		}
	}

	auto spectator_array = g_kernel.read_memory<fortnite::tarray<uintptr_t>>(player_state + 0xb98 + 0x108);

	constexpr size_t MAX_SPECTATORS = 15;
	if (!spectator_array.data || spectator_array.count == 0 || spectator_array.count > MAX_SPECTATORS)
		return;

	auto tmp_spectator_list = std::make_shared<std::vector<std::string>>();

	tmp_spectator_list->reserve(spectator_array.count);

	for (auto idx = 0; idx < spectator_array.size(); idx++)
	{
		auto player_state = g_kernel.read_memory<uintptr_t>(spectator_array.get_address() + (idx * 0x18) + 0x10);
		if (!player_state)
			continue;

		auto spectator_name = g_player.get_username(player_state);
		if (spectator_name.empty())
			continue;

		tmp_spectator_list->push_back(spectator_name);
	}

	this->spectators.store(tmp_spectator_list);
}
