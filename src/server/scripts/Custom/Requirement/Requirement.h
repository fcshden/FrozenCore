////所需条件及奖励模板////
struct ReqMapData
{
	uint32 map;
	uint32 zone;
	uint32 area;
};

struct ReqCommandTemplate
{
	std::string command;
	std::string des;
	std::string icon;
};

#define REQ_ITEM_MAX 10

struct ReqTemplate
{
	uint32		meetLevel				;
	int32		meetHRRank				;
	int32		meetVipLevel			;
	int32		meetRankLevel			;
	int32		meetFaction				;
	uint32		meetAchievementPoints	;
	uint32		desXp					;
	uint32		desGoldCount			;
	uint32		desTokenCount			;
	uint32		desHRPoints				;
	uint32		desArenaPoints			;
	uint32		desSpiritPower			;
	bool		inInstance				;
	std::vector<ReqCommandTemplate> CommandDataVec;
	int32		reincarnation			;

	uint32 desItem[REQ_ITEM_MAX];
	uint32 desItemCount[REQ_ITEM_MAX];
	std::vector<ReqMapData> MapDataVec;
	std::vector<int32> SpellDataVec;
	std::vector<int32> QuestDataVec;
	std::vector<uint32> AchieveDataVec;
};

extern std::unordered_map<uint32, ReqTemplate> ReqMap;

class Req
{
public:
	static Req* instance()
	{
		static Req instance;
		return &instance;
	}
	void Load();
	bool Check(Player* player, uint32 reqId, uint32 count = 1, bool notice = true);
	void Des(Player* player, uint32 reqId, uint32 count = 1, uint32 expItemId = 0);
	std::string Notice(Player* player, uint32 reqId, std::string generalText, std::string text, uint32 count = 1, uint32 chance = 100, uint32 vipRateType = 0,uint32 hrRateType = 0);
	uint32 Golds(uint32 reqId, uint32 count = 1);
	std::string GetDescription(uint32 reqId, Player* player, uint32 count = 1);
	bool IsExist(uint32 reqId);
	std::string GetExtraDes(uint32 reqId, uint32 count = 1);

	bool CheckMap(Player* pl, uint32 reqId, bool notice = true);
	bool CheckSpell(Player* pl, uint32 reqId, bool notice = true);
	bool CheckQuest(Player* pl, uint32 reqId, bool notice = true);
	bool CheckAcheive(Player* pl, uint32 reqId, bool notice = true);
private:

};
#define sReq Req::instance()
