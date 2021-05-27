struct FactionTemplate
{
	std::string name;
	std::string gossipText;
	uint32 rewId;
	std::string suffix;
	uint32 quitReqId;
};

extern std::map<uint32, FactionTemplate> FactionDataMap;

class Faction
{
public:
	static Faction* instance()
	{
		static Faction instance;
		return &instance;
	}
	void Load();
	std::string GetName(uint32 faction);
	std::string GetGosstipText(uint32 faction);
	uint32 GetRewId(uint32 faction);
	uint32 GetReqId(uint32 faction);
	std::string GetSuffix(uint32 faction);

	void Join(Player* player,uint32 faction);
	void AddGossip(Player* player, Object* obj);
	void Action(Player*player, uint32 action, Object*obj);
private:

};
#define sFaction Faction::instance()