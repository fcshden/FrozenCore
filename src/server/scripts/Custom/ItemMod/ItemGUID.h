#define MAX_ITEM_ENCHANTS 5

struct ItemGUIDTemplate
{
	uint32 Points;
	uint32 Enchants[MAX_ITEM_ENCHANTS];
};

extern std::unordered_map<uint32, ItemGUIDTemplate> ItemGUIDMap;

struct ItemPointsTemplate
{
	uint32 Points;
	uint32 TargetEntry;
};

extern std::unordered_map<uint32, ItemPointsTemplate> ItemPointsMap;

class ItemGUID
{
public:
	static ItemGUID* instance()
	{
		static ItemGUID instance;
		return &instance;
	}

	void Insert(uint32 guid);
	void Delete(uint32 guid);
	void AddStatPoints(uint32 guid, uint32 statpoints);
	void UpdateEnchants(uint32 guid, uint8 slot, uint32 enchantid);
	void SendAllData(Player* player);
	void SendUpdateDataToAll(uint32 guid);
private:

};
#define sItemGUID ItemGUID::instance()