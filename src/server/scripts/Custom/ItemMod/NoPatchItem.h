struct _NpItemStat
{
	uint32  ItemStatType;
	int32   ItemStatMinValue;
	int32	ItemStatMaxValue;
};

struct _NpItemDamage
{
	uint32 MinDamageMin;
	uint32 MinDamageMax;
	uint32 MaxDamageMin;
	uint32 MaxDamageMax;
};

struct _NpItemDelay
{
	uint32 MinDelay;
	uint32 MaxDelay;
};

enum NpLevelTypes
{
	TYPE_MUI_VALUE,
	TYPE_ADD_VALUE,
};

struct _NpItemLevel
{
	uint8 LevelIndex;
	uint32 Level;
	uint32 ReqId;
	float Chance;

	uint32 SpellIndex;
	uint32 AddSpellCount;

	uint32 StatIndex;
	uint32 AddStatCount;

	uint8 LevelStatModIndex;

	NpLevelTypes DamageLevelType;
	float DamageValue;

	std::string Suffix;
	uint32 ItemLevel;
	uint32 Quality;

	uint32 EnchantIndex;
};

struct _NpItemLevelStatModTemplate
{
	uint8 LevelStatModIndex;
	NpLevelTypes ModType;
	uint32 StatType;
	float StatValue;
};

struct NpTemplate
{
	uint32 SpellIndex;
	uint32 StatIndex;
	uint32 StatCount;
	uint32 SpellCount;
	uint32 SrcIndex;
	uint32 LevelIndex;
	_NpItemDamage Damage;
	_NpItemDelay Delay;
	std::string Suffix;
	uint32 ItemLevel;
	int32 Quality;
};

struct NpSrcTemplate
{
	uint32 SrcIndex;
	uint32 SrcEntry;
	float SrcChance;
};

struct NpStatTemplate
{
	uint32 StatIndex;
	_NpItemStat Stat;
	float StatChance;
};

struct NpSpellTemplate
{
	uint32 SpellIndex;
	_Spell Spell;
	float SpellChance;
};

extern std::unordered_map<uint32/*entry*/, NpTemplate> NpMap;
extern std::vector<NpSrcTemplate> NpSrcVec;
extern std::vector<NpStatTemplate> NpStatVec;
extern std::vector<NpSpellTemplate> NpSpellVec;
extern std::vector<_NpItemLevel> NpLevelVec;
extern std::unordered_map<uint32, std::string> NpPrefixMap;
extern std::unordered_map<uint32, Item*> ItemQueryMap;
extern std::vector<_NpItemLevelStatModTemplate> NpLevelStatModVec;

struct NpToLevelTemplate
{
	float chance;
	uint32 level;
};

extern std::unordered_map<uint32, NpToLevelTemplate> NpToLevelMap;

struct MapNpTemplate
{
	uint32 MapId;
	Difficulty Diff;
	uint32 ChallengeLv;
	uint32 TempIndex;
	float Chance;
};

struct MapNpTempTemplate
{
	uint32 Quality;
	uint32 ItemLevel;
	std::string Suffix;
	std::string Prefix;
	uint32 LevelIndex;
	uint32 EnchantIndex;
	uint32 StatModIndex;
	std::unordered_map<uint32, uint32>StatAddMap;
	std::unordered_map<uint32, uint32>StatOverMap;
	int32 SpellIndex;
	uint32 SpellCount;
	uint32 WeaponIndex;
	uint32 SellRewId;
};

struct MapNpStatModTemplate
{
	uint32 StatModIndex;
	uint32 StatType;
	float ModStatMin;
	float ModStatMax;
};


struct MapNpEnchantTemplate
{
	uint32 EnchantIndex;
	uint8 Slot;
	uint32 EnchantGroupId;
};

struct MapNpWeaponTemplate
{
	uint32 WeaponIndex;
	uint8 SubClass;
	float MinDamageMod;
	float MaxDamageMod;
	_NpItemDamage Damage;
	_NpItemDelay Delay;
};

struct NpAuraData
{
	uint32 LevelIndex;
	uint32 Level;
};

struct NpAuraTemplate
{
	uint32 LevelIndex;
	uint32 Level;
	uint32 Count;
	uint32 Auras[5];
	uint32 RewId;
};

struct NpSameTemplate
{
	uint32 SrcTempIndex;
	uint32 TargetTempIndex;
	uint32 MeetLevel;
	uint32 RewId;
};

extern std::vector<MapNpTemplate> MapNpVector;
extern std::unordered_map<uint32, MapNpTempTemplate> MapNpTempMap;
extern std::vector<MapNpStatModTemplate> MapNpStatModVector;
extern std::vector<MapNpEnchantTemplate> MapNpEnchantVector;
extern std::vector<MapNpWeaponTemplate> MapNpWeaponVector;
extern std::vector<NpAuraTemplate> NpAuraVector;
extern std::vector<NpSameTemplate> NpSameTempIndexVector;

class NoPatchItem
{
public:
	static NoPatchItem* instance()
	{
		static NoPatchItem instance;
		return &instance;
	}
	void Load();
	void LoadMap();

	bool CalBase(uint32 entry, uint32 &SrcIndex, uint32 &SpellIndex, uint32 &SpellCount, uint32 &StatIndex, uint32 &StatCount, 
		uint32 &minDamage, uint32 &maxDamage, uint32 &delay, std::string &Suffix, uint32 &ItemLvel, int32 &Quality);
	bool CalLevel(Item* item, uint32 &L_ReqId, float &L_Chance,
		uint32 &L_SpellIndex, uint32 &L_AddSpellCount, uint32 &L_StatIndex, uint32 &L_AddStatCount,
		uint8 &L_LevelStatModIndex, NpLevelTypes &L_DamageLevelType, float &L_DamageValue,
		std::string &L_Suffix, uint32 &L_ItemLvel, uint32 &L_Quality, uint32 &L_EnchantIndex);
	int32 GetLevelStat(uint32 LevelStatModIndex, uint32 type, int32 value);
	bool GetExchange(Item* item, uint32 &L_ReqId, float &L_Chance);
	uint32 GetSellRewId(Item* item);
	std::string GetPrefix(Item* item);
	uint32 GetRandomSrcEntry(uint32 SrcIndex);
	_Spell GetRandomSpell(_Spell Spells[MAX_ITEM_PROTO_SPELLS], uint32 SpellIndex);
	_ItemStat GetRandomStat(uint32 StatIndex);

	uint32 GetItemLevel(uint64 LevelData)   { return uint32((LevelData & 0x000000000000FFFF)); }
	uint32 GetQuality(uint64 LevelData)		{ return uint32((LevelData & 0x0000000000FF0000) >> 16); }
	uint8 GetLevelNow(uint64 LevelData)		{ return uint8((LevelData & 0x00000000FF000000) >> 24); }
	uint32 GetLevelEntry(uint64 LevelData)	{ return uint32((LevelData & 0xFFFFFFFF00000000) >> 32); }
	uint64 GetLevelData(uint32 entry, uint8 LevelNow, uint32 Quality, uint32 ItemLvel){ return (uint64)entry << 32 | (uint64)LevelNow << 24 | (uint64)Quality << 16 | (uint64)ItemLvel; }

	uint64 GetMapData(uint32 mapId, uint8 levelNow, uint8 Difficulty, uint32 ChallengeLv)
	{
		return (uint64)mapId << 32 | (uint64)levelNow << 24 | (uint64)Difficulty << 16 | (uint64)ChallengeLv;
	}

	uint32 GetChallegeLv(uint64 MapData){ return uint32((MapData & 0x000000000000FFFF)); }
	uint8 GetDifficulty(uint64 MapData){ return uint8((MapData & 0x0000000000FF0000) >> 16); }
	uint8 GetMapLevelNow(uint64 MapData){ return uint8((MapData & 0x00000000FF000000) >> 24); }
	uint32 GetMapId(uint64 MapData)	{ return uint32((MapData & 0xFFFFFFFF00000000) >> 32); }

	void MergeStat(_ItemStat(&Stats)[MAX_ITEM_PROTO_STATS]);

	bool Create(Player* owner, Item* item);
	bool Create(Map* map, Item* item);
	bool CanCompound(Item* item);
	void GetTargetTemp(Item* item, uint32 &TargetTempIndex, uint32 &RewId);
	bool Compound(Player* owner, Item* item);
	Item* GetSameItem(Player* owner, Item* item);
	uint32 GetRandomTempIndex(uint32 MapId, Difficulty Diff, uint32 ChallengeLv);
	uint32 GetCreateEnchantId(uint32 EnchantIndex, uint8 Slot);

	bool LevelUp(Player* owner, Item* item);

	uint32 GetQueryId(Item* item){ return item->GetGUIDLow() + 1000000; }
	uint32 GetGUID(uint32 queryid){ return queryid - 1000000; }
	uint8 GetInsert(ItemTemplate temp);
	uint32 GetItemFlag(Item* item);
	void SetItemFlag(Item* item);

	bool ItemQuery(Player* player, uint32 item);

	bool CanOpenItem(Player* player);
	void GetMapPrefixSuffix(Item* item, std::string &prefix, std::string &suffix);

	uint32 GetLevelIndex(Item* item);
	void UpdateAuras(Player* pl);
	
private:

};
#define sNoPatchItem NoPatchItem::instance()