#define MAX_QUEST_MOD_REW_COUNT 4
#define MAX_RANDOM_QUEST_COUNT 5

//任务区域传送
struct QuestModTemplate
{
	uint32 reqId;
	bool desReq;
	uint32 randomRewId[MAX_QUEST_MOD_REW_COUNT];
	uint32 rewChance[MAX_QUEST_MOD_REW_COUNT];
	uint32 telePosId;
};
extern std::unordered_map<uint32, QuestModTemplate> QuestModMap;

extern std::unordered_map<uint32, float> QuestRandomMap;

class QuestMod
{
public:
	static QuestMod* instance()
	{
		static QuestMod instance;
		return &instance;
	}
	void Load();
	void Tele(Player* player,uint32 questId);
	void AddTeleGossip(Player* player, Object* obj);

	bool CanTakeQuest(Player* player, uint32 questId);
	void RandomReward(Player* player, uint32 questId);
	bool CanRandomReward(uint32 chance);

	std::string GetExtraDes(uint32 questId, Player* player);

	bool AddRandomQuest(Player* player);
	void AddAllRandomQuest(Player* player);
private:

};
#define sQuestMod QuestMod::instance()
