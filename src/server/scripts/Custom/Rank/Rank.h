struct RankTemplate
{
	uint32 meetValue;
	std::string name;
	std::string gossipText;
	uint32 rewId;
	std::string prefix;
};

extern std::map<uint32, RankTemplate> RankDataMap;

class Rank
{
public:
	static Rank* instance()
	{
		static Rank instance;
		return &instance;
	}
	void Load();

	bool AccountBind();
	uint32 GetMeetValue(uint32 level);
	std::string GetName(uint32 level);
	std::string GetGosstipText(uint32 level);
	uint32 GetRewId(uint32 level);
	std::string GetPrefix(uint32 level);
	uint32 GetMaxLevel();

	void UpdateDBValue(Player* player);
	void GetDBValue(Player* player, uint32 &rankLevel, uint32 &rankValue);
	void Update(Player* player, uint32 value = 0, bool updateDB = false);
	void SendPacket(Player* player);
	void AddGossip(Player* player,Object* obj);
	void Action (Player*player, uint32 action, Object*obj);
private:

};
#define sRank Rank::instance()