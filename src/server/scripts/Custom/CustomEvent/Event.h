//抢答
#define EVENT_ID_QUICK_RESPONSE					70

//假人更新
#define EVENT_ID_FAKE_PLAYER_UPDATE_ZONE_ID		71
#define EVENT_ID_FAKE_PLAYER_LOGIN_OR_LOGOUT	72
#define EVENT_ID_FAKE_PLAYER_LEVEL_UP			73

//大逃杀
#define UKBG_EVENT_ID	0
#define UKBG_LENGTH		18000
#define UKBG_H_REWID	32
#define UKBG_M_REWID	33
#define UKBG_L_REWID	33

//狭路相逢
#define NARROW_EVENT_ID				0
#define NARROW_EVENT_DEFENSE_POSID	2
#define NARROW_EVENT_OFFENSE_POSID	3
#define NARROW_EVENT_DEBUFF_ID		60001

#define BGIC_EVENT_ID		0
#define BGIC_EVENT_MAP		628
#define BGIC_SSS_REWARD_ID	1
#define BGIC_REWARD_ID		2
#define BGIC_DISPLAY_MAX	10
#define BGIC_STOP_KILLS		50


struct PosTemplate
{
	uint32 map;
	float x;
	float y;
	float z;
	float o;
};
extern std::unordered_map<uint32, PosTemplate> PosMap;

enum GroupTypes
{
	C_GT_ONE_ALL,				
	C_GT_ONE_A,				
	C_GT_ONE_H,				
	C_GT_RANDOM_A_H,		
	C_GT_RANDOM_PLAYER,
};

enum RejoinTypes
{
	C_RT_BANED,
	C_RT_KEEP_GROUP,
	C_RT_REDIS_GROUP,
};

enum EventTypes
{
	C_ET_NONE,
	C_ET_WS,
	C_ET_AB,
	C_ET_EY,
	C_ET_SA,
	C_ET_AV,
	C_ET_IC,
	C_ET_TELE,
	C_ET_LINE,
	C_ET_FACTION,
	C_ET_GUILD,
	C_ET_SINGLE,
};

struct EventPhaseFactionTemplate
{
	uint8 Phase;
	uint32 Faction;
	uint32 GraveyardPos;
	uint32 GraveyardSafe;
	uint32 GraveyardHealer;
	uint32 StopTelePos;
	uint32 StopPlayerKills;
	uint32 StopPlayerKilleds;
	std::string GraveyardSafeNotice;
	std::string StopNotice;
};

struct EventPhaseTemplate
{
	uint8 Phase;
	int64 StopGUID;
};

#define ALT_GUID_MAX 5

struct EventAltTemplate
{
	int64 GUID;
	uint32 AltFlag;
	std::string NoticeText;
	int64 TargetEntry[ALT_GUID_MAX];
	uint32 TargetCount[ALT_GUID_MAX];
	uint32 TargetKillCount[ALT_GUID_MAX];
};

struct EventAltFactionTemplate
{
	int64 GUID;
	EventFactionId Faction;
	uint32 Kills;
	uint32 Killeds;
};

struct EventStopTemplate
{
	EventFactionId Faction;
	TypeID StopType;
	uint32 Param;
	uint32 Weight;
};

struct EventKillInfoTemplate
{
	uint32 Kills;
	uint32 Killeds;
};

#define WORLDSTATE_ENTRY_MAX 3

struct EventWorldStateTemplate
{
	uint32 ID;
	int32 InitValue;
	int64 Entry[WORLDSTATE_ENTRY_MAX];
	int32 Step[WORLDSTATE_ENTRY_MAX];
	int32 NoticeValue;
	std::string Notice;
};

struct EventWorldStateFactionTemplate
{
	uint32 ID;
	uint32 Faction;
	bool Kill;
	int32 Step;
	int32 NoticeValue;
	std::string Notice;
};

#define MAX_SPAWNNOTICE_COUNT 3

struct EventSpawnTemplate
{
	int64 Entry;
	uint32 Phase;
	uint32 SpawnTime;
	uint32 PosId;
	uint32 Duration;
	uint32 NoticeTime[MAX_SPAWNNOTICE_COUNT];
	std::string SpawnNotice[MAX_SPAWNNOTICE_COUNT];
};

struct EventListTemplate
{
	std::string EventName;
	std::string StartTime;
	std::string Occurence;
	std::string Length;
	EventTypes EventType;
};

extern std::unordered_map<uint32, EventListTemplate> EventListMap;

class Event
{
public:
	static Event* instance()
	{
		static Event instance;
		return &instance;
	}

public:
	void Reset();
	void InitParams(uint32 eventId);
	void LoadPos();
	void Start(uint32 eventId);
	void Stop(uint32 eventId);
	bool AcceptInvitation(Player* player, uint32 menuId);
	bool CanRew(Player* pl);
	void Update(Map* map, uint32 diff);
	void UpdateRevive(uint32 diff);
	void UpdateSafe(uint32 diff);
	int32 GetWeightLeft(EventFactionId faction);
	float GetDistance(Player* player, uint32 pos);
	void Tele(Player* player, uint32 pos);
	void TeleToGraveyard(Player* player);
	void ReviveAll(bool start);
	void StartCall();
	void RewAndRest();
	void Join(Player* pl);
	EventFactionId Subgroup(Player* pl);
	void Leave(Player* pl);
	void DieTele(Player* pl);
	void SafeTele(Player* pl);
	void UpdateActive(Object* obj, bool kill = false);
	void UpdatePhase(Object* obj, bool kill = false);
	void UpdateWorldState(uint32 ID, uint32 value);
	void InitWorldState();
	void UpdateWorldState(Object* obj, bool kill = false);
	void UpdateSpawn(uint32 diff);
	void Process(Object* obj);
	uint32 GetFactionMembers(EventFactionId faction);
	bool DiedTele(Player* player);
	bool SendAreaSpiritHealerQueryOpcode(Player* player);
	void AddEventList(Player* player, Object* obj, bool isBG = false);
	void GetTelePos(uint32 posId, uint32 &map, float &x, float &y, float &z, float &o);
	bool GobUnk1Check(Unit* target, GameObject *const gob);
	bool IpCheck(Player* player); 
	bool IsFixedTimeBgActive(BattlegroundTypeId bgTypeId);
	int32 GetZoneId(){ return _ZoneId; }
	void SummonSoulHealer();
	void SendScreenMessage(std::string text, EventFactionId faction = 0, bool onlydead = false);
	bool Valid(){ if (_EventId == 0 || _EventType < C_ET_LINE || _EventId == BGIC_EVENT_ID) return false; return true; }
private:
	/*_game_event*/
	uint32 _EventId;
	std::string _EventName;
	EventTypes _EventType;
	GroupTypes _GroupType;
	RejoinTypes _RejoinType;
	std::string _NoticeText;
	std::string _GossipText;
	int32 _ZoneId;
	Map* _Map;
	uint8 _Phase;
	bool _RandomAH;
	uint32 _WinRewId;
	uint32 _LosRewId;
	uint32 _DamageForRew;
	uint32 _HealForRew;
	uint32 _KillsForRew;
	uint32 _KilledForRew;

	//Start
	std::unordered_map<EventFactionId, std::string> _FactionNameMap;
	std::unordered_map<EventFactionId, uint32> _StartPosMap;
	std::unordered_map<EventFactionId, uint32> _StopPosMap;
	std::unordered_map<uint32, EventFactionId>_GroupMap;
	std::vector<EventFactionId> _FactionVec;
	std::unordered_map<uint32, EventFactionId>_PlayersMap;
	std::unordered_map<EventFactionId, EventKillInfoTemplate> _KillInfoMap;
	
	//Stop
	std::vector<EventStopTemplate>_StopVec;

	//Alt
	std::vector<EventAltTemplate>_AltVec;
	std::vector<EventAltFactionTemplate>_AltFactionVec;
	std::vector<int64>_AltedVec;
	
	//Phase
	std::vector<EventPhaseFactionTemplate> _PhaseFactionVec;
	std::vector<EventPhaseTemplate> _PhaseVec;
	//Healer
	std::unordered_map<EventFactionId, uint64> _HealerMap;

	//WorldState
	std::vector<EventWorldStateTemplate> _WorldStateVec;
	std::vector<EventWorldStateFactionTemplate> _WorldStateFactionVec;

	//Spawn
	std::vector<EventSpawnTemplate> _SpawnVec;
	uint32 _UpdateSpawnTimer;

	uint32 _UpdateSafeTimer;
	uint32 _UpdateReviveTimer;
	bool _SoulVisual;

	bool _DelayStartCall;
	bool _DelayStopCall;
	uint32 _UpdateDelayNotice;
	EventFactionId _WinFaction;
public:
	uint32 GetPhaseGraveyardPos(EventFactionId faction)
	{
		for (auto itr = _PhaseFactionVec.begin(); itr != _PhaseFactionVec.end(); itr++)
			if (itr->Faction == faction && itr->Phase == _Phase)
				return itr->GraveyardPos;

		return 0;
	}

	void GetKillInfo(EventFactionId faction, uint32 &Kills, uint32 &Killeds)
	{
		auto itr = _KillInfoMap.find(faction);
		if (itr != _KillInfoMap.end())
		{
			Kills = itr->second.Kills;
			Killeds = itr->second.Killeds;
		}
	}

	void UpdateKillInfo(Player* player, bool kill)
	{
		auto itr = _KillInfoMap.find(player->GetEventFaction());
		if (itr != _KillInfoMap.end())
		{
			if (kill)
				itr->second.Kills++;
			else
				itr->second.Killeds++;
		}
	}
};
#define sEvent Event::instance()
