#define PLAYER_LOOT_GAMEOBJECT 5
#define PLAYER_LOOT_GAMEOBJECT_DURATION_TIME 120

struct PlayerLootTemplate
{
	uint32 lootCount;
	float chance;
	bool banBank;
	bool banBag;
};

extern std::unordered_map<uint32/*itemid*/, PlayerLootTemplate> PlayerLootMap;

class PlayerLoot
{
public:
	static PlayerLoot* instance()
	{
		static PlayerLoot instance;
		return &instance;
	}

public:
	void Load();
	bool GetLootInfo(uint32 entry, uint32 &lootCount, bool &banBag, bool &banBank);
	uint32 GetItemCount(Player* player, uint32 entry, bool banBag, bool banBank);
	void LootOnKilled(Player* killer, Player* victim);
};
#define sPlayerLoot PlayerLoot::instance()