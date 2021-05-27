#define BIND_INSTANCE_DIFF 0

struct ChallengeTemplate
{
	uint32 MapId;
	uint32 Level;
	int32 ReqId;
	std::string GossipText;
};
extern std::vector<ChallengeTemplate> ChallengeVec;

struct PlayerChallengeTemplate
{
	uint32 guid;
	uint32 mapId;
	uint32 challengeLv;
};

extern std::vector<PlayerChallengeTemplate> PlayerChallengeVec;

class ChallengeMod
{
public:
	static ChallengeMod* instance()
	{
		static ChallengeMod instance;
		return &instance;
	}

	void Load();
	std::string GetGossipText(uint32 mapId, uint32 challengeLv);
	int32 GetReqId(uint32 mapId, uint32 challengeLv);
	bool IsChallengeMap(Player* player, uint32 mapId);
	void ChangeLevel(Player* player, uint32 triggerId, uint32 challengeLv);
	bool AddGossipOrTele(Player* player, uint32 triggerId);

	void ReplaceChallenge(Player* player, uint32 mapId, uint32 challengeLv);
	bool HasChallenge(Player* player, uint32 mapId, uint32 challengeLv);
	void ResetChallenge(uint32 guidlow, uint32 mapId);
private:

};
#define sChallengeMod ChallengeMod::instance()