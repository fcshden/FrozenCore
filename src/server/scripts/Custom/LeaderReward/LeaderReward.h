//副本信息
struct LeaderRewardTemplate
{
	uint32 mapIdOrZoneId;
	bool isInstance;
	uint8 diff;
	std::vector<uint32 /*bossid*/> bossInfo;
	uint32 rewId;
	uint32 range;
	uint32 nearbyPlayers;
	uint32 rewType;
	uint32 ipMaxCount;
};

extern std::vector<LeaderRewardTemplate> LeaderRewardVec;

//玩家信息
struct LearderDataTemplate
{
	uint32 guid;
	bool isInstance;/*是否是副本*/
	uint8 diff;
	uint32 mapIdOrZoneId;/*副本使用mapId,非副本使用zoneId*/
	//std::vector<uint32/*bossId*/> BossIdVec;/*击杀后如果容器已有此怪物信息则先清空容器后压入，如果没有则直接压入*/
	uint32 bossId;
};

extern std::vector<LearderDataTemplate> LeaderDataVec;

class LeaderReward
{
public:
	static LeaderReward* instance()
	{
		static LeaderReward instance;
		return &instance;
	}

	void Load();
	void GetLeaderInfo(uint32 entry, uint32 mapIdOrZoneId, bool isInstance, uint8 diff, std::vector<uint32/*bossId*/> &bossVec, uint32 &rewId, uint32 &range, uint32 &nearbyPlayers, uint32 &rewType, uint32 &ipMaxCount);
	bool CheckLeaderData(Player* killer, Creature* boss);
	uint32 GetNearbyPlayers(Player* leader,float range);
	uint32 GetSameIpCount(Player* leader);
	void GetBossInfoVec(uint32 mapIdOrZoneId,bool isInstance,uint8 diff, uint32 entry, std::vector<uint32> &bossinfovec);
	void GetPlayerBossIdVec(uint32 guid, bool isInstance, uint8 diff, uint32 mapIdOrZoneId, std::vector<uint32> &playerbossidvec);
};
#define sLeaderReward LeaderReward::instance()
