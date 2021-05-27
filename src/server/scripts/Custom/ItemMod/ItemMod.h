/*
classMask							itemMask
1			战士						1			八件套
2			骑士						2			饰品、戒指、披风、项链
4			猎人						4			双手、弓、枪、弩
8			盗贼						8			主手、单手
16			牧师						16			副手
32			死亡骑士					32			远程、魔杖
64			萨满						64			衬衣
128			法师						128			战袍
256			术士						256			带光效的武器FM
1024		德鲁伊					512			符印

全职业		classMask	1535		
纯物理		classMask	45
纯法系		classMask	400
混合职业		classMask	1090
flagMsk
同一个mask 只能出现强化其中一个

*/

#define TRANS_FLAG_ENCHANT_ID 6000000

//强化位置数量

#define COUNT_SUIT							2
#define COUNT_ORNAMENT						2
#define COUNT_2H_WEAPON						3
#define COUNT_1H_WEAPON						3
#define COUNT_OFFHAND						3
#define COUNT_RANGED						3
#define COUNT_SHIRT							2
#define COUNT_TABARD						2
#define COUNT_SIGIL							10

//保留强化效果开关
#define KEEP_ENCHANT_SUIT					true
#define KEEP_ENCHANT_ORNAMENT				true
#define KEEP_ENCHANT_2H_WEAPON				true
#define KEEP_ENCHANT_1H_WEAPON				true
#define KEEP_ENCHANT_OFFHAND				true
#define KEEP_ENCHANT_RANGED					true
#define KEEP_ENCHANT_SHIRT					true
#define KEEP_ENCHANT_TABARD					true

//FM等动作发生时施法的视觉效果技能ID
#define VISUAL_SPELL_ID						61456

// 查看光效FM时间
#define LOOKUP_WEAPON_PERM_SECONDS			8

//每页显示最大FM行数
#define MAX_ROWS_COUNT						25

//符印是否自带FM，设置初始ID SIGIL_OWN_ENCHANT_ID_START（end = start + COUNT_SIGIL）
//SIGIL_OWN_ENCHANT_TITLE_0,SIGIL_OWN_ENCHANT_TITLE_1 符印可自带标题 点击回到当前页 如果不用 可设置为 -1
#define SIGIL_OWN_ENCHANT					true
#define SIGIL_OWN_ENCHANT_ID_START			220000 
#define SIGIL_OWN_ENCHANT_TITLE_0			220000
#define SIGIL_OWN_ENCHANT_TITLE_1			220006


//鉴定和卜卦reqId
#define ITEM_IDENTIFY_REQID					2000
#define ITEM_ASTROLOGY_REQID				2001

//默认移除FM 金币数量
#define DEFAULT_REMOVE_ENCHANT_GOLDS		0



#define MENU_ID 7890
#define ACTION_MAINMENU_BACK						1001
#define ACTION_ITEM_ENCHANT_CURR_MENU_SHOW			1002
#define ACTION_WEAPON_PERMENCHANT_CURR_MENU_SHOW	1003
#define ACTION_ITEM_ENCHANT_NEXT_MENU_SHOW			1004
#define ACTION_WEAPON_PERMENCHANT_NEXT_MENU_SHOW	1005
#define ACTION_SIGIL_ENCHANT_NEXT_MENU_SHOW			1006
#define ACTION_ITEM_ENCHANT							1007
#define ACTION_ITEM_REMOVE_ENCHANT					1008
#define ACTION_REMOVE_ENCHANT_UNDEFINE				1009
#define ACTION_ITEM_EXCHANGE						1010
#define ACTION_ITEM_NO_PATCH_COMPOUND				1015
#define ACTION_GEM_MENU_SHOW						1011
#define ACTION_ITEM_REMOVEBIND						1012
#define ACTION_ITEM_REMOVEGEM						1013
#define ACTION_ITEM_IDENTIFY						1014

#define ACTION_PREV_PAGE							1016

#define ACTION_WEAPON_LOOKUPANDBUY_SHOW				1017
#define ACTION_WEAPONPERM_LOOKUP					1028
#define ACTION_WEAPONPERM_BUY						1029
#define ACTION_TRANS								1030

const std::string ENCHANT_ACTION_STRING = "镶嵌";
const std::string ENCHANT_NAME_STRING	= "符文";



struct UIItemEntryTemplate
{
	std::string des;
	std::string heroText;
	uint32 daylimit;
	uint32 maxGems;
	uint32 exchange1;
	uint32 exchangeReqId1;
	uint32 exchange2;
	uint32 exchangeReqId2;
	uint32 unbindReqId;
	uint32 useReqId;
	uint32 equipReqId;
	uint32 buyReqId;
	uint32 sellRewId;
	uint32 recoveryRewId;
	uint32 gs;
};

extern std::unordered_map<uint32/*entry*/, UIItemEntryTemplate> UIItemEntryMap;

//item buy  equip sale use 

struct ItemBuyTemplate
{
	uint32 entry;
	uint32 reqId;
};
extern std::vector<ItemBuyTemplate> ItemBuyVec;

struct ItemVendorBuyTemplate
{
	uint32 item;
	uint32 reqId;
	uint32 vendor;
	uint8 clientSlot;
	uint32 buyMaxCount;
};
extern std::vector<ItemVendorBuyTemplate> ItemVendorBuyVec;

extern std::unordered_map<uint32/*entry*/,uint32/*reqId*/> ItemEquipMap;

struct ItemSaleTemplate
{
	uint32 entry;
	uint32 rewId;
	uint32 rewChance;
	std::string command;
};
extern std::vector<ItemSaleTemplate> ItemSaleVec;

struct ItemUseTemplate
{
	uint32 entry;
	uint32 reqId;
	uint32 rewId;
	uint32 rewChance;
	std::string command;
	uint32 spellId1;
	uint32 spellId2;
	uint32 spellId3;
};
extern std::vector<ItemUseTemplate> ItemUseVec;

struct ItemDesTemplate
{
	std::string description;
	std::string heroText;
};

extern std::unordered_map<uint32, ItemDesTemplate> ItemDesMap;

//隐藏物品
struct HiddenItemTemplate
{
	uint32 entry;
};
extern std::vector<HiddenItemTemplate> HiddenItemInfo;

//item Mod
struct ItemModTemplate
{
	uint32 entry;
	uint32 src_entry;
	float stat_muilt;
	float mindmg_muilt;
	float maxdmg_muilt;
};
extern std::vector<ItemModTemplate> ItemModVec;

//ItemUnbindCost
struct ItemUnbindCostTemplate
{
	uint32 entry;
	uint32 reqId;
};
extern std::vector<ItemUnbindCostTemplate> ItemUnbindCostInfo;

////去除宝石////
struct RemoveGemTemplate
{
	uint32 entry;
	uint32 reqId;
	uint32 chance;
};
extern std::vector<RemoveGemTemplate> RemoveGemInfo;

////装备升级////

/*
ALTER TABLE _itemmod_exchange_item ADD destroyOnFail BOOL DEFAULT FALSE AFTER chance;
ALTER TABLE _itemmod_exchange_item ADD rewIdOnFail INT UNSIGNED DEFAULT 0 AFTER destroy;
ALTER TABLE _itemmod_exchange_item ADD keepEnchant BOOL DEFAULT TRUE AFTER exchangedItem;
*/

struct ItemExchangeTemplate
{
	uint32 item;
	uint32 exchangeditem;
	uint32 reqId;
	uint32 chance;
	uint32 flag;//0 合成//强化
	uint32 rewIdOnFail;
	bool destroyOnFail;
	bool keepEnchant;
};
extern std::vector<ItemExchangeTemplate> ItemExchangeInfo;

////装备强化////
struct ItemUpgradeTemplate
{
	uint32 enchantId;
	uint32 prevEnchantId;
	uint32 enchantReqId;
	uint32 removeEnchantRewId;
	std::string description;
	uint32 chance;

	uint32 itemMask;
	uint32 enchantMask;
	uint32 classMask;
};
extern std::vector<ItemUpgradeTemplate> UpgradeVec;

struct FilterTemplate
{
	uint32 enchantId;
	uint32 prevEnchantId;
	uint32 enchantReqId;
	uint32 removeEnchantRewId;
	std::string description;
	uint32 chance;

	uint32 itemMask;
	uint32 enchantMask;
	uint32 classMask;
};
extern std::vector<FilterTemplate> FilterVec;

//宝石上限
struct GemCountLimitTemplate
{
	uint32 entry;
	uint32 limitCount;
};
extern std::vector<GemCountLimitTemplate> GemCountLimitInfo;

extern std::unordered_map<uint32/*entry*/, uint32/*count*/> GetmCountLimitMap;

//鉴定和卦象
struct IdentifyTemplate
{
	uint32 entry;
	uint32 groupId;
	uint32 reqId;
	uint32 slot;
	std::string gossipText;
	bool slotHasEnchant;
};
extern std::vector<IdentifyTemplate> IdentifyVec;


struct CreateEnchantTemplate
{
	uint32 entry;
	uint32 groupId;
	uint8 slot;
	float chance;
};
extern std::vector<CreateEnchantTemplate> CreateEnchantVec;

struct EnchantGroupTemplate
{
	uint32 groupId;
	uint32 enchantId;
	float chance;
};
extern std::vector<EnchantGroupTemplate> EnchantGroupVec;
extern std::list<uint32> GCAddonEnchantGroupVec;

//装备组
struct ItemAddTemplate
{
	uint32 categoryId;
	uint32 entry;
	uint32 count;
};
extern std::vector<ItemAddTemplate> ItemAddVec;

enum ITEM_ENCHANT_TYPES
{
	ITEM_ENCHANT_UPGRADE,
	ITEM_ENCHANT_WEAPON_PERM,
	ITEM_ENCHANT_SIGIL,
};

enum ITEM_MASK
{
	ITEM_MASK_NONE			=	0,
	ITEM_MASK_SUIT			=	1,
	ITEM_MASK_ORNAMENT		=	2,
	ITEM_MASK_2H_WEAPON		=	4,
	ITEM_MASK_1H_WEAPON		=	8,
	ITEM_MASK_OFFHAND		=	16,
	ITEM_MASK_RANGED		=	32,
	ITEM_MASK_SHIRT			=	64,
	ITEM_MASK_TABARD		=	128,
	ITEM_MASK_WEAPON_PERM	=	256,
	ITEM_MASK_SIGIL			=	512,
};

enum RateStoneTypes
{
	RATE_ITEM_EXCHANGE_0 = 1,
	RATE_ITEM_EXCHANGE_1,
	RATE_ITEM_STRENGTHEN,
	RATE_SIGIL,
	RATE_GEM_REMOVE,
};

struct RateStoneTemplate
{
	uint32 entry;
	RateStoneTypes type;
	uint32 rate;
};

extern std::vector<RateStoneTemplate> RateStoneVec;

extern std::vector<uint32 /*itemid*/> CurrencyLikeItemVec;

extern std::unordered_map<uint32, uint32> DayLimitItemMap;

class ItemMod
{
public:
	static ItemMod* instance()
	{
		static ItemMod instance;
		return &instance;
	}

	void Load();
	void LoadBuyEquipSaleUse();
	std::string GetExDes(uint32 entry, uint32 flag = 0);
	std::string GetUnbindDes(uint32 entry);
	std::string GetMaxGemDes(uint32 entry);

	uint32 GetBuyReqId(uint32 entry, uint32 vendor, uint8 slot);
	uint32 GetBuyMaxCount(uint32 entry, uint32 vendor, uint8 slot);
	uint32 GetEquipInfo(uint32 entry);
	void GetSaleInfo(uint32 entry, uint32 &rewId, uint32 &rewChance, std::string &command);
	void GetUseInfo(uint32 entry, uint32 &reqId, uint32 &rewId, uint32 &rewChance, std::string &command, uint32 &spellId1, uint32 &spellId2, uint32 &spellId3);

	uint8 getSlot(uint32 sender); 
	uint32 getEnchant(uint32 sender);
	uint32 getGemId(uint32 sender);
	uint32 senderValue(uint8 slot, uint32 id);
	std::string GetEnchantDescription(Item* item, uint32 enchantId);
	std::string GetItemEnchantDescription(Player* player, uint32 enchantId);
	std::string GetGemName(Item* item, uint8 slot);
	uint32 getGemId(Item* item, uint8 slot);

	void ExchangeItem(Player* player, Item* item, uint32 update = 0/*0 合成 1强化*/);
	void RemoveBind(Player* player, Item* item);
	Item* AddItem(Player* player, uint32 itemId, uint32 count);
	void AddGemList(Player* player, Item* item);
	void RemoveGem(Player* player, Item* item, uint32 sender);
	void RemoveItemEnchant(Player* player, Item* item, uint8 slot, uint32 enchantId);
	void RemoveUndefinEnchant(Player* player, Item* item, uint8 slot, uint32 enchantId);

	uint32 GetItemMask(uint32 entry);
	bool CanApply(uint32 mask, Item* item, ITEM_ENCHANT_TYPES type = ITEM_ENCHANT_UPGRADE);
	bool CheckClass(Player* player, uint32 classMask);

	bool hasEnchantByMask(Item* item, uint32 enchantId);
	void setEnchantMask(Item* item);

	void AddCastMenu(Player* player, Item* item);
	bool AddItemExchangeMenu(Player* player, Item* item, uint32 update = 0/*0 合成 1强化 2NoPatchItem*/);
	bool AddItemRemovBindMenu(Player* player, Item* item);
	bool AddRemoveGemMenu(Player* player, Item* item);

	bool AddItemEnchantMenu(Player* player, Item* item);
	void AddItemCurrEnchantList(Player* player, Item* item, ITEM_ENCHANT_TYPES type = ITEM_ENCHANT_UPGRADE);
	void AddItemNextEnchantList(Player* player, Item* item, uint32 sender, ITEM_ENCHANT_TYPES type = ITEM_ENCHANT_UPGRADE);
	void EnchantItem(Player* player, Item* item, uint8 slot, uint32 enchantId);

	uint8 GetEnchantStartSlot(uint32 itemMask);

	bool IsUpgradeSetted(Player* player, Item* item, ITEM_ENCHANT_TYPES type = ITEM_ENCHANT_UPGRADE);

	//鉴定和封象
	uint32 GetIdentifyGroupId(uint32 entry, EnchantmentSlot slot);
	uint32 GetIdentifyEnchantId(uint32 entry, EnchantmentSlot slot);
	void GetIdentifyInfo(uint32 entry, EnchantmentSlot slot, uint32 &reqId, std::string &gossipText, bool &slotHasEnchant);
	bool AddIdentifyMenu(Player* player, Item* item);
	

	void RefreshItem(Player* player, Item* item, EnchantmentSlot slot);


	void SetFilterVec(Player* player, Item* item, ITEM_ENCHANT_TYPES type, uint32 currEnchantId);

	void LookupOrBuyWeaponPermEnchant(Player* player, Item* item, uint32 sender);
	void LookupWeaponPermEnchant(Player* player, Item* item, uint32 enchantId);
	void BuyWeaponPermEnchant(Player* player, Item* item, uint32 enchantId);
	void AddWeaponPermList(Player* player, Item* item);
	void RecoverWeaponPermEnchant(Player* player);

	//几率宝石
	uint32 GetRate(Player* player, RateStoneTypes type);
	uint32 GetRateAndDes(Player* player, RateStoneTypes type);
	//自带FM
	uint32 GenerateEnchantId(uint32 entry,uint8 slot);

	//项链成长
	void NeckUp(Player* player, bool isPVP);

	//幻化
	bool HasTransFlag(Item* item);

	void ApplyHiddenItem(Player* player, Item* item, bool apply);

	bool IsCurrencyLike(uint32 itemid);

	void ResetDayLimitItem();
	std::string GetDayLimitDes(Player* player, uint32 entry);

	std::string GetUseDes(uint32 entry);
	std::string GetEquipDes(uint32 entry);
	std::string GetBuyDes(uint32 entry);
	void GetDes(uint32 entry, std::string &description, std::string &heroText);

	void AddCategoryItem(Player* player, Player* target, uint32 categoryId);
	

	void UnApplyEquipments(Player* player);

	//void AddEnchant(Player* player, uint32 enchantId);
	//void LoadEnchant(Player* player);
	//bool HasEnchant(Player* player, uint32 enchantId);

	bool IsGemLimited(Player* player);

	int32 GetGemCount(Item* pItem, uint32 gemEntry);
	bool IsGemLimited(Player* player, Item* pItem1, Item* pItem2);

	void SendData(Player* player);
	std::string GetUIEnchantDescription(Item* item, EnchantmentSlot slot);


	void InitUIItemEntryData();

private:

};
#define sItemMod ItemMod::instance()
