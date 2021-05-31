//#include "common.hpp"
#include "features.hpp"
#include "script.hpp"
#include "natives.hpp"
//#include "gui.hpp"
#include "gui/misc.h"
//#include "gui/base_tab.h"
#include "pointers.hpp"
#include <script_global.hpp>
//#include "math.h"
//#include <gta/replay.hpp>

using namespace rage;

//The messiest file on the planet. it's complete garbage. don't say I didnt warn you! (:

namespace big::features
{
	bool injected = false;
	bool protection = true;
	bool god_mode = true;

	volatile Player player = 0;
	volatile Ped ped = 0;
	volatile Player _cam = 0;

	volatile uint64_t sync = 0;
	volatile uint64_t network = 0;
	volatile uint64_t script = 0;

	std::list<std::string> user;
	std::mutex mutex;

	bool first = true;

	const bool LOG_LOG = false;

	///////////////////////////// below vars added by me

	bool features_crashprotection = protection;
	bool features_eventprotection = true;

	bool Lfeatures_godmode = true;
	bool Lfeatures_noragdoll = true;
	bool Lfeatures_neverwanted = false;
	bool Lfeatures_offradar = false;
	bool Lfeatures_invisible = false;
	bool Lfeatures_teleportwp = false;
	bool Lfeatures_noclip = false;
	bool Lfeatures_nophone = false;
	bool Lfeatures_nomental = false;

	bool Wfeatures_infammo = true;
	bool Wfeatures_autoshoot = false;
	bool Wfeatures_instakill = false;
	bool Wfeatures_addweapons = false;

	bool ESPfeatures_health = true;
	bool ESPfeatures_visible = false;
	bool ESPfeatures = false;

	bool Pfeatures_teleport = false;
	bool Pfeatures_kickfromveh = false;
	bool Pfeatures_kick = false;
	bool Pfeatures_crash = false;
	bool Pfeatures_crashall = false;
	bool Pfeatures_kickall = false;

	bool Ofeatures_skipcutscene = false;

	bool Vfeatures_spawngodmode = false;
	bool Vfeatures_requestentity = false;
	bool Vfeatures_spawninveh = false;
	bool Vfeatures_randomizeveh = false;
	bool Vfeatures_randomizecol = false;
	bool Vfeatures_spawnupgraded = false;
	bool Vfeatures_autoupgrade = false;
	bool Vfeatures_autoclean = false;
	bool Vfeatures_hornboost = false;
	bool Vfeatures_godmode = false;

	enum class logtype
	{
		LOG_NONE,
		LOG_INFO,
		LOG_WARN,
		LOG_ERROR,
	};

	void log_map(std::string str, logtype type)
	{
		std::string a;

		switch (type)
		{
		case logtype::LOG_NONE:
			a = "";
			break;
		case logtype::LOG_INFO:
			a = "~m~[~f~INFO~m~]~w~";
			break;
		case logtype::LOG_WARN:
			a = "~m~[~r~WARN~m~]~w~";
			break;
		case logtype::LOG_ERROR:
			a = "~m~[~y~ERROR~m~]~w~";
			break;
		}

		str = (fmt::format("{} {}", a, str));

		QUEUE_JOB_BEGIN_CLAUSE(str)
		{
			UI::SET_TEXT_OUTLINE();
			UI::_SET_NOTIFICATION_TEXT_ENTRY((char*)"STRING");
			UI::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(str.c_str());
			UI::_DRAW_NOTIFICATION(FALSE, FALSE);

		} QUEUE_JOB_END_CLAUSE
	}

	void show_bye() {}

	void show_stat()
	{
		if (first) 
		{
			log_map("~b~Trainer installation successful. Have fun!", logtype::LOG_NONE);
			first = false;
		}
		else
			char log[256];
	}

	void add_user(std::string _user)
	{
		std::unique_lock<std::mutex> mlock(mutex);
		user.push_back(_user);
	}

	void show_user() //removed, obsolete
	{
		std::unique_lock<std::mutex> mlock(mutex);

		if (!user.empty())
		{
			char log[256];
			std::string _user = user.front();
			user.pop_front();
		}
	}

#pragma region TO BE RELOCATED
	rage::CNetGamePlayer* getNetGamePlayer(int player) { return reinterpret_cast<rage::CNetGamePlayer*>(g_pointers->m_net_player(player)); }

	double wDegreeToRadian(double n) { return n * 0.017453292519943295; }

	Vector3 wRotationToDirection(Vector3 rot)
	{
		double num = wDegreeToRadian(rot.z);
		double num2 = wDegreeToRadian(rot.x);
		double val = cos(num2);
		double num3 = abs(val);
		rot.x = (float)(-(float)sin(num) * num3);
		rot.y = (float)(cos(num) * num3);
		rot.z = (float)sin(num2);
		return rot;
	}

	Vector3 waddVector(Vector3 vector, Vector3 vector2)
	{
		vector.x += vector2.x;
		vector.y += vector2.y;
		vector.z += vector2.z;
		return vector;
	}

	Vector3 wmultiplyVector(Vector3 vector, float inc)
	{
		vector.x *= inc;
		vector.y *= inc;
		vector.z *= inc;
		return vector;
	}

	float deg_to_rad(float deg) { return deg * 3.141592653589793f / 180.f; }

	Vector3 transformRotToDir(Vector3 nig)
	{
		double	a = deg_to_rad(nig.x),
			b = deg_to_rad(nig.z),
			c = cos(a);
		nig.x = (float)-(c * sin(b));
		nig.y = (float)(c * cos(b));
		nig.z = (float)sin(a);

		return nig;
	}

	Vector3	get_coords_infront_of_coords(Vector3 pos, Vector3 rot, float dist)
	{
		Vector3 transformed = transformRotToDir(rot);

		transformed.x *= dist;
		transformed.y *= dist;
		transformed.z *= dist;

		transformed.x += pos.x;
		transformed.y += pos.y;
		transformed.z += pos.z;

		return transformed;
	}

	Vector3 get_coords_above_coords(Vector3 pos, float dist)
	{
		pos.z += dist;
		return pos;
	}

	Vector3 FromHSB(float hue, float saturation, float brightness) // put this shit somewhere else
	{
		float h = hue == 1.0f ? 0 : hue * 6.0f;
		float f = h - (int)h;
		float p = brightness * (1.0f - saturation);
		float q = brightness * (1.0f - saturation * f);
		float t = brightness * (1.0f - (saturation * (1.0f - f)));
		if (h < 1)
			return Vector3((brightness * 255), (t * 255), (p * 255));
		else if (h < 2)
			return Vector3((q * 255), (brightness * 255), (p * 255));
		else if (h < 3)
			return Vector3((p * 255), (brightness * 255), (t * 255));
		else if (h < 4)
			return Vector3((p * 255), (q * 255), (brightness * 255));
		else if (h < 5)
			return Vector3((t * 255), (p * 255), (brightness * 255));
		else
			return Vector3((brightness * 255), (p * 255), (q * 255));
	
	}

	Vector3 find_blip()
	{
		bool waypoint = false;
		static Vector3 zero;
		Vector3 coords;
		bool blipFound = false;
		int blipIterator = UI::_GET_BLIP_INFO_ID_ITERATOR();
		for (Blip i = UI::GET_FIRST_BLIP_INFO_ID(blipIterator); UI::DOES_BLIP_EXIST(i) != 0; i = UI::GET_NEXT_BLIP_INFO_ID(blipIterator))
		{
			if (UI::GET_BLIP_INFO_ID_TYPE(i) == 4)
			{
				coords = UI::GET_BLIP_INFO_ID_COORD(i);
				blipFound = true;
				break;
			}
		}
		if (blipFound)
		{
			waypoint = true;
			return coords;
		}
		else
			waypoint = false;
		return zero;
	}

	Hash load(const char* name)
	{
		Hash hash = MISC::GET_HASH_KEY(name);
		STREAMING::REQUEST_MODEL(hash);
		while (!STREAMING::HAS_MODEL_LOADED(hash))
			script::get_current()->yield();
		return hash;
	}

	void RequestControlOfid(Entity netid)
	{
		if (!NETWORK::NETWORK_HAS_CONTROL_OF_NETWORK_ID(netid))
		{
			NETWORK::NETWORK_REQUEST_CONTROL_OF_NETWORK_ID(netid);
			script::get_current()->yield();
		}
	}

	void RequestControlOfEnt(Entity entity)
	{
		if (!NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(entity))
		{
			NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(entity);
			script::get_current()->yield();
		}

		if (NETWORK::NETWORK_IS_SESSION_STARTED())
		{
			int netID = NETWORK::NETWORK_GET_NETWORK_ID_FROM_ENTITY(entity);
			RequestControlOfid(netID);
			NETWORK::SET_NETWORK_ID_CAN_MIGRATE(netID, 1);
		}
	}

	bool isPlayerFriend(int player)
	{
		int handle[76]; //var num3 = sub_34009(A_0, (A_1) + 264, (A_1) + 272);
		NETWORK::NETWORK_HANDLE_FROM_PLAYER(player, &handle[0], 13);

		if (NETWORK::NETWORK_IS_HANDLE_VALID(&handle[0], 13))
			return NETWORK::NETWORK_IS_FRIEND(&handle[0]) == 1;
		else
			return false;
	}

	void kick(int player)
	{
		if (NETWORK::NETWORK_IS_HOST())
		{
			NETWORK::NETWORK_SESSION_KICK_PLAYER(player);
			return;
		}

		std::int64_t args1[3] = { -435067392, player, *script_global(1630317).at(player, 595).at(506).as<int*>() };
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args1, 3, 1 << player);

		std::int64_t args2[24] = { 1070934291, player, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10 };
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args2, 24, 1 << player);

		std::int64_t args3[4] = { -1729804184, player, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(-2147483647, 2147483647), player };
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args3, 4, 1 << player);

		std::int64_t args4[3] = { 1428412924, player, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(-2147483647, 2147483647) };
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args4, 3, 1 << player);

		std::int64_t args5[6] = { 823645419, player, -1, -1, -1, -1 };
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args5, 6, 1 << player);

		std::int64_t args6[4] = { -442306200, player, -1, 0 };
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args6, 4, 1 << player);

		std::int64_t args7[3] = { -2120750352, player, *script_global(1630317).at(player, 595).at(506).as<int*>() };
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args7, 3, 1 << player);

		std::int64_t args8[17] = { -922075519, player, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, -1 };
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args8, 17, 1 << player);

		std::int64_t args9[11] = { -1975590661, 84857178, 61749268, -80053711, -78045655, 56341553, -78686524, -46044922, -22412109, 29388428, -56335450 };
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args9, 11, 1 << player);

		auto pos = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(player), 0);
		std::int64_t args10[11] = { -1975590661, player, (int)pos.x, (int)pos.y, (int)pos.z, 0, 0, 2147483647, 0, *script_global(1590682).at(player, 883).at(99).at(28).as<int*>(), 1 };
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args10, 11, 1 << player);

		std::int64_t args11[11] = { -1975590661, player, (int)pos.x, (int)pos.y, (int)pos.z, 0, 0, 1000, 0, *script_global(1590682).at(player, 883).at(99).at(28).as<int*>(), 1 };
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args11, 11, 1 << player);

		std::int64_t args12[7] = { -2122716210, 91645, -99683, 1788, 60877, 55085, 72028 };
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args12, 7, 1 << player);

		std::int64_t args13[3] = { -2120750352, player, *script_global(1630317).at(player, 595).at(506).as<int*>() };
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args13, 3, 1 << player);

		std::int64_t args14[7] = { -2122716210, 91645, -99683, 1788, 60877, 55085, 72028 };
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args14, 7, 1 << player);

		std::int64_t args15[3] = { 0xE6116600, player, *script_global(1630317).at(player, 595).at(506).as<int*>() };
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args15, 3, 1 << player);

		std::int64_t args16[6] = { 0xF5CB92DB, 0, 0, 46190868, 0, 2 };
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args16, 6, 1 << player);

		std::int64_t args17[6] = { 0xF5CB92DB,  46190868, 0, 46190868, 46190868, 2 };
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args17, 6, 1 << player);

		std::int64_t args18[8] = { 0xF5CB92DB, 1337, -1, 1, 1, 0, 0, 0 };
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args18, 8, 1 << player);

		std::int64_t args19[9] = { 0xF5CB92DB, player, 1337, -1, 1, 1, 0, 0, 0 };
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args19, 9, 1 << player);

		static std::vector< std::int64_t> rotor = { -1726396442, 154008137, 428882541, -1714354434 };
		for (int i = 1; i < rotor.size(); i++)
		{
			std::int64_t args7[14] = { -1949011582, player, rotor.at(i), i, 1, -10, -10, -10, -10, -10, player, -10, -10, -10 };
			SCRIPT::TRIGGER_SCRIPT_EVENT(1, args7, 4, 1 << player);
		}

		static std::vector<std::int64_t> kicks =
		{
			-1559754940, // disturbed func_16657
			2017765964, // disturbed
			324865135,  // disturbed func_16648
			-1212832151,  // disturbed func_16919
			-1890951223, // disturbed func_16659
			1302185744,  // disturbed
			639032041,  // disturbed
			665709549 // func_16655
		};

		for (int i = 0; i < kicks.size(); i++)
		{
			std::int64_t args[2] = { kicks.at(i), player };
			SCRIPT::TRIGGER_SCRIPT_EVENT(1, args, 2, 1 << player);
		}
	}

	void scriptCrash(int player) // ive noticed that the events with the // ? next to them arent in freemode.c, and if i comment them out and script crash someone they still crash
	{
		std::int64_t args[7] = { 3317451851, -72614, 63007, 59027, -12012, -26996, 33399 }; // ?
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args, 7, 1 << player);

		std::int64_t args2[7] = { -2122716210, 91645, -99683, 1788, 60877, 55085, 72028 }; // only sending this event does nothing
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args2, 7, 1 << player);

		std::int64_t args3[3] = { -2120750352, player, *script_global(1630317).at(player, 595).at(506).as<int*>() }; // ?
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args3, 3, 1 << player);

		std::int64_t args4[3] = { 3859899904, player, *script_global(1630317).at(player, 595).at(506).as<int*>() }; // ?
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args4, 3, 1 << player);

		std::int64_t args5[6] = { -977515445, -1, 500000, 849451549, -1, -1 }; // only sending this event freezes your game for a few seconds and then closes it
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args5, 6, 1 << player);

		std::int64_t args6[6] = { 767605081, -1, 500000, 849451549, -1, -1 }; // only sending this event freezes your game for a few seconds and then closes it
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args6, 6, 1 << player);

		std::int64_t args7[5] = { -1949011582, -1139568479, -1, 1, 100099 }; // only sending this event does nothing
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args7, 5, 1 << player);

		std::int64_t args8[16] = { -2122716210, -1139568479, -1, 1, 100099, -1, 500000, 849451549, -1, -1, 91645, -99683, 1788, 60877, 55085, 72028 }; // only sending this instantly closes your game
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args8, 16, 1 << player);

		std::int64_t args9[20] = { -922075519, -1, -1, -1, -1, -1139568479, -1, 1, 100099, -1, 500000, 849451549, -1, -1, 91645, -99683, 1788, 60877, 55085, 72028 }; // only sending this event does nothing
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args9, 20, 1 << player);

		std::int64_t args10[16] = { -1975590661, -1139568479, -1, 1, 100099, -1, 500000, 849451549, -1, -1, 91645, -99683, 1788, 60877, 55085, 72028 };  // only sending this event does nothing
		SCRIPT::TRIGGER_SCRIPT_EVENT(1, args10, 16, 1 << player);
	}

	void features::kickFunc()
	{
		while (true)
		{
			if (features::Pfeatures_kick)
			{
				kick(selectedPlayer);
				log_map(fmt::format("Attempting to kick player ~r~{}", features::players[features::selectedPlayer].name), logtype::LOG_WARN);
				features::Pfeatures_kick = false;
			}

			if (features::Pfeatures_crash)
			{
				log_map(fmt::format("Attempting to desktop player ~r~{}", features::players[features::selectedPlayer].name), logtype::LOG_WARN);
				scriptCrash(selectedPlayer);
				features::Pfeatures_crash = false;
			}

			if (features::Pfeatures_kickfromveh)
			{
				if (PED::IS_PED_IN_ANY_VEHICLE(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(features::selectedPlayer), FALSE))
				{
					AI::CLEAR_PED_TASKS_IMMEDIATELY(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(features::selectedPlayer));
					AI::CLEAR_PED_TASKS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(features::selectedPlayer));
					AI::CLEAR_PED_SECONDARY_TASK(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(features::selectedPlayer));

					std::int64_t args[2] = { -1333236192, features::selectedPlayer };
					SCRIPT::TRIGGER_SCRIPT_EVENT(1, args, 2, 1 << features::selectedPlayer);

					log_map(fmt::format("Removing ~g~{}~w~ from their vehicle", features::players[features::selectedPlayer].name), logtype::LOG_INFO);
				}
				else
					log_map(fmt::format("~g~{}~w~ is not operating a vehicle", features::players[features::selectedPlayer].name), logtype::LOG_ERROR);

				features::Pfeatures_kickfromveh = false;
			}

			if (features::Pfeatures_kickall)
			{
				log_map("Attempting to kick ~r~all players", logtype::LOG_WARN);

				for (int i = 0; i < 32; i++)
				{
					if (i == player)
						continue;

					if (!PLAYER::GET_PLAYER_PED(i))
						continue;

					if (features::players[i].isfriend)
						continue;

					kick(i);

					script::get_current()->yield();
				}
				features::Pfeatures_kickall = false;
			}

			if (features::Pfeatures_crashall)
			{
				log_map("Attempting to desktop ~r~all players", logtype::LOG_WARN);

				for (int i = 0; i < 32; i++)
				{
					if (i == player)
						continue;

					if (!PLAYER::GET_PLAYER_PED(i))
						continue;

					if (features::players[i].isfriend)
						continue;

					scriptCrash(i);

					script::get_current()->yield();
				}
				features::Pfeatures_crashall = false;
			}

			//bounty
			/*int amt = 10000;
			for (int i = 0; i < 32; i++)
			{
				for (size_t d = 0; d < 32; d++)
				{
					DECORATOR::DECOR_SET_INT(PLAYER::GET_PLAYER_PED(d), "MPBitset", 3);
					int64_t args[22] = { -116602735, i, d, 3, amt, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, *script_global(1652336).at(9).as<int32_t*>(), *script_global(1652336).at(10).as<int32_t*>() };
					SCRIPT::TRIGGER_SCRIPT_EVENT(1, args, 22, 1 << i);

				}
				script::get_current()->yield();
			}*/

			//teleport to apartment
			/*int location = 1;
			for (int i = 0; i < 32; i++)
			{
				if (i == player)
					continue;

				if (features::players[i].isfriend)
					continue;

				int64_t args[9] = { -171207973, i, 1, -1, 1, location, 0, 0, 0 };
				SCRIPT::TRIGGER_SCRIPT_EVENT(1, args, 9, 1 << i);
			}*/

			//kick from vehicle

			script::get_current()->yield();
		}
	}

	void storeSkeleton(Ped ped, int s, ImVec2* out)
	{
		float x, y;
		Vector3 vec = PED::GET_PED_BONE_COORDS(ped, s, 0.0f, 0.0f, 0.0f);
		GRAPHICS::GET_SCREEN_COORD_FROM_WORLD_COORD(vec.x, vec.y, vec.z, &x, &y);
		out->x = features::screenSize.x * x;
		out->y = features::screenSize.y * y;
	}

#pragma endregion

	void features::spawnvehicle(const char* name)
	{
		//need to add delete previous vehicle

		QUEUE_JOB_BEGIN_CLAUSE(name)
		{
			Hash hash_vehicle;

			auto pos = ENTITY::GET_ENTITY_COORDS(big::features::ped, TRUE);
			auto forward = ENTITY::GET_ENTITY_FORWARD_VECTOR(big::features::ped);
			auto heading = ENTITY::GET_ENTITY_HEADING(big::features::ped);

			pos.x += 10 * forward.x;
			pos.y += 10 * forward.y;

			MISC::GET_GROUND_Z_FOR_3D_COORD(pos.x, pos.y, pos.z, &pos.z, FALSE, FALSE);

			int arrSize = sizeof(features::vehicleModels) / sizeof(features::vehicleModels[0]); //fix, spawns the same random vehicle
			int RandIndex = rand() % arrSize;
			static const char* randomModel = features::vehicleModels[RandIndex];

			features::Vfeatures_randomizeveh ? hash_vehicle = load(randomModel) : hash_vehicle = load(name);

			*(unsigned short*)g_pointers->m_model_spawn_bypass = 0x9090;
			auto veh = VEHICLE::CREATE_VEHICLE(hash_vehicle, pos.x, pos.y, pos.z + 3, heading + 90.0f, TRUE, TRUE, FALSE);
			*(unsigned short*)g_pointers->m_model_spawn_bypass = 0x0574;


			NETWORK::NETWORK_FADE_IN_ENTITY(veh, TRUE);

			if (features::Vfeatures_randomizecol)
			{
				VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(vehicle, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255));
				VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(vehicle, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255));
				VEHICLE::SET_VEHICLE_TYRE_SMOKE_COLOR(vehicle, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255));
				VEHICLE::_SET_VEHICLE_NEON_LIGHTS_COLOUR(vehicle, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255));
				VEHICLE::SET_VEHICLE_EXTRA_COLOURS(vehicle, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255));
				VEHICLE::SET_VEHICLE_MOD_COLOR_1(vehicle, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), 0);
				VEHICLE::SET_VEHICLE_MOD_COLOR_2(vehicle, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255));
				VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255));
				VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255));
				VEHICLE::SET_VEHICLE_TYRE_SMOKE_COLOR(veh, GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255), GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, 255));
				//VEHICLE::_SET_HEADLIGHT_COLOR(vehicle, cfg.headlightColor);
			}
			else
			{
				float a, b, c;
				a = features::Vfeatures_vcol[0] * 255, b = features::Vfeatures_vcol[1] * 255, c = features::Vfeatures_vcol[2] * 255;
				VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(vehicle, a, b, c);
				VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(vehicle, a, b, c);
				VEHICLE::SET_VEHICLE_TYRE_SMOKE_COLOR(vehicle, a, b, c);
				VEHICLE::_SET_VEHICLE_NEON_LIGHTS_COLOUR(vehicle, a, b, c);
				VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, a, b, c);
				VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, a, b, c);
				VEHICLE::SET_VEHICLE_TYRE_SMOKE_COLOR(veh, a, b, c);
			}

			if (features::Vfeatures_spawninveh)
			{
				for (int i = -1; i < VEHICLE::GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(veh); i++)
				{
					if (VEHICLE::IS_VEHICLE_SEAT_FREE(veh, i))
					{
						RequestControlOfEnt(veh);
						PED::SET_PED_INTO_VEHICLE(PLAYER::PLAYER_PED_ID(), veh, i);
						break;
					}
				}
			}

			if (features::Vfeatures_spawngodmode)
			{
				ENTITY::SET_ENTITY_INVINCIBLE(veh, TRUE);
				ENTITY::SET_ENTITY_CAN_BE_DAMAGED(veh, FALSE);
				ENTITY::SET_ENTITY_PROOFS(veh, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);
				VEHICLE::SET_VEHICLE_EXPLODES_ON_HIGH_EXPLOSION_DAMAGE(veh, FALSE);
				VEHICLE::SET_VEHICLE_WHEELS_CAN_BREAK(veh, FALSE);
				VEHICLE::SET_VEHICLE_CAN_BE_VISIBLY_DAMAGED(veh, FALSE);
				VEHICLE::SET_VEHICLE_ENGINE_HEALTH(veh, 1000.0f);
				VEHICLE::SET_VEHICLE_UNDRIVEABLE(veh, FALSE);
				VEHICLE::SET_VEHICLE_CAN_BE_TARGETTED(veh, FALSE);
				VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(veh, FALSE);
				VEHICLE::SET_VEHICLE_WHEELS_CAN_BREAK(veh, FALSE);
				if (VEHICLE::IS_THIS_MODEL_A_PLANE(hash_vehicle))
					VEHICLE::SET_PLANE_TURBULENCE_MULTIPLIER(veh, 0.0f);
			}

			if (features::Vfeatures_spawnupgraded)
			{
				VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0);

				//49, 50 = livery

				for (int i = 0; i < 48; i++)
				{
					VEHICLE::TOGGLE_VEHICLE_MOD(veh, i, TRUE);
					VEHICLE::SET_VEHICLE_MOD(veh, i, VEHICLE::GET_NUM_VEHICLE_MODS(veh, i) - 1, FALSE);
				}

				for (int i = 0; i < 8; i++)
					VEHICLE::_SET_VEHICLE_NEON_LIGHT_ENABLED(veh, i, 1);
			}

			RequestControlOfEnt(veh);

		} QUEUE_JOB_END_CLAUSE
	}

	void block_main()
	{
		//block afk
		misc::set_global(2550148 + 296, -1000000);
		misc::set_global(1377236 + 1149, -1000000);
		//block vote
		misc::set_global(1388057, 0);
		misc::set_global(1388059, 0);
		misc::set_global(1388060, 0);
		misc::set_global(1388062, 0);
		misc::set_global(1388063, 0);
	}

	void features_local()
	{
		if (features::Lfeatures_godmode)
		{
			PED::CLEAR_PED_BLOOD_DAMAGE(ped);
			PED::CLEAR_PED_DECORATIONS(ped);
			PED::SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(ped, FALSE);
			ENTITY::SET_ENTITY_INVINCIBLE(ped, TRUE);
			ENTITY::SET_ENTITY_ONLY_DAMAGED_BY_PLAYER(ped, TRUE);
			ENTITY::SET_ENTITY_CAN_BE_DAMAGED(ped, FALSE);
			ENTITY::SET_ENTITY_PROOFS(ped, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);
			PLAYER::SET_PLAYER_INVINCIBLE(player, TRUE);
			PLAYER::SET_PLAYER_HEALTH_RECHARGE_MULTIPLIER(player, 10);
			ENTITY::SET_ENTITY_HEALTH(ped, ENTITY::GET_ENTITY_MAX_HEALTH(ped));
		}
		else
		{
			ENTITY::SET_ENTITY_INVINCIBLE(ped, FALSE);
			ENTITY::SET_ENTITY_ONLY_DAMAGED_BY_PLAYER(ped, FALSE);
			ENTITY::SET_ENTITY_CAN_BE_DAMAGED(ped, TRUE);
			ENTITY::SET_ENTITY_PROOFS(ped, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE);
			PLAYER::SET_PLAYER_INVINCIBLE(player, FALSE);
			PLAYER::SET_PLAYER_HEALTH_RECHARGE_MULTIPLIER(player, 1);
		}

		if (features::Lfeatures_noragdoll)
		{
			PED::SET_PED_CAN_RAGDOLL(ped, FALSE);
			PED::SET_PED_CAN_RAGDOLL_FROM_PLAYER_IMPACT(ped, FALSE);
			PED::SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(ped, FALSE);
		}
		else
		{
			PED::SET_PED_CAN_RAGDOLL(ped, TRUE);
			PED::SET_PED_CAN_RAGDOLL_FROM_PLAYER_IMPACT(ped, TRUE);
			PED::SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(ped, TRUE);
		}

		if (features::Lfeatures_neverwanted)
		{
			PLAYER::SET_MAX_WANTED_LEVEL(0);
			PLAYER::CLEAR_PLAYER_WANTED_LEVEL(player);
			PLAYER::RESET_WANTED_LEVEL_DIFFICULTY(player);
			PLAYER::SET_POLICE_IGNORE_PLAYER(player, TRUE);
			PLAYER::SET_DISPATCH_COPS_FOR_PLAYER(player, FALSE);
			PLAYER::SET_PLAYER_WANTED_LEVEL_NOW(player, FALSE);
		}

		if (features::Lfeatures_offradar)
		{
			*script_global(2426097).at(player, 443).at(204).as<bool*>() = true;
			*script_global(2440277).at(70).as<int32_t*>() = NETWORK::GET_NETWORK_TIME();
			*script_global(2540612).at(4625).as<int32_t*>() = 4;
		}

		Ped _ped = ped; //to prevent declaration confusion in the follow statements
		if (PED::IS_PED_IN_ANY_VEHICLE(_ped, FALSE) && owns_veh(_ped)) // kinda a ghetto fix but it works for now 
			_ped = PED::GET_VEHICLE_PED_IS_IN(_ped, FALSE);

		if (features::Lfeatures_invisible) //better, but still kinda messy
			ENTITY::SET_ENTITY_VISIBLE(_ped, FALSE, FALSE);
		else
			ENTITY::SET_ENTITY_VISIBLE(_ped, TRUE, TRUE);

		//if (GetAsyncKeyState(VK_F5) || CONTROLS::IS_CONTROL_PRESSED(2, INPUT_SCRIPT_PAD_DOWN))
			//features::Lfeatures_teleportwp = true;

		if (features::Lfeatures_teleportwp)
		{
			if (UI::IS_WAYPOINT_ACTIVE()) // ?
			{
				if (!find_blip().z || !find_blip().x || !find_blip().y) // ?
				{
					features::log_map("You do not have a waypoint set", logtype::LOG_ERROR);
					return;
				}

				Vector3 coords = find_blip();

				//if you really care then add a ground check so you can teleport to chilliad
				if (PED::IS_PED_IN_ANY_VEHICLE(ped, 0) && owns_veh(ped))
					ped = PED::GET_VEHICLE_PED_IS_USING(ped);

				//if (GetAsyncKeyState(VK_F5) || CONTROLS::IS_CONTROL_PRESSED(2, INPUT_SCRIPT_PAD_DOWN))
				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(ped, coords.x, coords.y, coords.z + 100, 0, 0, 1);

				features::log_map(fmt::format("Teleporting to ~g~x{} y{} z{}", coords.x, coords.y, coords.z), logtype::LOG_INFO);
			}

			features::Lfeatures_teleportwp = false;
		}

		if (features::Lfeatures_noclip) //needs to be completely redone using local face camera angles. it's also messy.
		{
			if (PED::IS_PED_IN_ANY_VEHICLE(_ped, FALSE) && !features::owns_veh(ped)) //fix for being in others vehicles
			{
				log_map("Noclip is unavailable as a passenger", logtype::LOG_ERROR);
				features::Lfeatures_noclip = false;
				return;
			}

			auto speed = GetAsyncKeyState(VK_SHIFT) ? 10.f : 2.f;
			Vector3 rot = { 0.f, 0.f, 0.f };
			Vector3 playerCoords = ENTITY::GET_ENTITY_COORDS(_ped, 1);

			ENTITY::SET_ENTITY_COLLISION(_ped, FALSE, FALSE);
			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(_ped, playerCoords.x, playerCoords.y, playerCoords.z, 0, 0, 0);

			//forward
			if (GetAsyncKeyState(VK_KEY_W))
			{
				rot = ENTITY::GET_ENTITY_ROTATION(_ped, 0);
				Vector3 to = get_coords_infront_of_coords(playerCoords, rot, .25f * (speed * 1.5f));
				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(_ped, to.x, to.y, to.z, 0, 0, 0);
			}

			//back
			if (GetAsyncKeyState(VK_KEY_S))
			{
				rot = ENTITY::GET_ENTITY_ROTATION(_ped, 0);
				Vector3 to = get_coords_infront_of_coords(playerCoords, rot, -.25f * (speed * 1.5f));
				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(_ped, to.x, to.y, to.z, 0, 0, 0);
			}

			//left
			if (GetAsyncKeyState(VK_KEY_A))
				ENTITY::SET_ENTITY_HEADING(_ped, ENTITY::GET_ENTITY_HEADING(_ped) + 2.5f * 2.f);

			//right
			if (GetAsyncKeyState(VK_KEY_D))
				ENTITY::SET_ENTITY_HEADING(_ped, ENTITY::GET_ENTITY_HEADING(_ped) - 2.5f * 2.f);

			//up
			if (GetAsyncKeyState(VK_SPACE))
			{
				Vector3 to = get_coords_above_coords(playerCoords, .2f * speed);
				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(_ped, to.x, to.y, to.z, 0, 0, 0);
			}

			//down
			if (GetAsyncKeyState(VK_CONTROL))
			{
				Vector3 to = get_coords_above_coords(playerCoords, -.2f * speed);
				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(_ped, to.x, to.y, to.z, 0, 0, 0);
			}
		}
		else
			ENTITY::SET_ENTITY_COLLISION(_ped, TRUE, TRUE);
	}

	void features_weapon()
	{
		if (features::Wfeatures_infammo)
		{
			Hash cur;
			if (WEAPON::GET_CURRENT_PED_WEAPON(ped, &cur, 1))
			{
				if (WEAPON::IS_WEAPON_VALID(cur))
				{
					int maxAmmo;
					if (WEAPON::GET_MAX_AMMO(ped, cur, &maxAmmo))
					{
						WEAPON::SET_PED_AMMO(ped, cur, maxAmmo);

						maxAmmo = WEAPON::GET_MAX_AMMO_IN_CLIP(ped, cur, 1);
						if (maxAmmo > 0)
							WEAPON::SET_AMMO_IN_CLIP(ped, cur, maxAmmo);
					}
				}
			}
		}

		if (features::Wfeatures_instakill)
		{
			Vector3 CamCoords = CAM::_GET_GAMEPLAY_CAM_COORDS();
			Vector3 CamRotation = CAM::GET_GAMEPLAY_CAM_ROT(0);
			Vector3 CamDirection = wRotationToDirection(CamRotation);
			Vector3 StartCoords = waddVector(CamCoords, (wmultiplyVector(CamDirection, 1.0f)));
			Vector3 EndCoords = waddVector(StartCoords, wmultiplyVector(CamDirection, 500.0f));
			if (PED::IS_PED_SHOOTING(ped))
			{
				for (int i = 0; i < 6; i++)
					GAMEPLAY::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(StartCoords.x, StartCoords.y, StartCoords.z, EndCoords.x, EndCoords.y, EndCoords.z, 250, true, GAMEPLAY::GET_HASH_KEY("WEAPON_REMOTESNIPER"), ped, true, true, -1.0f);
			}
		}

		//TODO: update to account for instakill being enabled
		if (features::Wfeatures_autoshoot) //need to fix inaccuracy when firing at ped in vehicle
		{
			Entity AimedAtEntity;
			if (PLAYER::GET_ENTITY_PLAYER_IS_FREE_AIMING_AT(player, &AimedAtEntity))
			{
				if (ENTITY::IS_ENTITY_A_PED(AimedAtEntity) && !ENTITY::IS_ENTITY_DEAD(AimedAtEntity) && ENTITY::GET_ENTITY_ALPHA(AimedAtEntity) == 255)
				{
					Vector3 Mouth = PED::GET_PED_BONE_COORDS(AimedAtEntity, 31086, 0.1f, 0.0f, 0.0f);
					PED::SET_PED_SHOOTS_AT_COORD(ped, Mouth.x, Mouth.y, Mouth.z, true);
				}
			}
		}

		if (features::Wfeatures_addweapons)
		{
			log_map("Adding all weapons to weapon wheel", logtype::LOG_INFO);

			for (int i = 0; i < (sizeof(features::Weapons) / 4); i++)
			{
				WEAPON::GIVE_DELAYED_WEAPON_TO_PED(ped, features::Weapons[i], 9999, 1);
				features::Wfeatures_addweapons = false;
			}
		}
	}

	void features_vehicle_delay()
	{
		/// <summary>
		/// Vehicle godmode, this is very messy, a few redeclarations, will redo eventually
		/// </summary>

		Vehicle v = PED::GET_VEHICLE_PED_IS_IN(ped, player);

		if (features::Vfeatures_godmode) //a little cleaner but still messy
		{
			if (features::owns_veh(ped)) //boss patch
			{
				ENTITY::SET_ENTITY_INVINCIBLE(v, TRUE);
				ENTITY::SET_ENTITY_CAN_BE_DAMAGED(v, FALSE);
				ENTITY::SET_ENTITY_PROOFS(v, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);
				ENTITY::SET_ENTITY_MAX_SPEED(v, 1000000000);
				VEHICLE::SET_VEHICLE_EXPLODES_ON_HIGH_EXPLOSION_DAMAGE(v, FALSE);
				VEHICLE::SET_VEHICLE_WHEELS_CAN_BREAK(v, FALSE);

				VEHICLE::SET_VEHICLE_ENGINE_HEALTH(v, 1000.0f);
				VEHICLE::SET_VEHICLE_UNDRIVEABLE(v, FALSE);
				VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(v, FALSE);
				VEHICLE::SET_VEHICLE_WHEELS_CAN_BREAK(v, FALSE);
				if (VEHICLE::_HAS_VEHICLE_ROCKET_BOOST(v))
				{
					VEHICLE::_SET_VEHICLE_ROCKET_BOOST_REFILL_TIME(v, 0);
					VEHICLE::_SET_VEHICLE_ROCKET_BOOST_PERCENTAGE(v, 100.0f);
				}
			}
		}
		else
		{
			if (features::owns_veh(ped))
			{
				ENTITY::SET_ENTITY_INVINCIBLE(v, FALSE);
				ENTITY::SET_ENTITY_CAN_BE_DAMAGED(v, TRUE);
				ENTITY::SET_ENTITY_PROOFS(v, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE);
				VEHICLE::SET_VEHICLE_EXPLODES_ON_HIGH_EXPLOSION_DAMAGE(v, TRUE);
				VEHICLE::SET_VEHICLE_WHEELS_CAN_BREAK(v, TRUE);
			}
		}

		if (features::Vfeatures_autoclean)
		{
			if (features::owns_veh(ped))
			{
				Vehicle _v = PED::GET_VEHICLE_PED_IS_USING(scriptIndex);
				VEHICLE::SET_VEHICLE_FIXED(_v);
				VEHICLE::SET_VEHICLE_DEFORMATION_FIXED(_v);
				VEHICLE::SET_VEHICLE_DIRT_LEVEL(_v, 0);
				VEHICLE::SET_VEHICLE_CAN_BE_VISIBLY_DAMAGED(v, FALSE);
			}

		}
		else
		{
			if (features::owns_veh(ped))
				VEHICLE::SET_VEHICLE_CAN_BE_VISIBLY_DAMAGED(v, TRUE);
		}
	}

	void features_vehicle()
	{
		if (features::Vfeatures_requestentity)
		{
			if (features::Vfeatures_randomizeveh)
				log_map("Importing random vehicle model", logtype::LOG_INFO);
			else
				log_map(fmt::format("Importing vehicle model ~g~{}", features::carToSpawn), logtype::LOG_INFO);
			features::spawnvehicle(features::carToSpawn.c_str());
			features::Vfeatures_requestentity = false;
		}

		if (features::Vfeatures_autoupgrade)
		{
			if (PED::IS_PED_IN_ANY_VEHICLE(ped, FALSE))
			{
				if (features::owns_veh(ped))
				{
					log_map("Current vehicle has been fully upgraded", logtype::LOG_INFO);

					VEHICLE::SET_VEHICLE_MOD_KIT(vehicle, 0);

					for (int i = 0; i < 48; i++) //49, 50 = livery
					{
						VEHICLE::TOGGLE_VEHICLE_MOD(vehicle, i, TRUE);
						VEHICLE::SET_VEHICLE_MOD(vehicle, i, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, i) - 1, FALSE);
					}

					for (int i = 0; i < 8; i++)
						VEHICLE::_SET_VEHICLE_NEON_LIGHT_ENABLED(vehicle, i, 1);
				}
				else
					log_map("You are not the owner of the vehicle", logtype::LOG_ERROR);
			}
			else
				log_map("You are not in a vehicle", logtype::LOG_ERROR);

			features::Vfeatures_autoupgrade = false;
		}

		if (features::Vfeatures_hornboost)
		{
			if (PED::IS_PED_IN_ANY_VEHICLE(ped, 0))
			{
				if (features::owns_veh(ped) && PLAYER::IS_PLAYER_PRESSING_HORN(player))
				{
					RequestControlOfEnt(vehicle);
					if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(vehicle))
						VEHICLE::SET_VEHICLE_FORWARD_SPEED(vehicle, 79);
				}
			}
		}
	}

	void cache_player(int i)
	{
		int netHandle[13];
		auto ped = PLAYER::GET_PLAYER_PED(i);
		auto netPlayer = getNetGamePlayer(i);
		Vector3 pos = ENTITY::GET_ENTITY_COORDS(ped, 1);
		Vector3 mycoords = ENTITY::GET_ENTITY_COORDS(ped, 1);

		if (netPlayer)
		{
			players[i].player = netPlayer;
			players[i].netData = netPlayer->get_net_data();

			players[i].info = netPlayer->m_PlayerInfo;

			if (players[i].netData)
				players[i].rockstarId2 = players[i].netData->m_rockstar_id;

			if (players[i].info && players[i].info->ped)
			{
				//players[i].invincible = players[i].info->ped->m_godmode;
				players[i].rockstarId3 = players[i].info->rockstarID;
			}

			players[i].isSessionHost = netPlayer->is_host();
		}

		if (local)
		{
			localInfo = local->m_PlayerInfo;
			//if (localInfo)
				//localCPed = localInfo->ped;
		}

		//cache local info real quick
		if (i == player) { localPed = ped;	localIndex = i; }

		NETWORK::NETWORK_HANDLE_FROM_PLAYER(i, netHandle, 13);

		players[i].ped = ped;
		players[i].index = i;
		players[i].exists = ENTITY::DOES_ENTITY_EXIST(ped);;
		players[i].name = PLAYER::GET_PLAYER_NAME(i);
		players[i].invehicle = PED::IS_PED_IN_ANY_VEHICLE(PLAYER::GET_PLAYER_PED(i), 0);
		players[i].interior = INTERIOR::GET_INTERIOR_FROM_ENTITY(ped);
		players[i].isfriend = isPlayerFriend(i);
		players[i].mutedMe = NETWORK::NETWORK_AM_I_MUTED_BY_PLAYER(i);
		players[i].blockedMe = NETWORK::NETWORK_AM_I_BLOCKED_BY_PLAYER(i);
		players[i].connected = NETWORK::NETWORK_IS_PLAYER_CONNECTED(i);
		players[i].active = NETWORK::NETWORK_IS_PLAYER_ACTIVE(i);
		players[i].health = ENTITY::GET_ENTITY_HEALTH(ped);
		players[i].maxHealth = ENTITY::GET_ENTITY_MAX_HEALTH(ped);
		players[i].rank = *script_global(1590682).at(i, 883).at(211).at(6).as<int32_t*>();
		players[i].RP = *script_global(1590682).at(i, 883).at(211).at(1).as<int32_t*>();
		players[i].cash = *script_global(1590682).at(i, 883).at(211).at(3).as<int32_t*>();
		players[i].visible = ENTITY::HAS_ENTITY_CLEAR_LOS_TO_ENTITY(ped, PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(player), 17);
		players[i].distanceToLocal = GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(pos.x, pos.y, pos.z, mycoords.x, mycoords.y, mycoords.z, true);
		players[i].rockstarId = NETWORK::NETWORK_MEMBER_ID_FROM_GAMER_HANDLE(&netHandle[0]);

		//store bone maxtrix for each player
		storeSkeleton(ped, SKEL_Head, &players[i].skeleton.head);
		storeSkeleton(ped, SKEL_Neck_1, &players[i].skeleton.neck1);
		storeSkeleton(ped, SKEL_Spine0, &players[i].skeleton.spine0);
		storeSkeleton(ped, SKEL_Spine1, &players[i].skeleton.spine1);
		storeSkeleton(ped, SKEL_Spine2, &players[i].skeleton.spine2);
		storeSkeleton(ped, SKEL_Spine3, &players[i].skeleton.spine3);
		storeSkeleton(ped, SKEL_Pelvis, &players[i].skeleton.pelvis);
		storeSkeleton(ped, SKEL_L_Thigh, &players[i].skeleton.SKEL_L_Thigh);
		storeSkeleton(ped, SKEL_L_Calf, &players[i].skeleton.SKEL_L_Calf);
		storeSkeleton(ped, SKEL_L_Foot, &players[i].skeleton.SKEL_L_Foot);
		storeSkeleton(ped, SKEL_L_Toe0, &players[i].skeleton.SKEL_L_Toe0);
		storeSkeleton(ped, SKEL_R_Thigh, &players[i].skeleton.SKEL_R_Thigh);
		storeSkeleton(ped, SKEL_R_Calf, &players[i].skeleton.SKEL_R_Calf);
		storeSkeleton(ped, SKEL_R_Foot, &players[i].skeleton.SKEL_R_Foot);
		storeSkeleton(ped, SKEL_R_Toe0, &players[i].skeleton.SKEL_R_Toe0);
		storeSkeleton(ped, SKEL_L_Clavicle, &players[i].skeleton.SKEL_L_Clavicle);
		storeSkeleton(ped, SKEL_L_UpperArm, &players[i].skeleton.SKEL_L_UpperArm);
		storeSkeleton(ped, SKEL_L_Forearm, &players[i].skeleton.SKEL_L_Forearm);
		storeSkeleton(ped, SKEL_L_Hand, &players[i].skeleton.SKEL_L_Hand);
		storeSkeleton(ped, SKEL_R_Clavicle, &players[i].skeleton.SKEL_R_Clavicle);
		storeSkeleton(ped, SKEL_R_UpperArm, &players[i].skeleton.SKEL_R_UpperArm);
		storeSkeleton(ped, SKEL_R_Forearm, &players[i].skeleton.SKEL_R_Forearm);
		storeSkeleton(ped, SKEL_R_Hand, &players[i].skeleton.SKEL_R_Hand);
	}

	void features_esp(int p)
	{
		int handle = PLAYER::GET_PLAYER_PED(p);
		auto pname = PLAYER::GET_PLAYER_NAME(p);

		Vector3 theircoords = ENTITY::GET_ENTITY_COORDS(handle, TRUE);
		Vector3 mycoords = ENTITY::GET_ENTITY_COORDS(ped, TRUE);

		float xoffset, yoffset;

		float distance = GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(theircoords.x, theircoords.y, theircoords.z, mycoords.x, mycoords.y, mycoords.z, true);

		float ColR, ColG, ColB;
		int maxHealth = features::players[p].maxHealth - 100;
		int health = std::clamp(features::players[p].health - 100, 0, maxHealth);

		Vector3 healthColor = features::FromHSB(std::clamp((float)(health) / (float)(maxHealth * 3.6f), 0.f, 0.277777777778f), 1.f, 1.f);

		features::ESPfeatures_health ? (ColR = healthColor.x, ColG = healthColor.y, ColB = healthColor.z) : (ColR = features::ESPfeatures_col[0] * 255, ColG = features::ESPfeatures_col[1] * 255, ColB = features::ESPfeatures_col[2] * 255);

		if (!ENTITY::DOES_ENTITY_EXIST(handle))
			return;

		if (p == player)
			return;

		if (distance > 10000)
			return;

		if (features::ESPfeatures_visible && !ENTITY::HAS_ENTITY_CLEAR_LOS_TO_ENTITY(ped, PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(p), 17))
			return;

		GRAPHICS::GET_SCREEN_COORD_FROM_WORLD_COORD(theircoords.x, theircoords.y, theircoords.z + 1.3f, &xoffset, &yoffset);

		if (big::features::ESPfeatures)
		{
			if (PLAYER::GET_PLAYER_NAME(p))
			{
				UI::SET_TEXT_CENTRE(true);
				UI::SET_TEXT_COLOUR(ColR, ColG, ColB, 255);
				UI::SET_TEXT_FONT(0);
				UI::SET_TEXT_SCALE(0.21f, 0.21f);
				UI::SET_TEXT_DROPSHADOW(1, 0, 0, 0, 0);
				UI::SET_TEXT_EDGE(1, 0, 0, 0, 0);
				UI::SET_TEXT_OUTLINE();
				UI::BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
				UI::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(fmt::format("[{}]m~n~{}", (int)distance, pname).c_str());
				UI::END_TEXT_COMMAND_DISPLAY_TEXT(xoffset, yoffset);
			}

			GRAPHICS::DRAW_LINE(mycoords.x, mycoords.y, mycoords.z, theircoords.x, theircoords.y, theircoords.z, ColR, ColG, ColB, 255); //snapline
			GRAPHICS::DRAW_MARKER(20, theircoords.x, theircoords.y, theircoords.z + 2.0f, 0, 0, 0, 180.0f, 360.0f, 0, 1.5f, 1.5f, 1.5f, ColR, ColG, ColB, 100, true, false, 2, true, false, false, false); //marker

			//box
			GRAPHICS::DRAW_LINE(theircoords.x - 0.5f, theircoords.y + 0.5f, theircoords.z + 1.f, theircoords.x + 0.5f, theircoords.y + 0.5f, theircoords.z + 1.f, ColR, ColG, ColB, 255);
			GRAPHICS::DRAW_LINE(theircoords.x - 0.5f, theircoords.y + 0.5f, theircoords.z + 1.f, theircoords.x - 0.5f, theircoords.y + 0.5f, theircoords.z - 0.9f, ColR, ColG, ColB, 255);
			GRAPHICS::DRAW_LINE(theircoords.x + 0.5f, theircoords.y + 0.5f, theircoords.z + 1.f, theircoords.x + 0.5f, theircoords.y + 0.5f, theircoords.z - 0.9f, ColR, ColG, ColB, 255);
			GRAPHICS::DRAW_LINE(theircoords.x - 0.5f, theircoords.y + 0.5f, theircoords.z - 0.9f, theircoords.x + 0.5f, theircoords.y + 0.5f, theircoords.z - 0.9f, ColR, ColG, ColB, 255);
			GRAPHICS::DRAW_LINE(theircoords.x - 0.5f, theircoords.y - 0.5f, theircoords.z + 1.f, theircoords.x + 0.5f, theircoords.y - 0.5f, theircoords.z + 1.f, ColR, ColG, ColB, 255);
			GRAPHICS::DRAW_LINE(theircoords.x - 0.5f, theircoords.y - 0.5f, theircoords.z + 1.f, theircoords.x - 0.5f, theircoords.y - 0.5f, theircoords.z - 0.9f, ColR, ColG, ColB, 255);
			GRAPHICS::DRAW_LINE(theircoords.x + 0.5f, theircoords.y - 0.5f, theircoords.z + 1.f, theircoords.x + 0.5f, theircoords.y - 0.5f, theircoords.z - 0.9f, ColR, ColG, ColB, 255);
			GRAPHICS::DRAW_LINE(theircoords.x - 0.5f, theircoords.y - 0.5f, theircoords.z - 0.9f, theircoords.x + 0.5f, theircoords.y - 0.5f, theircoords.z - 0.9f, ColR, ColG, ColB, 255);
			GRAPHICS::DRAW_LINE(theircoords.x + 0.5f, theircoords.y - 0.5f, theircoords.z - 0.9f, theircoords.x + 0.5f, theircoords.y + 0.5f, theircoords.z - 0.9f, ColR, ColG, ColB, 255);
			GRAPHICS::DRAW_LINE(theircoords.x - 0.5f, theircoords.y - 0.5f, theircoords.z - 0.9f, theircoords.x - 0.5f, theircoords.y + 0.5f, theircoords.z - 0.9f, ColR, ColG, ColB, 255);
			GRAPHICS::DRAW_LINE(theircoords.x + 0.5f, theircoords.y - 0.5f, theircoords.z + 1.f, theircoords.x + 0.5f, theircoords.y + 0.5f, theircoords.z + 1.f, ColR, ColG, ColB, 255);
			GRAPHICS::DRAW_LINE(theircoords.x - 0.5f, theircoords.y - 0.5f, theircoords.z + 1.f, theircoords.x - 0.5f, theircoords.y + 0.5f, theircoords.z + 1.f, ColR, ColG, ColB, 255);

		}
	}

	void features_online()
	{
		for (std::uint32_t i = 0; i < 32; ++i) //initialize functions for each player in the lobby
		{
			cache_player(i);
			features_esp(i);
		}

		if (features::Lfeatures_nophone)
		{
			misc::set_global(19664, 1);
			MOBILE::_CELL_CAM_DISABLE_THIS_FRAME(TRUE);
			PAD::DISABLE_CONTROL_ACTION(2, INPUT_PHONE, TRUE);
		}

		if (features::Lfeatures_nomental)
			STATS::STAT_SET_FLOAT(GAMEPLAY::GET_HASH_KEY("MP0_PLAYER_MENTAL_STATE"), 0.1, 1);

		if (features::Ofeatures_skipcutscene)
		{
			if (CUTSCENE::IS_CUTSCENE_ACTIVE() || CUTSCENE::IS_CUTSCENE_PLAYING())
			{
				CUTSCENE::STOP_CUTSCENE;
				CUTSCENE::STOP_CUTSCENE_IMMEDIATELY();
			}
			else
				log_map("No cutscene is currently active", logtype::LOG_ERROR);

			features::Ofeatures_skipcutscene = false;
		}

	}

	void features_player()
	{
		if (features::Pfeatures_teleport)
		{
			Ped handle = ped; //to prevent redeclaration confusion autism
			Vector3 coords = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED(features::selectedPlayer), FALSE);
			int player = PLAYER::GET_PLAYER_PED(features::selectedPlayer);

			if (PED::IS_PED_IN_ANY_VEHICLE(handle, FALSE))
			{
				if (features::owns_veh(handle))
					handle = PED::GET_VEHICLE_PED_IS_IN(handle, vehicle);
				ENTITY::SET_ENTITY_COORDS(handle, coords.x, coords.y, coords.z, 0, 0, 0, 1);
			}
			else if (PED::IS_PED_IN_ANY_VEHICLE(player, FALSE))
			{
				bool foundSeat = false;
				Vehicle playersVehicle = PED::GET_VEHICLE_PED_IS_IN(player, false);
				if (ENTITY::DOES_ENTITY_EXIST(playersVehicle) && ENTITY::IS_ENTITY_A_VEHICLE(playersVehicle))
				{
					for (int i = 0; i < VEHICLE::GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(playersVehicle); i++)
					{
						if (VEHICLE::IS_VEHICLE_SEAT_FREE(playersVehicle, i))
						{
							RequestControlOfEnt(playersVehicle);
							PED::SET_PED_INTO_VEHICLE(handle, playersVehicle, i);
							foundSeat = true;
							break;
						}
					}

					if (!foundSeat)
					{
						if (PED::IS_PED_IN_ANY_VEHICLE(handle, FALSE))
							handle = PED::GET_VEHICLE_PED_IS_IN(handle, 0);

						ENTITY::SET_ENTITY_COORDS(handle, coords.x, coords.y, coords.z, 0, 0, 0, 1);
					}
				}
			}
			else
				ENTITY::SET_ENTITY_COORDS(handle, coords.x, coords.y, coords.z, 0, 0, 0, 1);

			log_map(fmt::format("Teleporting to ~g~{}", features::players[features::selectedPlayer].name), logtype::LOG_INFO);
			features::Pfeatures_teleport = false;
		}
	}

	void dec_var()
	{
		player = PLAYER::PLAYER_ID();
		ped = PLAYER::PLAYER_PED_ID();
		local = getNetGamePlayer(player);
		inSession = NETWORK::NETWORK_IS_IN_SESSION();
		sessionActive = NETWORK::NETWORK_IS_SESSION_ACTIVE();
		scriptIndex = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(player);
		vehicle = PED::GET_VEHICLE_PED_IS_IN(ped, player); //calls a function that checks if ur in a vehicle, what seat, and if you're the owner. will make everything else easier.

		if (inSession)
		{
			numberOfPlayers = PLAYER::GET_NUMBER_OF_PLAYERS();
			scriptHost = NETWORK::NETWORK_GET_HOST_OF_SCRIPT("freemode", -1, 0);
		}
	}

	bool features::owns_veh(Ped _ped) //move somewhere else
	{
		Vehicle vehicle = PED::GET_VEHICLE_PED_IS_IN(_ped, player);
		if (ENTITY::DOES_ENTITY_EXIST(vehicle) || ENTITY::IS_ENTITY_A_VEHICLE(vehicle))
		{
			if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(vehicle) && VEHICLE::GET_PED_IN_VEHICLE_SEAT(vehicle, -1) == _ped)
				return true;
		}
		return false;
	}

	void run_tick()
	{
		dec_var(); //global variable declaration function, see above

		QUEUE_JOB_BEGIN_CLAUSE()
		{
			static ULONGLONG tick_1 = 0;
			static ULONGLONG tick_60 = 0;
			ULONGLONG now = GetTickCount64();

			if (now - tick_60 > 60000)
			{
				block_main();
				show_stat();
				tick_60 = now;
			}

			if (now - tick_1 > 1000) //prevents vehicle seizures & allows you to flip your vehicle
			{
				features_vehicle_delay();
				tick_1 = now;
			}

			features_vehicle();
			features_local();
			features_weapon();

			if (inSession)
			{
				features_online();
				features_player();
			}

		} QUEUE_JOB_END_CLAUSE
	}

	void script_func()
	{
		while (true)
		{
			TRY_CLAUSE{ run_tick(); }
				EXCEPT_CLAUSE
				script::get_current()->yield();
		}
	}
}
