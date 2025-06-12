#include "general.hpp"

inline int selected_weapon = 0;

static std::vector<const char*> weapon_preset = { "Shotgun", "Rifle", "SMG", "Sniper", "Pistol", "DMR" };
static std::vector<const char*> aimbone_preset = { "Head", "Neck", "Chest", "Hip" };

static FancyFadeTabs tabs({ "Aimbot", "Triggerbot", "Visuals", "Radar", "World", "Misc", "Colors" },
	{
		// Aimbot Tab
		[]()
	{
		if (ImGui::BeginChild(oxorany("aimbot_content"), ImVec2(0, 0), true))
		{
			ImVec2 textSize = ImGui::CalcTextSize(oxorany("Aimbot"));
			ImVec2 region = ImGui::GetContentRegionAvail();
			ImGui::SetCursorPosX((region.x - textSize.x) * 0.5f);
			ImGui::Text(oxorany("Aimbot"));
			ImGui::Separator();
			ImGui::Columns(2, nullptr, false);

			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor(20, 20, 20, 255).Value);
			if (ImGui::BeginChild(oxorany("aimbot_left"), ImVec2(0, 0), true))
			{
				customUI::checkbox(oxorany("Enable Aimbot"), &settings::aimbot::enabled);
				static hotkey aimbot_hotkey(settings::aimbot::hotkey);
				ImGui::SameLine(200, 0);
				aimbot_hotkey.add_button(ImVec2(110, 19));

				if (settings::aimbot::enabled)
				{
					ImGui::Indent(12.0f);
					customUI::checkbox(oxorany("Draw FOV"), &settings::aimbot::render_fov);
					customUI::checkbox(oxorany("Prediction"), &settings::aimbot::prediction);
					customUI::checkbox(oxorany("Dynamic FOV"), &settings::aimbot::dynamic_fov);
					customUI::checkbox(oxorany("Visible Check"), &settings::aimbot::visible_check);
					customUI::checkbox(oxorany("Knocked Check"), &settings::aimbot::knocked_check);
					customUI::checkbox(oxorany("Weapon Configurations"), &settings::aimbot::weapon_configs);

					ImGui::Unindent(12.0f);
				}

				ImGui::EndChild();
			}
			ImGui::PopStyleColor();

			ImGui::NextColumn();

			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor(20, 20, 20, 255).Value);

			if (ImGui::BeginChild(oxorany("aimbot_right"), ImVec2(0, 0), true))
			{
				customUI::slider(oxorany("Aimbot Refresh"), &settings::aimbot::refresh_rate, 1.0f, 360.0f);

				auto get_current_weapon_config = [](int selected_weapon) -> settings::aimbot::weapon_data& {
					switch (selected_weapon) {
					case 0: return settings::aimbot::shotgun;
					case 1: return settings::aimbot::rifle;
					case 2: return settings::aimbot::smg;
					case 3: return settings::aimbot::sniper;
					case 4: return settings::aimbot::pistol;
					case 5: return settings::aimbot::dmr;
					default: return settings::aimbot::basic;
					}
					};

				if (!settings::aimbot::weapon_configs)
				{
					ImGui::PushID("Basic");

					customUI::slider(oxorany("Field of View"), &settings::aimbot::basic.fov, 0, 360);
					customUI::slider(oxorany("Smoothing"), &settings::aimbot::basic.smoothing, 1, 20);

					ImGui::Text(oxorany("Hitbox"));
					ImGui::SameLine(166);
					ImGui::Text("Selected Weapon");

					// Note: These need unique internal IDs to avoid label conflicts
					customUI::combo(oxorany("##Hitbox"), &settings::aimbot::basic.hitbox, aimbone_preset);
					ImGui::SameLine();
					customUI::combo(oxorany("##SelectedWeapon"), &selected_weapon, weapon_preset);

					ImGui::PopID();
				}
				else // Weapon-specific configs
				{
					// Push weapon index as ID to isolate UI elements for each weapon
					ImGui::PushID(selected_weapon);
					auto& weapon_cfg = get_current_weapon_config(selected_weapon);

					customUI::slider(oxorany("Field of View"), &weapon_cfg.fov, 0, 360);
					customUI::slider(oxorany("Smoothing"), &weapon_cfg.smoothing, 1, 20);

					ImGui::Text(oxorany("Hitbox"));
					ImGui::SameLine(166);
					ImGui::Text("Selected Weapon");

					customUI::combo(oxorany("##Hitbox"), &weapon_cfg.hitbox, aimbone_preset);
					ImGui::SameLine();
					customUI::combo(oxorany("##SelectedWeapon"), &selected_weapon, weapon_preset);

					ImGui::PopID();
				}
			}
			ImGui::EndChild();

			ImGui::PopStyleColor();

			ImGui::Columns(1);
		}
		ImGui::EndChild();
	},

	//Triggerbot
	[]()
	{
		if (ImGui::BeginChild(oxorany("triggerbot_content"), ImVec2(0, 0), true))
		{
			ImVec2 textSize = ImGui::CalcTextSize(oxorany("Triggerbot"));
			ImVec2 region = ImGui::GetContentRegionAvail();
			ImGui::SetCursorPosX((region.x - textSize.x) * 0.5f);
			ImGui::Text(oxorany("Triggerbot"));
			ImGui::Separator();

			ImGui::Columns(2, nullptr, false);

			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor(20, 20, 20, 255).Value);
			if (ImGui::BeginChild(oxorany("triggerbot_left"), ImVec2(0, 0), true))
			{
				customUI::checkbox(oxorany("Enable Triggerbot"), &settings::triggerbot::enabled);
				static hotkey triggerbot_hotkey(settings::triggerbot::hotkey);

				if (settings::triggerbot::enabled)
				{
					if (!settings::triggerbot::always_on)
					{
						ImGui::SameLine(200, 0);
						triggerbot_hotkey.add_button(ImVec2(110, 19));
					}

					customUI::checkbox(oxorany("Always On"), &settings::triggerbot::always_on);
					customUI::checkbox(oxorany("Shotgun Only"), &settings::triggerbot::shotgun_only);
				}


				ImGui::EndChild();
			}
			ImGui::PopStyleColor();

			ImGui::NextColumn();

			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor(20, 20, 20, 255).Value);
			if (ImGui::BeginChild(oxorany("triggerbot_right"), ImVec2(0, 0), true))
			{
				customUI::slider(oxorany("Maximum Distance"), &settings::triggerbot::maximum_distance, 1, 200);
				customUI::slider(oxorany("TriggerBot Post Delay"), &settings::triggerbot::post_delay, 1, 200);
				customUI::slider(oxorany("TriggerBot Pre Delay"), &settings::triggerbot::pre_delay, 1, 200);

				ImGui::EndChild();
			}
			ImGui::PopStyleColor();

			ImGui::Columns(1);
			ImGui::EndChild();
		}
	},

	// Visuals Tab
	[]()
	{
		if (ImGui::BeginChild(oxorany("visuals_content"), ImVec2(0, 0), true))
		{
			ImVec2 textSize = ImGui::CalcTextSize(oxorany("Visuals"));
			ImVec2 region = ImGui::GetContentRegionAvail();
			ImGui::SetCursorPosX((region.x - textSize.x) * 0.5f);
			ImGui::Text(oxorany("Visuals"));
			ImGui::Separator();
			ImGui::Columns(2, nullptr, false);

			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor(20, 20, 20, 255).Value);
			if (ImGui::BeginChild(oxorany("visuals_left"), ImVec2(0, 0), true))
			{
				customUI::checkbox(oxorany("ESP Enabled"), &settings::visuals::enabled);
				if (settings::visuals::enabled)
				{
					ImGui::Indent(12.0f);

					customUI::checkbox(oxorany("Skeleton"), &settings::visuals::skeleton::enabled);
					if (settings::visuals::skeleton::enabled)
					{
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20);
						customUI::checkbox(oxorany("Skeleton Outlined"), &settings::visuals::skeleton::outlined);
					}
					customUI::checkbox(oxorany("Box"), &settings::visuals::box::enabled);
					if (settings::visuals::box::enabled)
					{
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20);
						customUI::checkbox(oxorany("Box Filled"), &settings::visuals::box::filled);

						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20);
						customUI::checkbox(oxorany("Box Outlined"), &settings::visuals::box::outlined);
					}
					customUI::checkbox(oxorany("Indicators"), &settings::visuals::indicators);

					customUI::checkbox(oxorany("Distance"), &settings::visuals::distance);
					customUI::checkbox(oxorany("Team"), &settings::visuals::team);
					customUI::checkbox(oxorany("Platform"), &settings::visuals::platform);
					customUI::checkbox(oxorany("Username"), &settings::visuals::username);
					customUI::checkbox(oxorany("Weapon"), &settings::visuals::weapon);

					if (settings::visuals::weapon)
					{
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20);
						customUI::checkbox(oxorany("Weapon Rarity"), &settings::visuals::weapon_rarity);
					}

					ImGui::Unindent(12.0f);
				}

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);

				ImGui::EndChild();
			}
			ImGui::PopStyleColor();

			ImGui::NextColumn();

			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor(20, 20, 20, 255).Value);
			if (ImGui::BeginChild(oxorany("visuals_right"), ImVec2(0, 0), true))
			{
				ImGui::Text(oxorany("Skeleton Presets"));
				customUI::combo(oxorany("##skeleton_preset"), &settings::visuals::skeleton::type, { "Preset 1", "Preset 2" });

				ImGui::EndGroup();



				ImGui::EndGroup();
			}
			ImGui::EndChild();

			ImGui::PopStyleColor();

			ImGui::Columns(1);
			ImGui::EndChild();
		}
	},

	//Radar
	[]()
	{
		if (ImGui::BeginChild(oxorany("radar_content"), ImVec2(0, 0), true))
		{
			ImVec2 textSize = ImGui::CalcTextSize(oxorany("Radar"));
			ImVec2 region = ImGui::GetContentRegionAvail();
			ImGui::SetCursorPosX((region.x - textSize.x) * 0.5f);
			ImGui::Text(oxorany("Radar"));
			ImGui::Separator();

			ImGui::Columns(2, nullptr, false);

			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor(20, 20, 20, 255).Value);
			if (ImGui::BeginChild(oxorany("radar_left"), ImVec2(0, 0), true))
			{
				customUI::checkbox(oxorany("Enable Radar"), &settings::radar::enabled);

				if (settings::radar::enabled)
				{
					customUI::checkbox(oxorany("Enable Radar Distance"), &settings::radar::distance);
					customUI::slider(oxorany("Radar Size"), &settings::radar::size, 50, 500);
				}

				ImGui::EndChild();
			}
			ImGui::PopStyleColor();

			ImGui::NextColumn();

			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor(20, 20, 20, 255).Value);
			if (ImGui::BeginChild(oxorany("radar_right"), ImVec2(0, 0), true))
			{

				ImGui::Text(oxorany("Radar Type"));
				customUI::combo(oxorany("Radar Type"), &settings::radar::type, {oxorany("Circle"), oxorany("Square")});

				ImGui::EndChild();
			}
			ImGui::PopStyleColor();

			ImGui::Columns(1);
			ImGui::EndChild();
		}
	},

	[]()
	{
		if (ImGui::BeginChild(oxorany("world_content"), ImVec2(0, 0), true))
		{
			ImVec2 textSize = ImGui::CalcTextSize(oxorany("World"));
			ImVec2 region = ImGui::GetContentRegionAvail();
			ImGui::SetCursorPosX((region.x - textSize.x) * 0.5f);
			ImGui::Text(oxorany("World"));
			ImGui::Separator();

			ImGui::Columns(2, nullptr, false);

			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor(20, 20, 20, 255).Value);
			if (ImGui::BeginChild(oxorany("world_left"), ImVec2(0, 0), true))
			{
				customUI::checkbox(oxorany("Enable World"), &settings::environment::enabled);

				if (settings::radar::enabled)
				{
					ImGui::Indent(20);

					customUI::checkbox(oxorany("chest"), &settings::environment::chests);
					customUI::checkbox(oxorany("ammo container"), &settings::environment::ammo_containers);
					customUI::checkbox(oxorany("items"), &settings::environment::items);

					if (settings::environment::items)
					{
						ImGui::Indent(20);

						customUI::checkbox(oxorany("common"), &settings::environment::common);
						customUI::checkbox(oxorany("uncommon"), &settings::environment::uncommon);
						customUI::checkbox(oxorany("rare"), &settings::environment::rare);
						customUI::checkbox(oxorany("epic"), &settings::environment::epic);
						customUI::checkbox(oxorany("legendary"), &settings::environment::legendary);
						customUI::checkbox(oxorany("mythic"), &settings::environment::mythic);

						ImGui::Unindent(20);
					}

					ImGui::Unindent(20);
				}

				ImGui::EndChild();
			}
			ImGui::PopStyleColor();

			ImGui::NextColumn();

			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor(20, 20, 20, 255).Value);
			if (ImGui::BeginChild(oxorany("world_right"), ImVec2(0, 0), true))
			{
				customUI::slider(oxorany("maximum distance"), &settings::environment::maximum_distance, 15.f, 270.f);

				ImGui::EndChild();
			}
			ImGui::PopStyleColor();

			ImGui::Columns(1);
			ImGui::EndChild();
		}
	},

	// Misc Tab
	[]()
	{
		if (ImGui::BeginChild(oxorany("misc_content"), ImVec2(0, 0), true))
		{
			ImVec2 textSize = ImGui::CalcTextSize(oxorany("Misc"));
			ImVec2 region = ImGui::GetContentRegionAvail();
			ImGui::SetCursorPosX((region.x - textSize.x) * 0.5f);
			ImGui::Text(oxorany("Misc"));
			ImGui::Separator();

			ImGui::Columns(2, nullptr, false);

			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor(20, 20, 20, 255).Value);

			if (ImGui::BeginChild(oxorany("misc_left"), ImVec2(0, 0), true))
			{
				customUI::checkbox(oxorany("Spectator List"), &settings::misc::spectator_list);

			}
			ImGui::EndChild();

			ImGui::PopStyleColor();

			ImGui::NextColumn();

			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor(20, 20, 20, 255).Value);
			if (ImGui::BeginChild(oxorany("misc_right"), ImVec2(0, 0), true))
			{
				customUI::checkbox(oxorany("Enable Cheat FPS"), &settings::misc::fps_counter);
				customUI::checkbox(oxorany("Watermark"), &settings::misc::watermark);
				customUI::checkbox(oxorany("VSYNC"), &settings::misc::vsync);

				customUI::slider(oxorany("Skeleton Thickness"), &settings::visuals::skeleton::thickness, 1, 10);
				customUI::slider(oxorany("Box Thickness"), &settings::visuals::box::thickness, 1, 10);

				if (settings::misc::spectator_list)
				{
					customUI::slider(oxorany("Spectator List Opacity"), &settings::misc::spectator_list_opacity, 0, 255);
				}

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.0f, 0.0f, 1.0f));

				if (ImGui::Button(oxorany("Save Config"))) {

					settings::save_config(oxorany("C:\\astrid.json"));

				}
				if (ImGui::Button(oxorany("Load Config"))) {

					settings::load_config(oxorany("C:\\astrid.json"));

				}
				ImGui::EndChild();
			}
			ImGui::PopStyleColor();

			ImGui::Columns(1);
			ImGui::EndChild();
		}
	},

	// Colors Tab
	[]()
	{
		if (ImGui::BeginChild(oxorany("colors_content"), ImVec2(0, 0), true))
		{
			ImVec2 textSize = ImGui::CalcTextSize(oxorany("Colors"));
			ImVec2 region = ImGui::GetContentRegionAvail();
			ImGui::SetCursorPosX((region.x - textSize.x) * 0.5f);
			ImGui::Text(oxorany("Colors"));
			ImGui::Separator();

			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor(20, 20, 20, 255).Value);

			if (ImGui::BeginChild(oxorany("colors_inner"), ImVec2(0, 0), true))
			{
				customUI::color_picker(oxorany("Box Visible Color"), settings::visuals::visible_color);
				customUI::color_picker(oxorany("Box Invisible Color"), settings::visuals::invisible_color);
				customUI::color_picker(oxorany("Skeleton Visible Color"), settings::visuals::skeleton_color_visible);
				customUI::color_picker(oxorany("Skeleton Invisible Color"), settings::visuals::skeleton_color_invisible);
				customUI::color_picker(oxorany("Text Visible Color"), settings::visuals::text_color_visible);
				customUI::color_picker(oxorany("Text Invisible Color"), settings::visuals::text_color_invisible);
				customUI::color_picker(oxorany("Knocked Color"), settings::visuals::knocked_color);
				customUI::color_picker(oxorany("FOV Circle Color"), settings::visuals::fov_color);
			}
			ImGui::EndChild();

			ImGui::PopStyleColor();
		}
		ImGui::EndChild();
	}

	}
);

void menu::render()
{
	if (!c_menu.rendering)
		return;

	initialize_gui();

	ImGui::Begin("main", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
	{
		tabs.Draw();
	}
	ImGui::End();
}