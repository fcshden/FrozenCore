#define MAX_EXTRA_EQUIPMENT_ROWS_COUNT 25

#define EXTRA_EQUIPMENT_NEXT_MENULIST 90004
#define EXTRA_EQUIPMENT_NEXT_PAGE 90000
#define EXTRA_EQUIPMENT_PREV_PAGE 90001
#define EXTRA_EQUIPMENT_BACK 90002
#define EXTRA_EQUIPMENT_UPDATE 90003

extern float extra_stat_muil;
extern float extra_enchant_stat_muil;
extern bool extra_origin_item_back;

extern std::vector<uint32 /*entry*/> ExtraEuipMentEntryVec;

class ExtraEquipment
{
public:
	static ExtraEquipment* instance()
	{
		static ExtraEquipment instance;
		return &instance;
	}
	void Load();

	std::string GetItemName(uint32 entry, uint32 width, uint32 height, int x, int y);
	std::string GetSlotDefaultText(EquipmentSlots slot, uint32 width, uint32 height, int x, int y);
	std::string GetSlotText(Player* player, EquipmentSlots slot);
	std::string GetStatDes(uint32 entry);
	std::string GetDes(Player* player, EquipmentSlots slot);
	std::string GetDes(Item* item);
	uint32 GetItemEntry(Player* player, EquipmentSlots slot);


	void LoadPlayerData(Player* player);

	void Apply(Player* player, bool apply);
	void Update(Player* player, Item* item, EquipmentSlots slot);
	void GiveItemBack(Player* player, EquipmentSlots slot);

	void AddMenuList(Player* player, Object* obj);
	void AddNextMenuList(Player* player, Object* obj, uint32 sender, bool firstOpen = true, bool fordisplay = false);
	void Action(Player* player, Object* obj, uint32 sender,bool fordisplay = false);

	bool IsForSlot(Item* item, EquipmentSlots slot);
	
};
#define sExtraEquipment ExtraEquipment::instance()