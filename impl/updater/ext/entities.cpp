#include "general.hpp"

void updater::entities_t::update()
{
	auto player_array = g_kernel.read_memory<fortnite::tarray<uintptr_t>>(g_engine.game_state + offsets::player_array);
	if (!player_array.count || !player_array.data || player_array.count > 110) return;

	auto player_array_extracted = player_array.fetch_data();
	if (player_array_extracted.empty()) return;

	if (!player_array_extracted.size() || !player_array_extracted.data() || player_array_extracted.size() > 110) return;

	auto tmp_player_list = std::make_shared<std::vector<fortnite::player>>();

	tmp_player_list->reserve(player_array_extracted.size());

	for (auto& player_state : player_array_extracted)
	{
		if (!player_state) continue;

		auto pawn_private = g_kernel.read_memory<uintptr_t>(player_state + offsets::pawn_private);
		if (!pawn_private || pawn_private == g_engine.acknowledged_pawn) continue;

		auto skeletal_mesh = g_kernel.read_memory<uintptr_t>(pawn_private + offsets::skeletal_mesh);
		if (!skeletal_mesh) continue;

		auto character_movement = g_kernel.read_memory<uintptr_t>(pawn_private + 0x1B0);
		if (!character_movement) continue;

		auto team_index = g_kernel.read_memory<int>(player_state + offsets::team_index);
		if (!team_index || team_index == g_engine.team_index) continue;

		fortnite::player s_player =
		{
			.player_state = player_state,
			.pawn_private = pawn_private,
			.skeletal_mesh = skeletal_mesh,
			.character_movement = character_movement,
			.team_index = team_index,
			.username = g_player.get_username(s_player),
			.platform = g_player.get_platform(s_player),
			.visibility = g_player.get_visibility(s_player),
			.knocked = (g_kernel.read_memory<uint8_t>(pawn_private + offsets::bIsDbno) >> 7) & 1
		};

		g_player.get_weapon(s_player);

		tmp_player_list->push_back(s_player);
	}

	player_list.store(tmp_player_list);
}

void updater::entities_t::tick()
{
	auto list_ptr = player_list.load();
	if (!list_ptr || list_ptr->empty())
		return;

	auto modified_list = std::make_shared<std::vector<fortnite::player>>(*list_ptr);

	for (auto it = modified_list->begin(); it != modified_list->end(); )
	{
		auto& player = *it;

		if (!player.pawn_private)
		{
			it = modified_list->erase(it);
			continue;
		}

		bool is_dying = (g_kernel.read_memory<uint8_t>(player.pawn_private + offsets::bIsDying) >> 5) & 1;

		if (g_engine.server_world_time != 0.0f && is_dying) 
		{
			it = modified_list->erase(it);
			continue;
		}

		g_player.get_weapon(player);
		player.visibility = g_player.get_visibility(player);
		player.knocked = (g_kernel.read_memory<uint8_t>(player.pawn_private + offsets::bIsDbno) >> 7) & 1;

		++it;
	}

	player_list.store(modified_list);
}