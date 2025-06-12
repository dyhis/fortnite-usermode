#include "general.hpp"
#include <cmath>
#include <string>
#include <vector>
constexpr ImColor white{1.0f, 1.0f, 1.0f, 1.0f};

static auto last_aimbot_time = std::chrono::high_resolution_clock::now();

inline void draw_filled_box( ImDrawList* drawlist, float x, float y, float w, float h,
	ImU32 outline_color, float thickness = 1.0f,
	ImColor top_color = ImColor( 0, 0, 0, 60 ),
	ImColor bottom_color = ImColor( 0, 0, 0, 180 ) )
{
	if (w <= 0.0f || h <= 0.0f || !drawlist)
		return;

	if (w > 2000 || h > 2000)
		return;

	ImVec2 top_left( x, y );
	ImVec2 bottom_right( x + w, y + h );

	drawlist->AddRectFilledMultiColor(
		top_left,
		bottom_right,
		top_color,
		top_color,
		bottom_color,
		bottom_color
	);

	drawlist->AddRect( top_left, bottom_right, outline_color, 0.0f, 0, thickness );
}


void add_layout( ImDrawList* drawlist )
{
	static ImVec2 radar_position = settings::radar::radar_position;
	static bool is_dragging = false;

	float min_radar_size = 100.0f;
	float max_radar_size = 600.0f;

	float& radar_size = settings::radar::size;

	ImGui::SetNextWindowPos( radar_position, ImGuiCond_Always );
	ImGui::SetNextWindowSize( ImVec2( radar_size, radar_size ), ImGuiCond_Always );

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoResize;

	if (ImGui::Begin( "Radar", nullptr, window_flags ))
	{
		if (c_menu.rendering)
		{
			if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked( 0 ))
			{
				is_dragging = true;
			}

			if (is_dragging && ImGui::IsMouseDown( 0 ))
			{
				ImVec2 delta = ImGui::GetIO().MouseDelta;
				radar_position.x += delta.x;
				radar_position.y += delta.y;
				ImGui::SetWindowPos( radar_position );
				settings::radar::radar_position = radar_position;
			}
			else if (!ImGui::IsMouseDown( 0 ))
			{
				is_dragging = false;
			}

			if (ImGui::IsWindowHovered() && ImGui::GetIO().MouseWheel != 0)
			{
				float scroll_delta = ImGui::GetIO().MouseWheel * 20.0f;
				radar_size = std::clamp( radar_size + scroll_delta, min_radar_size, max_radar_size );
				settings::radar::size = radar_size;
			}
		}

		radar_position = ImGui::GetWindowPos();
		settings::radar::radar_position = radar_position;

		ImVec2 rect_top_left = ImGui::GetWindowPos();
		ImVec2 rect_bottom_right = {rect_top_left.x + radar_size, rect_top_left.y + radar_size};
		ImVec2 center = {rect_top_left.x + radar_size / 2, rect_top_left.y + radar_size / 2};

		drawlist->PushClipRect( rect_top_left, rect_bottom_right, true );

		if (settings::radar::type == 0)
		{
			ImColor circle_bg_color( 0, 0, 0, 200 );
			drawlist->AddCircleFilled( center, radar_size / 2.0f, circle_bg_color, 64 );

			ImColor outline_color( 30, 30, 30, 255 );
			drawlist->AddCircle( center, radar_size / 2.0f, outline_color, 64, 1.0f );

			for (float r = radar_size / 10.0f; r < radar_size / 2.0f; r += radar_size / 10.0f)
			{
				drawlist->AddCircle( center, r, ImColor( 60, 60, 60, 220 ), 64, 1.5f );
			}

			float marker_scale_circle = radar_size * 0.015f;

			ImVec2 p1 = ImVec2( center.x, center.y - marker_scale_circle );             // Top
			ImVec2 p2 = ImVec2( center.x - marker_scale_circle, center.y + marker_scale_circle ); // Bottom left
			ImVec2 p3 = ImVec2( center.x + marker_scale_circle, center.y + marker_scale_circle ); // Bottom right

			drawlist->AddTriangleFilled( p1, p2, p3, ImColor( 255, 255, 255, 255 ) );
		}
		else if (settings::radar::type == 1)
		{

			ImColor square_bg_color( 0, 0, 0, 200 );
			drawlist->AddRectFilled( rect_top_left, rect_bottom_right, square_bg_color );

			ImColor outline_color( 30, 30, 30, 255 );
			drawlist->AddRect( rect_top_left, rect_bottom_right, outline_color, 0.0f, 0, 1.0f );

			float grid_spacing = radar_size / 10.0f;
			ImColor grid_color( 60, 60, 60, 220 );

			for (float y = grid_spacing; y < radar_size; y += grid_spacing)
			{
				drawlist->AddLine(
					ImVec2( rect_top_left.x, rect_top_left.y + y ),
					ImVec2( rect_bottom_right.x, rect_top_left.y + y ),
					grid_color, 1.5f );
			}

			for (float x = grid_spacing; x < radar_size; x += grid_spacing)
			{
				drawlist->AddLine(
					ImVec2( rect_top_left.x + x, rect_top_left.y ),
					ImVec2( rect_top_left.x + x, rect_bottom_right.y ),
					grid_color, 1.5f );
			}

			ImColor crosshair_color( 80, 80, 80, 240 );
			drawlist->AddLine( {center.x, rect_top_left.y}, {center.x, rect_bottom_right.y}, crosshair_color, 2.0f );
			drawlist->AddLine( {rect_top_left.x, center.y}, {rect_bottom_right.x, center.y}, crosshair_color, 2.0f );

			drawlist->AddLine(
				ImVec2( rect_top_left.x, rect_top_left.y ),
				ImVec2( rect_bottom_right.x, rect_bottom_right.y ),
				grid_color, 1.5f );
			drawlist->AddLine(
				ImVec2( rect_bottom_right.x, rect_top_left.y ),
				ImVec2( rect_top_left.x, rect_bottom_right.y ),
				grid_color, 1.5f );

			float marker_scale = radar_size * 0.015f;

			ImVec2 p1 = ImVec2( center.x, center.y - marker_scale );             // Top
			ImVec2 p2 = ImVec2( center.x - marker_scale, center.y + marker_scale ); // Bottom left
			ImVec2 p3 = ImVec2( center.x + marker_scale, center.y + marker_scale ); // Bottom right

			drawlist->AddTriangleFilled( p1, p2, p3, ImColor( 255, 255, 255, 255 ) );
		}

		drawlist->PopClipRect();
		ImGui::End();
	}
}

static void range_to( float* x, float* y, float range )
{
	if (fabs( (*x) ) > range || fabs( (*y) ) > range)
	{
		if ((*y) > (*x))
		{
			if ((*y) > -(*x))
			{
				(*x) = range * (*x) / (*y);
				(*y) = range;
			}
			else
			{
				(*y) = -range * (*y) / (*x);
				(*x) = -range;
			}
		}
		else
		{
			if ((*y) > -(*x))
			{
				(*y) = range * (*y) / (*x);
				(*x) = range;
			}
			else
			{
				(*x) = -range * (*x) / (*y);
				(*y) = -range;
			}
		}
	}
}

static math::fvector2d calculate_radar_point( math::fvector location )
{
	const math::fvector camera_rotation = g_camera.rotation;
	const float yaw = camera_rotation.y * static_cast<float>(3.14) / 180.0f;

	const float delta_x = location.x - g_camera.location.x;
	const float delta_y = location.y - g_camera.location.y;

	const float sin_yaw = sinf( yaw );
	const float cos_yaw = cosf( yaw );

	float x = delta_y * -cos_yaw + delta_x * sin_yaw;
	x = -x;
	float y = delta_x * -cos_yaw - delta_y * sin_yaw;

	constexpr float range = 10.0f * 1000.0f;
	range_to( &x, &y, range );

	ImVec2 radar_pos = settings::radar::radar_position;
	float radar_size_x = static_cast<float>(settings::radar::size);
	float radar_size_y = static_cast<float>(settings::radar::size);

	ImVec2 radar_center = ImVec2( radar_pos.x + radar_size_x / 2, radar_pos.y + radar_size_y / 2 );

	float screen_x = radar_center.x + (x / range * radar_size_x * 0.5f);
	float screen_y = radar_center.y + (y / range * radar_size_y * 0.5f);

	// For circular radar, we need to ensure points stay within the circle
	if (settings::radar::type == 0)
	{
		// Calculate distance from center to point
		float dx = screen_x - radar_center.x;
		float dy = screen_y - radar_center.y;
		float distance = std::sqrt( dx * dx + dy * dy );

		// If outside the circle radius, scale it back
		float max_radius = (radar_size_x / 2.0f) - 5.0f;
		if (distance > max_radius)
		{
			// Scale to the edge of the circle
			float scale_factor = max_radius / distance;
			screen_x = radar_center.x + dx * scale_factor;
			screen_y = radar_center.y + dy * scale_factor;
		}
	}
	else // Square radar - use original clamping logic
	{
		const int radar_min_x = radar_pos.x;
		const int radar_min_y = radar_pos.y;
		const int radar_max_x = radar_pos.x + radar_size_x - 5;
		const int radar_max_y = radar_pos.y + radar_size_y - 5;

		screen_x = std::clamp( screen_x, static_cast<float>(radar_min_x), static_cast<float>(radar_max_x) );
		screen_y = std::clamp( screen_y, static_cast<float>(radar_min_y), static_cast<float>(radar_max_y) );
	}

	return math::fvector2d( screen_x, screen_y );
}

void add_actors( ImDrawList* drawlist, math::fvector location, int id, const std::string& username, ImColor color )
{
	if (!settings::radar::enabled)
		return;

	auto location_to_screen = calculate_radar_point( location );

	float actor_base_size = 4.5f;
	float scale_factor = settings::radar::size / 300.0f;
	float actor_size = std::max( actor_base_size * scale_factor, 3.0f );

	drawlist->AddCircleFilled(
		ImVec2( location_to_screen.x, location_to_screen.y ),
		actor_size,
		color,
		20
	);

	if (settings::radar::distance)
	{
		// Calculate distance from camera to actor
		int distance = static_cast<int>(g_camera.location.distance(location)) / 100;
		std::string distance_text = std::to_string(distance) + "m";
		auto distance_text_size = ImGui::CalcTextSize(distance_text.c_str());

		if (!distance_text.empty())
		{
			ImVec2 text_pos = ImVec2(
				location_to_screen.x - distance_text_size.x / 2.0f,           // Center horizontally
				location_to_screen.y + actor_size + 2.0f                      // Position just below the dot
			);

			// Smaller faded-out text
			drawlist->AddText(
				NULL,
				ImGui::GetFontSize() * 0.75f, // 75% of normal size
				text_pos,
				ImColor(180, 180, 180, 160),  // faded grey
				distance_text.c_str()
			);
		}
	}
}
void  oof_arrows( ImDrawList* drawlist, const math::fvector2d& location_projected, const ImColor& col )
{
	const math::fvector2d center( globals::screen_width * 0.5f, globals::screen_height * 0.5f );
	const math::fvector2d enemy_pos( location_projected.x, location_projected.y );

	const auto angle = std::atan2( enemy_pos.y - center.y, enemy_pos.x - center.x );

	const auto triangle_center = center + math::fvector2d::from_angle( angle, aimbot::fov );

	const auto p1 = triangle_center + math::fvector2d::from_angle( angle, 25.0f );
	const auto p2 = triangle_center + math::fvector2d::from_angle( angle + (std::numbers::pi / 3.0f), 20.0f );
	const auto p3 = triangle_center + math::fvector2d::from_angle( angle - (std::numbers::pi / 3.0f), 20.0f );

	drawlist->AddTriangleFilled( ImVec2( p1.x, p1.y ), ImVec2( p2.x, p2.y ), ImVec2( p3.x, p3.y ), col );

	drawlist->AddTriangle( ImVec2( p1.x, p1.y ), ImVec2( p2.x, p2.y ), ImVec2( p3.x, p3.y ), ImColor(255, 255, 255) );
}

void visuals::render()
{
	g_camera.update();

	auto aimbot_pwn = fortnite::player();
	auto aimbot_pwn_distance = DBL_MAX;
	auto drawlist = ImGui::GetBackgroundDrawList();

	auto adjusted_field_of_view = settings::aimbot::dynamic_fov ? (aimbot::fov / 90) * g_camera.field_of_view : aimbot::fov;

	if (settings::aimbot::render_fov)
	{
		drawlist->AddCircle( ImVec2( globals::screen_width_center, globals::screen_height_center ), aimbot::fov, settings::visuals::fov_color, 64, 1.5f );
	}

	if (settings::misc::spectator_list)
	{
		static ImVec2 spectator_list_position = ImVec2(50, 50);
		static bool is_dragging = false;
		static ImVec2 list_size = ImVec2(200, 200);

		ImGui::SetNextWindowPos(spectator_list_position, ImGuiCond_Always);
		ImGui::SetNextWindowSize(list_size, ImGuiCond_Always);

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground;

		drawlist->AddRectFilled(settings::misc::spectator_list_pos, settings::misc::spectator_list_pos + list_size, ImColor(25.f / 255.f, 25.f / 255.f, 25.f / 255.f, settings::misc::spectator_list_opacity / 255.f));

		if (ImGui::Begin("Spectator Window", nullptr, window_flags))
		{
			if (c_menu.rendering)
			{
				if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && ImGui::IsMouseClicked(0))
				{
					is_dragging = true;
				}

				if (is_dragging && ImGui::IsMouseDown(0))
				{
					ImVec2 delta = ImGui::GetIO().MouseDelta;
					spectator_list_position.x += delta.x;
					spectator_list_position.y += delta.y;
					settings::misc::spectator_list_pos = { spectator_list_position.x, spectator_list_position.y };
				}
				else if (!ImGui::IsMouseDown(0))
				{
					is_dragging = false;
				}
			}

			auto window_size = ImGui::GetWindowSize();

			std::string title_str = "Spectator List";
			ImVec2 title_size = ImGui::CalcTextSize(title_str.c_str());

			ImGui::SetCursorPos(ImVec2((window_size.x / 2) - title_size.x / 2, 4));

			ImGui::Text(title_str.c_str());

			ImGui::Separator();

			auto spectator_list = g_engine.spectators.load();
			if (spectator_list != nullptr)
			{
				for (auto& spectator : *spectator_list)
				{
					ImVec2 text_size = ImGui::CalcTextSize(spectator.c_str());
					ImGui::SetCursorPosX((window_size.x - text_size.x) / 2);
					ImGui::Text(spectator.c_str());
				}
			}
		}
		ImGui::End();
	}

	if (settings::radar::enabled)
	{
		add_layout( drawlist );
	}

	auto player_list_ = g_entities.player_list.load();
	if (!player_list_) return;

	auto player_list__ = *player_list_;

	for (auto& player : *player_list_)
	{
		if (!g_player.get_transforms( player )) continue;

		auto box_esp_color = player.visibility ? settings::visuals::visible_color : settings::visuals::invisible_color;
		auto skeleton_esp_color = player.visibility ? settings::visuals::skeleton_color_visible : settings::visuals::skeleton_color_invisible;
		auto text_esp_color = player.visibility ? settings::visuals::text_color_visible : settings::visuals::text_color_invisible;

		if (player.knocked)
		{
			box_esp_color = settings::visuals::knocked_color;
			skeleton_esp_color = settings::visuals::knocked_color;
			text_esp_color = settings::visuals::knocked_color;
		}

		auto head_location = g_player.get_bone_location( player, 110 );
		auto root_location = c_uengine.s_player.get_bone_location( player, 0 );

		if (!head_location.is_valid() || !root_location.is_valid())
			continue;

		auto head_screen_location = g_wscreen.get_screen_location( head_location );
		auto root_screen_location = c_uengine.s_wscreen.get_screen_location( root_location );

		if (!head_screen_location.is_valid() || !root_screen_location.is_valid())
			continue;

		auto head_adjusted_screen_location = g_wscreen.get_screen_location( math::fvector{head_location.x, head_location.y, head_location.z + 20} );

		auto distance = c_uengine.s_camera.location.distance( root_location ) / 100;

		add_actors( drawlist, head_location, player.team_index, player.username, text_esp_color );

		if (settings::visuals::enabled)
		{
			const std::vector<int> bone_indices = {110, 71, 78, 72, 79, 83, 76, 9, 38, 33, 62};
			std::vector<math::fvector2d> bone_screen_coords( bone_indices.size() );

			std::transform( bone_indices.begin(), bone_indices.end(), bone_screen_coords.begin(), [&]( int index ) {
				return c_uengine.s_wscreen.get_screen_location(
					c_uengine.s_player.get_bone_location( player, index ) );
				} );

			auto [min_x, max_x] = std::minmax_element( bone_screen_coords.begin(), bone_screen_coords.end(), []( const auto& a, const auto& b ) { return a.x < b.x; } );
			auto [min_y, max_y] = std::minmax_element( bone_screen_coords.begin(), bone_screen_coords.end(), []( const auto& a, const auto& b ) { return a.y < b.y; } );

			const auto box_height = max_y->y - min_y->y;
			const auto box_width = max_x->x - min_x->x;

			const auto width_offset = box_width * 0.175f;
			const auto height_offset_top = box_height * 0.125f;
			const auto height_offset_bottom = box_height * 0.05f;

			auto box_min_x = min_x->x - width_offset;
			auto box_max_x = max_x->x + width_offset;
			auto box_min_y = min_y->y - height_offset_top;
			auto box_max_y = max_y->y + height_offset_bottom;

			int top_offset = 20, bottom_offset = 0;
		
			if (settings::visuals::box::enabled)
			{
				if (head_screen_location.x < globals::screen_width && head_screen_location.x > 0 && head_screen_location.y < globals::screen_height && head_screen_location.y > 0)
				{
					if (settings::visuals::box::outlined)
					{
						drawlist->AddRect(ImVec2(box_min_x - settings::visuals::box::thickness, box_min_y - settings::visuals::box::thickness), ImVec2(box_max_x + settings::visuals::box::thickness, box_max_y + settings::visuals::box::thickness), ImColor(255, 255, 255), 0, 0, settings::visuals::box::thickness);
						drawlist->AddRect(ImVec2(box_min_x + settings::visuals::box::thickness, box_min_y + settings::visuals::box::thickness), ImVec2(box_max_x - settings::visuals::box::thickness, box_max_y - settings::visuals::box::thickness), ImColor(255, 255, 255), 0, 0, settings::visuals::box::thickness);
					}

					if (settings::visuals::box::filled)
					{
						auto vec4_color = box_esp_color.Value;
						drawlist->AddRectFilled(ImVec2(box_min_x, box_min_y), ImVec2(box_max_x, box_max_y), ImColor(int(vec4_color.x * 255), int(vec4_color.y * 255), int(vec4_color.z * 255), int(80)));
					}

					drawlist->AddRect(ImVec2(box_min_x, box_min_y), ImVec2(box_max_x, box_max_y), box_esp_color, 0, 0, settings::visuals::box::thickness);
				}
			}

			if (settings::visuals::skeleton::enabled)
			{
				std::vector<std::pair<int, int>> using_bone_pairs;

				if (settings::visuals::skeleton::type == 0)
				{
					using_bone_pairs = {
						{110, 66},
						{66, 7},
						{7, 9},
						{7, 38},
						{9, 10},
						{38, 39},
						{10, 11},
						{39, 40},
						{7, 2},
						{2, 78},
						{2, 71},
						{78, 79},
						{79, 82},
						{82, 83},
						{71, 72},
						{72, 75},
						{75, 76}
					};
				}
				else if (settings::visuals::skeleton::type == 1)
				{
					using_bone_pairs = {
						//{7, 2},
						{2, 66},
						{66, 9},
						{66, 38},
						{9, 10},
						{38, 39},
						{10, 11},
						{39, 40},
						{2, 78},
						{78, 79},
						{79, 82},
						{82, 83},
						{2, 71},
						{71, 72},
						{72, 75},
						{75, 76},
						{110, 66}
					};
				}
				else if (settings::visuals::skeleton::type == 2)
				{
					using_bone_pairs = {
						/*{7, 2},*/
						{2, 66},
						{66, 10},
						{66, 39}, //
						{10, 11},
						{39, 40}, //
						{2, 78},
						{78, 82}, //
						{2, 71},
						{71, 75}  //
					};
				}

				for (auto pair : using_bone_pairs)
				{
					auto a_s_location = c_uengine.s_wscreen.get_screen_location( c_uengine.s_player.get_bone_location( player, pair.first ) );
					auto b_s_location = c_uengine.s_wscreen.get_screen_location( c_uengine.s_player.get_bone_location( player, pair.second ) );

					if (settings::visuals::skeleton::outlined)
						drawlist->AddLine( ImVec2( a_s_location.x, a_s_location.y ), ImVec2( b_s_location.x, b_s_location.y ), ImColor( 255, 255, 255 ), settings::visuals::skeleton::thickness + 1 );

					drawlist->AddLine( ImVec2( a_s_location.x, a_s_location.y ), ImVec2( b_s_location.x, b_s_location.y ), skeleton_esp_color, settings::visuals::skeleton::thickness );
				}
			}

			if (settings::visuals::username && settings::visuals::platform)
			{
				auto string = std::string( player.username );
				if (!player.platform.empty())
					string += " [" + player.platform + "]";

				c_render.text( (box_min_x + box_max_x) / 2, box_min_y - top_offset, text_esp_color, string.c_str() );
				top_offset += ImGui::CalcTextSize( string.c_str() ).y - 2;
			}
			else if (settings::visuals::username || settings::visuals::platform)
			{
				auto string = std::string( settings::visuals::username ? player.username : player.platform );
				c_render.text( (box_min_x + box_max_x) / 2, box_min_y - top_offset, text_esp_color, string.c_str() );
				top_offset += ImGui::CalcTextSize( string.c_str() ).y - 2;
			}

			if (settings::visuals::distance)
			{
				auto string = std::string( std::to_string( int( distance ) ) + "M" );
				c_render.text( (box_min_x + box_max_x) / 2, box_max_y + bottom_offset, text_esp_color, string.c_str() );
				bottom_offset += ImGui::CalcTextSize( string.c_str() ).y - 2;
			}

			if (settings::visuals::team)
			{
				auto string = std::string( "id: " + std::to_string( player.team_index ) );
				c_render.text( (box_min_x + box_max_x) / 2, box_max_y + bottom_offset, text_esp_color, string.c_str() );
				bottom_offset += ImGui::CalcTextSize( string.c_str() ).y - 2;
			}

			if (settings::visuals::weapon)
			{
				auto string = player.weapon;
				c_render.text( (box_min_x + box_max_x) / 2, box_max_y + bottom_offset, settings::visuals::weapon_rarity ? player.weapon_rarity : text_esp_color, string.c_str() );
				bottom_offset += ImGui::CalcTextSize( string.c_str() ).y - 2;
			}
		}

		auto delta_x = head_screen_location.x - globals::screen_width_center;
		auto delta_y = head_screen_location.y - globals::screen_height_center;
		auto distance_squared = delta_x * delta_x + delta_y * delta_y;

		if (distance_squared < aimbot_pwn_distance && sqrt( distance_squared ) < aimbot::fov)
		{
			if (player.visibility || !settings::aimbot::visible_check)
			{
				if (!settings::aimbot::knocked_check || !player.knocked)
				{
					aimbot_pwn_distance = distance_squared;
					aimbot_pwn = player;
				}
			}
		}

		if (settings::visuals::enabled && settings::visuals::indicators && sqrt( distance_squared ) > aimbot::fov)
		{
			auto location_projected_pre = g_wscreen.get_screen_location( g_player.get_bone_location( player, 110 ) );
			oof_arrows( drawlist, location_projected_pre, text_esp_color );
		}
	}

	auto container_list_ = g_enviroment.container_list.load();
	if (!container_list_) return;

	for (auto& container : *container_list_)
	{
		auto screen_pos = g_wscreen.get_screen_location(container.relative_location);
		g_render.text(screen_pos.x, screen_pos.y, container.display_color, container.display_name.c_str());
	}

	g_triggerbot.go();

	auto now = std::chrono::high_resolution_clock::now();
	if (settings::aimbot::enabled && now - last_aimbot_time >= std::chrono::duration<double, std::milli>( 1000.0 / settings::aimbot::refresh_rate ))
	{
		if (aimbot_pwn.skeletal_mesh != 0)
		{
			math::fvector target_location;

			auto estimate_enemy_position = []( const math::fvector& location, const math::fvector& location_velocity, float distance ) -> math::fvector
				{
					auto target = location;
					auto time = distance / g_engine.projectile_speed;

					target.add_scale( location_velocity, time );

					auto gravity = fabs( -980.0f * g_engine.projectile_gravity ) * 0.5f * time * time;
					target.z += gravity;

					return target;
				};

			if (aimbot::hitbox == 0
				) target_location = c_uengine.s_player.get_bone_location( aimbot_pwn, 110 );

			else if (aimbot::hitbox == 1
				) target_location = c_uengine.s_player.get_bone_location( aimbot_pwn, 66 ); // neck

			else if (aimbot::hitbox == 2
				) target_location = c_uengine.s_player.get_bone_location( aimbot_pwn, 7 ); // chest

			else if (aimbot::hitbox == 3
				) target_location = c_uengine.s_player.get_bone_location(aimbot_pwn, 2); // chest

			if (!target_location.is_valid()
				) return;

			if (settings::aimbot::prediction && g_engine.projectile_speed > 5000 && g_engine.projectile_speed < 200000)
			{
				auto distance = g_camera.location.distance( target_location );

				auto root_component = g_kernel.read_memory<uintptr_t>( aimbot_pwn.pawn_private + 0x1B0 );
				auto last_update_velocity = g_kernel.read_memory<math::fvector>( root_component + 0x180 );

				target_location = estimate_enemy_position( target_location, last_update_velocity, distance );
			}

			auto target_screen_location = c_uengine.s_wscreen.get_screen_location( target_location );

			if (!target_screen_location.is_valid()
				) return;


			if (GetAsyncKeyState( settings::aimbot::hotkey ) & 0x8000)
			{
				c_input.move_to( target_screen_location.x, target_screen_location.y, aimbot::smoothing );
			}
		}
		last_aimbot_time = now;
	}
}