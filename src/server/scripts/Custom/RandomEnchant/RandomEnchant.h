#define ON_CREATE_ENCHANT_ID 1

struct RandomEnchantTemplate
{
	uint32 entry;
	uint32 enchantId;
};

extern std::vector<RandomEnchantTemplate> RandomEnchantVec;

struct ExtractEnchantTemplate
{
	bool CanExtract;
	bool CanEnchant;
};

extern std::unordered_map<uint32, ExtractEnchantTemplate> ExtractEnchantMap;

class RandomEnchant
{
public:
	static RandomEnchant* instance()
	{
		static RandomEnchant instance;
		return &instance;
	}

	void Load();
	void OnCreate(Item* item);
	bool Enchant(Player* owner, Item* castItem, Item* targetItem);
private:

};
#define sRandomEnchant RandomEnchant::instance()