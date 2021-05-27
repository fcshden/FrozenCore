struct EquipmentTemplate
{
    uint8 _class;
    int32 MapOrArea;
    uint32 items[EQUIPMENT_SLOT_END];
};

extern std::vector<EquipmentTemplate> EquipmentVec;

class EquipmentManager
{
public:
	static EquipmentManager* instance()
	{
		static EquipmentManager instance;
		return &instance;
	}
	void Load();
	bool SlotIsFreeze(Player* pl, uint8 slot);
	bool InKeyings(Item* item);
	bool InKeyings(uint8 bag, uint8 slot);
	void Update(Player* pl, uint8 update);
private:

};
#define sEM EquipmentManager::instance()
