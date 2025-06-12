#include "general.hpp"

void triggerbot::go()
{
	if (!settings::triggerbot::enabled || !settings::triggerbot::always_on && !GetAsyncKeyState( settings::triggerbot::hotkey ) || settings::triggerbot::shotgun_only && g_engine.current_weapon_type != updater::weapon_type::shotgun)
		return;
	
	if (!g_kernel.read_memory<uintptr_t>(g_engine.player_controller + 0x18C0))
		return;
		
	static bool ready_to_shoot = false;
	static auto shoot_start_time = std::chrono::steady_clock::now();

	if (!ready_to_shoot)
	{
		ready_to_shoot = true;
		shoot_start_time = std::chrono::steady_clock::now();
		return;
	}

	auto now = std::chrono::steady_clock::now();
	auto pre_shoot_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - shoot_start_time).count();

	if (pre_shoot_elapsed < settings::triggerbot::pre_delay
		) return;

	if (has_clicked)
	{
		begin = now;
		has_clicked = 0;
	}

	auto time_since = std::chrono::duration_cast<std::chrono::milliseconds>( now - begin ).count();

	if (time_since >= settings::triggerbot::post_delay)
	{
		c_input.click();
		has_clicked = 1;
		ready_to_shoot = false;
	}
}