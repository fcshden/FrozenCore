#define TALISMAN_VALUE_ITEM 100010

extern std::unordered_map<uint32,uint32> TalismanMap;


class Talisman
{
public:
	static Talisman* instance()
	{
		static Talisman instance;
		return &instance;
	}

	void Load();
	/*
	std::string GetDescription(TalismanTypes type);
	void SendTalismanQuery(Player* player, uint32 entry);
	void UpdateTalisman(Player* player, uint8 upgradeType, bool updataDB);
	void UpdataDB(Player* player);
	void UpdatePlayerStat(Player* player, TalismanTypes type, float val1, float val2, uint32 entry);

	void OnCreatureKill(Player* player, Creature* creature);
	void OnQuestComplete(Player* player, uint32 questId);
	void OnKillStreak(Player* player);
	void OnBGWin(Player* player);
	void OnJoinEvent(Player* player);
	void OnWorldBossKill(Player* player);
	*/
	bool HasTalismanEquiped(Player* player, uint32 entry);
	uint32 GetEntryByID(Player* player, uint32 ID);
	std::string GetIcon(uint32 entry);
	std::string GetName(uint32 entry);
	uint32 GetGroupId(uint32 entry);
	void SendPacket(Player* player);
	void EquipTalisman(Player* player, uint32 ID, uint32 entry);
	void ApplyOnEquip(Player* player, uint32 entry, bool apply);
	void ApplyOnLogin(Player* player);

	
	void UpdateTalismanValueOnEquip(Player* player, bool equip);
	void SendTalisManValue(Player* player);
	void AddTalismanValue(Player* player, uint32 value, bool kill);
	void SaveTalisManValue(Player* player,bool logout = false);

	bool IsMidItem(uint32 entry);
private:

};
#define sTalisman Talisman::instance()