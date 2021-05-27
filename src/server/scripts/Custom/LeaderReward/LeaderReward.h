//������Ϣ
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

//�����Ϣ
struct LearderDataTemplate
{
	uint32 guid;
	bool isInstance;/*�Ƿ��Ǹ���*/
	uint8 diff;
	uint32 mapIdOrZoneId;/*����ʹ��mapId,�Ǹ���ʹ��zoneId*/
	//std::vector<uint32/*bossId*/> BossIdVec;/*��ɱ������������д˹�����Ϣ�������������ѹ�룬���û����ֱ��ѹ��*/
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
