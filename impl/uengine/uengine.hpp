#pragma once

class uengine
{
public:

	struct camera
	{
		math::fvector location, rotation;
		math::fmatrix rotational_matrix;
		float field_of_view;

		void update();
	} s_camera;

	struct wscreen
	{
		math::fvector2d get_screen_location(math::fvector world_location);
	}s_wscreen;

	struct player
	{
		bool get_visibility(fortnite::player& player);
		bool get_transforms(fortnite::player& player);
		bool get_weapon(fortnite::player& player);
		std::string get_weapon( uintptr_t pawn );
		std::string get_username(fortnite::player& player);
		std::string get_username(uintptr_t& player_state);
		std::string get_platform(fortnite::player& player);
		math::fvector get_bone_location(fortnite::player& player, int bone_id);
	} s_player;

}inline c_uengine;