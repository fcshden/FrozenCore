#pragma execution_character_set("utf-8")
#include "../PrecompiledHeaders/ScriptPCH.h"
#include "../DataLoader/DataLoader.h"
#include "../CommonFunc/CommonFunc.h"
#include "../Requirement/Requirement.h"
#include "../Reward/Reward.h"
#include "ItemMod.h"
#include "Object.h"
#include "../VIP/VIP.h"
#include "../HonorRank/HonorRank.h"
#include "../ExtraEquipment/ExtraEquipment.h"
#include "../String/myString.h"
#include "../GCAddon/GCAddon.h"
#include "../ItemSet/ItemSet.h"
#include "../DBCCreate/DBCCreate.h"
#include "NoPatchItem.h"
#include "../Switch/Switch.h"
#include "../Recovery/Recovery.h"
#include "../GS/GS.h"

std::unordered_map<uint32/*entry*/, UIItemEntryTemplate> UIItemEntryMap;

uint8 ItemMod::GetEnchantStartSlot(uint32 itemMask)
{
	switch (itemMask)
	{
	case ITEM_MASK_SUIT:
		return MAX_ENCHANTMENT_SLOT - atoi(sSwitch->GetFlagByIndex(ST_ENCHANTSLOT_MAX, 1).c_str());
	case ITEM_MASK_ORNAMENT:
		return MAX_ENCHANTMENT_SLOT - atoi(sSwitch->GetFlagByIndex(ST_ENCHANTSLOT_MAX, 2).c_str());
	case ITEM_MASK_2H_WEAPON:		 
		return MAX_ENCHANTMENT_SLOT - atoi(sSwitch->GetFlagByIndex(ST_ENCHANTSLOT_MAX, 3).c_str());
	case ITEM_MASK_1H_WEAPON:		 
		return MAX_ENCHANTMENT_SLOT - atoi(sSwitch->GetFlagByIndex(ST_ENCHANTSLOT_MAX, 4).c_str());
	case ITEM_MASK_OFFHAND:			  
		return MAX_ENCHANTMENT_SLOT - atoi(sSwitch->GetFlagByIndex(ST_ENCHANTSLOT_MAX, 5).c_str());
	case ITEM_MASK_RANGED:			
		return MAX_ENCHANTMENT_SLOT - atoi(sSwitch->GetFlagByIndex(ST_ENCHANTSLOT_MAX, 6).c_str());
	case ITEM_MASK_SHIRT:			
		return MAX_ENCHANTMENT_SLOT - atoi(sSwitch->GetFlagByIndex(ST_ENCHANTSLOT_MAX, 7).c_str());
	case ITEM_MASK_TABARD:			
		return MAX_ENCHANTMENT_SLOT - atoi(sSwitch->GetFlagByIndex(ST_ENCHANTSLOT_MAX, 8).c_str());
	case ITEM_MASK_SIGIL:
		return MAX_ENCHANTMENT_SLOT - COUNT_SIGIL;
	}

	return 0;
}


std::vector<ItemModTemplate> ItemModVec;
std::vector<ItemUnbindCostTemplate> ItemUnbindCostInfo;
std::vector<RemoveGemTemplate> RemoveGemInfo;
std::vector<ItemExchangeTemplate> ItemExchangeInfo;
std::vector<ItemUpgradeTemplate> UpgradeVec;
std::vector<FilterTemplate> FilterVec;
std::vector<GemCountLimitTemplate> GemCountLimitInfo;
std::vector<HiddenItemTemplate> HiddenItemInfo;
std::vector<IdentifyTemplate> IdentifyVec;
std::vector<RateStoneTemplate> RateStoneVec;
std::vector<CreateEnchantTemplate> CreateEnchantVec;
std::vector<EnchantGroupTemplate> EnchantGroupVec;
std::list<uint32> GCAddonEnchantGroupVec;
std::vector<ItemBuyTemplate> ItemBuyVec;
std::vector<ItemVendorBuyTemplate> ItemVendorBuyVec;
std::unordered_map<uint32/*entry*/, uint32/*reqId*/> ItemEquipMap;
std::vector<ItemSaleTemplate> ItemSaleVec;
std::vector<ItemUseTemplate> ItemUseVec;
std::vector<uint32 /*itemid*/> CurrencyLikeItemVec;
std::unordered_map<uint32, uint32> DayLimitItemMap;
std::unordered_map<uint32, ItemDesTemplate> ItemDesMap;
std::vector<ItemAddTemplate> ItemAddVec;

std::unordered_map<uint32/*entry*/, uint32/*count*/> GetmCountLimitMap;

void ItemMod::LoadBuyEquipSaleUse()
{
	ItemVendorBuyVec.clear();
	QueryResult result = WorldDatabase.PQuery("SELECT item, reqId, entry, clientSlot, buyMaxCount from npc_vendor");
	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			ItemVendorBuyTemplate Temp;
			Temp.item = fields[0].GetUInt32();
			Temp.reqId = fields[1].GetUInt32();
			Temp.vendor = fields[2].GetUInt32();
			Temp.clientSlot = fields[3].GetUInt8();
			Temp.buyMaxCount = fields[4].GetUInt32();
			ItemVendorBuyVec.push_back(Temp);
		} while (result->NextRow());
	}

	ItemBuyVec.clear();
	result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 物品ID,需求模板ID from _物品_当购买时" :
		"SELECT entry,reqId from _itemmod_on_buy");
	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			ItemBuyTemplate Temp;
			Temp.entry = fields[0].GetUInt32();
			Temp.reqId = fields[1].GetUInt32();
			ItemBuyVec.push_back(Temp);
		} while (result->NextRow());
	}

	ItemEquipMap.clear();
	result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ?
		"SELECT 物品ID,需求模板ID from _物品_当装备时" :
		"SELECT entry,reqId from _itemmod_on_equip");
	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			ItemEquipMap.insert(std::make_pair(fields[0].GetUInt32(), fields[1].GetUInt32()));
		} while (result->NextRow());
	}

	ItemSaleVec.clear();
	result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ?
		"SELECT 物品ID,奖励模板ID,获得奖励几率,GM命令组 from _物品_当售卖时" :
		"SELECT entry,rewId,rewChance,command from _itemmod_on_sale");
	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			ItemSaleTemplate Temp;
			Temp.entry		= fields[0].GetUInt32();
			Temp.rewId		= fields[1].GetUInt32();
			Temp.rewChance	= fields[2].GetUInt32();
			Temp.command	= fields[3].GetString();
			ItemSaleVec.push_back(Temp);
		} while (result->NextRow());
	}

	ItemUseVec.clear();
	result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ?
		"SELECT 物品ID,需求模板ID,奖励模板ID,获得奖励几率,GM命令组,触发技能ID1,触发技能ID2,触发技能ID3 from  _物品_当使用时" :
		"SELECT entry,reqId,rewId,rewChance,command,spell1,spell2,spell3 from _itemmod_on_use");
	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			ItemUseTemplate Temp;
			Temp.entry		= fields[0].GetUInt32();
			Temp.reqId		= fields[1].GetUInt32();
			Temp.rewId		= fields[2].GetUInt32();
			Temp.rewChance	= fields[3].GetUInt32();
			Temp.command	= fields[4].GetString();
			Temp.spellId1	= fields[5].GetUInt32();
			Temp.spellId2	= fields[6].GetUInt32();
			Temp.spellId3	= fields[7].GetUInt32();
			ItemUseVec.push_back(Temp);
		} while (result->NextRow());
	}

	ItemDesMap.clear();
	result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 物品ID,额外描述,英雄模式文本 from _物品_额外描述" :
		"SELECT Entry,Description,HeroText from _itemmod_description");
	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			uint32 entry = fields[0].GetUInt32();
			ItemDesTemplate temp;
			temp.description = fields[1].GetString();
			temp.heroText = fields[2].GetString();
			ItemDesMap.insert(std::make_pair(entry, temp));
		} while (result->NextRow());
	}

	ItemAddVec.clear();
	//result = WorldDatabase.PQuery("SELECT categoryId,entry,count from _itemmod_add");
	//if (result)
	//{
	//	do
	//	{
	//		Field* fields = result->Fetch();
	//		ItemAddTemplate Temp;
	//		Temp.categoryId = fields[0].GetUInt32();
	//		Temp.entry		= fields[1].GetUInt32();
	//		Temp.count		= fields[2].GetUInt32();
	//		ItemAddVec.push_back(Temp);
	//	} while (result->NextRow());
	//}
}


void ItemMod::Load()
{
	LoadBuyEquipSaleUse();

	RateStoneVec.clear();
	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 物品ID,类型,提高成功几率 from _物品_几率宝石" :
		"SELECT entry,type,rate from _itemmod_rate_stone");
	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			RateStoneTemplate Temp;
			Temp.entry	= fields[0].GetUInt32();
			Temp.type	= RateStoneTypes(fields[1].GetUInt32());
			Temp.rate	= fields[2].GetUInt32();
			RateStoneVec.push_back(Temp);
		} while (result->NextRow());
	}


	ItemUnbindCostInfo.clear();
	result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 物品ID,需求模板ID from _物品_解除绑定" :
		"SELECT entry,reqId from _itemmod_unbind");
	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			ItemUnbindCostTemplate ItemUnbindCostTemp;
			ItemUnbindCostTemp.entry = fields[0].GetUInt32();
			ItemUnbindCostTemp.reqId = fields[1].GetUInt32();

			ItemUnbindCostInfo.push_back(ItemUnbindCostTemp);
		} while (result->NextRow());
	}


	RemoveGemInfo.clear();
	result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 宝石物品ID, 需求模板ID,成功几率 FROM _物品_移除宝石" :
		"SELECT entry, reqId,chance FROM _itemmod_gem_remove");
	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			RemoveGemTemplate RemoveGemTemp;
			RemoveGemTemp.entry = fields[0].GetUInt32();
			RemoveGemTemp.reqId = fields[1].GetUInt32();
			RemoveGemTemp.chance = fields[2].GetUInt32();
			RemoveGemInfo.push_back(RemoveGemTemp);
		} while (result->NextRow());
	}


	ItemExchangeInfo.clear();
	result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 物品ID, 升级后物品ID, 需求模板ID,成功几率,升级方式,失败时奖励模板ID,失败时是否摧毁原物品,升级后是否保留附魔效果 FROM _物品_升级" :
		"SELECT item, exchangedItem, reqId,chance,upFlag,rewIdOnFail,destroyOnFail,keepEnchant FROM _itemmod_exchange_item");
	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			ItemExchangeTemplate ItemExchangeTemp;
			ItemExchangeTemp.item			= fields[0].GetUInt32();
			ItemExchangeTemp.exchangeditem	= fields[1].GetUInt32();
			ItemExchangeTemp.reqId			= fields[2].GetUInt32();
			ItemExchangeTemp.chance			= fields[3].GetUInt32();
			ItemExchangeTemp.flag			= fields[4].GetBool();
			ItemExchangeTemp.rewIdOnFail	= fields[5].GetUInt32();
			ItemExchangeTemp.destroyOnFail	= fields[6].GetBool();
			ItemExchangeTemp.keepEnchant	= fields[7].GetBool();
			ItemExchangeInfo.push_back(ItemExchangeTemp);
		} while (result->NextRow());
	}

	UpgradeVec.clear();
	result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 附魔ID, 上级附魔ID, 需求模板ID,移除时附魔奖励模板ID,菜单文本,成功几率,物品类型掩码,分组掩码,职业掩码 FROM _物品_强化" :
		"SELECT enchant_id, prev_enchant_id, enchantReqId,removeEnchantRewId,description,chance,itemMask,flagMask,classMask FROM _itemmod_strengthen_item");

	if (result)
	{
		do
		{
			Field* fields = result->Fetch();

			ItemUpgradeTemplate ItemUpgradeTemp;
			ItemUpgradeTemp.enchantId = fields[0].GetUInt32();
			ItemUpgradeTemp.prevEnchantId = fields[1].GetUInt32();
			ItemUpgradeTemp.enchantReqId = fields[2].GetUInt32();
			ItemUpgradeTemp.removeEnchantRewId = fields[3].GetUInt32();
			ItemUpgradeTemp.description = fields[4].GetString();
			ItemUpgradeTemp.chance = fields[5].GetUInt32();
			ItemUpgradeTemp.itemMask = fields[6].GetUInt32();
			ItemUpgradeTemp.enchantMask = fields[7].GetUInt32();
			ItemUpgradeTemp.classMask = fields[8].GetUInt32();
			UpgradeVec.push_back(ItemUpgradeTemp);
		} while (result->NextRow());
	}

	GemCountLimitInfo.clear();
	GetmCountLimitMap.clear();
	result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 宝石物品ID,数量上限 FROM _物品_宝石上限" :
		"SELECT entry,limitCount FROM _itemmod_gem_limit");
	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			

			uint32 entry = fields[0].GetUInt32();
			if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(entry))
				if (uint32 count = fields[1].GetUInt32())
				{
					GetmCountLimitMap.insert(std::make_pair(entry, count));

					GemCountLimitTemplate GemCountLimitTemp;
					GemCountLimitTemp.entry = entry;
					GemCountLimitTemp.limitCount = count;
					GemCountLimitInfo.push_back(GemCountLimitTemp);
				}
					

		} while (result->NextRow());
	}
	
	IdentifyVec.clear();
	result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 物品ID,附魔组模板ID,需求模板ID,附魔位置,菜单文本,是否需要当前位置有附魔 FROM _物品_刷新附魔" :
		"SELECT entry,groupId,reqId,slot,gossipText FROM _itemmod_refresh");
	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			IdentifyTemplate Temp;
			Temp.entry = fields[0].GetUInt32();
			Temp.groupId = fields[1].GetUInt32();
			Temp.reqId = fields[2].GetUInt32();
			Temp.slot = fields[3].GetUInt32();
			Temp.gossipText = fields[4].GetString();
			Temp.slotHasEnchant = fields[5].GetBool();
			IdentifyVec.push_back(Temp);
		} while (result->NextRow());
	}


	CreateEnchantVec.clear();
	result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 物品ID,附魔组模板ID,附魔位置,自带附魔几率 FROM _物品_自带附魔" :
		"SELECT entry,groupId,slot,chance FROM _itemmod_creation_enchant");
	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			CreateEnchantTemplate Temp;
			Temp.entry = fields[0].GetUInt32();
			Temp.groupId = fields[1].GetUInt32();
			Temp.slot = fields[2].GetUInt8();
			Temp.chance = fields[3].GetFloat();
			CreateEnchantVec.push_back(Temp);
		} while (result->NextRow());
	}
	

	EnchantGroupVec.clear();
	GCAddonEnchantGroupVec.clear();
	result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 附魔组模板ID,附魔ID,抽取几率 FROM _模板_附魔组" :
		"SELECT groupId,enchantId,chance FROM _itemmod_enchant_groups");
	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			EnchantGroupTemplate Temp;
			Temp.groupId = fields[0].GetUInt32();
			uint32 EnchantId = fields[1].GetUInt32();
			Temp.enchantId = EnchantId;
			Temp.chance = fields[2].GetFloat();
			EnchantGroupVec.push_back(Temp);

			auto it = std::find(GCAddonEnchantGroupVec.begin(), GCAddonEnchantGroupVec.end(), EnchantId);
			if (it == GCAddonEnchantGroupVec.end())
				GCAddonEnchantGroupVec.push_back(EnchantId);

		} while (result->NextRow());
	}
	

	sObjectMgr->RestItemQueryData();

	HiddenItemInfo.clear();
	result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 物品ID FROM _物品_背包中获得属性" :
		"SELECT entry FROM _itemmod_hidden");
	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			HiddenItemTemplate HiddenItemTemp;
			HiddenItemTemp.entry = fields[0].GetUInt32();
			HiddenItemInfo.push_back(HiddenItemTemp);
		} while (result->NextRow());
	}

	CurrencyLikeItemVec.clear();
	result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 物品ID FROM _物品_徽章掉落方式" :
		"SELECT entry FROM _itemmod_currency_like");
	if (result)
	{
		do
		{
			CurrencyLikeItemVec.push_back(result->Fetch()[0].GetUInt32());
		} while (result->NextRow());
	}


	DayLimitItemMap.clear();
	result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 物品ID,每日上限 FROM _物品_每日上限" :
		"SELECT entry,limitCount FROM _itemmod_day_limit");
	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			DayLimitItemMap.insert(std::make_pair(fields[0].GetUInt32(), fields[1].GetUInt32()));
		} while (result->NextRow());
	}
}

std::string ItemMod::GetExDes(uint32 entry, uint32 flag)
{
	std::ostringstream oss;
	uint32 reqId = 0;
	uint32 len = ItemExchangeInfo.size();

	for (size_t i = 0; i < len; i++)
		if (entry == ItemExchangeInfo[i].item && flag == ItemExchangeInfo[i].flag)
		{
			ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(ItemExchangeInfo[i].exchangeditem);
			if (pProto)
			{
				reqId = ItemExchangeInfo[i].reqId;
				break;
			}
		}
	
	if (reqId != 0)
	{
		if (flag == 0)
			oss << "|cFF00FF00「升级需要满足」|r\n";
		else
			oss << "|cFF00FF00「升级需要满足」|r\n";

		oss << sReq->GetExtraDes(reqId);
	}
	
	return oss.str();
}

std::string ItemMod::GetUnbindDes(uint32 entry)
{
	const ItemTemplate * temp = sObjectMgr->GetItemTemplate(entry);
	if (!temp)
		return "";

	std::ostringstream oss;
	uint32 reqId = 0;

	uint32 len = ItemUnbindCostInfo.size();

	for (size_t i = 0; i < len; i++)
	{
		if (entry == ItemUnbindCostInfo[i].entry)
		{
			reqId = ItemUnbindCostInfo[i].reqId;
			break;
		}			
	}

	if (reqId != 0)
	{
		oss << "|cFF00FF00「解绑需要满足」|r\n";
		oss << sReq->GetExtraDes(reqId);
	}
	
	return oss.str();
}

std::string ItemMod::GetMaxGemDes(uint32 entry)
{
	std::ostringstream oss;

	uint32 len = GemCountLimitInfo.size();

	for (size_t i = 0; i < len; i++)
	{
		if (entry == GemCountLimitInfo[i].entry)
		{
			oss << "|cFF00FF00「最大镶嵌数量";
			oss << GemCountLimitInfo[i].limitCount;
			oss << "」|r\n";
			return oss.str();
		}			
	}
	return "";
}

uint32 ItemMod::GetBuyReqId(uint32 entry, uint32 vendor, uint8 slot)
{
	std::vector<ItemVendorBuyTemplate>::iterator it;
	for (it = ItemVendorBuyVec.begin(); it != ItemVendorBuyVec.end(); ++it)
		if (entry == it->item && vendor == it->vendor && slot == it->clientSlot)
		{
			if (it->reqId != 0)
				return it->reqId;

			break;
		}


	std::vector<ItemBuyTemplate>::iterator itr;
	for (itr = ItemBuyVec.begin(); itr != ItemBuyVec.end(); ++itr)
		if (entry == itr->entry)
			return itr->reqId;

	return 0;
}

uint32 ItemMod::GetBuyMaxCount(uint32 entry, uint32 vendor, uint8 slot)
{
	std::vector<ItemVendorBuyTemplate>::iterator it;
	for (it = ItemVendorBuyVec.begin(); it != ItemVendorBuyVec.end(); ++it)
		if (entry == it->item && vendor == it->vendor && slot == it->clientSlot)
			return it->buyMaxCount;

	return 0;
}

uint32 ItemMod::GetEquipInfo(uint32 entry)
{
	const ItemTemplate * temp = sObjectMgr->GetItemTemplate(entry);
	if (!temp)
		return 0;

	std::unordered_map<uint32/*entry*/, uint32/*reqId*/>::iterator itr = ItemEquipMap.find(entry);
	if (itr != ItemEquipMap.end())
		return itr->second;

	return 0;
}
void ItemMod::GetSaleInfo(uint32 entry, uint32 &rewId, uint32 &rewChance, std::string &command)
{
	const ItemTemplate * temp = sObjectMgr->GetItemTemplate(entry);
	if (!temp)
		return;

	std::vector<ItemSaleTemplate>::iterator itr;
	for (itr = ItemSaleVec.begin(); itr != ItemSaleVec.end(); ++itr)
		if (entry == itr->entry)
		{
			rewId = itr->rewId;
			rewChance = itr->rewChance;
			command = itr->command;
			break;
		}
}
void ItemMod::GetUseInfo(uint32 entry, uint32 &reqId, uint32 &rewId, uint32 &rewChance, std::string &command, uint32 &spellId1, uint32 &spellId2, uint32 &spellId3)
{
	const ItemTemplate * temp = sObjectMgr->GetItemTemplate(entry);
	if (!temp)
		return;

	std::vector<ItemUseTemplate>::iterator itr;
	for (itr = ItemUseVec.begin(); itr != ItemUseVec.end(); ++itr)
		if (entry == itr->entry)
		{
			reqId = itr->reqId;
			rewId = itr->rewId;
			rewChance = itr->rewChance;
			command = itr->command;
			spellId1 = itr->spellId1;
			spellId2 = itr->spellId2;
			spellId3 = itr->spellId3;
			break;
		}
}

uint8 ItemMod::getSlot(uint32 sender) {
	return (uint8)((sender - GOSSIP_SENDER_MAIN) >> 24);
}

uint32 ItemMod::getEnchant(uint32 sender) {
	return (uint32)((sender - GOSSIP_SENDER_MAIN) & 0xFFFFFF);//屏蔽高8位
}

uint32 ItemMod::getGemId(uint32 sender) {
	return (uint32)((sender - GOSSIP_SENDER_MAIN) & 0xFFFFFF);
}

uint32 ItemMod::senderValue(uint8 slot, uint32 id) {
	return (uint32)(GOSSIP_SENDER_MAIN + ((slot << 24) | (id & 0xFFFFFF)));
}

std::string ItemMod::GetEnchantDescription(Item* item, uint32 enchantId)
{
	SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchantId);
	if (!enchantEntry) return sString->GetText(CORE_STR_TYPES(STR_NO_EFFECT));

	for (uint8 i = 0; i < 16; ++i)
		if (strlen(enchantEntry->description[i]))
			return enchantEntry->description[i];

	return sString->GetText(CORE_STR_TYPES(STR_NO_EFFECT));
}

std::string ItemMod::GetItemEnchantDescription(Player* player, uint32 enchantId)
{
	SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchantId);
	if (!enchantEntry) 
		return sString->GetText(CORE_STR_TYPES(STR_NO_EFFECT));
	
	for (uint32 i = 0; i < UpgradeVec.size(); i++)
		if (enchantEntry->ID == UpgradeVec[i].enchantId)
			if (!UpgradeVec[i].description.empty())
					return UpgradeVec[i].description;
	
	for (uint8 i = 0; i < 16; ++i)
		if (strlen(enchantEntry->description[i]))
			return enchantEntry->description[i];

	return sString->GetText(CORE_STR_TYPES(STR_NO_EFFECT));
}

std::string ItemMod::GetGemName(Item* item, uint8 slot)
{
	uint32 enchant_id = item->GetEnchantmentId(EnchantmentSlot(slot));

	if (!enchant_id) 
		return "";

	SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchant_id);

	if (!enchantEntry) 
		return "";

	uint32 gemid = enchantEntry->GemID;

	if (!gemid) 
		return "";

	ItemTemplate const* gemProto = sObjectMgr->GetItemTemplate(gemid);

	if (!gemProto) 
		return "";

	ItemDisplayInfoEntry const* displayInfo = sItemDisplayInfoStore.LookupEntry(gemProto->DisplayInfoID);

	if (!displayInfo)
		return "";

	std::ostringstream oss;

	oss << "|TInterface/ICONS/" << displayInfo->inventoryIcon << ":28:28:0:0|t " << "|c" << std::hex << ItemQualityColors[gemProto->Quality] << std::dec <<
		"|Hitem:" << gemid << ":0:0:0:0:0:0:0:0:0|h[" << gemProto->Name1 << "]|h|r";

	return oss.str();
}

uint32 ItemMod::getGemId(Item* item, uint8 slot)
{
	uint32 enchant_id = item->GetEnchantmentId(EnchantmentSlot(slot));
	if (!enchant_id) return 0;
	SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
	if (!enchantEntry) return 0;
	return enchantEntry->GemID;
}

bool ItemMod::AddRemoveGemMenu(Player* player, Item* item)
{
	for (uint8 slot = SOCK_ENCHANTMENT_SLOT; slot < SOCK_ENCHANTMENT_SLOT + MAX_GEM_SOCKETS; slot++)
	{
		std::string gemName = GetGemName(item, slot);
		if (!gemName.empty())
		{
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sString->GetText(CORE_STR_TYPES(STR_GEM_REMOVE)), GOSSIP_SENDER_MAIN, ACTION_GEM_MENU_SHOW);
			return true;
		}
	}

	return false;
}

bool ItemMod::AddItemRemovBindMenu(Player* player, Item* item)
{
	const ItemTemplate * temp = sObjectMgr->GetItemTemplate(item->GetEntry());
	if (!temp)
		return false;

	uint32 len = ItemUnbindCostInfo.size();
	for (uint32 i = 0; i < len; i++)
	{
		if (item->GetEntry() == ItemUnbindCostInfo[i].entry && !item->UnBinded && item->IsSoulBound())
		{
			std::ostringstream oss;
			oss << sReq->Notice(player, ItemUnbindCostInfo[i].reqId, "解绑", sCF->GetItemLink(item->GetEntry()),item->GetCount());
			player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, sString->GetText(CORE_STR_TYPES(STR_ITEM_UNBIND)), GOSSIP_SENDER_MAIN, ACTION_ITEM_REMOVEBIND, oss.str().c_str(), sReq->Golds(ItemUnbindCostInfo[i].reqId), 0);
			return true;
		}
	}

	return false;
}

bool ItemMod::AddItemExchangeMenu(Player* player, Item* item, uint32 update)
{
	bool flag = false;

	if (item->IsNoPatch() && update == 0)
	{
		uint32 L_ReqId = 0;
		float L_Chance = 100;

		if (sNoPatchItem->GetExchange(item, L_ReqId, L_Chance))
		{
			std::ostringstream oss;
			oss << sCF->GetItemLink(item->GetEntry());
			oss << sReq->Notice(player, L_ReqId, "\n升级\n", "", 1, L_Chance, VIP_RATE_ITEM_EXCHANGE_0, HR_RATE_ITEM_EXCHANGE_0);
			player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, sString->GetText(CORE_STR_TYPES(STR_ITEM_EXCHANGE)), update, ACTION_ITEM_EXCHANGE, oss.str().c_str(), L_ReqId, 0);
			flag = true;
		}

		if (sNoPatchItem->CanCompound(item))
		{
			std::ostringstream oss;
			oss << sCF->GetItemLink(item->GetEntry()) << " X 2" << "\n\n合成\n\n";
			player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, sString->GetText(CORE_STR_TYPES(STR_STAT_PANEL)), update, ACTION_ITEM_NO_PATCH_COMPOUND, oss.str().c_str(), 0, 0);
			flag = true;
		}
	}
	else
	{
		uint32 len = ItemExchangeInfo.size();
		for (uint32 i = 0; i < len; i++){
			if (item->GetEntry() == ItemExchangeInfo[i].item && item->GetTemplate()->Stackable == 1 && ItemExchangeInfo[i].flag == update)
			{
				std::ostringstream oss;
				oss << sCF->GetItemLink(item->GetEntry());

				if (update == 0)
					oss << sReq->Notice(player, ItemExchangeInfo[i].reqId, "\n升级为\n", sCF->GetItemLink(ItemExchangeInfo[i].exchangeditem), 1, ItemExchangeInfo[i].chance, VIP_RATE_ITEM_EXCHANGE_0, HR_RATE_ITEM_EXCHANGE_0);
				else
					oss << sReq->Notice(player, ItemExchangeInfo[i].reqId, "\n升级为\n", sCF->GetItemLink(ItemExchangeInfo[i].exchangeditem), 1, ItemExchangeInfo[i].chance, VIP_RATE_ITEM_EXCHANGE_1, HR_RATE_ITEM_EXCHANGE_1);

				player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, update == 0 ? sString->GetText(CORE_STR_TYPES(STR_ITEM_EXCHANGE)) : sString->GetText(CORE_STR_TYPES(STR_ITEM_UPGRADE)), update, ACTION_ITEM_EXCHANGE, oss.str().c_str(), sReq->Golds(ItemExchangeInfo[i].reqId), 0);
				flag = true;
			}
		}
	}

	return flag;
}

void ItemMod::AddCastMenu(Player* player, Item* item)
{
	//if (HasTransFlag(item))
	//{
	//	player->CLOSE_GOSSIP_MENU();
	//	return;
	//}

	player->flag_i = 0;
	player->rowId = 0;
	player->pageId = 0;

	player->PlayerTalkClass->ClearMenus();

	setEnchantMask(item);

	bool reGem = AddRemoveGemMenu(player, item);
	bool reBind = AddItemRemovBindMenu(player, item);
	bool ItemEnchant = AddItemEnchantMenu(player, item);
	bool ItemExchange = AddItemExchangeMenu(player, item);
	bool ItemExchange2 = AddItemExchangeMenu(player, item, 1);
	bool Identify = AddIdentifyMenu(player, item);

	if (reGem || reBind || ItemEnchant || ItemExchange || ItemExchange2 || Identify)
	{
		player->PlayerTalkClass->GetGossipMenu().SetMenuId(MENU_ID);
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, player->GetGUID());
	}
	else
		player->CLOSE_GOSSIP_MENU();
}

bool ItemMod::hasEnchantByMask(Item* item, uint32 enchantId)
{
	uint32 enchantMask = 0;
	for (uint32 i = 0; i < UpgradeVec.size(); i++)
	{
		if (enchantId == UpgradeVec[i].enchantId)
			enchantMask = UpgradeVec[i].enchantMask;
	}

	if (enchantMask == 0)
		return true;

	return (enchantMask & item->enchantMask) != enchantMask;
}

void ItemMod::setEnchantMask(Item* item)
{
	item->enchantMask = 0;

	for (uint8 slot = PROP_ENCHANTMENT_SLOT_0; slot < MAX_ENCHANTMENT_SLOT; slot++)
	{
		uint32 currEnchantId = item->GetEnchantmentId(EnchantmentSlot(slot));

		for (uint32 i = 0; i < UpgradeVec.size(); i++)
		{
			if (UpgradeVec[i].enchantId == currEnchantId)
			{
				if ((item->enchantMask & UpgradeVec[i].enchantMask) != UpgradeVec[i].enchantMask)
					item->enchantMask = item->enchantMask | UpgradeVec[i].enchantMask;
			}
		}
	}
}

uint32 ItemMod::GetItemMask(uint32 entry)
{
	ItemTemplate const* proto = sObjectMgr->GetItemTemplate(entry);
	if (!proto)
		return ITEM_MASK_NONE;
	
	if (proto->Class != ITEM_CLASS_WEAPON && proto->Class != ITEM_CLASS_ARMOR)
		return ITEM_MASK_NONE;

	if (proto->Class == ITEM_CLASS_WEAPON && proto->SubClass == ITEM_SUBCLASS_WEAPON_WAND)
		return ITEM_MASK_RANGED;

	switch (proto->InventoryType)
	{
		//饰物
	case INVTYPE_NECK:
	case INVTYPE_FINGER:
	case INVTYPE_TRINKET:
	case INVTYPE_CLOAK:
		return ITEM_MASK_ORNAMENT;
	//八件套
	case INVTYPE_HEAD:
	case INVTYPE_SHOULDERS:	
	case INVTYPE_CHEST:
	case INVTYPE_WAIST:
	case INVTYPE_LEGS:
	case INVTYPE_FEET:
	case INVTYPE_WRISTS:
	case INVTYPE_HANDS:
	case INVTYPE_ROBE:
		return ITEM_MASK_SUIT;

	//衬衣
	case INVTYPE_BODY:
		return ITEM_MASK_SHIRT;
	//战袍
	case INVTYPE_TABARD:
		return ITEM_MASK_TABARD;

	//双手 弓 弩 枪
	case INVTYPE_2HWEAPON:
	case INVTYPE_RANGEDRIGHT://不能包括魔杖
	case INVTYPE_RANGED:
		return ITEM_MASK_2H_WEAPON;

	//单手、主手
	case INVTYPE_WEAPON:
	case INVTYPE_WEAPONMAINHAND:
		return ITEM_MASK_1H_WEAPON;
	//盾、副手
	case INVTYPE_SHIELD:
	case INVTYPE_WEAPONOFFHAND:
		return ITEM_MASK_OFFHAND;
	//远程
	case INVTYPE_THROWN://包括魔杖
	case INVTYPE_RELIC:
		return ITEM_MASK_RANGED;

	case INVTYPE_BAG:
	case INVTYPE_HOLDABLE:
	case INVTYPE_AMMO:
	case INVTYPE_QUIVER:
		return ITEM_MASK_NONE;
	}

	return ITEM_MASK_NONE;
}

bool ItemMod::CanApply(uint32 mask, Item* item, ITEM_ENCHANT_TYPES type)
{
	ItemTemplate const* proto = item->GetTemplate();
	
	if (!proto)
		return false;

	switch (type)
	{
	case ITEM_ENCHANT_UPGRADE:
		return (mask & GetItemMask(item->GetEntry())) != 0;
	case ITEM_ENCHANT_WEAPON_PERM:
		return (mask & ITEM_MASK_WEAPON_PERM) != 0;
	case ITEM_ENCHANT_SIGIL:
		return (mask & ITEM_MASK_SIGIL) != 0;
	default:
		return false;
	}
}

bool ItemMod::CheckClass(Player* player, uint32 classMask)
{
	return (classMask & player->getClassMask()) != 0;
}

bool ItemMod::IsUpgradeSetted(Player* player, Item* item, ITEM_ENCHANT_TYPES type)
{
	uint32 itemMask = GetItemMask(item->GetEntry());
	uint32 len = UpgradeVec.size();

	switch (type)
	{
	case ITEM_ENCHANT_UPGRADE:
		for (size_t i = 0; i < len; i++)
			if (UpgradeVec[i].prevEnchantId == 0 && (UpgradeVec[i].itemMask & itemMask) != 0 && (UpgradeVec[i].classMask & player->getClassMask()) != 0)
				return true;
		break;
	case ITEM_ENCHANT_WEAPON_PERM:
		for (size_t i = 0; i < len; i++)
			if (UpgradeVec[i].prevEnchantId == 0 && (UpgradeVec[i].itemMask & ITEM_MASK_WEAPON_PERM) != 0 && (UpgradeVec[i].classMask & player->getClassMask()) != 0)
				return true;
		break;
	case ITEM_ENCHANT_SIGIL:
		for (size_t i = 0; i < len; i++)
			if (UpgradeVec[i].prevEnchantId == 0 && (UpgradeVec[i].itemMask & ITEM_MASK_SIGIL) != 0 && (UpgradeVec[i].classMask & player->getClassMask()) != 0)
				return true;
		break;
	default:
		return false;
	}
	
	return false;
}

bool ItemMod::AddItemEnchantMenu(Player* player, Item* item)
{
	bool flag = false;

	if ((item->GetTemplate()->Class == ITEM_CLASS_ARMOR || item->GetTemplate()->Class == ITEM_CLASS_WEAPON) && IsUpgradeSetted(player,item))
	{
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sString->GetText(CORE_STR_TYPES(STR_ITEM_ENCHANT)), GOSSIP_SENDER_MAIN, ACTION_ITEM_ENCHANT_CURR_MENU_SHOW);
		flag = true;
	}

	if (item->GetTemplate()->Class == ITEM_CLASS_WEAPON && IsUpgradeSetted(player, item, ITEM_ENCHANT_WEAPON_PERM))
	{
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sString->GetText(CORE_STR_TYPES(STR_WEAPON_PERM)), GOSSIP_SENDER_MAIN, ACTION_WEAPON_PERMENCHANT_CURR_MENU_SHOW);
		flag = true;
	}
		
	return flag;
}
void ItemMod::AddItemCurrEnchantList(Player* player, Item* item, ITEM_ENCHANT_TYPES type)
{
	setEnchantMask(item);

	switch (type)
	{
	case ITEM_ENCHANT_UPGRADE:
	{
		for (uint8 slot = GetEnchantStartSlot(GetItemMask(item->GetEntry())); slot < MAX_ENCHANTMENT_SLOT; slot++)
		{
			std::ostringstream oss;
			uint32 currEnchantId = item->GetEnchantmentId(EnchantmentSlot(slot));
			oss << GetItemEnchantDescription(player, currEnchantId);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, oss.str(), senderValue(slot, currEnchantId), ACTION_ITEM_ENCHANT_NEXT_MENU_SHOW);
		}
	}
	break;
	case ITEM_ENCHANT_SIGIL:
	{
		for (uint8 slot = GetEnchantStartSlot(ITEM_MASK_SIGIL); slot < MAX_ENCHANTMENT_SLOT; slot++)
		{
			std::ostringstream oss;
			uint32 currEnchantId = item->GetEnchantmentId(EnchantmentSlot(slot));
			//oss << "[当前：" << GetItemEnchantDescription(item, currEnchantId) << "]";
			oss << GetItemEnchantDescription(player, currEnchantId);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, oss.str(), senderValue(slot, currEnchantId), ACTION_SIGIL_ENCHANT_NEXT_MENU_SHOW);
		}
	}
	break;
	default:
		break;
	}

	if (SIGIL_OWN_ENCHANT && type != ITEM_ENCHANT_SIGIL)
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "<< 返回 >>", GOSSIP_SENDER_MAIN, ACTION_MAINMENU_BACK);
	player->PlayerTalkClass->GetGossipMenu().SetMenuId(MENU_ID);
	player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, player->GetGUID());
}

void ItemMod::AddItemNextEnchantList(Player* player, Item* item, uint32 sender, ITEM_ENCHANT_TYPES type)
{
	item->enchantType = type;

	uint32 currEnchantId = getEnchant(sender);

	//符印中 有两行是固定值 点击显示当前菜单
	if (SIGIL_OWN_ENCHANT && (currEnchantId == SIGIL_OWN_ENCHANT_TITLE_0 || currEnchantId == SIGIL_OWN_ENCHANT_TITLE_1))
	{
		AddItemCurrEnchantList(player, player->playerItem, ITEM_ENCHANT_SIGIL);
		return;
	}

	bool undefineEnchant = true;
	uint8 slot = getSlot(sender);
	SetFilterVec(player, item, type, currEnchantId);
	uint32 len = FilterVec.size();

	if (SIGIL_OWN_ENCHANT && type != ITEM_ENCHANT_SIGIL)
	{
		for (uint32 i = 0; i < len; i++)
		{
			if (currEnchantId == FilterVec[i].enchantId)
			{
				if (currEnchantId != 0)
				{
					std::ostringstream oss1;
					std::ostringstream oss2;
					oss1 << "移除" << GetItemEnchantDescription(player, currEnchantId);
					oss2 << "\n移除\n" << GetEnchantDescription(item, currEnchantId);
					player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, oss1.str(), senderValue(slot, currEnchantId), ACTION_ITEM_REMOVE_ENCHANT, oss2.str().c_str(), 0, 0);
					undefineEnchant = false;
					break;
				}
			}
		}

		if (undefineEnchant && currEnchantId != 0)
		{
			std::ostringstream oss1;
			std::ostringstream oss2;
			oss1 << "|cFFFF0000移除 —> " << GetItemEnchantDescription(player, currEnchantId) << "|r";
			oss2 << sCF->GetItemLink(item->GetEntry()) << "\n\n移除 —> " << GetEnchantDescription(item, currEnchantId);
			player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, oss1.str(), senderValue(slot, currEnchantId), ACTION_ITEM_REMOVE_ENCHANT, oss2.str().c_str(), DEFAULT_REMOVE_ENCHANT_GOLDS * GOLD, 0);
		}
	}
	
	//防止宕机
	if(len > 30)
		len = 30;

	for (uint32 i = 0; i < len; i++)
		player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, ENCHANT_ACTION_STRING + " —> " + GetItemEnchantDescription(player, FilterVec[i].enchantId), senderValue(slot, FilterVec[i].enchantId), ACTION_ITEM_ENCHANT, sReq->Notice(player, FilterVec[i].enchantReqId, sCF->GetItemLink(item->GetEntry()), "\n\n" + ENCHANT_ACTION_STRING + " —> " + GetItemEnchantDescription(player, FilterVec[i].enchantId), 1, FilterVec[i].chance, VIP_RATE_ITEM_STRENGTHEN, HR_RATE_ITEM_STRENGTHEN), sReq->Golds(FilterVec[i].enchantReqId), 0);

	if (SIGIL_OWN_ENCHANT && type != ITEM_ENCHANT_SIGIL)
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "<< 返回 >>", GOSSIP_SENDER_MAIN, ACTION_ITEM_ENCHANT_CURR_MENU_SHOW);

	player->PlayerTalkClass->GetGossipMenu().SetMenuId(MENU_ID);
	player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, player->GetGUID());
}

void ItemMod::SetFilterVec(Player* player, Item* item, ITEM_ENCHANT_TYPES type, uint32 currEnchantId)
{
	FilterVec.clear();

	uint32 len = UpgradeVec.size();

	for (uint32 i = 0; i < len; i++)
	{
		if (currEnchantId == UpgradeVec[i].prevEnchantId && sItemMod->CanApply(UpgradeVec[i].itemMask, item, type) && sItemMod->CheckClass(player, UpgradeVec[i].classMask))
		{
			SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(UpgradeVec[i].enchantId);
			if (enchantEntry)
				if (hasEnchantByMask(item, UpgradeVec[i].enchantId) || UpgradeVec[i].prevEnchantId != 0)
				{
					FilterTemplate temp;
					temp.enchantId = UpgradeVec[i].enchantId;
					temp.prevEnchantId = UpgradeVec[i].prevEnchantId;
					/*HasEnchant(player, UpgradeVec[i].enchantId) ? temp.enchantReqId = 0 : */temp.enchantReqId = UpgradeVec[i].enchantReqId;
					temp.removeEnchantRewId = UpgradeVec[i].removeEnchantRewId;
					temp.description = UpgradeVec[i].description;
					temp.chance = UpgradeVec[i].chance;
					temp.itemMask = UpgradeVec[i].itemMask;
					temp.enchantMask = UpgradeVec[i].enchantMask;
					temp.classMask = UpgradeVec[i].classMask;
					FilterVec.push_back(temp);
				}
		}
	}
}


void ItemMod::EnchantItem(Player* player, Item* item, uint8 slot, uint32 enchantId)
{
	uint32 len = UpgradeVec.size();
	for (uint32 i = 0; i < len; i++)
	{
		if (enchantId == UpgradeVec[i].enchantId)
		{

			if (sReq->Check(player, UpgradeVec[i].enchantReqId))
			{
				uint32 rate = 0;
				
				if (item->enchantType == ITEM_ENCHANT_SIGIL)
				{
					rate = UpgradeVec[i].chance + sVIP->GetRate(player, VIP_RATE_SIGIL) + sHR->GetRate(player, HR_RATE_SIGIL);
					
					if (rate < 100)
						rate += GetRateAndDes(player, RATE_SIGIL);
				}
					
				else
				{
					rate = UpgradeVec[i].chance + sVIP->GetRate(player, VIP_RATE_ITEM_STRENGTHEN) + sHR->GetRate(player, HR_RATE_ITEM_STRENGTHEN) + GetRateAndDes(player, RATE_ITEM_STRENGTHEN);
					
					if (rate < 100)
						rate += GetRateAndDes(player, RATE_SIGIL);
				}
				
				
				if (urand(1, 100) > rate)
				{
					sReq->Des(player, UpgradeVec[i].enchantReqId);
					std::string noticetext = ENCHANT_ACTION_STRING + "失败";
					player->GetSession()->SendAreaTriggerMessage(noticetext.c_str());
					player->CLOSE_GOSSIP_MENU();
					return;
				}

				if (item->enchantType == ITEM_ENCHANT_SIGIL)
					player->_ApplyItemModsCustom(item, 0, false);

				player->ApplyEnchantment(item, EnchantmentSlot(slot), false);
				item->SetEnchantment(EnchantmentSlot(slot), enchantId, 0, 0);
				player->ApplyEnchantment(item, EnchantmentSlot(slot), true);
				player->CastSpell(player, VISUAL_SPELL_ID, true, NULL, NULL, player->GetGUID());

				if (SpellItemEnchantmentEntry const* info = sSpellItemEnchantmentStore.LookupEntry(enchantId))
				{
					std::string noticetext = ENCHANT_ACTION_STRING + info->description[4] + "成功";
					player->GetSession()->SendAreaTriggerMessage(noticetext.c_str());
				}
				
				//AddEnchant(player, enchantId);

				sReq->Des(player, UpgradeVec[i].enchantReqId);

				if (item->enchantType == ITEM_ENCHANT_SIGIL)
					player->_ApplyItemModsCustom(item, 0, true);

				AddItemCurrEnchantList(player, item);
				return;
			}
		}
	}
	player->CLOSE_GOSSIP_MENU();
}

//物品解绑
void ItemMod::RemoveBind(Player* player, Item* item)
{
	uint32 len = ItemUnbindCostInfo.size();
	for (uint32 i = 0; i < len; i++)
	{
		if (item->GetEntry() == ItemUnbindCostInfo[i].entry)
		{
			if (sReq->Check(player, ItemUnbindCostInfo[i].reqId, item->GetCount()))
			{
				item->ClearEnchantment(PERM_ENCHANTMENT_SLOT);
				item->UnBinded = true;
				item->SetBinding(false);
				item->SetState(ITEM_CHANGED, player);
				player->CastSpell(player, VISUAL_SPELL_ID, true, NULL, NULL, player->GetGUID());
				player->GetSession()->SendAreaTriggerMessage("成功解除物品绑定");
				sReq->Des(player, ItemUnbindCostInfo[i].reqId, item->GetCount());
			}
			player->CLOSE_GOSSIP_MENU();
			return;
		}
	}
	player->CLOSE_GOSSIP_MENU();
}

//物品升级
void ItemMod::ExchangeItem(Player* player, Item* item, uint32 update/*0 升级 1强化*/)
{
	uint32 len = ItemExchangeInfo.size();
	for (uint32 i = 0; i < len; i++)
	{
		if (update != ItemExchangeInfo[i].flag)
			continue;

		if (item->GetEntry() == ItemExchangeInfo[i].item)
		{
			if (sReq->Check(player, ItemExchangeInfo[i].reqId))
			{
				uint32 vipRate		= 0;
				uint32 hrRate		= 0;
				uint32 stoneRate	= 0;

				if (update == 0)
				{
					vipRate			= sVIP->GetRate(player, VIP_RATE_ITEM_EXCHANGE_0);
					hrRate			= sHR->GetRate(player, HR_RATE_ITEM_EXCHANGE_0);
				}
				else
				{
					vipRate			= sVIP->GetRate(player, VIP_RATE_ITEM_EXCHANGE_1);
					hrRate			= sHR->GetRate(player, HR_RATE_ITEM_EXCHANGE_1);
				}

				uint32 rate = ItemExchangeInfo[i].chance + vipRate + hrRate;

				if (rate < 100)
					update == 0 ? rate += GetRateAndDes(player, RATE_ITEM_EXCHANGE_0) : rate += GetRateAndDes(player, RATE_ITEM_EXCHANGE_1);


				if (urand(1, 100) > rate)
				{		
					uint32 expItemId = item->GetEntry();

					std::ostringstream oss;
					oss << sCF->GetItemLink(item, player->GetSession());
					update == 0 ? oss << "升级失败" : oss << "升级失败";

					if (ItemExchangeInfo[i].destroyOnFail)
					{
						player->DestroyItem(item->GetBagSlot(), item->GetSlot(), true);
						oss << ",被摧毁！";
					}
					
					player->GetSession()->SendAreaTriggerMessage(oss.str().c_str());

					if (ItemExchangeInfo[i].destroyOnFail)
						sReq->Des(player, ItemExchangeInfo[i].reqId, 1, expItemId);
					else
						sReq->Des(player, ItemExchangeInfo[i].reqId);

					sRew->Rew(player, ItemExchangeInfo[i].rewIdOnFail);
					
					player->CLOSE_GOSSIP_MENU();
					return;
				}

				Item* exchangeItem = AddItem(player, ItemExchangeInfo[i].exchangeditem, 1);
				if (!exchangeItem)
				{
					player->CLOSE_GOSSIP_MENU();
					return;
				}

				if (ItemExchangeInfo[i].keepEnchant) //保留附魔效果
				{
					for (uint8 slot = PERM_ENCHANTMENT_SLOT; slot < MAX_ENCHANTMENT_SLOT; slot++)
					{
						uint32 enchantId = item->GetEnchantmentId(EnchantmentSlot(slot));

						if (SpellItemEnchantmentEntry const* info = sSpellItemEnchantmentStore.LookupEntry(enchantId))
						{
							player->ApplyEnchantment(exchangeItem, EnchantmentSlot(slot), false);
							exchangeItem->SetEnchantment(EnchantmentSlot(slot), enchantId, 0, 0);
							player->ApplyEnchantment(exchangeItem, EnchantmentSlot(slot), true);
						}
					}
				}
			
				player->CastSpell(player, VISUAL_SPELL_ID, true, NULL, NULL, player->GetGUID());
				uint32 expItemId = item->GetEntry();
				player->DestroyItem(item->GetBagSlot(), item->GetSlot(), true);
				sReq->Des(player, ItemExchangeInfo[i].reqId, 1, expItemId);
				update == 0 ? player->GetSession()->SendAreaTriggerMessage("升级成功") : player->GetSession()->SendAreaTriggerMessage("升级成功");
				player->CLOSE_GOSSIP_MENU();
				return;
			}
		}
	}
	player->CLOSE_GOSSIP_MENU();
}
Item* ItemMod::AddItem(Player* player, uint32 itemId, uint32 count)
{
	uint32 noSpaceForCount = 0;
	ItemPosCountVec dest;
	InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemId, count, &noSpaceForCount);
	if (msg != EQUIP_ERR_OK)
		count -= noSpaceForCount;

	if (count == 0 || dest.empty())
	{
		if (dest.empty()) player->GetSession()->SendNotification("背包已满，或升级后的物品已存在且唯一！");
		// -- TODO: Send to mailbox if no space
		//ChatHandler(GetSession()).PSendSysMessage("You don't have any space in your bags.");
		return NULL;
	}

	Item* item = player->StoreNewItem(dest, itemId, true, Item::GenerateItemRandomPropertyId(itemId));
	if (item)
		player->SendNewItem(item, count, true, false);
	else
	{
		player->GetSession()->SendNotification("失败，请检查是否升级后的物品存在且唯一！");
		return NULL;
	}
	return item;
}
//去除宝石
void ItemMod::AddGemList(Player* player, Item* item)
{
	bool gemExsit = false;

	uint32 len = RemoveGemInfo.size();
	for (uint32 i = 0; i < len; i++) {
		for (uint8 slot = SOCK_ENCHANTMENT_SLOT; slot < SOCK_ENCHANTMENT_SLOT + MAX_GEM_SOCKETS; slot++)
		{
			std::string gemName = GetGemName(item, slot);
			uint32 gemId = getGemId(item, slot);
			if (gemId != 0 && gemId == RemoveGemInfo[i].entry)
			{
				gemExsit = true;
				player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, gemName.c_str(), senderValue(slot, gemId), ACTION_ITEM_REMOVEGEM, sReq->Notice(player, RemoveGemInfo[i].reqId, sCF->GetItemLink(gemId), "移除", 1, RemoveGemInfo[i].chance, VIP_RATE_GEM_REMOVE, HR_RATE_GEM_REMOVE), sReq->Golds(RemoveGemInfo[i].reqId), 0);
			}
		}
	}

	if (!gemExsit)
	{
		player->CLOSE_GOSSIP_MENU();
		return;
	}

	player->PlayerTalkClass->GetGossipMenu().SetMenuId(MENU_ID);
	player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, player->GetGUID());
}

//鉴定和封象
bool ItemMod::AddIdentifyMenu(Player* player, Item* item)
{
	bool flag = false;

	for (uint8 slot = PERM_ENCHANTMENT_SLOT; slot < MAX_ENCHANTMENT_SLOT; slot++)
	{
		if (GetIdentifyGroupId(item->GetEntry(), EnchantmentSlot(slot)) != 0)
		{
			uint32 reqId = 0;
			std::string gossipText = "";
			bool slotHasEnchant = false;

			GetIdentifyInfo(item->GetEntry(), EnchantmentSlot(slot), reqId, gossipText, slotHasEnchant);

			SpellItemEnchantmentEntry const* info = sSpellItemEnchantmentStore.LookupEntry(item->GetEnchantmentId(EnchantmentSlot(slot)));

			if (slotHasEnchant && !info)
				continue;

			flag = true;

			std::ostringstream oss;

			if (info)
			{
				std::string str = info->description[4];
				sDBCCreate->RepStr(str, "|cFF00FF00", "");
				sDBCCreate->RepStr(str, "|r", "");
				oss << sString->GetText(CORE_STR_TYPES(STR_IDENTIFY)) << str;
			}
			else
				oss << sString->GetText(CORE_STR_TYPES(STR_IDENTIFY)) << "获得新的属性";

			if (!gossipText.empty())
				oss << "\n" << gossipText;

			player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, oss.str(), slot, ACTION_ITEM_IDENTIFY, sReq->Notice(player, reqId, "重新炼化", sCF->GetItemLink(item->GetEntry())), sReq->Golds(reqId), 0);
		}
	}

	return flag;
}

void ItemMod::RemoveGem(Player* player, Item* item, uint32 sender)
{
	uint32 gemId = getGemId(sender);
	uint8 slot = getSlot(sender);
	uint32 len = RemoveGemInfo.size();
	for (uint32 i = 0; i < len; i++)
	{
		if (gemId == RemoveGemInfo[i].entry)
		{
			if (sReq->Check(player, RemoveGemInfo[i].reqId))
			{

				uint32 rate = RemoveGemInfo[i].chance + sVIP->GetRate(player, VIP_RATE_GEM_REMOVE) + sHR->GetRate(player, HR_RATE_GEM_REMOVE);

				if (rate < 100)
					rate += GetRateAndDes(player, RATE_GEM_REMOVE);

				if (urand(1, 100) > rate)
				{
					sReq->Des(player, RemoveGemInfo[i].reqId);
					player->GetSession()->SendAreaTriggerMessage("移除失败");
					AddGemList(player, player->playerItem);
					return;
				}

				player->ApplyEnchantment(item, EnchantmentSlot(slot), false);
				item->ClearEnchantment(EnchantmentSlot(slot));
				player->CastSpell(player, VISUAL_SPELL_ID, true, NULL, NULL, player->GetGUID());
				player->AddItem(gemId, 1);
				player->GetSession()->SendAreaTriggerMessage("移除成功");
				sReq->Des(player, RemoveGemInfo[i].reqId);
				player->CastSpell(player, VISUAL_SPELL_ID, true, NULL, NULL, player->GetGUID());
				AddGemList(player, player->playerItem);
				return;
			}
		}
	}
	player->CLOSE_GOSSIP_MENU();
}

void ItemMod::RemoveItemEnchant(Player* player, Item* item, uint8 slot, uint32 enchantId)
{
	SpellItemEnchantmentEntry const* info = sSpellItemEnchantmentStore.LookupEntry(enchantId);

	if (!info)
		player->CLOSE_GOSSIP_MENU();

	for (uint32 i = 0; i < UpgradeVec.size(); i++)
		if (enchantId == UpgradeVec[i].enchantId)
			sRew->Rew(player, UpgradeVec[i].removeEnchantRewId);
		
	player->ApplyEnchantment(item, EnchantmentSlot(slot), false);
	item->ClearEnchantment(EnchantmentSlot(slot));
	player->CastSpell(player, VISUAL_SPELL_ID, true, NULL, NULL, player->GetGUID());
	std::string text = "移除" + ENCHANT_NAME_STRING + info->description[4];
	player->GetSession()->SendAreaTriggerMessage(text.c_str());
	AddItemCurrEnchantList(player, item);
}

void ItemMod::RemoveUndefinEnchant(Player* player, Item* item, uint8 slot, uint32 enchantId)
{
	if (!player->HasEnoughMoney(100 * GOLD))
	{
		player->GetSession()->SendNotification("你没有足够的金钱");
		return;
	}
	player->ApplyEnchantment(item, EnchantmentSlot(slot), false);
	item->ClearEnchantment(EnchantmentSlot(slot));
	player->CastSpell(player, VISUAL_SPELL_ID, true, NULL, NULL, player->GetGUID());
	player->GetSession()->SendAreaTriggerMessage("移除附魔");
	player->CLOSE_GOSSIP_MENU();
	return;
}

void ItemMod::RefreshItem(Player* player, Item* item, EnchantmentSlot slot)
{
	uint32 enchantId = GetIdentifyEnchantId(item->GetEntry(), slot);
	uint32 reqId = 0;
	std::string gossipText = "";
	bool slotHasEnchant = false;

	GetIdentifyInfo(item->GetEntry(), slot, reqId, gossipText, slotHasEnchant);

	uint32 enchant_id = item->GetEnchantmentId(slot);

	//if (enchant_id != 8004 && slot == PROP_ENCHANTMENT_SLOT_0 || enchant_id != 8005 && slot == PROP_ENCHANTMENT_SLOT_1)
	//{
	//	player->CLOSE_GOSSIP_MENU();
	//	player->GetSession()->SendNotification("该装备只能鉴定或淬炼一次！");
	//	return;
	//}
		
	if (sReq->Check(player, reqId))
	{
		player->ApplyEnchantment(item, slot, false);
		item->SetEnchantment(slot, enchantId, 0, 0, 0, true);
		player->ApplyEnchantment(item, slot, true);

		item->SetBinding(true);
		item->SetNotRefundable(player);

		WorldPacket data(SMSG_ITEM_REFUND_INFO_RESPONSE, 8 + 4 + 4 + 4 + 4 * 4 + 4 * 4 + 4 + 4);
		data << uint64(item->GetGUID()); // item guid
		data << uint32(0);               // money cost
		data << uint32(0);               // honor point cost
		data << uint32(0);               // arena point cost
		for (uint8 i = 0; i < MAX_ITEM_EXTENDED_COST_REQUIREMENTS; ++i)// item cost data
		{
			data << uint32(0);
			data << uint32(0);
		}
		data << uint32(0);
		data << uint32(0);
		player->GetSession()->SendPacket(&data);

		sReq->Des(player, reqId);
		player->CastSpell(player, VISUAL_SPELL_ID, true, NULL, NULL, player->GetGUID());
		if (SpellItemEnchantmentEntry const* info = sSpellItemEnchantmentStore.LookupEntry(enchantId))
			player->GetSession()->SendAreaTriggerMessage("%s|cFF00FF00获得|r%s", sCF->GetItemLink(item->GetEntry()).c_str(), info->description[4]);;
	}

	//<< 返回 >>祭炼菜单
	AddCastMenu(player, item);
}

class spell_item_mod : public SpellScriptLoader
{
public:
	spell_item_mod() : SpellScriptLoader("spell_item_mod") { }

	class spell_item_mod_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_item_mod_SpellScript);
		void HandleAfterCast()
		{
			Player* player = GetCaster()->ToPlayer();
			if (!player)
				return;


			Item* item = GetExplTargetItem();
			if (!item)
				return;

			if (item->GetOwner() != player)
			{
				ChatHandler(player->GetSession()).PSendSysMessage("你不拥有这件装备！");
				return;
			}

			player->playerItem = item;

			sItemMod->AddCastMenu(player, item);
		}

		void Register() override
		{
			AfterCast += SpellCastFn(spell_item_mod_SpellScript::HandleAfterCast);
		}
	};

	SpellScript* GetSpellScript() const override
	{
		return new spell_item_mod_SpellScript();
	}
};

class item_mod_playerscript : public PlayerScript
{
public:
	item_mod_playerscript() : PlayerScript("item_mod_playerscript") {}

	void OnGossipSelect(Player* player, uint32 menu_id, uint32 sender, uint32 action) override
	{

		if (menu_id != MENU_ID || !player->playerItem || !player->playerItem->IsInWorld())
		{
			player->playerItem = NULL;
			player->CLOSE_GOSSIP_MENU();
			return;
		}

		player->PlayerTalkClass->ClearMenus();

		switch (action)
		{
		case ACTION_MAINMENU_BACK:
			sItemMod->AddCastMenu(player, player->playerItem);
			break;
			//物品附魔
		case ACTION_ITEM_ENCHANT_CURR_MENU_SHOW:
			sItemMod->AddItemCurrEnchantList(player, player->playerItem);
			break;
		case ACTION_ITEM_ENCHANT_NEXT_MENU_SHOW:
			sItemMod->AddItemNextEnchantList(player, player->playerItem, sender);
			break;
		case ACTION_WEAPON_PERMENCHANT_CURR_MENU_SHOW:
			player->flag_i = 0;
			player->rowId = 0;
			player->pageId = 1;
			sItemMod->AddWeaponPermList(player, player->playerItem);
			break;
		case ACTION_WEAPON_PERMENCHANT_NEXT_MENU_SHOW:
			player->pageId += 1;
			sItemMod->AddWeaponPermList(player, player->playerItem);
			break;	
		case ACTION_PREV_PAGE:
			player->pageId -= 1;
			player->flag_i -= 2 * MAX_ROWS_COUNT;
			sItemMod->AddWeaponPermList(player, player->playerItem);
			break;
		case ACTION_WEAPON_LOOKUPANDBUY_SHOW:
			sItemMod->LookupOrBuyWeaponPermEnchant(player,player->playerItem,sender);
			break;
		case ACTION_WEAPONPERM_LOOKUP:
			sItemMod->LookupWeaponPermEnchant(player, player->playerItem, sender);
			break;
		case ACTION_WEAPONPERM_BUY:
			sItemMod->BuyWeaponPermEnchant(player, player->playerItem, sender);
			player->SaveToDB(false, false);
			break;
		case ACTION_SIGIL_ENCHANT_NEXT_MENU_SHOW:
			sItemMod->AddItemNextEnchantList(player, player->playerItem, sender, ITEM_ENCHANT_SIGIL);
			break;
		case ACTION_ITEM_ENCHANT:
			sItemMod->EnchantItem(player, player->playerItem, sItemMod->getSlot(sender), sItemMod->getEnchant(sender));
			player->SaveToDB(false, false);
			break;
		case ACTION_ITEM_REMOVE_ENCHANT:
			sItemMod->RemoveItemEnchant(player, player->playerItem, sItemMod->getSlot(sender), sItemMod->getEnchant(sender));
			player->SaveToDB(false, false);
			break;

			//移除未定义附魔
		case ACTION_REMOVE_ENCHANT_UNDEFINE:
			sItemMod->RemoveUndefinEnchant(player, player->playerItem, sItemMod->getSlot(sender), sItemMod->getEnchant(sender));
			player->SaveToDB(false, false);
			break;
			//移除宝石
		case ACTION_GEM_MENU_SHOW:
			sItemMod->AddGemList(player, player->playerItem);
			break;
		case ACTION_ITEM_REMOVEGEM:
			sItemMod->RemoveGem(player, player->playerItem, sender);
			player->SaveToDB(false, false);
			break;
			//物品升级
		case ACTION_ITEM_EXCHANGE:
			if (player->playerItem->IsNoPatch())
				sNoPatchItem->LevelUp(player, player->playerItem);
			else
				sItemMod->ExchangeItem(player, player->playerItem,sender);
			player->SaveToDB(false, false);
			break;
		case ACTION_ITEM_NO_PATCH_COMPOUND:
			sNoPatchItem->Compound(player, player->playerItem);
			player->CLOSE_GOSSIP_MENU();
			player->SaveToDB(false, false);
			break;
			//物品解绑
		case ACTION_ITEM_REMOVEBIND:
			sItemMod->RemoveBind(player, player->playerItem);
			break;
		case ACTION_ITEM_IDENTIFY:
			sItemMod->RefreshItem(player, player->playerItem, EnchantmentSlot(sender));
			player->SaveToDB(false, false);
			break;
		default:
			break;
		}
	}

	void OnLogout(Player* player) override
	{
		sItemMod->RecoverWeaponPermEnchant(player);
	}

	void OnLogin(Player* player) override
	{
		//主背包
		for (uint8 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++)
			if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
				for (uint32 k = 0; k < HiddenItemInfo.size(); k++)
					if (item->GetEntry() == HiddenItemInfo[k].entry)
						player->_ApplyItemModsCustom(item, 0, true);

		//额外三个背包
		for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
			if (Bag* pBag = player->GetBagByPos(i))
				for (uint32 j = 0; j < pBag->GetBagSize(); j++)
					if (Item* item = player->GetItemByPos(i, j))
						for (uint32 k = 0; k < HiddenItemInfo.size(); k++)
							if (item->GetEntry() == HiddenItemInfo[k].entry)
								player->_ApplyItemModsCustom(item, 0, true);

		//银行
		for (uint8 i = BANK_SLOT_ITEM_START; i < BANK_SLOT_BAG_END; ++i)
			if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
				for (uint32 k = 0; k < HiddenItemInfo.size(); k++)
					if (item->GetEntry() == HiddenItemInfo[k].entry)
						player->_ApplyItemModsCustom(item, 0, true);
		//银行背包
		for (uint8 i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; ++i)
			if (Bag* pBag = player->GetBagByPos(i))
				for (uint32 j = 0; j < pBag->GetBagSize(); j++)
					if (Item* item = player->GetItemByPos(i, j))
						for (uint32 k = 0; k < HiddenItemInfo.size(); k++)
							if (item->GetEntry() == HiddenItemInfo[k].entry)
								player->_ApplyItemModsCustom(item, 0, true);


		player->InitDayLimitItem();

		//sItemMod->LoadEnchant(player);

		sNoPatchItem->UpdateAuras(player);
	}

	void OnUpdateZone(Player* player, uint32 /*newZone*/, uint32 /*newArea*/) 
	{ 
		//sItemMod->UnApplyEquipments(player);
	}
};

//符印
class sigil_item_upgrade : public ItemScript
{
public:
	sigil_item_upgrade() : ItemScript("sigil_item_upgrade") { }

	bool OnUse(Player* player, Item* item, SpellCastTargets const& targets) override
	{
		player->PlayerTalkClass->ClearMenus();
		if (!item || !sItemMod->IsUpgradeSetted(player, item, ITEM_ENCHANT_SIGIL) || !item->IsInWorld())
		{
			player->CLOSE_GOSSIP_MENU();
			return false;
		}

		if (SIGIL_OWN_ENCHANT)
		{
			bool hasInitEnchant = false;

			for (uint32 slot = PERM_ENCHANTMENT_SLOT; slot < MAX_ENCHANTMENT_SLOT; ++slot)
				if (uint32 enchantId = item->GetEnchantmentId(EnchantmentSlot(slot)))
				{
					hasInitEnchant = true;
					break;
				}

			if (!hasInitEnchant)
			{
				for (uint32 slot = MAX_ENCHANTMENT_SLOT - COUNT_SIGIL; slot < MAX_ENCHANTMENT_SLOT; ++slot)
				{
					uint32 enchantId = slot + SIGIL_OWN_ENCHANT_ID_START - (MAX_ENCHANTMENT_SLOT - COUNT_SIGIL);
					player->ApplyEnchantment(item, EnchantmentSlot(slot), false);
					item->SetEnchantment(EnchantmentSlot(slot), enchantId, 0, 0);
					player->ApplyEnchantment(item, EnchantmentSlot(slot), true);
					player->SaveToDB(false, false);
				}
			}
		}
		
		player->playerItem = item;
		sItemMod->setEnchantMask(item);
		sItemMod->AddItemCurrEnchantList(player, player->playerItem, ITEM_ENCHANT_SIGIL);
		return true;
	}
};


void AddSC_ITEM_MOD()
{
	new item_mod_playerscript();
	new spell_item_mod();
	new sigil_item_upgrade();
}



//几率宝石

uint32 ItemMod::GetRate(Player* player, RateStoneTypes type)
{
	for (std::vector<RateStoneTemplate>::iterator itr = RateStoneVec.begin(); itr != RateStoneVec.end(); ++itr)
		if (type == itr->type)
			if (player->HasItemCount(itr->entry, 1))
				return itr->rate;

	return 0;
}

uint32 ItemMod::GetRateAndDes(Player* player, RateStoneTypes type)
{
	for (std::vector<RateStoneTemplate>::iterator itr = RateStoneVec.begin(); itr != RateStoneVec.end(); ++itr)
		if (type == itr->type)
			if (player->HasItemCount(itr->entry, 1))
			{
				player->DestroyItemCount(itr->entry, 1, true);
				return itr->rate;
			}
	return 0;
}

bool ItemMod::HasTransFlag(Item* item)
{
	bool flag = false;

	for (uint8 slot = PROP_ENCHANTMENT_SLOT_0; slot < MAX_ENCHANTMENT_SLOT; slot++)
	{
		uint32 enchantId = item->GetEnchantmentId(EnchantmentSlot(slot));
		if (enchantId == TRANS_FLAG_ENCHANT_ID)
			flag = true;
	}
	
	return flag;
}

void ItemMod::ApplyHiddenItem(Player* player, Item* item, bool apply)
{
	for (std::vector<HiddenItemTemplate>::iterator i = HiddenItemInfo.begin(); i != HiddenItemInfo.end(); ++i)
		if (item->GetEntry() == i->entry)
		{
			player->_ApplyItemModsCustom(item, 0, apply);
			sGS->UpdateGS(player, item->GetEntry(), GS_TYPE_ITEM_HIDDEN, apply);
			return;
		}
}

bool ItemMod::IsCurrencyLike(uint32 itemid)
{
	if (std::find(CurrencyLikeItemVec.begin(), CurrencyLikeItemVec.end(), itemid) != CurrencyLikeItemVec.end())
		return true;

	return false;
}

void ItemMod::ResetDayLimitItem()
{
	SessionMap const& smap = sWorld->GetAllSessions();
	for (SessionMap::const_iterator iter = smap.begin(); iter != smap.end(); ++iter)
		if (Player* player = iter->second->GetPlayer())
			player->PDayLimitItemMap.clear();

	CharacterDatabase.Execute(CharacterDatabase.GetPreparedStatement(CHAR_DEL_ITEM_DAY));
}

std::string ItemMod::GetDayLimitDes(Player* player, uint32 entry)
{
	std::ostringstream oss;
	uint32 limit = 0;
	uint32 count = 0;

	
	auto itr = DayLimitItemMap.find(entry);
	if (itr != DayLimitItemMap.end())
		limit = itr->second;
	else
		return "";

	itr = player->PDayLimitItemMap.find(entry);
	if (itr != player->PDayLimitItemMap.end())
		count = itr->second;

	if (count > limit)
		count = limit;

	oss << "|cFF00FF00「每日上限 |cFFFFCC00" << count << "/" << limit << "|r |cFF00FF00」|r\n";

	return oss.str();
}


std::string ItemMod::GetUseDes(uint32 entry)
{
	std::ostringstream oss;
	uint32 reqId = 0;

	uint32 len = ItemUseVec.size();

	for (size_t i = 0; i < len; i++)
	{
		if (entry == ItemUseVec[i].entry)
		{
			reqId = ItemUseVec[i].reqId;
			break;
		}
	}

	if (reqId != 0)
	{
		oss << "|cFF00FF00「使用需要满足」|r\n";
		oss << sReq->GetExtraDes(reqId);
	}

	return oss.str();
}

std::string ItemMod::GetEquipDes(uint32 entry)
{
	std::ostringstream oss;

	uint32 reqId = GetEquipInfo(entry);

	if (reqId != 0)
	{
		oss << "|cFF00FF00「装备需要满足」|r\n";
		oss << sReq->GetExtraDes(reqId);
	}
	return oss.str();
}

std::string ItemMod::GetBuyDes(uint32 entry)
{
	std::ostringstream oss;
	uint32 reqId = 0;

	uint32 len = ItemBuyVec.size();

	for (size_t i = 0; i < len; i++)
	{
		if (entry == ItemBuyVec[i].entry)
		{
			reqId = ItemBuyVec[i].reqId;
			
			break;
		}
	}

	if (reqId != 0)
	{
		oss << "|cFF00FF00「购买需要满足」|r\n";
		oss << sReq->GetExtraDes(reqId);
	}
	
	return oss.str();
}

void ItemMod::GetDes(uint32 entry, std::string &description, std::string &heroText)
{
	auto iter = ItemDesMap.find(entry);

	if (iter != ItemDesMap.end())
	{
		description = iter->second.description;
		heroText = iter->second.heroText;
	}
}

void ItemMod::AddCategoryItem(Player* player, Player* target, uint32 categoryId)
{
	uint32 len = ItemAddVec.size();

	for (size_t i = 0; i < len; i++)
	{
		if (categoryId == ItemAddVec[i].categoryId)
		{
			if (ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(ItemAddVec[i].entry))
			{
				if (AddItem(target, ItemAddVec[i].entry, ItemAddVec[i].count))
					ChatHandler(player->GetSession()).PSendSysMessage("%s获得%s X %u", target->GetName().c_str(), pProto->Name1.c_str(), ItemAddVec[i].count);
				else
					ChatHandler(player->GetSession()).PSendSysMessage("添加%s失败", pProto->Name1.c_str());
			}
		}
	}
}

void ItemMod::UnApplyEquipments(Player* player)
{
	for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; slot++)
	{
		if (Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
		{
			ItemTemplate const* pProto = pItem->GetTemplate();

			if (!pProto)
				continue;

			bool valid = true;

			std::unordered_map<uint32/*entry*/, uint32/*reqId*/>::iterator iter = ItemEquipMap.find(pItem->GetEntry());
			if (iter != ItemEquipMap.end())
				if(!sReq->Check(player, iter->second))
					valid = false;

			if (valid)
				continue;

			if (pProto && pProto->ItemSet)
				RemoveItemsSetItem(player, pProto);

			player->_ApplyItemMods(pItem, slot, false);

			if (slot < EQUIPMENT_SLOT_END)
			{
				player->RemoveItemDependentAurasAndCasts(pItem);
				switch (slot)
				{
				case EQUIPMENT_SLOT_MAINHAND:
				case EQUIPMENT_SLOT_OFFHAND:
				case EQUIPMENT_SLOT_RANGED:
					player->RecalculateRating(CR_ARMOR_PENETRATION);
				default:
					break;
				}

				if (slot == EQUIPMENT_SLOT_MAINHAND)
					player->UpdateExpertise(BASE_ATTACK);
				else if (slot == EQUIPMENT_SLOT_OFFHAND)
					player->UpdateExpertise(OFF_ATTACK);
			}

			ChatHandler(player->GetSession()).PSendSysMessage("卸载%s属性", sCF->GetItemLink(pItem, player->GetSession()).c_str());
		}
	}
}

/*
void ItemMod::AddEnchant(Player* player, uint32 enchantId)
{
	if (HasEnchant(player, enchantId))
		return;

	player->RuneVec.push_back(enchantId);

	//更新数据库
	PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_REP_RUNE);
	stmt->setUInt32(0, player->GetGUIDLow());
	stmt->setUInt32(1, enchantId);
	CharacterDatabase.Execute(stmt);
}
void ItemMod::LoadEnchant(Player* player)
{
	player->RuneVec.clear();
	QueryResult result = CharacterDatabase.PQuery("SELECT enchantId from character_rune where guid = '%u'", player->GetGUIDLow());

	if (result)
	{
		do
		{
			player->RuneVec.push_back(result->Fetch()[0].GetUInt32());
		} while (result->NextRow());
	}
}
bool ItemMod::HasEnchant(Player* player, uint32 enchantId)
{
	if (player->RuneVec.empty())
		return false;

	return std::find(player->RuneVec.begin(), player->RuneVec.end(), enchantId) != player->RuneVec.end();
}
*/

bool ItemMod::IsGemLimited(Player* player)
{
	bool flag = false;

	for (auto itr = GetmCountLimitMap.begin(); itr != GetmCountLimitMap.end(); itr++)
	{
		int32 count = 0;
		for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
			if (Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
				count += pItem->GetGemCountWithID(itr->first);
		
		if (count >= itr->second)
			flag = true;
	}

	return flag;
}

int32 ItemMod::GetGemCount(Item* pItem, uint32 gemEntry)
{
	if (!pItem)
		return 0;

	int32 count = 0;

	for (uint8 slot = SOCK_ENCHANTMENT_SLOT; slot <= SOCK_ENCHANTMENT_SLOT_3; slot++)
	{
		uint32 enchant_id = pItem->GetEnchantmentId(EnchantmentSlot(slot));

		if (SpellItemEnchantmentEntry const* pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id))
			if (const ItemTemplate * itemProto = sObjectMgr->GetItemTemplate(pEnchant->GemID))
				if (gemEntry == pEnchant->GemID)
					count++;
	}

	return count;
}

bool ItemMod::IsGemLimited(Player* player, Item* pItem1, Item* pItem2)
{
	for (auto itr = GetmCountLimitMap.begin(); itr != GetmCountLimitMap.end(); itr++)
	{
		int32 count = 0;
		for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
			if (Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
				count += pItem->GetGemCountWithID(itr->first);

		if (count + GetGemCount(pItem2, itr->first) - GetGemCount(pItem1, itr->first) >= itr->second)
			return true;
	}

	return false;
}

void ItemMod::SendData(Player* player)
{
	if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, INVENTORY_SLOT_ITEM_START))
	{
		uint32 itemId = item->GetEntry();

		std::ostringstream oss;
		oss << itemId << "#";
		oss << item->GetEnchantmentId(PERM_ENCHANTMENT_SLOT) << "*";
		//oss << getGemId(item, SOCK_ENCHANTMENT_SLOT) << "*";
		//oss << getGemId(item, SOCK_ENCHANTMENT_SLOT_2) << "*";
		//oss << getGemId(item, SOCK_ENCHANTMENT_SLOT_3) << "*";
		oss << item->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT) << "*";
		oss << item->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT_2) << "*";
		oss << item->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT_3) << "*";
		oss << item->GetEnchantmentId(BONUS_ENCHANTMENT_SLOT) << "*";
		oss << item->GetItemRandomPropertyId() << "*";

		for (size_t i = PROP_ENCHANTMENT_SLOT_0; i < MAX_ENCHANTMENT_SLOT; i++)
			oss << GetUIEnchantDescription(item, EnchantmentSlot(i)) << "*";

		sGCAddon->SendPacketTo(player, "GC_S_ITEMDATA", oss.str());

		sLog->outString(oss.str().c_str());
	}
}

std::string ItemMod::GetUIEnchantDescription(Item* item, EnchantmentSlot slot)
{
	uint32 enchantId = item->GetEnchantmentId(slot);
	SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchantId);
	if (!enchantEntry)
		return "0";

	for (uint8 i = 0; i < 16; ++i)
		if (strlen(enchantEntry->description[i]))
			return enchantEntry->description[i];

	return "0";
}

void ItemMod::InitUIItemEntryData()
{
	UIItemEntryMap.clear(); 

	//des heroText
	for (auto iter = ItemDesMap.begin(); iter != ItemDesMap.end(); iter++)
	{
		auto it = UIItemEntryMap.find(iter->first);
		if (it != UIItemEntryMap.end())
		{
			it->second.des = iter->second.description;
			it->second.heroText = iter->second.heroText;
		}		
		else
		{
			UIItemEntryTemplate temp;
			temp.des = iter->second.description;
			temp.heroText = iter->second.heroText;
			temp.daylimit = 0;
			temp.maxGems = 0;
			temp.exchange1 = 0;
			temp.exchangeReqId1 = 0;
			temp.exchange2 = 0;
			temp.exchangeReqId2 = 0;
			temp.unbindReqId = 0;
			temp.useReqId = 0;
			temp.equipReqId = 0;
			temp.buyReqId = 0;
			temp.sellRewId = 0;
			temp.recoveryRewId = 0;
			temp.gs = 0;
			UIItemEntryMap.insert(std::make_pair(iter->first, temp));
		}
	}

	//daylimit
	for (auto iter = DayLimitItemMap.begin(); iter != DayLimitItemMap.end(); iter++)
	{
		auto it = UIItemEntryMap.find(iter->first);
		if (it != UIItemEntryMap.end())
			it->second.daylimit = iter->second;
		else
		{
			UIItemEntryTemplate temp;
			temp.des = "";
			temp.heroText = "";
			temp.daylimit = iter->second;
			temp.maxGems = 0;
			temp.exchange1 = 0;
			temp.exchangeReqId1 = 0;
			temp.exchange2 = 0;
			temp.exchangeReqId2 = 0;
			temp.unbindReqId = 0;
			temp.useReqId = 0;
			temp.equipReqId = 0;
			temp.buyReqId = 0;
			temp.sellRewId = 0;
			temp.recoveryRewId = 0;
			temp.gs = 0;
			UIItemEntryMap.insert(std::make_pair(iter->first, temp));
		}
	}

	//maxGems
	for (auto iter = GemCountLimitInfo.begin(); iter != GemCountLimitInfo.end(); iter++)
	{
		auto it = UIItemEntryMap.find(iter->entry);
		if (it != UIItemEntryMap.end())
			it->second.maxGems = iter->limitCount;
		else
		{
			UIItemEntryTemplate temp;
			temp.des = "";
			temp.heroText = "";
			temp.daylimit = 0;
			temp.maxGems = iter->limitCount;
			temp.exchange1 = 0;
			temp.exchangeReqId1 = 0;
			temp.exchange2 = 0;
			temp.exchangeReqId2 = 0;
			temp.unbindReqId = 0;
			temp.useReqId = 0;
			temp.equipReqId = 0;
			temp.buyReqId = 0;
			temp.sellRewId = 0;
			temp.recoveryRewId = 0;
			temp.gs = 0;
			UIItemEntryMap.insert(std::make_pair(iter->entry, temp));
		}
	}

	//exchange
	for (auto iter = ItemExchangeInfo.begin(); iter != ItemExchangeInfo.end(); iter++)
	{
		auto it = UIItemEntryMap.find(iter->item);
		if (it != UIItemEntryMap.end())
		{
			if (iter->flag == 0)
			{
				it->second.exchange1 = iter->exchangeditem;
				it->second.exchangeReqId1 = iter->reqId;
			}
			else
			{
				it->second.exchange2 = iter->exchangeditem;
				it->second.exchangeReqId2 = iter->reqId;
			}
		}
		else
		{
			UIItemEntryTemplate temp;
			temp.des = "";
			temp.heroText = "";
			temp.daylimit = 0;
			temp.maxGems = 0;

			if (iter->flag == 0)
			{
				temp.exchange1 = iter->exchangeditem;
				temp.exchangeReqId1 = iter->reqId;
				temp.exchange2 = 0;
				temp.exchangeReqId2 = 0;
			}
			else
			{
				temp.exchange1 = 0;
				temp.exchangeReqId1 = 0;
				temp.exchange2 = iter->exchangeditem;
				temp.exchangeReqId2 = iter->reqId;
			}
			temp.unbindReqId = 0;
			temp.useReqId = 0;
			temp.equipReqId = 0;
			temp.buyReqId = 0;
			temp.sellRewId = 0;
			temp.recoveryRewId = 0;
			temp.gs = 0;
			UIItemEntryMap.insert(std::make_pair(iter->item, temp));
		}
	}

	//unbind
	for (auto iter = ItemUnbindCostInfo.begin(); iter != ItemUnbindCostInfo.end(); iter++)
	{
		auto it = UIItemEntryMap.find(iter->entry);
		if (it != UIItemEntryMap.end())
			it->second.unbindReqId = iter->reqId;
		else
		{
			UIItemEntryTemplate temp;
			temp.des = "";
			temp.heroText = "";
			temp.daylimit = 0;
			temp.maxGems = 0;
			temp.exchange1 = 0;
			temp.exchangeReqId1 = 0;
			temp.exchange2 = 0;
			temp.exchangeReqId2 = 0;
			temp.unbindReqId = iter->reqId;
			temp.useReqId = 0;
			temp.equipReqId = 0;
			temp.buyReqId = 0;
			temp.sellRewId = 0;
			temp.recoveryRewId = 0;
			temp.gs = 0;
			UIItemEntryMap.insert(std::make_pair(iter->entry, temp));
		}
	}

	//use
	for (auto iter = ItemUseVec.begin(); iter != ItemUseVec.end(); iter++)
	{
		auto it = UIItemEntryMap.find(iter->entry);
		if (it != UIItemEntryMap.end())
			it->second.useReqId = iter->reqId;
		else
		{
			UIItemEntryTemplate temp;
			temp.des = "";
			temp.heroText = "";
			temp.daylimit = 0;
			temp.maxGems = 0;
			temp.exchange1 = 0;
			temp.exchangeReqId1 = 0;
			temp.exchange2 = 0;
			temp.exchangeReqId2 = 0;
			temp.unbindReqId = 0;
			temp.useReqId = iter->reqId;
			temp.equipReqId = 0;
			temp.buyReqId = 0;
			temp.sellRewId = 0;
			temp.recoveryRewId = 0;
			temp.gs = 0;
			UIItemEntryMap.insert(std::make_pair(iter->entry, temp));
		}
	}

	//equip
	for (auto iter = ItemEquipMap.begin(); iter != ItemEquipMap.end(); iter++)
	{
		auto it = UIItemEntryMap.find(iter->first);
		if (it != UIItemEntryMap.end())
			it->second.equipReqId = iter->second;
		else
		{
			UIItemEntryTemplate temp;
			temp.des = "";
			temp.heroText = "";
			temp.daylimit = 0;
			temp.maxGems = 0;
			temp.exchange1 = 0;
			temp.exchangeReqId1 = 0;
			temp.exchange2 = 0;
			temp.exchangeReqId2 = 0;
			temp.unbindReqId = 0;
			temp.useReqId = 0;
			temp.equipReqId = iter->second;
			temp.buyReqId = 0;
			temp.sellRewId = 0;
			temp.recoveryRewId = 0;
			temp.gs = 0;
			UIItemEntryMap.insert(std::make_pair(iter->first, temp));
		}
	}

	//buy
	for (auto iter = ItemBuyVec.begin(); iter != ItemBuyVec.end(); iter++)
	{
		auto it = UIItemEntryMap.find(iter->entry);
		if (it != UIItemEntryMap.end())
			it->second.buyReqId = iter->reqId;
		else
		{
			UIItemEntryTemplate temp;
			temp.des = "";
			temp.heroText = "";
			temp.daylimit = 0;
			temp.maxGems = 0;
			temp.exchange1 = 0;
			temp.exchangeReqId1 = 0;
			temp.exchange2 = 0;
			temp.exchangeReqId2 = 0;
			temp.unbindReqId = 0;
			temp.useReqId = 0;
			temp.equipReqId = 0;
			temp.buyReqId = iter->reqId;
			temp.sellRewId = 0;
			temp.recoveryRewId = 0;
			temp.gs = 0;
			UIItemEntryMap.insert(std::make_pair(iter->entry, temp));
		}
	}

	//sell
	for (auto iter = ItemSaleVec.begin(); iter != ItemSaleVec.end(); iter++)
	{
		auto it = UIItemEntryMap.find(iter->entry);
		if (it != UIItemEntryMap.end())
			it->second.sellRewId = iter->rewId;
		else
		{
			UIItemEntryTemplate temp;
			temp.des = "";
			temp.heroText = "";
			temp.daylimit = 0;
			temp.maxGems = 0;
			temp.exchange1 = 0;
			temp.exchangeReqId1 = 0;
			temp.exchange2 = 0;
			temp.exchangeReqId2 = 0;
			temp.unbindReqId = 0;
			temp.useReqId = 0;
			temp.equipReqId = 0;
			temp.buyReqId = 0;
			temp.sellRewId = iter->rewId;
			temp.recoveryRewId = 0;
			temp.gs = 0;
			UIItemEntryMap.insert(std::make_pair(iter->entry, temp));
		}
	}

	//recovery
	for (auto iter = RecoveryMap.begin(); iter != RecoveryMap.end(); iter++)
	{
		auto it = UIItemEntryMap.find(iter->first);
		if (it != UIItemEntryMap.end())
			it->second.recoveryRewId = iter->first;
		else
		{
			UIItemEntryTemplate temp;
			temp.des = "";
			temp.heroText = "";
			temp.daylimit = 0;
			temp.maxGems = 0;
			temp.exchange1 = 0;
			temp.exchangeReqId1 = 0;
			temp.exchange2 = 0;
			temp.exchangeReqId2 = 0;
			temp.unbindReqId = 0;
			temp.useReqId = 0;
			temp.equipReqId = 0;
			temp.buyReqId = 0;
			temp.sellRewId = 0;
			temp.recoveryRewId = iter->first;
			temp.gs = 0;
			UIItemEntryMap.insert(std::make_pair(iter->first, temp));
		}
	}

	//gs

	//recovery
	for (auto iter = GSVec.begin(); iter != GSVec.end(); iter++)
	{
		if (iter->type == GS_TYPE_ITEM_EQUIP || iter->type == GS_TYPE_ITEM_HIDDEN)
		{
			auto it = UIItemEntryMap.find(iter->id);
			if (it != UIItemEntryMap.end())
				it->second.gs = iter->gs;
			else
			{
				UIItemEntryTemplate temp;
				temp.des = "";
				temp.heroText = "";
				temp.daylimit = 0;
				temp.maxGems = 0;
				temp.exchange1 = 0;
				temp.exchangeReqId1 = 0;
				temp.exchange2 = 0;
				temp.exchangeReqId2 = 0;
				temp.unbindReqId = 0;
				temp.useReqId = 0;
				temp.equipReqId = 0;
				temp.buyReqId = 0;
				temp.sellRewId = 0;
				temp.recoveryRewId = 0;
				temp.gs = iter->gs;
				UIItemEntryMap.insert(std::make_pair(iter->id, temp));
			}
		}
	}
}
