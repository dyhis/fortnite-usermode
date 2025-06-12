#pragma once

class updater
{
public:

	enum weapon_type : uint8_t
	{
		shotgun,
		sniper,
		smg,
		rifle,
		pistol,
		dmr,
		harvesting_tool,
		unknown
	};

	struct engine_t
	{
		uintptr_t
			world,
			location_pointer,
			rotation_pointer,
			game_instance,
			game_state,
			local_player,
			player_controller,
			acknowledged_pawn,
			player_state,
			view_state;

		float
			server_world_time,
			projectile_speed,
			projectile_gravity;

		int
			team_index;

		std::string
			current_weapon;

		weapon_type
			current_weapon_type;

		std::atomic<std::shared_ptr<std::vector<std::string>>> spectators;

		void update();
		void tick();
	} static inline engine;

	struct entities_t
	{
		std::atomic<std::shared_ptr<std::vector<fortnite::player>>> player_list;

		void update();
		void tick();
	} static inline entities;

	struct environment_t
	{
		std::atomic<std::shared_ptr<std::vector<fortnite::container>>> container_list;

		enum actor_type : uint8_t
		{
			ammo_container,
			chest,
			pickup,
			projectile,
			building
		};

		struct world_t
		{
			void process_world_actor(fortnite::container& container);
		} static inline world;

		void update();
	} static inline environment;

	void go()
	{
		using namespace std::chrono;
		using namespace std::literals;

		auto next_engine_update = steady_clock::now();
		auto next_engine_tick = next_engine_update;
		auto next_entities_update = next_engine_update;
		auto next_entities_tick = next_engine_update;
		auto next_environment_tick = next_engine_update;

		while (true)
		{
			auto now = steady_clock::now();

			if (now >= next_engine_update)
			{
				engine.update();
				next_engine_update = now + 7000ms;
			}

			if (now >= next_engine_tick)
			{
				engine.tick();
				next_engine_tick = now + 150ms;
			}

			if (now >= next_entities_update)
			{
				entities.update();
				next_entities_update = now + 1200ms;
			}

			if (now >= next_entities_tick)
			{
				entities.tick();
				next_entities_tick = now + 120ms;
			}

			if (now >= next_environment_tick)
			{
				environment.update();
				next_environment_tick = now + 10000ms;
			}

			auto next_event = std::min({
				next_engine_update,
				next_engine_tick,
				next_entities_update,
				next_entities_tick,
				next_environment_tick
				});

			if (next_event > now
				) std::this_thread::sleep_until(next_event);
		}
	}
} static inline g_updater; 

