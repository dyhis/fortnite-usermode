#include "general.hpp"

void uengine::camera::update()
{

#pragma pack(push, 1)
	static struct rotation_data_t
	{
		double a;
		char pad1[0x20 - 0x8];
		double b;
		char pad2[0x1D0 - 0x28];
		double c;
	};
#pragma pack(pop)

	auto rotation_data = g_kernel.read_memory<rotation_data_t>(g_engine.rotation_pointer);

	this->rotation.x = asin(rotation_data.c) * (180.0 / std::numbers::pi);
	this->rotation.y = ((atan2(rotation_data.a * -1, rotation_data.b) * (180.0 / std::numbers::pi)) * -1) * -1;;
	this->rotation.z = 0.0f;

	this->rotational_matrix = math::create_rotation_matrix(rotation);

	this->location = g_kernel.read_memory<math::fvector>(g_engine.location_pointer);
	this->field_of_view = g_kernel.read_memory<float>(g_engine.player_controller + 0x3AC) * 90.f;
}

math::fvector2d uengine::wscreen::get_screen_location(math::fvector world_location)
{
	float aspect_ratio = static_cast<float>(globals::screen_width) / globals::screen_height;
	auto inverted_fov = aspect_ratio / (16.0f / 9.0f) * tanf(c_uengine.s_camera.field_of_view * static_cast<float>(std::numbers::pi) / 360.0f);

	const auto& camera_location = c_uengine.s_camera.location;

	math::fvector delta = {
		world_location.x - camera_location.x,
		world_location.y - camera_location.y,
		world_location.z - camera_location.z
	};

	auto transformed_x = delta.x * c_uengine.s_camera.rotational_matrix.m[1][0] + delta.y * c_uengine.s_camera.rotational_matrix.m[1][1] + delta.z * c_uengine.s_camera.rotational_matrix.m[1][2];
	auto transformed_y = delta.x * c_uengine.s_camera.rotational_matrix.m[2][0] + delta.y * c_uengine.s_camera.rotational_matrix.m[2][1] + delta.z * c_uengine.s_camera.rotational_matrix.m[2][2];
	auto transformed_z = delta.x * c_uengine.s_camera.rotational_matrix.m[0][0] + delta.y * c_uengine.s_camera.rotational_matrix.m[0][1] + delta.z * c_uengine.s_camera.rotational_matrix.m[0][2];

	transformed_z = std::max(static_cast<float>(transformed_z), 1.0f);

	auto inv_transformed_z = 1.0f / transformed_z;
	auto scale = globals::screen_width_center / inverted_fov * inv_transformed_z;

	auto screen_x = globals::screen_width_center + transformed_x * scale;
	auto screen_y = globals::screen_height_center - transformed_y * scale;

	return { screen_x, screen_y };
}

bool uengine::player::get_visibility(fortnite::player& player)
{
	auto server_seconds = g_kernel.read_memory<double>(g_engine.world + offsets::location_pointer + 0x20);
	auto last_render_time = g_kernel.read_memory<float>(player.skeletal_mesh + 0x32c);
	return server_seconds - last_render_time <= 0.06f;
}

bool uengine::player::get_transforms(fortnite::player& player)
{
	auto bone_array = g_kernel.read_memory<fortnite::tarray<math::ftransform>>(player.skeletal_mesh + offsets::bone_array);
	if (!bone_array.is_valid() || bone_array.size() < 110)
	{
		bone_array = g_kernel.read_memory<fortnite::tarray<math::ftransform>>(player.skeletal_mesh + offsets::bone_array);
	}

	if (!bone_array.is_valid() || bone_array.size() < 110)
	{
		return false;
	}

	auto all_bones = bone_array.fetch_data();
	if (all_bones.empty())
	{
		return false;
	}

	auto component_to_world = g_kernel.read_memory<math::ftransform>(player.skeletal_mesh + offsets::component_to_world);

	player.bone_data = std::optional{ bone_data{all_bones, component_to_world} };
}

bool uengine::player::get_weapon(fortnite::player& player)
{
	auto current_weapon = g_kernel.read_memory<uintptr_t>(player.pawn_private + offsets::current_weapon);
	if (!current_weapon) return false;

	auto definition = g_kernel.read_memory<uintptr_t>(current_weapon + offsets::weapon_data);
	if (!definition) return false;

	auto rarity = g_kernel.read_memory<fortnite::EFortRarity>(definition + 0xa2);

	auto fname_text = g_kernel.read_memory<std::uint64_t>(definition + 0x40);
	if (!fname_text) return false;

	auto name_length = g_kernel.read_memory<int32_t>(fname_text + 0x28);
	if (!name_length) return false;

	std::unique_ptr<wchar_t[]> weapon_name(new wchar_t[name_length + 1]);

	if (!g_kernel.read_memory_raw(reinterpret_cast<ULONG64>(g_kernel.read_memory<PVOID>(fname_text + 0x20)), weapon_name.get(), name_length * sizeof(wchar_t)))  return { FALSE };

	std::wstring w_buffer{ weapon_name.get() };

	if (!w_buffer.empty())
	{
		auto player_weapon = std::string(w_buffer.begin(), w_buffer.end());

		player.weapon = std::string(w_buffer.begin(), w_buffer.end());

		player.weapon_rarity =
			rarity == fortnite::EFortRarity::Common ? im_gray :
			rarity == fortnite::EFortRarity::Uncommon ? im_green :
			rarity == fortnite::EFortRarity::Rare ? im_blue :
			rarity == fortnite::EFortRarity::Epic ? ImColor(153, 50, 204) :
			rarity == fortnite::EFortRarity::Legendary ? ImColor(255, 165, 0) :
			rarity == fortnite::EFortRarity::Transcendent ? ImColor(0, 255, 255) :
			rarity == fortnite::EFortRarity::Mythic ? ImColor(240, 230, 140) : ImColor(0, 0, 0);
	}
}

std::string uengine::player::get_weapon(uintptr_t pawn)
{
	auto current_weapon = g_kernel.read_memory<uintptr_t>(pawn + offsets::current_weapon);

	auto definition = g_kernel.read_memory<uintptr_t>(current_weapon + offsets::weapon_data);
	if (!definition) return "";

	auto fname_text = g_kernel.read_memory<std::uint64_t>(definition + 0x40);
	if (!fname_text) return "";

	auto name_length = g_kernel.read_memory<int32_t>(fname_text + 0x28);

	std::unique_ptr<wchar_t[]> weapon_name(new wchar_t[name_length + 1]);

	if (!g_kernel.read_memory_raw(reinterpret_cast<ULONG64>(g_kernel.read_memory<PVOID>(fname_text + 0x20)), weapon_name.get(), name_length * sizeof(wchar_t)))  return "";

	std::wstring w_buffer{ weapon_name.get() };

	return std::string(w_buffer.begin(), w_buffer.end());
}

std::string uengine::player::get_username(fortnite::player& player)
{
	auto f_string = g_kernel.read_memory<__int64>(player.player_state + 0xb20);
	if (!f_string) return std::string("");

	auto length = g_kernel.read_memory<int>(f_string + 0x10);
	if (!length || length < 0 || length > 255) return std::string("bot");

	auto f_text = g_kernel.read_memory<__int64>(f_string + 0x8);

	wchar_t* buffer = new wchar_t[length];
	g_kernel.read_memory_raw(f_text, buffer, length * sizeof(wchar_t));

	auto v6 = (__int64)length;

	char v21;
	int v22;
	int i;

	int v25;
	unsigned short* v23;

	v21 = v6 - 1;
	if (!(unsigned int)v6)
		v21 = 0;
	v22 = 0;
	v23 = (unsigned short*)buffer;
	for (i = (v21) & 3;; *v23++ += i & 7)
	{
		v25 = v6 - 1;
		if (!(unsigned int)v6)
			v25 = 0;
		if (v22 >= v25)
			break;
		i += 3;
		++v22;
	}

	std::wstring w_buffer{ buffer };

	delete[] buffer;

	return std::string(w_buffer.begin(), w_buffer.end());
}

std::string uengine::player::get_username(uintptr_t& player_state)
{
	auto f_string = g_kernel.read_memory<__int64>(player_state + 0xb20);
	if (!f_string) return std::string("");

	auto length = g_kernel.read_memory<int>(f_string + 0x10);
	if (!length || length < 0 || length > 255) return std::string("bot");

	auto f_text = g_kernel.read_memory<__int64>(f_string + 0x8);

	wchar_t* buffer = new wchar_t[length];
	g_kernel.read_memory_raw(f_text, buffer, length * sizeof(wchar_t));

	auto v6 = (__int64)length;

	char v21;
	int v22;
	int i;

	int v25;
	unsigned short* v23;

	v21 = v6 - 1;
	if (!(unsigned int)v6)
		v21 = 0;
	v22 = 0;
	v23 = (unsigned short*)buffer;
	for (i = (v21) & 3;; *v23++ += i & 7)
	{
		v25 = v6 - 1;
		if (!(unsigned int)v6)
			v25 = 0;
		if (v22 >= v25)
			break;
		i += 3;
		++v22;
	}

	std::wstring w_buffer{ buffer };

	delete[] buffer;

	return std::string(w_buffer.begin(), w_buffer.end());
}

std::string uengine::player::get_platform(fortnite::player& player)
{
	auto platform_pointer = g_kernel.read_memory<uintptr_t>(player.player_state + 0x430);
	if (!platform_pointer) return "";

	wchar_t platform_buffer[64];

	if (g_kernel.read_memory_raw(platform_pointer, platform_buffer, sizeof(platform_buffer)))
	{
		if (!platform_buffer)
		{
			return "";
		}

		return { platform_buffer, platform_buffer + wcslen(platform_buffer) };
	}

	return "";
}

math::fvector uengine::player::get_bone_location(fortnite::player& player, int bone_id)
{
	const bone_data& bone_data = *player.bone_data;

	if (bone_id < 0 || bone_id >= bone_data.bones.size())
	{
		return { 0.0f, 0.0f, 0.0f };
	}

	auto bone = bone_data.bones[bone_id];
	const auto& component_to_world = bone_data.component_to_world;

	auto bone_matrix = math::matrix_multiplication(bone.to_matrix(), component_to_world.to_matrix());
	return { bone_matrix._41, bone_matrix._42, bone_matrix._43 };
}