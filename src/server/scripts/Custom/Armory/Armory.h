struct ArmoryTokenTemplate
{
	uint32 guid;
	uint32 account;
	uint32 amount;
};

extern std::vector<ArmoryTokenTemplate> ArmoryTokenVec;

struct ArmorySigilTemplate
{
	uint32 guid;
	uint32 account;
	uint32 amount;
};

extern std::vector<ArmorySigilTemplate> ArmorySigilVec;

struct ArmoryPvpTemplate
{
	uint32 guid;
	uint32 account;
	uint32 amount;
};

extern std::vector<ArmoryPvpTemplate> ArmoryPvpVec;

class Armory
{
public:
	static Armory* instance()
	{
		static Armory instance;
		return &instance;
	}
	void Load();
	uint32 QueryToken(Player* player);
	uint32 QuerySigil(Player* player);
	uint32 QueryPvp(Player* player);

	void UpdateTokenArmory(Player* player);
	void UpdateSigilArmory(Player* player);
	void UpdatePvpArmory(Player* player);

	void SendArmoryTokenMsg(Player* player);
	void SendArmorySigilMsg(Player* player);
	void SendArmoryPvpMsg(Player* player);

	std::string GetRace(uint8 race);
	std::string GetGender(uint8 gender);
	std::string GetClass(uint8 playerClass);
private:

};
#define sArmory Armory::instance()