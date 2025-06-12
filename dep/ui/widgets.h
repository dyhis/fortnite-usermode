#pragma once

inline ImVec2 operator+( const ImVec2& lhs, const ImVec2& rhs )
{
	return ImVec2( lhs.x + rhs.x, lhs.y + rhs.y );
}

struct Combo_State {
	ImVec4 Combo_BG;
	ImVec4 Border_Color;
	ImVec4 Text_Color;
};

namespace colors {
	const ImVec4 bg = {0.03137255f, 0.03137255f, 0.03137255f, 0.95};
	const ImVec4 lbg = {0.04313726f, 0.04313726f, 0.04313726f, 1};

	const ImVec4 SecondColor = ImColor( 2, 80, 173, 255 );
	const ImVec4 MainColor = ImColor( 8, 149, 220, 255 );

	const ImVec4 White = {1, 1, 1, 1};
	const ImVec4 lwhite = {0.75960787f, 0.7556863f, 0.7556863f, 1.0f};
	const ImVec4 Gray = {0.14811765f, 0.14811765f, 0.14811765f, 1.0f};
	const ImVec4 Gray2 = {0.4f, 0.4f, 0.4f, 1.0f};
	const ImVec4 Green = {0.247, 0.6824, 0.247, 1};
	const ImVec4 Red = {0.6824, 0.1608, 0.1608, 1};

	const ImVec4 Transparent = {0., 0., 0., .0f};
}
namespace C1 {
	const ImVec4 c = ImVec4( colors::lbg.x, colors::lbg.y, colors::lbg.z, colors::lbg.w * 2 );
	/*************************************************************************************/
	const ImVec4 BG_Active = c; // Background color when its active
	const ImVec4 BG_Hovered = c; // Background color when its hovered
	const ImVec4 BG = c; // Background color when nothing its happening
	/*************************************************************************************/
	const ImVec4 Text_Active = colors::MainColor; // Text color when its active
	const ImVec4 Text_Hovered = colors::lwhite; // Text color when its hovered
	const ImVec4 Text = colors::Gray2; // Text color when nothing its happening
	/*************************************************************************************/
	const ImVec4 Border_Active = colors::Gray; // Border color when its active
	const ImVec4 Border_Hovered = colors::Gray; // Border color when its hovered
	const ImVec4 Border = colors::Gray; // Border color when nothing is happening
	/*************************************************************************************/
	const float Speed = 10.f; // Animation's speed
	const float Rounding = 3.f; // Animation's speed
	/*************************************************************************************/
}

struct FancyFadeTabs
{
	std::vector<const char*> labels;
	std::vector<std::function<void()>> callbacks;

	int current = 0;
	float animDuration = 0.3f;

	FancyFadeTabs( const std::vector<const char*>& _labels, const std::vector<std::function<void()>>& _callbacks )
		: labels( _labels ), callbacks( _callbacks )
	{
		IM_ASSERT( labels.size() == callbacks.size() );
	}

	void Draw()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImDrawList* draw = ImGui::GetWindowDrawList();
		ImVec2 winSize = ImGui::GetWindowSize();
		float tabHeight = 30.0f;
		float spacingY = 4.0f;
		float totalHeight = tabHeight * labels.size() + spacingY * (labels.size() - 1);
		float startY = (winSize.y - totalHeight) * 0.5f - 20.0f;

		ImGui::BeginGroup();
		ImGui::SetCursorPos( ImVec2( 0.0f, startY ) );
		ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0, 0 ) );

		float cursorY = ImGui::GetCursorPosY();
		for (int i = 0; i < labels.size(); ++i)
		{
			ImGui::PushID( i );
			ImVec2 size( 100, tabHeight );

			ImGui::SetCursorPos( ImVec2( 0.0f, cursorY ) );
			ImGui::InvisibleButton( "##tab", size );
			bool hovered = ImGui::IsItemHovered();
			bool clicked = ImGui::IsItemClicked();

			if (clicked && i != current)
				current = i;

			float baseAlpha = 0.25f;
			if (hovered) baseAlpha = 0.35f;
			if (i == current) baseAlpha = 0.5f;

			ImVec2 min = ImGui::GetItemRectMin();
			ImVec2 max = ImGui::GetItemRectMax();
			draw->AddRectFilledMultiColor(
				min, max,
				ImColor( 45, 45, 45, int( baseAlpha * 20 ) ),
				ImColor( 45, 45, 45, int( baseAlpha * 180 ) ),
				ImColor( 45, 45, 45, int( baseAlpha * 180 ) ),
				ImColor( 45, 45, 45, int( baseAlpha * 20 ) )
			);

			if (i == current)
				draw->AddLine( ImVec2( min.x, max.y ), ImVec2( max.x, max.y ), IM_COL32( 255, 50, 50, 255 ), 2.0f );

			ImVec4 color = (i == current) ? ImVec4( 1, 0.3f, 0.3f, 1 ) : ImVec4( 0.85f, 0.85f, 0.85f, 1 );
			ImVec2 textPos(
				min.x + (size.x - ImGui::CalcTextSize( labels[i] ).x) * 0.5f,
				min.y + (tabHeight - ImGui::GetTextLineHeight()) * 0.5f
			);
			draw->AddText( textPos, ImGui::ColorConvertFloat4ToU32( color ), labels[i] );

			cursorY += size.y + spacingY;
			ImGui::PopID();
		}

		ImGui::PopStyleVar();
		ImGui::EndGroup();

		ImGui::SameLine();
		ImGui::BeginGroup();
		ImGui::SetCursorPosY( 0 );
		ImGui::BeginChild( "##TabContent", ImGui::GetContentRegionAvail(), false );

		callbacks[current]();

		ImGui::EndChild();
		ImGui::EndGroup();
	}
};

namespace customUI
{
	inline float AnimateAlpha( bool target, float& alpha, float dt = 0.1f, float speed = 10.0f )
	{
		float goal = target ? 1.0f : 0.0f;
		alpha += (goal - alpha) * std::min( dt * speed, 1.0f );
		return std::clamp( alpha, 0.0f, 1.0f );
	}

	inline bool checkbox( const char* label, bool* v )
	{
		ImGui::PushID( label );

		ImVec2 boxSize( 16, 16 );
		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImVec2 textSize = ImGui::CalcTextSize( label );
		ImVec2 labelPos( pos.x + boxSize.x + 8, pos.y - 1 );

		ImGui::InvisibleButton( "##checkbox", ImVec2( boxSize.x + 8 + textSize.x, boxSize.y ) );
		bool hovered = ImGui::IsItemHovered();
		bool clicked = ImGui::IsItemClicked();
		if (clicked) *v = !*v;

		ImGuiID id = ImGui::GetItemID();
		static std::unordered_map<ImGuiID, float> anims;
		float& anim = anims[id];
		float target = *v ? 1.0f : (hovered ? 0.4f : 0.0f);
		anim += (target - anim) * 0.2f;

		ImDrawList* draw = ImGui::GetWindowDrawList();
		ImU32 bg = IM_COL32( 25, 25, 25, 255 );
		ImU32 border = IM_COL32( 80, 80, 80, 255 );
		ImU32 fill = IM_COL32( 255, 30, 30, static_cast<int>(anim * 255) );

		draw->AddRectFilled( pos, ImVec2( pos.x + boxSize.x, pos.y + boxSize.y ), bg, 3.0f );
		draw->AddRect( pos, ImVec2( pos.x + boxSize.x, pos.y + boxSize.y ), border, 3.0f );
		if (anim > 0.05f)
			draw->AddRectFilled( pos, ImVec2( pos.x + boxSize.x, pos.y + boxSize.y ), fill, 3.0f );

		draw->AddText( labelPos, IM_COL32( 230, 230, 230, 255 ), label );
		ImGui::PopID();
		return clicked;
	}

	inline bool combo( const char* label, int* current_item, const std::vector<const char*>& items )
	{
		ImGuiID id = ImGui::GetID( label );
		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImVec2 size( 150, 25 );
		ImDrawList* draw = ImGui::GetWindowDrawList();

		ImGui::InvisibleButton( label, size );
		bool hovered = ImGui::IsItemHovered();
		static std::unordered_map<ImGuiID, bool> open_map;

		bool& is_open = open_map[id];
		if (ImGui::IsItemClicked())
			is_open = !is_open;

		// Main combo area
		draw->AddRectFilled( pos, pos + size, IM_COL32( 25, 25, 25, 255 ), 4.0f );
		draw->AddText( pos + ImVec2( 10, 5 ), IM_COL32( 255, 255, 255, 255 ), items[*current_item] );

		ImVec2 arrow = pos + ImVec2( size.x - 14, size.y * 0.5f - 2 );
		draw->AddTriangleFilled( arrow, arrow + ImVec2( 5, 0 ), arrow + ImVec2( 2.5f, 5 ), IM_COL32( 255, 255, 255, 255 ) );

		if (is_open)
		{
			ImVec2 dropdown_pos = pos + ImVec2( 0, size.y + 2 );
			ImVec2 dropdown_max = dropdown_pos + ImVec2( size.x, size.y * items.size() );

			draw->AddRectFilled( dropdown_pos, dropdown_max, IM_COL32( 20, 20, 20, 255 ), 4.0f );
			draw->AddRect( dropdown_pos, dropdown_max, IM_COL32( 35, 35, 35, 255 ), 1.0f );

			for (int i = 0; i < items.size(); ++i)
			{
				ImVec2 item_min = dropdown_pos + ImVec2( 0, size.y * i );
				ImVec2 item_max = item_min + size;

				bool hovered = ImGui::IsMouseHoveringRect( item_min, item_max );
				if (hovered)
				{
					draw->AddRectFilled( item_min, item_max, IM_COL32( 255, 30, 30, 100 ) );
					if (ImGui::IsMouseClicked( ImGuiMouseButton_Left ))
					{
						*current_item = i;
						is_open = false;
						return true;
					}
				}

				draw->AddText( item_min + ImVec2( 10, 5 ), IM_COL32( 220, 220, 220, 255 ), items[i] );
			}
		}

		return false;
	}

	inline bool slider( const char* label, float* v, float min, float max, const char* format = "%.0f" )
	{
		ImGui::PushID( label );
		ImGui::TextUnformatted( label );
		ImGui::Spacing();

		ImVec2 barSize( 180, 6 );
		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImDrawList* draw = ImGui::GetWindowDrawList();

		ImGui::InvisibleButton( "##slider", ImVec2( barSize.x, 20 ) );
		bool active = ImGui::IsItemActive();

		static std::unordered_map<ImGuiID, float> smooth;
		ImGuiID id = ImGui::GetItemID();
		float& displayed = smooth[id];

		if (active)
		{
			float ratio = std::clamp( (ImGui::GetIO().MousePos.x - pos.x) / barSize.x, 0.0f, 1.0f );
			*v = min + (max - min) * ratio;
		}

		float t = std::clamp( (*v - min) / (max - min), 0.0f, 1.0f );
		displayed += (t - displayed) * 0.1f;

		draw->AddRectFilled( pos, ImVec2( pos.x + barSize.x, pos.y + barSize.y ), IM_COL32( 25, 25, 25, 255 ), 3.0f );
		draw->AddRectFilledMultiColor(
			pos,
			ImVec2( pos.x + barSize.x * displayed, pos.y + barSize.y ),
			IM_COL32( 100, 0, 0, 255 ),
			IM_COL32( 255, 30, 30, 255 ),
			IM_COL32( 255, 30, 30, 255 ),
			IM_COL32( 100, 0, 0, 255 )
		);


		char buffer[16];
		snprintf( buffer, sizeof( buffer ), format, *v );
		ImVec2 valueSize = ImGui::CalcTextSize( buffer );
		draw->AddText( ImVec2( pos.x + barSize.x + 6, pos.y - 2 ), IM_COL32( 255, 255, 255, 255 ), buffer );

		ImGui::PopID();
		return active;
	}

	inline void color_row( const char* label, float color[4] )
	{
		float item_spacing = ImGui::GetStyle().ItemSpacing.x;
		float label_width = 150.0f;
		float color_box_width = 40.0f;

		ImGui::BeginGroup();
		ImGui::AlignTextToFramePadding();
		ImGui::SetCursorPosX( ImGui::GetCursorPosX() + 8 );
		ImGui::TextUnformatted( label );
		ImGui::SameLine();

		float full_width = ImGui::GetContentRegionAvail().x + ImGui::GetCursorPosX();
		float picker_x = full_width - color_box_width;
		ImGui::SetCursorPosX( picker_x );

		ImGui::ColorEdit4( ("##" + std::string( label )).c_str(), color,
			ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoTooltip );
		ImGui::EndGroup();
	}

	inline void color_picker( const char* label, ImColor& color )
	{
		color_row( label, (float*)&color.Value );
	}
}

inline void initialize_gui() 
{
	ImGui::GetStyle().WindowRounding = 0.0f;
	ImGui::GetStyle().WindowBorderSize = 0.0f;
	ImGui::GetStyle().GrabMinSize = 0.0f;
	ImGui::SetNextWindowSize( {800, 400} );
	ImGuiStyle& style = ImGui::GetStyle();
	ImGui::PushStyleColor( ImGuiCol_Separator, ImColor( 255, 50, 50 ).Value );

	style.Colors[ImGuiCol_Border] = ImColor( 10, 10, 10 );
	style.Colors[ImGuiCol_BorderShadow] = ImColor( 0, 0, 0, 0 );
	ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImColor( 15, 15, 15, 255 );

	ImGui::GetStyle().Colors[ImGuiCol_ResizeGrip] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Bright red
	ImGui::GetStyle().Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.0f, 0.5f, 0.5f, 1.0f); // Light red
	ImGui::GetStyle().Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.8f, 0.0f, 0.0f, 1.0f); // Darker red
}


