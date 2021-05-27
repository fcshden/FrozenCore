//#define HS_OFFENSE_ENTRY 59999
//#define HS_DEFENSE_ENTRY 59998
//
//#define REVIVE_INTERVALS  20 * IN_MILLISECONDS
//
////抢答
//#define EVENT_ID_QUICK_RESPONSE					70
//
////假人更新
//#define EVENT_ID_FAKE_PLAYER_UPDATE_ZONE_ID		101
//#define EVENT_ID_FAKE_PLAYER_LOGIN_OR_LOGOUT	100
//#define EVENT_ID_FAKE_PLAYER_LEVEL_UP			102
//
////大逃杀
//#define UKBG_EVENT_ID	74
//#define UKBG_LENGTH		18000
//#define UKBG_H_REWID	32
//#define UKBG_M_REWID	33
//#define UKBG_L_REWID	33
//
////狭路相逢
//#define NARROW_EVENT_ID				72
//#define NARROW_EVENT_DEFENSE_POSID	2
//#define NARROW_EVENT_OFFENSE_POSID	3
//#define NARROW_EVENT_DEBUFF_ID		60001
//
////先锋军港口争夺战
//#define PORT_EVENT_ID					75
//#define PORT_EVENT_DEFENSE_GRAVEYARD_1	XX
//#define PORT_EVENT_DEFENSE_GRAVEYARD_2	XX
//#define PORT_EVENT_DEFENSE_GRAVEYARD_3	XX
//
//#define PORT_EVENT_OFFENSE_GRAVEYARD_1	XX
//#define PORT_EVENT_OFFENSE_GRAVEYARD_2	XX
//#define PORT_EVENT_OFFENSE_GRAVEYARD_3	XX
//
//
//
///*
//group_type
//1	lm bl 全在进攻方
//2	lm 攻 bl 守
//3	lm 守 bl 攻
//4	lm bl 阵营攻守随机
//5	lm bl 玩家攻守随机
//
//rejoin_type
//0 禁止
//1 第一次加入事件的队伍
//2 重新分配队伍
//
//event_type
//0 系统事件
//1 战歌
//2 阿拉希
//3 风暴
//4 远古海滩
//5 奥山
//6 征服之岛
//7 只传送  lm->defense_start_pos	 bl->offense_start_pos
//8 攻守活动
//*/
//
//#define DEFENSE_FACTION 1
//#define OFFENSE_FACTION 2
//
//enum TeamTypes
//{
//	C_TEAM_NONE,
//	C_TEAM_DEFENSE,
//	C_TEAM_OFFENSE,
//};
//
//enum GroupTypes
//{
//	C_GT_NONE,
//	C_GT_OFFENSE_ALL,				
//	C_GT_OFFENSE_ALLY,				
//	C_GT_OFFENSE_HORDE,				
//	C_GT_OFFENSE_FACTION_RANDOM,		
//	C_GT_OFFENSE_PLAYER_RANDOM,		
//};
//
//enum RejoinTypes
//{
//	C_RT_BANED,
//	C_RT_KEEP_GROUP,
//	C_RT_REDIS_GROUP,
//};
//
//enum EventTypes
//{
//	C_ET_NONE,
//	C_ET_WS,
//	C_ET_AB,
//	C_ET_EY,
//	C_ET_SA,
//	C_ET_AV,
//	C_ET_IC,
//	C_ET_TELE,
//	C_ET_GROUP,
//};
//
//enum PosTypes
//{
//	C_PT_START_DEFENSE,
//	C_PT_START_OFFENSE,
//};
//
//struct EventPlayerTemplate
//{
//	TeamTypes	team;
//	uint32		kills;
//	uint32		killeds;
//	uint32		damage;
//	uint32		heal;
//	bool		valid;
//};
//
//struct EventPhaseTemplate
//{
//	uint32 stop_creature;
//	uint32 stop_gameobject;
//	uint32 stop_kills;
//	uint32 stop_defense_tele_pos;
//	uint32 stop_offense_tele_pos;
//	uint32 defense_graveyard_pos;
//	uint32 offense_graveyard_pos;
//	std::string stop_defense_notice;
//	std::string stop_offense_notice;
//	float defense_graveyard_safe;
//	float offense_graveyard_safe;
//	uint32 defense_graveyard_healer;
//	uint32 offense_graveyard_healer;
//};
//
//struct EventDataTemplate
//{
//	EventTypes		eventType;
//	GroupTypes		groupType;
//	RejoinTypes		rejoinType;
//	std::string		eventName;
//	std::string		gossipText;
//	std::string		noticeText;
//	
//	std::string		defenseName;
//	std::string		offenseName;
//
//	uint32	startDefensePos;
//	uint32	startOffensePos;
//
//	uint32	zoneId;
//
//	std::unordered_map<uint32/*phase*/, EventPhaseTemplate> eventPhaseMap;
//
//	uint32	rew_winRewId;
//	uint32	rew_losRewId;
//	uint32	rew_killsForRew;
//	uint32	rew_killedsForRew;
//	float	rew_damageForRew;
//	float	rew_healForRew;
//
//	uint32 stop_creature1;
//	uint32 stop_creatureCount1;
//	uint32 stop_creature2;
//	uint32 stop_creatureCount2;
//	uint32 stop_creature3;
//	uint32 stop_creatureCount3;
//	uint32 stop_creature4;
//	uint32 stop_creatureCount4;
//	uint32 stop_creature5;
//	uint32 stop_creatureCount5;
//	uint32 stop_gameobject1;
//	uint32 stop_gameobjectCount1;
//	uint32 stop_gameobject2;
//	uint32 stop_gameobjectCount2;
//	uint32 stop_gameobject3;
//	uint32 stop_gameobjectCount3;
//	uint32 stop_gameobject4;
//	uint32 stop_gameobjectCount4;
//	uint32 stop_gameobject5;
//	uint32 stop_gameobjectCount5;
//	uint32 stop_defenseKills;
//	uint32 stop_offenseKills;
//	uint32 stop_killGaps;
//
//	uint32 state_creature1;
//	uint32 state_creatureWorldState1;
//	uint32 state_creature2;
//	uint32 state_creatureWorldState2;
//	uint32 state_creature3;
//	uint32 state_creatureWorldState3;
//	uint32 state_gameobject1;
//	uint32 state_gameobjectWorldState1;
//	uint32 state_gameobject2;
//	uint32 state_gameobjectWorldState2;
//	uint32 state_gameobject3;
//	uint32 state_gameobjectWorldState3;
//	uint32 state_defenseWorldState;
//	uint32 state_offenseWorldState;
//};
//
//extern std::unordered_map<uint32, EventDataTemplate> EventDataMap;
//
//struct EventActiveTemplate
//{
//	uint32 eventId;
//	uint32 activeGUID;
//	uint32 activeType;
//	uint32 creatureEntry;
//	uint32 killCount;
//	uint32 gameobjectEntry;
//	uint32 destroyCount;
//};
//
//extern std::vector<EventActiveTemplate> EventActiveVec;
//
//
///*击杀生物*/
//extern std::unordered_map<uint32/*creature entry*/, uint32> KillCreatureMap;
///*摧毁物体*/
//extern std::unordered_map<uint32/*gob entry*/, uint32> DestroyGobMap;
///*击杀玩家*/
//extern std::unordered_map<TeamTypes/*event team*/, uint32> KillPlayerMap;
///*玩家信息*/
//extern std::unordered_map<uint32/*guid*/, EventPlayerTemplate> PlayerDataMap;
///*队伍信息*/
//extern std::unordered_map<uint32/*guid*/, TeamTypes> GroupDataMap;
//
//
//struct PosTemplate
//{
//	uint32 map;
//	float x;
//	float y;
//	float z;
//	float o;
//};
//extern std::unordered_map<uint32, PosTemplate> PosMap;
//
//class CustomEvent
//{
//public:
//	static CustomEvent* instance()
//	{
//		static CustomEvent instance;
//		return &instance;
//	}
//
//	//加载自定义事件信息
//	void Load();
//	void LoadPos();
//
//	//检测自定义事件合法 开始 结束 动作 初始化参数
//	void Start(uint32 eventId);
//	void Stop(uint32 eventId, bool expired = true);
//	
//
//	//WorldStat
//	void InitWorldState(uint32 eventId);
//	void _UpdateWorldState(uint32 index, uint32 value);
//	void UpdateWorldState(Object* obj, Player* player);
//
//	//玩家相关 发送邀请 接受 事件列表 获取玩家信息 修改玩家信息等等
//
//	void SendInvitationToAll(std::string text);
//	bool AcceptInvitation(Player*, uint32 menuId);
//	void AddEventList(Player* player, Object* obj, bool isBG = false);
//	void AddBgList(Player* player);
//	void Teleport(Player* player, uint32 posId);
//	void IncreaseDamage(Player* player, Unit* target, uint32 damage);
//	void IncreaseHeal(Player* player, Unit* target, uint32 heal);
//	void IncreaseKills(Player* player);
//	void IncreaseKilleds(Player* player);
//
//	//结束事件的条件 检测条件是否满足
//	void Process(Player* player, Object* obj);
//	bool DiedTele(Player* player);
//	bool SendAreaSpiritHealerQueryOpcode(Player* player);
//	//分发奖励
//	void RewardAllPlayers();
//	void ResetAllPlayers();
//	void RemovePlayer(Player* player);
//	//分组操作
//	uint32 GetTeamMembers(TeamTypes team);
//	TeamTypes SetTeam(Player* player);
//	void SetFaction(Player* player,TeamTypes team);
//	TeamTypes GetTeam(Player* player);
//	void GetTelePos(uint32 eventId, PosTypes type, uint32 &map, float &x, float &y, float &z, float &o);
//	void GetTelePos(uint32 posId, uint32 &map, float &x, float &y, float &z, float &o);
//	void TelePort(Player* player);
//	TeamTypes AddPlayer(Player* player);
//	void AddNewToGroup(Player* player);
//
//	bool IsFixedTimeBgActive(BattlegroundTypeId bgTypeId);
//
//	void UpdateActive(Object *obj);
//	void UpdatePhase(Object* obj);
//	void Update(Map* map, uint32 diff);
//	void SendScreenMessage(const char *text, TeamTypes team = C_TEAM_NONE, bool onlydead = false);
//	void UpdateRevive(uint32 diff);
//	void UpdateSafeArea(uint32 diff);
//	bool GobUnk1Check(Unit* target, GameObject *const gob);
//	void Revive(Player* player);
//	void SummonSoulHealer();
//	float GetDistance(Player* player, uint32 posId);
//	float GetDistance(uint32 posId1, uint32 posId2);
//	void PrintPos(Player* player, uint32 posId1, uint32 posId2, uint32 count, bool addPos);
//	void Update(Player* player);
//
//	bool CheckValid(Player* player);
//	void SetValid(Player* player, bool valid);
//
//public:
//	void InitParams(uint32 eventId);
//	void Reset();
//
//	std::string GetEventName(){ return _eventName; }
//	uint32 GetZoneId(){ return _zoneId; }
//	bool IsValid(){ return _eventId >= 70; }
//
//
//	uint32 GetKillPlayerCount(TeamTypes team)
//	{
//		std::unordered_map<TeamTypes/*event team*/, uint32>::iterator i = KillPlayerMap.find(team);
//		if (i != KillPlayerMap.end())
//			return i->second;
//
//		return 0;
//	}
//
//	uint32 GetKillCreatureCount(uint32 entry)
//	{
//		std::unordered_map<uint32/*creature entry*/, uint32>::iterator i = KillCreatureMap.find(entry);
//		if (i != KillCreatureMap.end())
//			return i->second;
//
//		return 0;
//	}
//
//	uint32 GetDestroyGobCount(uint32 entry)
//	{
//		std::unordered_map<uint32/*gob entry*/, uint32>::iterator i = DestroyGobMap.find(entry);
//		if (i != DestroyGobMap.end())
//			return i->second;
//
//		return 0;
//	}
//
//private:
//	/*获胜队伍*/
//	TeamTypes _winTeam;
//	/*是否过期*/
//	bool _expired;
//
//	bool _AllyAsOffense;
//
//
//	/*基本信息*/
//	uint32			_eventId;
//	EventTypes		_eventType;
//	GroupTypes		_groupType;
//	RejoinTypes		_rejoinType;
//	std::string		_eventName;
//	std::string		_gossipText;
//	std::string		_noticeText;
//	std::string		_defenseName;
//	std::string		_offenseName;
//	uint32	_startDefensePos;
//	uint32	_startOffensePos;
//	uint32 _zoneId;
//
//	uint32 _eventPhase;
//	uint32 _graveyard_defense_pos;
//	uint32 _graveyard_offense_pos;
//	uint64 _soulhealer_defense;
//	uint64 _soulhealer_offense;
//	bool _stop_creature_flag;
//	bool _stop_gameobject_flag;
//	bool _stop_kills_flag;
//	float _defense_graveyard_safe;
//	float _offense_graveyard_safe;
//	uint32 _defense_graveyard_healer;
//	uint32 _offense_graveyard_healer;
//
//	/*奖励*/
//	uint32	_rew_winRewId;
//	uint32	_rew_losRewId;
//	uint32	_rew_killsForRew;
//	uint32	_rew_killedsForRew;
//	float	_rew_damageForRew;
//	float	_rew_healForRew;
//
//	/*结束*/
//	uint32 _stop_creature1;
//	uint32 _stop_creatureCount1;
//	uint32 _stop_creature2;
//	uint32 _stop_creatureCount2;
//	uint32 _stop_creature3;
//	uint32 _stop_creatureCount3;
//	uint32 _stop_creature4;
//	uint32 _stop_creatureCount4;
//	uint32 _stop_creature5;
//	uint32 _stop_creatureCount5;
//	uint32 _stop_gameobject1;
//	uint32 _stop_gameobjectCount1;
//	uint32 _stop_gameobject2;
//	uint32 _stop_gameobjectCount2;
//	uint32 _stop_gameobject3;
//	uint32 _stop_gameobjectCount3;
//	uint32 _stop_gameobject4;
//	uint32 _stop_gameobjectCount4;
//	uint32 _stop_gameobject5;
//	uint32 _stop_gameobjectCount5;
//	uint32 _stop_defenseKills;
//	uint32 _stop_offenseKills;
//	uint32 _stop_killGaps;
//
//	/*世界状态*/
//	uint32 _state_creature1;
//	uint32 _state_creatureWorldState1;
//	uint32 _state_creature2;
//	uint32 _state_creatureWorldState2;
//	uint32 _state_creature3;
//	uint32 _state_creatureWorldState3;
//	uint32 _state_gameobject1;
//	uint32 _state_gameobjectWorldState1;
//	uint32 _state_gameobject2;
//	uint32 _state_gameobjectWorldState2;
//	uint32 _state_gameobject3;
//	uint32 _state_gameobjectWorldState3;
//	uint32 _state_defenseWorldState;
//	uint32 _state_offenseWorldState;
//
//	int32 _mapId;
//
//	//复活
//	int32 _reviveUpdateTimer;
//
//	//安全区
//	int32 _safeAreaUpdateTimer;
//};
//#define sEvent CustomEvent::instance()