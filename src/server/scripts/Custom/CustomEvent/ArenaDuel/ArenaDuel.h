#define ARENA_DUEL_START_EVENT_ID			101
#define ARENA_DUEL_SIGNUP_EVENT_ID			100
#define ARENA_DUEL_PILLAR_ENTRY				194587
#define ARENA_DUEL_EYE_ENTRY				184663
#define ARENA_DUEL_MAP						0
#define ARENA_DUEL_AREA_IN					2177
#define ARENA_DUEL_AREA_OUT					1741
#define ARENA_DUEL_WAIT_SECS				10
#define ARENA_DUEL_REW_INTERVALS			60 * IN_MILLISECONDS
#define ARENA_DUEL_GAMBLE_TIME				15 * IN_MILLISECONDS
#define ARENA_DUEL_SIGNUP_REQ_ID			1001		//_req 报名需求
#define ARENA_DUEL_PERMIN_REW_ID			1000		//_rew 每分钟奖励
#define ARENA_DUEL_END_REW_ID				1001		//_rew 活动结束每人奖励
#define ARENA_DUEL_WIN_REW_ID				1002		//_rew 决斗获胜奖励
#define ARENA_DUEL_LOS_REW_ID				1003		//_rew 决斗失败奖励
#define ARENA_DUEL_CHAMPION_REW_ID			1004		//_rew 冠军奖励
#define ARENA_DUEL_LOSER_SPELL				80900		//失败者技能
#define ARENA_DUEL_CHAMPION_SPELL			80901		//最终胜利者技能
#define	ARENA_DUEL_IN_GOB					700001		//结束中间宝箱
#define ARENA_DUEL_SEC_DEBUFF				2000021		//每秒叠BUFF
#define ARENA_DUEL_FLAG_1					2000022		//红队
#define ARENA_DUEL_FLAG_2					2000023		//蓝队
#define ARENA_DUEL_FREEZE_DEBUFF			2000024		//玩家冻结
#define ARENA_DUEL_GAMBLE_MENU_ID			7779

#define ARENA_DUEL_GAMBLE_ENABLE			false

struct ArenaDuelTemplate
{
	bool selected;
	uint32 winNum;
};

extern std::map<uint32, ArenaDuelTemplate> ArenaDuelMap;

enum ArenaDuelStates
{
	ARENA_DUEL_STATE_SIGNUP,
	ARENA_DUEL_STATE_START,
	ARENA_DUEL_STATE_SELECT,
	ARENA_DUEL_STATE_GAMBLE,
	ARENA_DUEL_STATE_DUEL,
	ARENA_DUEL_STATE_WAIT,
	ARENA_DUEL_STATE_END,
};

enum ArenaDuelGroupType
{
	ARENA_DUEL_GROUP_RED,
	ARENA_DUEL_GROUP_BLUE,
};

extern std::unordered_map<uint64, int64> AreaDuelGambleMap;

class ArenaDuel
{
public:
	static ArenaDuel* instance()
	{
		static ArenaDuel instance;
		return &instance;
	}

	void Load();
	void Signup(Player* pl);
	void Save(Player* pl);

	void SetState(ArenaDuelStates state);
	ArenaDuelStates GetState(){ return _State; }

	void Update(Map* map, uint32 diff);

	void TelePortAll();
	void TelePort(Player* pl);
	void TeleportDueler();

	bool HasDueler();
	bool CheckDueler(Player* dueler)
	{
		if (!dueler || !dueler->IsInWorld() || dueler->GetMapId() != ARENA_DUEL_MAP)
			return false;

		return true;
	}
	Player* GenerateDueler();
	void StartDuel();
	void EndDuel(Player* winner, Player* loser);

	bool IsDueler(Player* pl);

	void TogglePillar(Map * map);

	void InitPlayer(Player* pl);

	void PopGamble();
	void CloseGamble();
	void EndGamble(bool redwin);
private:

	Player* _DUELER_1;
	Player* _DUELER_2;

	ArenaDuelStates _State;
	uint32 _RewTimer;
	uint32 _DuelTimer;
	uint32 _DuelSedcs;
	uint32 _WaitTimer;
	uint32 _WaitSecs;
	uint32 _AreaCheckTimer;
	uint32 _PillarTimer;
	uint64 _Pillar[4];
	uint32 _GambleTimer;
};

#define sArenaDuel ArenaDuel::instance()

