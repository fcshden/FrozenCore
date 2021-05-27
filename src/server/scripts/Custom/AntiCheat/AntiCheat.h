enum AC_TYPES
{
	AC_FLY,
	AC_SPEED,
	AC_TELE,
	AC_OTHER,
};

enum AC_ACTION_TYPES
{
	AC_ACTION_NONE,
	AC_ACTION_PULL,
	AC_ACTION_TELE_HOME,
	AC_ACTION_PRISION,
	AC_ACTION_KICK,
	AC_ACTION_BAN_CHAR,
	AC_ACTION_BAN_ACC,
	AC_ACTION_FALL,
	AC_ACTION_IGNORE_SPEED = 100,
};

class AntiCheat
{
public:
	static AntiCheat* instance()
	{
		static AntiCheat instance;
		return &instance;
	}
	void CreatureReset(Creature* creature, uint32 diff);
	std::string GetTimeString();
	void SaveAndNotice(Player* player, AC_TYPES type, float z = 0);
	bool CheckMovementInfo(Unit* unit, MovementInfo const& movementInfo);
	void CheckMovementInfo(Player* pl);
private:

};
#define sAntiCheat AntiCheat::instance()



extern std::unordered_map<BattlegroundTypeId, std::vector<uint32>>FixtimeBGMap;

class FixtimeBG
{
public:
	static FixtimeBG* instance()
	{
		static FixtimeBG instance;
		return &instance;
	}
	void Load();
	bool IsActive(Player* pl, BattlegroundTypeId bgt);
private:

};
#define sFixtimeBG FixtimeBG::instance()
