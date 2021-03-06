#pragma once
#include "natives.hpp"
#include "gta/joaat.hpp"
#include "gta/enums.hpp"
#include "gta/replay.hpp"
#include "gta/player.hpp"
#define FIND(value, list) misc::find(value, sizeof(list) / sizeof(list[0]), list)
#define IS(value, list) FIND(value, list)
#define LEN(list) (sizeof(list) / sizeof(list[0]))

using namespace std;
using namespace fmt;

namespace big::misc
{
	const bool LOG_NATIVE = false;

	extern void log_clone(bool __log, rage::CNetGamePlayer* src, const char* type, int16_t object_type, int16_t object_id, int16_t object_flag, int32_t timestamp, int32_t ack_code, bool blocked, const char* status);
	extern void log_generic(bool __log, const char* type, bool blocked, const char* status);
	extern void log_buff(bool __log, int32_t sync_type, int16_t object_type, int index, int length, uint32_t value, bool blocked, const char* status);
	extern void log_model(bool __log, const char* type, int16_t object_type, uint32_t object_hash, bool blocked, const char* status);

	extern void log_network_event(bool __log, rage::CNetGamePlayer* src, int16_t event_type, int32_t event_id, int32_t bitset, int32_t n, uint8_t* data, bool block, const char* status);
	extern void log_script_event(bool __log, int32_t group, int32_t event_id, int64_t n, int64_t* data, bool block);

	extern bool model_valid(rage::netSyncTree* netSyncTree, int16_t object_type);
	
	extern bool object_location(rage::netSyncTree* netSyncTree, int32_t sync_type, int32_t sync_flag, int16_t object_type);
	extern bool object_close(rage::netSyncTree* netSyncTree, int32_t sync_type, int32_t sync_flag, int16_t object_type);
	extern bool object_limit(int16_t object_type);

	extern bool flood_create(rage::CNetGamePlayer* src, int32_t sync_type);
	extern bool flood_script(rage::CNetGamePlayer* src, int32_t sync_type, int32_t sync_flag);
	extern bool flood_respawn(rage::CNetGamePlayer* src, int16_t event_type);
	extern bool flood_pack(rage::CNetGamePlayer* src);
	extern bool flood_event(rage::CNetGamePlayer* src, int16_t event_type, uint32_t bitset);
	extern bool event_time(rage::CNetGamePlayer* src, int32_t timestamp);

	extern bool block_proto(rage::CNetGamePlayer* src, int32_t sync_type, int16_t object_type, int16_t object_id);
	extern bool block_proto(rage::CNetGamePlayer* src, int16_t event_type);
	extern bool block_user(rage::CNetGamePlayer* src, bool blocked = false);
	extern bool block_crash(int32_t n, const uint8_t* data);

	extern void set_global(uint32_t address, int64_t value);

	extern bool value(int32_t event_id);
	extern bool range(int64_t script);
	extern bool range(int32_t n, const int64_t* data);

	extern uint32_t read32(uint8_t* data, int pos, int len);
	extern void* vtable(void* object, int function);
	extern void vtable(void* object, int function, void* detour);
	extern string pointer(void* function);
	extern string get_address(uint32_t ip, uint16_t port);

	extern string CSV(int8_t i);
	extern string CSV(uint8_t i);
	extern string CSV(int32_t i);
	extern string CSV(uint32_t i);
	extern string CSV(int64_t i);
	extern string CSV(uint64_t i);
	extern string CSV(float i);
	extern string CSV(string s);

	extern void log_green(bool __log, const char* _log, bool warn);
	extern void log_blue(bool __log, const char* _log, bool warn);

	extern void NETWORK_CAN_BAIL(rage::scrNativeCallContext* src);
	extern void REQUEST_SCRIPT(rage::scrNativeCallContext* src);
	extern void NETWORK_SESSION_GET_KICK_VOTE(rage::scrNativeCallContext* src);
	extern void STAT_SET_BOOL_MASKED(rage::scrNativeCallContext* src);
	extern void STAT_SET_INT(rage::scrNativeCallContext* src);

	const int64_t blocked_user[] = { -2, -1, 0, 1, 2 };

	const int64_t blocked_script_0[] = { -1 };

	const int64_t blocked_script_2[] = { 7 };

	const string blocked_script[] = { "DISABLED-MainTransition", "DISABLED-cellphone_controller" };

	template<typename T>
	bool zero(int32_t n, const T* data)
	{
		for (int32_t i = 0; i < n; i++)
		{
			if (data[i] != 0)
				return false;
		}
		return true;
	}

	template<typename T>
	bool find(T item, int n, const T* data)
	{
		for (int i = 0; i < n; i++)
		{
			if (data[i] == item)
				return true;
		}
		return false;
	}
	template<typename T>
	int index(T item, int n, const T* data)
	{
		for (int i = 0; i < n; i++)
		{
			if (data[i] == item)
				return i;
		}
		return -1;
	}

	static unordered_map<rage::scrNativeHash, rage::scrNativeHandler> natives_replace =
	{
		{ 0x580CE4438479CC61, &NETWORK_CAN_BAIL},
		{ 0x6EB5F71AA68F2E8E, &REQUEST_SCRIPT},
		{ 0xD6D09A6F32F49EF1, &NETWORK_SESSION_GET_KICK_VOTE},
		{ 0x5BC62EC1937B9E5B, &STAT_SET_BOOL_MASKED},
		{ 0xB3271D7AB655B441, &STAT_SET_INT},
	};

	const uint32_t blocked_stat[] =
	{
		RAGE_JOAAT("MPPLY_BECAME_CHEATER_NUM"),RAGE_JOAAT("MPPLY_GRIEFING"),RAGE_JOAAT("MPPLY_VC_ANNOYINGME"),RAGE_JOAAT("MPPLY_VC_HATE"),RAGE_JOAAT("MPPLY_TC_ANNOYINGME"),RAGE_JOAAT("MPPLY_TC_HATE"),
		RAGE_JOAAT("MPPLY_OFFENSIVE_LANGUAGE"),RAGE_JOAAT("MPPLY_OFFENSIVE_TAGPLATE"),RAGE_JOAAT("MPPLY_OFFENSIVE_UGC"),RAGE_JOAAT("MPPLY_BAD_CREW_NAME"),
		RAGE_JOAAT("MPPLY_BAD_CREW_MOTTO"),RAGE_JOAAT("MPPLY_BAD_CREW_STATUS"),RAGE_JOAAT("MPPLY_BAD_CREW_EMBLEM"),RAGE_JOAAT("MPPLY_GAME_EXPLOITS"),RAGE_JOAAT("MPPLY_EXPLOITS"),
		RAGE_JOAAT("MPPLY_DM_CHEAT_START"),RAGE_JOAAT("MPPLY_DM_CHEAT_END"),RAGE_JOAAT("MPPLY_DM_CHEAT_QUIT"),RAGE_JOAAT("MPPLY_RACE_CHEAT_START"),RAGE_JOAAT("MPPLY_RACE_CHEAT_END"),
		RAGE_JOAAT("MPPLY_RACE_CHEAT_QUIT"),RAGE_JOAAT("MPPLY_MC_CHEAT_START"),RAGE_JOAAT("MPPLY_MC_CHEAT_END"),RAGE_JOAAT("MPPLY_MC_CHEAT_QUIT"),RAGE_JOAAT("MPPLY_MGAME_CHEAT_START"),
		RAGE_JOAAT("MPPLY_MGAME_CHEAT_END"),RAGE_JOAAT("MPPLY_MGAME_CHEAT_QUIT"),RAGE_JOAAT("MPPLY_VOTED_OUT"),RAGE_JOAAT("MPPLY_VOTED_OUT_DELTA"),RAGE_JOAAT("MPPLY_VOTED_OUT_QUIT"),
		RAGE_JOAAT("MPPLY_CAP_CHEAT_START"),RAGE_JOAAT("MPPLY_CAP_CHEAT_END"),RAGE_JOAAT("MPPLY_CAP_CHEAT_QUIT"),RAGE_JOAAT("MPPLY_SUR_CHEAT_START"),RAGE_JOAAT("MPPLY_SUR_CHEAT_END"),
		RAGE_JOAAT("MPPLY_SUR_CHEAT_QUIT"),RAGE_JOAAT("MPPLY_LTS_CHEAT_START"),RAGE_JOAAT("MPPLY_LTS_CHEAT_END"),RAGE_JOAAT("MPPLY_LTS_CHEAT_QUIT"),RAGE_JOAAT("MPPLY_PARA_CHEAT_START"),
		RAGE_JOAAT("MPPLY_PARA_CHEAT_END"),RAGE_JOAAT("MPPLY_PARA_CHEAT_QUIT"),RAGE_JOAAT("MPPLY_FMEVN_CHEAT_START"),RAGE_JOAAT("MPPLY_FMEVN_CHEAT_END"),RAGE_JOAAT("MPPLY_FMEVN_CHEAT_QUIT"),
	};

	const int64_t blocked_malev[] =
	{
		24902297,
		42685294,
		235171324,
		329939175,
		368550800,
		457967755,
		572063080,
		616006309,
		628662130,
		643374672,
		792221348,
		807875052,
		899228776,
		937772107,
		940943685,
		966612367,
		1080719624,
		1170588613,
		1219453777,
		1224100642,
		1249283253,
		1263226800,
		1272803094,
		1301287696,
		1401650071,
		1550611612,
		1597093459,
		1769871776,
		1796459838,
		1815270123,
		1980349969,
		2088750491,
		-60338860,
		-130843390,
		-199171978,
		-228041614,
		-328035840,
		-378461067,
		-452181427,
		-584961562,
		-631911105,
		-687617715,
		-722822506,
		-746774737,
		-812944463,
		-847811454,
		-857707587,
		-977347227,
		-985047251,
		-996700057,
		-1096495395,
		-1097543898,
		-1153175946,
		-1179558480,
		-1401804168,
		-1413108537,
		-1428075016,
		-1447352303,
		-1513913454,
		-1561952511,
		-1735153315,
		-1769069349,
		-1827882671,
		-1853459190,
		-1860492113,
		-2044296061,
		-2074781016,
		-10404073,
		-72590627,
		-89580849,
		-154480245,
		-161376970,
		-182074418,
		-184087091,
		-189246460,
		-208525380,
		-224978202,
		-228458318,
		-240257097,
		-243648045,
		-319516276,
		-334565594,
		-338399516,
		-338772048,
		-345908092,
		-355297939,
		-365271546,
		-375398087,
		-402069102,
		-402413174,
		-410464896,
		-438498801,
		-443787204,
		-493570582,
		-505351260,
		-547753294,
		-551637263,
		-577286683,
		-580260344,
		-591321928,
		-620075057,
		-627821011,
		-638863591,
		-666433007,
		-689821038,
		-717392224,
		-727492889,
		-730739328,
		-732021738,
		-743046526,
		-777834996,
		-801305185,
		-803719896,
		-810849546,
		-813003734,
		-833978318,
		-872920517,
		-882105348,
		-886994897,
		-891346918,
		-894602154,
		-911939009,
		-920707230,
		-948169674,
		-953155807,
		-1014199946,
		-1021838014,
		-1031098030,
		-1042174840,
		-1064189834,
		-1088196937,
		-1091676304,
		-1095309955,
		-1138756484,
		-1161523894,
		-1162153263,
		-1186250586,
		-1203311834,
		-1229987069,
		-1254364429,
		-1287716800,
		-1290202385,
		-1295633605,
		-1312277212,
		-1320195679,
		-1329008922,
		-1329398309,
		-1336925360,
		-1336932351,
		-1365544809,
		-1372360252,
		-1378191490,
		-1379398115,
		-1394589232,
		-1405248250,
		-1437387234,
		-1451310145,
		-1455659794,
		-1459006650,
		-1493184845,
		-1496922658,
		-1518828167,
		-1542610935,
		-1578719089,
		-1580390976,
		-1596113101,
		-1609983577,
		-1657159979,
		-1683107035,
		-1708346067,
		-1720012585,
		-1739477926,
		-1756175141,
		-1756910444,
		-1759105725,
		-1788867013,
		-1789621123,
		-1877166919,
		-1911813629,
		-1918283921,
		-1930763174,
		-1935646665,
		-1961361413,
		-2029779863,
		-2033338628,
		-2051964653,
		-2060629306,
		-2080461139,
		-2130721021,
		-2138224118,
		0x0000e439,
		0x0000ed5a,
		0x0000f67b,
		0x0000ff9c,
		0x0a998180,
		0x0eb2bb0b,
		0x0f4e5749,
		0x0f9dac9f,
		0x0fa985f0,
		0x1bc00b76,
		0x1c4a24ff,
		0x1c40a588,
		0x1d2a75aa,
		0x1d8dae58,
		0x1de2d9a0,
		0x1e837cc4,
		0x1fe3a54c,
		0x2a5dd030,
		0x2c32c846,
		0x2d98f9e6,
		0x2e6ba1f3,
		0x2e555be8,
		0x3b2f85f1,
		0x3ba1c66a,
		0x3d132397,
		0x3f98a3ca,
		0x3ffa22a5,
		0x4c4c8fcb,
		0x4c130127,
		0x4e1808a5,
		0x4efc49d7,
		0x4f5b7d5c,
		0x5c8b641d,
		0x5c1810d8,
		0x006a3dcc,
		0x6d5e8ece,
		0x6dc9d2f1,
		0x6de3263f,
		0x6f9a2e80,
		0x7aebeea8,
		0x7afefaef,
		0x7d1c023d,
		0x08f54e0c,
		0x8a3ee0fb,
		0x8b17fffb,
		0x8bd47182,
		0x8ca06037,
		0x8ceae45a,
		0x8da94f6f,
		0x8e0c0a03,
		0x9d39bed5,
		0x12c2099e,
		0x16d71869,
		0x18cd3fc7,
		0x0021eb95,
		0x23f136e8,
		0x24b7db37,
		0x24f85a01,
		0x31ce5ed3,
		0x39edfa7a,
		0x42b1910e,
		0x42ba32ca,
		0x046f9da4,
		0x49bbd840,
		0x52e5536f,
		0x64e127f7,
		0x67afd41a,
		0x68a8a4b2,
		0x68aa41d5,
		0x71e5de49,
		0x75a9d574,
		0x76fe5334,
		0x78b139f2,
		0x80ffc703,
		0x83feaead,
		0x85b18113,
		0x86cdb6fc,
		0x98ded49f,
		0x000108bd,
		0x153dc2a3,
		0x182d2435,
		0x202bbcd4,
		0x205e251c,
		0x224db825,
		0x0264ebc1,
		0x520a528f,
		0x537ce55d,
		0x603b15a4,
		0x628ffa2d,
		0x654e2ad9,
		0x0782d4b6,
		0x0792b490,
		0x852d4ac6,
		0x901cb4b9,
		0x997ab0d7,
		0x1908dec0,
		0x3087de54,
		0x3450a499,
		0x8179ebce,
		0x9747ac94,
		0x9752e4db,
		0x9851ac5a,
		0x39466a42,
		0x41269af4,
		0x42681ad9,
		0x43243cfc,
		0x44004d02,
		0x63305ff4,
		0x66409be5,
		0x75454b2e,
		0x95548c7d,
		0x95600e3b,
		0x97262d43,
		0x372505ba,
		0x516576a7,
		0x792814b2,
		0x5281523a,
		0x39498547,
		0x45819668,
		0x56319774,
		0x62548064,
		0x75406977,
		0x87040469,
		0xD3FE818F,
		0xa0dd3f33,
		0xa1cd25c0,
		0xa1e6a88f,
		0xa6ffceb3,
		0xa40da009,
		0xa71b3f7c,
		0xa93c64ee,
		0xa5788579,
		0xa9095346,
		0xaa53361e,
		0xac3d9d06,
		0xadc80e1d,
		0xae9b7097,
		0xae3371c4,
		0xb0c8eee6,
		0xb2c6333b,
		0xb5e26388,
		0xb6afe303,
		0xb7b5a669,
		0xb14f69a1,
		0xb33f0040,
		0xb53beaf3,
		0xb94b40a6,
		0xb846eb26,
		0xb31912ef,
		0xb0500801,
		0xb0502350,
		0xbabaf2d1,
		0xbac48d4a,
		0xbc1ff47c,
		0xc1e1ac88,
		0xc9a4ea3f,
		0xc28ab152,
		0xc38c8976,
		0xc77c1436,
		0xc90a3e81,
		0xc091c076,
		0xc120a0df,
		0xc317fd42,
		0xcaad7456,
		0xcadf201a,
		0xcb21882f,
		0xcbf84a3b,
		0xce4a8032,
		0xcf8a8c2a,
		0xcfab6af6,
		0xd1a32e0c,
		0xd3b60282,
		0xd03d0d9f,
		0xd4a356e7,
		0xd6e22a92,
		0xd9ebb719,
		0xd45e3c16,
		0xd53d76a0,
		0xd471cd80,
		0xd0183528,
		0xda94362d,
		0xdb0a67cf,
		0xdcc124b8,
		0xdd974de5,
		0xdf1eaef1,
		0xdf59f2b2,
		0xe1e0f3a4,
		0xe5dd0a0f,
		0xe99fe139,
		0xe808e992,
		0xea3a6606,
		0xead2956d,
		0xeb61dc84,
		0xebd46ee4,
		0xec0eef26,
		0xf1adf7b7,
		0xf4b85404,
		0xf6cad18b,
		0xf17a39d3,
		0xf2971ae6,
		0xf5070dcd,
		0xf26200b2,
		0xf6619536,
		0xfaa91acf,
		0xfbac5add,
		0xff613f17,
		50707886,
		55291550,
		55862314,
		116877169,
		240366033,
		246182814,
		265723083,
		273438779,
		283386134,
		310529291,
		314232747,
		444549672,
		560832604,
		632950117,
		676333254,
		771284519,
		897484282,
		960137118,
		965054819,
		993865248,
		1085277077,
		1307678422,
		1373384483,
		1592391033,
		1603064898,
		1646559452,
		1746883687,
		1804608241,
		1919870950,
		2099578296,
		1922258436,
		699592812,
		2444332215896,
		1973954729119,
		7304678485445861715,
		1920346205784,
		2379907706456,
		3545859464591534422,
		140698833660827,
		2409476653067,
		1971885813336,
		2294008360536,
		2865239010904,
		1120313136,
	};

	const int64_t blocked_kick[] =
	{
		3,
		-941739545,
		-297770348,
		-803645682,
		-771910813,
		-815817885,
		-531496987,
		-123645928,
		-332594529,
		-498955166,
		-1542848512,
		-4294967296,
		-2063859712,
		-2017629233,
		-1297785021,
		-1818550779,
		-1964956981,
		-1010395481,
		-1933245257,
		-1924332863,
		-1704719905,
		-1246838892,
		-1424895288,
		-1264063129,
		-1941292498,
		297770348,
		498709856,
		380850754,
		143196100,
		774421744,
		709335341,
		732781082,
		693546501,
		149365611,
		423635655,
		400031869,
		1432301416,
		2037380969,
		1818550779,
		1070706073,
		1152017566,
		1667907776,
		4136296512,
		3763470309,
		3030904167,
		1953937033,
		1062544784,
		1476326089,
		1800294560,
		1209128713,
		1062333317,
		20218675039,
		394650472960,
		140698833649664,
		140733193388032,
		4566469881341411328,
		4528819783743622349,
		7218762434461302784,
		140694538682381,
		871878361088,
		1317868303, //https://www.unknowncheats.me/forum/3008244-post2672.html
		-1243454584,
		-1252906024,
		-442306200,
		-966559987,
		1977655521,
		1998625272,
		1070934291,
		-1559754940, // disturbed
		2017765964, // disturbed
		324865135,  // disturbed
		-1212832151,  // disturbed
		-1890951223, // disturbed
		1302185744,  // disturbed
		639032041,  // disturbed
		665709549,
		-2066829834,
		5136154500792667972,
		2653002072128,
		1347850743,
		1428412924,
		-348418057,
		-435067392,
		4539628424389459975,
		1300962917,
		764638896,
		-345371965,
		-125347541,
		2914995077184,
		2910257599647,
		-1726396442,
		154008137,
		428882541,
		-1714354434,
		3574926665,
		1476181033024,
		80347137650685709,
		-142213115619008289,
		136105166828292200,
		140698833649668,
		7596553751065819476,
		8390047103201604173,
		8452798525969887811,
		8247613004187264328,
		2189145604160,
		2176260702272,
		4123759323,
		-2122716210,
		0xE6116600,
		-977515445,
		767605081,
		-1949011582,
		-922075519,
		-1975590661,
		-81613951,
		-1292453789,
		1623637790,
		-1905128202,
		1160415507,
		0xdf7de926,
		-1729804184,
		-1491386500,
		-1882923979,
		-1054826273,
		1620254541,
		1401831542,
		-1730227041,
		0xc5bc4c4b,
		0xB0886E20,
		0x9DB77399,
		1073741824000,
		526625102,
		1394869423,
		-1394869423,
		-1139568479,
		140694538682758,
		-720040631,
		-2147483647,
		4294967295,
		-85635367,
		-887834944,
		-889120084,
		1189794053,
		1310884765,
		1979460190,
		2068861029,
		266760875,
		340154634,
		442672710,
		493760552,
		544829784,
		677839212,
		848786118,
		911331561,
		915906776,
		917314517,
		972571707,
		992790930,
		-1169499038,
		2102749046,
		285955867,
		339302416384,
		1022202216448,
		618475290624,
		3852661096,
		3328407309,
		2735212356,
		3042061272,
		2404016073,
		733106126,
		1352706024,
		1881968783,
		7308324500346660212,
		392501634,
		-2042143896,
		-2135614041,
		1723477505,
		1045342179,
		1000351374,
		-1549630786,
		-2065346036,
		1213808185,
		-920663435,
		1307476082,
		2099816323,
		1381691752512,
		1379180326488,
		1381249242271,
		2253570113600, // to do, test these
		433791696896,
		7020396498516145514,
		2055958901,
		994306218,
		-120668417,
		-1835749229,
		2077565911,
		4156321,
		-1662909539,
		256346004,
		-15432926,
		-1243184386,
		1642479322,
		27391769,
		-2136787743,
		-1289983205,
		1586576930,
		1093247513,
		2021867503,
		-1184085326,
		-1996885757,
		1178932519,
		813647057,
		769347061,
		-2120750352,
		-1462751325,
		900146065,
		-985688171,
		999090520,
		-13389525,
		-173662905,
		314667560,
		-1411930028,
		-514046096,
		-1302989626,
		1091694297,
		275033734,
		459263416,
		1381803144,
		-1153009121,
		634715799,
		-1179957982,
		930233414,
		-1900102430,
		-452918768,
		420531845,
		-712526240,
		1826420594,
		-1708373442,
		-841066582,
		1417640596,
		-1089379066,
		2101106911,
		325218053,
		140694538682404,
		-171207973,
		-87967637,
	};

	const int16_t blocked_network[] =
	{
		//super common
		// 
		//REMOTE_SCRIPT_LEAVE_EVENT,		
		//REMOTE_SCRIPT_INFO_EVENT,
		//SCRIPTED_GAME_EVENT,
		//NETWORK_TRAIN_REPORT_EVENT,
		//NETWORK_ENTITY_AREA_STATUS_EVENT,

		//lobby organization + clear areas
		// 
		//PLAYER_CARD_STAT_EVENT,				
		//SCRIPT_WORLD_STATE_EVENT,
		//CLEAR_AREA_EVENT
		//CLEAR_RECTANGLE_AREA_EVENT

		//join / system
		//
		//NETWORK_CHECK_EXE_SIZE_EVENT,			
		//NETWORK_CHECK_CATALOG_CRC,
		//NETWORK_CRC_HASH_CHECK_EVENT,
		//NETWORK_TRAIN_REPORT_EVENT,
		//NETWORK_REQUEST_SYNCED_SCENE_EVENT,
		//NETWORK_START_SYNCED_SCENE_EVENT,
		//NETWORK_STOP_SYNCED_SCENE_EVENT,
		//NETWORK_UPDATE_SYNCED_SCENE_EVENT,
		//SCRIPT_ARRAY_DATA_VERIFY_EVENT,
		//VOICE_DRIVEN_MOUTH_MOVEMENT_FINISHED_EVENT,
		//REPORT_MYSELF_EVENT,

		//sent before they have a player
		// 
		//CACHE_PLAYER_HEAD_BLEND_DATA_EVENT,	
		//OBJECT_ID_FREED_EVENT,

		//could be causing create resends
		// 
		//MARK_AS_NO_LONGER_NEEDED_EVENT,
		//OBJECT_ID_REQUEST_EVENT,		
		
		//car trolling
		// 
		//GIVE_CONTROL_EVENT,
		//REQUEST_CONTROL_EVENT,					
		//REQUEST_DETACHMENT_EVENT,
		//VEHICLE_COMPONENT_CONTROL_EVENT,

		//explosion protection
		// 
		//FIRE_EVENT,								
		//EXPLOSION_EVENT,
		//WEAPON_DAMAGE_EVENT,
		//UPDATE_PLAYER_SCARS_EVENT,
		//START_PROJECTILE_EVENT,
		//INFORM_SILENCED_GUNSHOT_EVENT,
		//BLOW_UP_VEHICLE_EVENT,
	
		//kicks and common troll events
		//
		GAME_WEATHER_EVENT,
		NETWORK_SHOUT_TARGET_POSITION,
		BREAK_PROJECTILE_TARGET_LOCK_EVENT,
		GIVE_PED_SCRIPTED_TASK_EVENT,
		GIVE_PED_SEQUENCE_TASK_EVENT,
		NETWORK_CLEAR_PED_TASKS_EVENT,
		NETWORK_START_PED_ARREST_EVENT,
		NETWORK_START_PED_UNCUFF_EVENT,
		REMOVE_WEAPON_EVENT,
		REMOVE_ALL_WEAPONS_EVENT,
		GIVE_WEAPON_EVENT,
		INCIDENT_ENTITY_EVENT,
		ALTER_WANTED_LEVEL_EVENT,
		MODIFY_VEHICLE_LOCK_WORD_STATE_DATA,
		KICK_VOTES_EVENT,
		RAGDOLL_REQUEST_EVENT,
		NETWORK_INCREMENT_STAT_EVENT,
		NETWORK_PTFX_EVENT,						
		ACTIVATE_VEHICLE_SPECIAL_ABILITY_EVENT,
		BREAK_PROJECTILE_TARGET_LOCK_EVENT,

		//modders giving you shit
		//
		GIVE_PICKUP_REWARDS_EVENT,
		REPORT_CASH_SPAWN_EVENT,
	};
}
