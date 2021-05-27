#include "Transport.h"
#include "TransportMgr.h"

#define EVENT_THUDERBLUFF_BATTLE_ID  101

const uint32 GunShipHP = 10000;
const uint32 DmgPerThrow = 1000;

//阶段延迟时间
const uint32 TIME_PHASE_ONE_DELAY = 260 * IN_MILLISECONDS; //3分等人 途中2分
const uint32 TIME_PHASE_TWO_DELAY = 610 * IN_MILLISECONDS;//
const uint32 TIME_PHASE_THREE_DELAY = 1000 * IN_MILLISECONDS;
const uint32 TIME_PHASE_FINAL_DELAY =  1400 * IN_MILLISECONDS;

//RewSettingId
const uint32 RewSettingId_General_1 = 10001;
const uint32 RewSettingId_General_2 = 10002;
const uint32 RewSettingId_General_3 = 10003;
const uint32 RewSettingId_Baine = 10004;

extern MotionTransport* gunship;
extern bool isGeneral_1_Alive;
extern bool isGeneral_2_Alive;
extern bool isGeneral_3_Alive;
extern bool isBaine_Alive;
extern std::list<GameObject*> ThunderbluffGobList;
extern std::list<Creature*> ThunderbluffCreatureList;

extern int32 gunshipHP;

enum PhaseIds
{
	PHASE_ZERO,
	PHASE_ONE,
	PHASE_TWO,
	PHASE_THREE,
	PHASE_FINAL,
};

enum ThuderbluffBattleEvents
{
	EVENT_PHASE_ONE = 1,
	EVENT_PHASE_TWO,
	EVENT_PHASE_THREE,
	EVENT_PHASE_FINAL,
	EVENT_TIMER,
};


enum GameObjectIds
{
	GO_GUNSHIP				= 450000,
	GO_PORTAL_SHIP			= 450001,
	GO_PORTAL_HIGHLAND		= 450002,

	GO_HARPOON				= 192177,
};
enum NPCIds
{
	NPC_CONTROLLER		= 60035,
	NPC_SPIRITHEALER	= 60036,
	NPC_TELE_TRIGGER	= 60037,
	NPC_BAINE			= 60038,
	NPC_GENERAL_1		= 60039,
	NPC_GENERAL_2		= 60040,
	NPC_GENERAL_3		= 60041,

	NPC_VEHICLE_AIR		= 60042,
	NPC_GUNSHIP_SHELL	= 60043,

	NPC_HAROON_TRIGGER	= 60044,
};


enum WorldStatesGeneral
{
	WORLD_STATE_GENERAL_1 = 10001,
	WORLD_STATE_GENERAL_2 = 10002,
	WORLD_STATE_GENERAL_3 = 10003,
};

const Position GunshipAirVehicle[7] =
{
	{ 36.8f, -4.51f, 40.0f, 3.0f },
	{ 50.3f, -4.51f, 40.0f, 3.0f },
	{ 36.8f, 4.510f, 40.0f, 3.0f },
	{ 50.3f, 4.510f, 40.0f, 3.0f },
	{ 40.0f, -40.0f, 25.0f, 3.0f},
	{ 40.0f, 40.00f, 25.0f, 3.0f },
	{ 57.0f, 0.000f, 0.00f, 3.0f },
};

//飞艇传送器坐标
const Position GunShipPortalPos = { 0.00f, 0.00f, 23.00f, 3.00f };

//高地传送坐标
const Position HighLandPortalPos[4] =
{
	{ -1452.05f, -36.7138f, 157.544f, 2.35053f },
	{ -985.234f, 324.765f, 135.224f, 2.15026f },
	{ -1045.02f, -323.711f, 159.03f, 5.6335f },
	{ -1204.1f, -5.01502f, 165.478f, 4.63998f },
};

//进攻方墓地坐标
const Position OffenseHealerPos = { -43.86f, 0.00f, 20.54f, 7.22f };

//防守方墓坐标
const Position DefenseHealerPos[4] =
{
	{ -1404.47f, -142.373f, 159.254f, 1.42121f },
	{ -972.524f, 213.953f, 136.038f, 2.17912f },
	{ -1088.48f, -226.345f, 159.312f, 0.706521f },
	{ -1201.31f, 7.16812f, 165.2f, 4.37433f },
};

//牛头人将军坐标
const Position GeneralPos[3] =
{
	{ -1405.0f, -77.5565f, 158.935f, 5.12808f },
	{ -1008.41f, 236.936f, 135.337f, 4.18792f },
	{ -1058.64f, -233.276f, 159.03f, 1.72963f },
};

const Position BainePos = { -1209.63f, -100.15f, 162.667f, 1.50761f };

//传送触发器坐标 防止进攻方偷boss
const Position TeleTriggerPos[10] =
{
	{-1073.95f	,155.010f	,139.461f	,4.151f	},
	{-1070.80f	,-121.341f	,151.762f	,0.495f	},
	{-1353.51f	,-8.665f	,141.483f	,0.904f	},
	{-1341.23f	,-128.383f	,159.822f	,6.032f	},
	{-1111.61f	,-161.215f	,162.052f	,0.444f	},
	{-1126.04f	,195.709f	,120.951f	,0.456f	},
	{-1307.92f	,179.078f	,129.994f	,5.047f	},
	{-1285.01f	,184.123f	,129.994f	,4.587f	},
	{-1033.77f	,-25.413f	,140.694f	,2.910f	},
	{-1042.93f	,-47.504f	,140.694f	,2.286f	},
};

const Position HarpoonPos[15] =
{
	{ -1095.32f, -263.754f, 159.416f, 2.584f },
	{ -1081.52f, -328.406f, 159.955f, 3.637f },
	{ -1019.38f, -322.878f, 159.267f, 5.066f },
	{ -1004.02f, -267.671f, 159.283f, 0.032f },
	{ -1036.11f, -204.550f, 160.116f, 0.813f },
	{ -975.746f, 192.4350f, 136.380f, 4.964f },
	{ -935.655f, 247.7420f, 135.746f, 5.686f },
	{ -954.889f, 295.9860f, 135.592f, 0.762f },
	{ -1030.06f, 294.4780f, 135.745f, 2.309f },
	{ -1042.90f, 234.7220f, 134.700f, 2.949f },
	{ -1485.66f, -107.560f, 158.935f, 3.966f },
	{ -1438.70f, -33.1325f, 157.327f, 1.794f },
	{ -1405.68f, -160.389f, 158.935f, 4.260f },
	{ -1375.76f, -114.850f, 158.935f, 5.709f },
	{ -1374.20f, -58.6768f, 158.898f, 0.671f },
};

class ThunderbluffBattle
{
public:
	static ThunderbluffBattle* instance()
	{
		static ThunderbluffBattle instance;
		return &instance;
	}

	void Init(Creature* controller);
	void Stop();


	Creature* AddCreatureToShip(uint32 entry, float x, float y, float z, float o, MotionTransport* transport);
	GameObject* AddGameObjectToShip(uint32 entry, float x, float y, float z, float o, MotionTransport* transport);

	void DelAllCreatures();
	void DelAllGameObjects();

	void RewardByCreature(uint32 entry, bool kill);

	void TeleAllPlayers();
	void TelePlayerToShip(Player* player, MotionTransport* transport);
	void TelePlayerToShipHS(Player* player, MotionTransport* transport);
	void TelePlayerToHighLand(Player* player);
	void TelePlayerToHighLandHS(Player* player);

	void JoinTele(Player* player);
	bool DiedTele(Player* player);

	void SetPhase(PhaseIds phase){ m_curPahse = phase; }
	PhaseIds GetPhase(){ return m_curPahse; }

	void SetWorldState(WorldStatesGeneral type, uint32 value);

private:
	PhaseIds m_curPahse;
};
#define sThunderbluffBattle ThunderbluffBattle::instance()
