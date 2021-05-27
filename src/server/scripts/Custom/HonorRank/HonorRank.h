struct HRUpTemplate
{
	uint32 title;
	uint32 reqId;
	uint32 rewId;
	uint32 lootRate;
	uint32 exchangeRate_0;
	uint32 exchangeRate_1;
	uint32 strengthenRate;
	uint32 sigilRate;
	uint32 removeGemRate;
};

extern std::vector<HRUpTemplate> HRUpVec;

enum HRRateTypes
{
	HR_RATE_NONE,
	HR_RATE_LOOT,
	HR_RATE_ITEM_EXCHANGE_0,
	HR_RATE_ITEM_EXCHANGE_1,
	HR_RATE_ITEM_STRENGTHEN,
	HR_RATE_SIGIL,
	HR_RATE_GEM_REMOVE,
};

class HonorRank
{
public:
	static HonorRank* instance()
	{
		static HonorRank instance;
		return &instance;
	}
	void Load();
	uint32 GetHRRank(Player* player);
	std::string GetHRTitle(Player* player);
	std::string GetHRTitle(Player* player,uint32 rank);
	void AddCurrHRMenu(Player* player);
	void AddNextHRMenu(Player* player, Object* obj);
	void SetHRTitle(Player* player, uint32 id);
	std::string GetCurrHR(Player* player);
	uint32 GetReqId(uint32 title);
	uint32 GetRewId(uint32 title);

	uint32 GetRate(Player* player, HRRateTypes type);

	uint32 GetHRTiteId(Player* player);
private:

};
#define sHR HonorRank::instance()