////定时战场////

#define MAX_BG_RANDOM_BUFFS 5

struct FixedTimeBGTemplate
{
	uint32 winRewardTemplateId;
	uint32 loseRewardTemplateId;
	uint32 killsForEnd;
	uint32 healForReward;
	uint32 damageForReward;
	uint32 killsForReward;
	uint32 killedsForReward;
	uint32 resourcesMax;
	bool crossFaction;
	uint32 limitHP;
	uint32 RandomBuffInterval;
	uint32 randomBuffs[MAX_BG_RANDOM_BUFFS];
	std::string bgName;
};
extern std::unordered_map<BattlegroundTypeId,FixedTimeBGTemplate> FixedTimeBGMap;

class FixedTimeBG
{
public:
	static FixedTimeBG* instance()
	{
		static FixedTimeBG instance;
		return &instance;
	}
	void Load();

	uint32 GetLimitHP(BattlegroundTypeId bgTypeId);
	uint32 GetMaxRes(BattlegroundTypeId bgTypeId);
	bool GetCFFlag(BattlegroundTypeId bgTypeId);
	uint32 GetWinRewId(BattlegroundTypeId bgTypeId);
	uint32 GetLosRewId(BattlegroundTypeId bgTypeId);
	uint32 GetEndKilss(BattlegroundTypeId bgTypeId);
	uint32 GetRewHeal(BattlegroundTypeId bgTypeId);
	uint32 GetRewDmg(BattlegroundTypeId bgTypeId);
	uint32 GetRewKills(BattlegroundTypeId bgTypeId);
	uint32 GetRewKilleds(BattlegroundTypeId bgTypeId);
	void RewardPlayer(Player* player, BattlegroundTypeId bgTypeId, bool win);

	uint32 GetRandomBuffInterval(BattlegroundTypeId bgTypeId);
	void AddRandomBuff(Battleground* bg, uint32 diff);
	void RemoveRandomBuff(Battleground* bg, Player* player);
};
#define sFTB FixedTimeBG::instance()
