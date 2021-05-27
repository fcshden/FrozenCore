struct ItemSetTemplate
{
	uint32 ID;
	uint32 counts;
	uint32 spell1;
	uint32 spell2;
	uint32 spell3;
	std::string description;
};

extern std::vector<ItemSetTemplate> ItemSetVec;

struct ItemSetItemsTemplate
{
	uint32 ID;
	uint32 entry;
};

extern std::vector<ItemSetItemsTemplate> ItemSetItemsVec;

class ItemSet
{
public:
	static ItemSet* instance()
	{
		static ItemSet instance;
		return &instance;
	}
	void Load();
	std::string GetDes(uint32 entry);
	uint32 GetID(uint32 entry);
	void AddAllSpell(Player* player);
	void UpdateSpell(Player* player, uint32 addEntry, uint32 remEntry);
private:

};
#define sItemSet ItemSet::instance()