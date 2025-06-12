#pragma once

namespace fortnite
{
	template< class type >
	class tarray
	{
	public:
		tarray() : data(), count(), max_count() {}
		tarray(type* data, uint32_t count, uint32_t max_count) :
			data(data), count(count), max_count(max_count) {
		}

		type get(uintptr_t idx)
		{
			return c_kernel.read_memory< type >(
				std::bit_cast<uintptr_t>(this->data) + (idx * sizeof(type))
			);
		}

		std::vector<type> fetch_data() const
		{
			if (!this || this->data == nullptr || this->count <= 0 || this->max_count <= 0) {
				return {};
			}

			if (this->count > this->max_count) {
				return {};
			}

			constexpr size_t MAX_REASONABLE_COUNT = 512;
			if (this->count > MAX_REASONABLE_COUNT) {
				return {};
			}

			if (static_cast<size_t>(this->count) > SIZE_MAX / sizeof(type)) {
				return {};
			}

			const size_t total_size = sizeof(type) * static_cast<size_t>(this->count);
			if (total_size == 0) {
				return {};
			}

			const uintptr_t data_ptr = std::bit_cast<uintptr_t>(this->data);

			if (data_ptr == 0 || (data_ptr % alignof(type)) != 0) {
				return {};
			}

			std::vector<type> buffer(static_cast<size_t>(this->count));

			const bool read_ok = c_kernel.read_memory_raw(
				data_ptr,
				buffer.data(),
				total_size
			);

			if (!read_ok) {
				return {};
			}

			return buffer;
		}

		uintptr_t get_address()
		{
			return reinterpret_cast<uintptr_t>(this->data);
		}

		uint32_t size() const
		{
			return this->count;
		};

		uint32_t max_size() const
		{
			return this->max_count;
		};

		bool is_valid_index(int32_t index) const
		{
			return this->count >= 0 && index < count;
		};

		bool is_valid() const
		{
			return this->data != nullptr;
		};

		type* data;
		uint32_t count;
		uint32_t max_count;
	};

	enum class EFortBuildingState

		: uint8_t

	{


		EFortBuildingState__Placement

		= 0,

		EFortBuildingState__EditMode

		= 1,

		EFortBuildingState__None

		= 2,

		EFortBuildingState__EFortBuildingState_MAX

		= 3
	};

	enum class EFortRarity : uint8_t
	{
		Common = 0,
		Uncommon = 1,
		Rare = 2,
		Epic = 3,
		Legendary = 4,
		Mythic = 5,
		Transcendent = 6,
		Unattainable = 7,
		NumRarityValues = 8,
		EFortRarity_MAX = 9
	};

	struct player
	{
		uintptr_t player_state;
		uintptr_t pawn_private;
		uintptr_t skeletal_mesh;
		uintptr_t character_movement;

		int team_index;

		std::string username;
		std::string platform;
		std::string weapon;

		bool visibility;
		int knocked;
		uintptr_t current_weapon;

		ImColor weapon_rarity;

		std::optional<bone_data> bone_data;
	};

	struct container
	{
		uintptr_t actor;
		std::string actor_name;
		math::fvector relative_location;
		std::string display_name;
		ImColor display_color;

		bool valid;
	};
}