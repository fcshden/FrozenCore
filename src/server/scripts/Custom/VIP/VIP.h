
enum VIPRateTypes
{
	VIP_RATE_NONE,
	VIP_RATE_LOOT,
	VIP_RATE_ITEM_EXCHANGE_0,
	VIP_RATE_ITEM_EXCHANGE_1,
	VIP_RATE_ITEM_STRENGTHEN,
	VIP_RATE_SIGIL,
	VIP_RATE_GEM_REMOVE,
};
struct VIPTemplate
{
	uint32 vipLv;
	uint32 reqId;
	float lootRate;
	uint32 exchangeRate_0;
	uint32 exchangeRate_1;
	uint32 strengthenRate;
	uint32 sigilRate;
	uint32 removeGemRate;
	uint32 rewId;
	std::string name;
	std::string icon;
	uint32 timeRewId;
	std::string gossipText;
};
extern std::vector<VIPTemplate> VIPVec;


class VIP
{
public:
	static VIP* instance()
	{
		static VIP instance;
		return &instance;
	}

	void Load();
	uint32 GetRate(Player* player, VIPRateTypes type);
	float GetLootRate(Player* player);
	void Up(Player* player);
	void AddGossip(Player* player, Object* obj);
	uint32 GetReqId(Player* player);
	uint32 GetTimeRewId(Player* player);
	void GetVIPTitle(Player* player, std::string &vipTitle, std::string &vipIcon, bool fakeplayer = false, uint32 vip = 0);
	std::string GetVIPName(uint32 viplv);
	void GetVIP(Player* player, std::string &vipTitle, std::string &vipIcon);
	void GetNextVIP(Player* player, std::string &vipTitle, std::string &vipIcon);
	std::string GetGossipText(Player* player, bool next = false);
private:

};
#define sVIP VIP::instance()