struct ReincarnationTemplate
{
	uint32 reqId;
	uint32 rewId;
	std::string gossipText;
};

extern std::unordered_map<uint32, ReincarnationTemplate> ReincarnationMap;

class Reincarnation
{
public:
	static Reincarnation* instance()
	{
		static Reincarnation instance;
		return &instance;
	}

	void Load();
	uint32 GetMaxLevel();
	void GetParams(Player* player, uint32 &rewId, uint32 &reqId);
	std::string GetGossipText(Player* player);
	void AddGossip(Player* player, Object* obj);
	void DoAction(Player* player, Object* obj, uint32 action);
	void Save(Player* player);
	void Load(Player* player);
private:

};
#define sReincarnation Reincarnation::instance()