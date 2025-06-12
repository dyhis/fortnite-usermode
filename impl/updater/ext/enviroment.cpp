#include "general.hpp"

std::unordered_set<std::string> world_actor_names;

namespace fname
{
	static inline int32_t DecryptIndex(int32_t index)
	{
		if (index)
		{
			int32_t DecryptedIndex = ((index - 1) ^ 0xEFE54562) + 1;
			return DecryptedIndex ? DecryptedIndex : 270187166;
		}

		return 0;
	}

	static inline void DecryptFName(char* buffer, int length)
	{
		if (length)
		{
			int v8 = (2 * length) ^ (8031 * length + 21456027);
			for (int i = 0; i < length; ++i)
			{
				buffer[i] ^= (v8 - 22);
				v8 = (2 * v8) ^ (8031 * v8 + 21456027);
			}

		}

		buffer[length] = '\0';
	}

	static inline std::string decrypt_fname(int32_t index)
	{
		int32_t DecryptedIndex = DecryptIndex(index);
		uint64_t NamePoolChunk = g_kernel.read_memory<uint64_t>(g_kernel.target.process_base + (0x18B3A0C0 + 8 * (DecryptedIndex >> 16) + 16)) + 2 * (uint16_t)DecryptedIndex;
		uint16_t Pool = g_kernel.read_memory<uint16_t>(NamePoolChunk);

		if ((Pool ^ 0x13C0u) < 0x40)
		{
			DecryptedIndex = DecryptIndex(g_kernel.read_memory<int32_t>(NamePoolChunk + 6));
			NamePoolChunk = g_kernel.read_memory<uint64_t>(g_kernel.target.process_base + (0x18B3A0C0 + 8 * (DecryptedIndex >> 16) + 16)) + 2 * (uint16_t)DecryptedIndex;
			Pool = g_kernel.read_memory<uint16_t>(NamePoolChunk);
		}

		int32_t Length = (Pool ^ 0x13C0u) >> 6;
		Length *= (Pool & 1) == 0 ? 1 : 2;

		std::vector<char> NameBuffer(Length + 1);
		g_kernel.read_memory_raw(NamePoolChunk + 2, NameBuffer.data(), Length);
		DecryptFName(NameBuffer.data(), Length);
		return std::string(NameBuffer.data());
	}
}

std::unordered_map<fortnite::EFortRarity, ImColor> rarity_colors =
{
	{ fortnite::EFortRarity::Common, ImColor(140, 146, 151) },
	{ fortnite::EFortRarity::Uncommon, ImColor(0, 255, 0) },
	{ fortnite::EFortRarity::Rare, ImColor(0, 239, 255) },
	{ fortnite::EFortRarity::Epic, ImColor(128, 0, 128) },
	{ fortnite::EFortRarity::Legendary, ImColor(255, 165, 0) },
	{ fortnite::EFortRarity::Mythic, ImColor(255, 215, 0) }
};

void updater::environment_t::world_t::process_world_actor(fortnite::container& container)
{
	if (!settings::environment::enabled) return;

	static cacher<uintptr_t, uintptr_t> root_comp_cache;
	static cacher<uintptr_t, uintptr_t> item_def_cache;
	static cacher<uintptr_t, std::pair<std::string, fortnite::EFortRarity>> weapon_cache;

	auto root_component = root_comp_cache.cache_or_fetch(container.actor, [&]() {
		return g_kernel.read_memory<uintptr_t>(container.actor + offsets::root_component);
		});

	if (!root_component)
		return;

	auto relative_location = g_kernel.read_memory<math::fvector>(root_component + offsets::relative_location);
	if (!relative_location.is_valid())
		return;

	auto distance = g_camera.location.distance(relative_location) / 100;
	if (distance > settings::environment::maximum_distance)
		return;

	auto actor_type =
		container.actor_name.find("Pickup") != std::string::npos ? actor_type::pickup :
		container.actor_name.find("Ammo") != std::string::npos ? actor_type::ammo_container :
		actor_type::chest;

	if (actor_type == actor_type::chest || actor_type == actor_type::ammo_container)
	{
		auto b_already_searched = (g_kernel.read_memory<uint8_t>(container.actor + 0xdd2) >> 2) & 1;
		if (b_already_searched) return;

		container.display_name = actor_type == actor_type::ammo_container ? "Ammo Crate" : "Chest";
		container.display_color = actor_type == actor_type::ammo_container ? ImColor(50, 205, 50) : ImColor(255, 215, 0);
		container.relative_location = relative_location;
		container.valid = true;

		return;
	}
	else if (actor_type == actor_type::pickup)
	{
		auto item_definition = item_def_cache.cache_or_fetch(container.actor, [&]() {
			return g_kernel.read_memory<uintptr_t>(container.actor + 0x370 + 0x10);
			});

		if (!item_definition) return;

		auto item_data = weapon_cache.cache_or_fetch(item_definition, [&]() {
			auto item_name = g_kernel.read_memory<uintptr_t>(item_definition + 0x40);
			auto item_length = g_kernel.read_memory<int32_t>(item_name + 0x28);
			auto item_rarity = g_kernel.read_memory<fortnite::EFortRarity>(item_definition + 0xa2);

			std::unique_ptr<wchar_t[]> item_buffer(new wchar_t[item_length + 1]);
			if (!g_kernel.read_memory_raw(
				reinterpret_cast<ULONG64>(g_kernel.read_memory<PVOID>(item_name + 0x20)),
				item_buffer.get(),
				item_length * sizeof(wchar_t))) {
				return std::pair{ std::string(""), fortnite::EFortRarity::Common };
			}

			std::wstring w_buffer(item_buffer.get(), item_length);
			std::string item_string(w_buffer.begin(), w_buffer.end());

			return std::pair{ item_string, item_rarity };
			});

		if (item_data.first.empty()) return;

		auto is_rarity_enabled = [](fortnite::EFortRarity e) -> bool
			{
				switch (e)
				{
				case fortnite::EFortRarity::Common:
					if (settings::environment::common) return true;
					break;
				case fortnite::EFortRarity::Uncommon:
					if (settings::environment::uncommon) return true;
					break;
				case fortnite::EFortRarity::Rare:
					if (settings::environment::rare) return true;
					break;
				case fortnite::EFortRarity::Epic:
					if (settings::environment::epic) return true;
					break;
				case fortnite::EFortRarity::Legendary:
					if (settings::environment::legendary) return true;
					break;
				case fortnite::EFortRarity::Mythic:
					if (settings::environment::mythic) return true;
					break;
				}
				return false;
			};

		if (is_rarity_enabled(item_data.second))
		{
			container.display_name = item_data.first + " " + std::to_string((uint8_t)item_data.second);
			container.display_color = rarity_colors[item_data.second];
			container.relative_location = relative_location;
			container.valid = true;
		}

		return;
	}
}


void updater::environment_t::update()
{
	if (settings::environment::enabled)
	{
		{
			world_actor_names.clear();

			if (settings::environment::items)
			{
				world_actor_names.insert("FortPickupAthena");
			}

			if (settings::environment::chests)
			{
				world_actor_names.insert("Tiered_Chest");
			}

			if (settings::environment::ammo_containers)
			{
				world_actor_names.insert("Tiered_Ammo");
			}
		}
	}

	auto tmp_container_list = std::make_shared<std::vector<fortnite::container>>();

	auto levels = g_kernel.read_memory<fortnite::tarray<uintptr_t>>(g_engine.world + offsets::levels).fetch_data();

	for (auto& level : levels)
	{
		auto actors = g_kernel.read_memory<fortnite::tarray<uintptr_t>>(level + offsets::actors).fetch_data();

		for (auto& actor : actors)
		{
			if (!actor) continue;

			auto name_private = g_kernel.read_memory<int32_t>(actor + 0x8);
			if (!name_private) continue;

			auto actor_name = fname::decrypt_fname(name_private);
			if (actor_name.empty()) continue;

			fortnite::container container_pwn
			{
				.actor = actor,
				.actor_name = actor_name
			};

			if (world_actor_names.find(actor_name) != world_actor_names.end())
			{
				this->world.process_world_actor(container_pwn);
				if (!container_pwn.valid) continue;
			}

			tmp_container_list->push_back(container_pwn);
		}
	}

	container_list.store(tmp_container_list);
}