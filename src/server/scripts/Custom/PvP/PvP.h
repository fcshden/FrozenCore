#define FACTION_DUEL 10000
#define GUILD_DUEL 20000

enum PvPTypes
{
	PVP_TYPE_NONE = 0,	//保持原状
	PVP_TYPE_SANCTUARY = 1,	//安全区
	PvP_TYPE_FFA = 2,	//FFAPVP
	PvP_TYPE_NO_GROUP_FFA = 3,	//禁止组队FFAPVP
	PvP_TYPE_CUSTOM_FACTION = 4,	//自定义阵营
	PvP_TYPE_GUILD = 5,//公会
};

struct PvPTemplate
{
	uint32 zone;
	uint32 area;
	PvPTypes type;
	std::string prefix;
	std::string notice;
	uint32 minHp;
	uint32 maxHp;
	uint32 killRewId;
	uint32 killedRewId;
	std::vector<uint32> eventData;
	bool autoMaxHp;
};

extern std::vector<PvPTemplate> PvPVec;

class PvP
{
public:
	static PvP* instance()
	{
		static PvP instance;
		return &instance;
	}
	void Load();
	PvPTypes GetType(uint32 zone, uint32 area);
	bool EnableGroup(uint32 zone, uint32 area);
	void Update(Player* player, uint32 zone, uint32 area);
	uint32 GetMinHp(uint32 zone, uint32 area);
	uint32 GetMaxHp(uint32 zone, uint32 area);
	bool AutoMaxHP(uint32 zone, uint32 area);
	void RewOnKill(Player *pKiller, Player *pVictim);
private:
};
#define sPvP PvP::instance()
