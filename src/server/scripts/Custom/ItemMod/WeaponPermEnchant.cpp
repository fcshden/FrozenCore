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

void ItemMod::RecoverWeaponPermEnchant(Player* player)
{
	uint32 len = player->WeaponVec.size();

	for (size_t i = 0; i < len; i++)
	{
		Item* item = player->WeaponVec[i].item;

		if (item)
		{

			uint32 enchantId = player->WeaponVec[i].enchantId;
			player->ApplyEnchantment(item, PERM_ENCHANTMENT_SLOT, false);
			item->SetEnchantment(PERM_ENCHANTMENT_SLOT, enchantId, 0, 0);
			player->ApplyEnchantment(item, PERM_ENCHANTMENT_SLOT, true);
			//player->SaveToDB(false, false);
		}
	}

	player->WeaponVec.clear();
}


void ItemMod::AddWeaponPermList(Player* player, Item* item)
{
	uint32 currEnchantId = item->GetEnchantmentId(PERM_ENCHANTMENT_SLOT);
	
	//RemoveTempPermEnchant(player, item);
	//RecoverWeaponPermEnchant(player);

	SetFilterVec(player, item, ITEM_ENCHANT_WEAPON_PERM, 0);

	uint32 len = FilterVec.size();
	bool undefineEnchant = true;
	

	for (uint32 i = 0; i < len; i++)
		if (currEnchantId && currEnchantId == FilterVec[i].enchantId)
		{
			undefineEnchant = false;
			break;
		}

	std::ostringstream ossCurrEnchant;
	
	if (currEnchantId)
		ossCurrEnchant << "[当前：" << GetItemEnchantDescription(player, currEnchantId) << "]";
		
	if (undefineEnchant && currEnchantId)
		player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, ossCurrEnchant.str(), senderValue(PERM_ENCHANTMENT_SLOT, currEnchantId), ACTION_REMOVE_ENCHANT_UNDEFINE, "移除光效", DEFAULT_REMOVE_ENCHANT_GOLDS * GOLD, 0);

	if (!undefineEnchant && currEnchantId)
		for (uint32 i = 0; i < len; i++)
			if (currEnchantId == FilterVec[i].enchantId)
			{
				player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, ossCurrEnchant.str(), senderValue(PERM_ENCHANTMENT_SLOT, currEnchantId), ACTION_ITEM_REMOVE_ENCHANT, "移除光效" + sCF->GetItemLink(item->GetEntry()), 0, 0);
				break;
			}

	for (uint32 i = player->flag_i; i < len; i++)
	{
		if(currEnchantId != FilterVec[i].enchantId)
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GetItemEnchantDescription(player, FilterVec[i].enchantId), FilterVec[i].enchantId, ACTION_WEAPON_LOOKUPANDBUY_SHOW);
		player->flag_i++;
		player->rowId++;
		if (player->rowId >= MAX_ROWS_COUNT)
		{
			player->rowId = 0;
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "下页", GOSSIP_SENDER_MAIN, ACTION_WEAPON_PERMENCHANT_NEXT_MENU_SHOW);
			if (player->pageId >= 2)
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "上页", GOSSIP_SENDER_MAIN, ACTION_PREV_PAGE);
			break;
		}
	}

	if (player->rowId < MAX_ROWS_COUNT && player->rowId != 0 && player->pageId != 1)
	{
		player->flag_i = player->flag_i + MAX_ROWS_COUNT - player->rowId;
		player->rowId = 0;
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "上页", GOSSIP_SENDER_MAIN, ACTION_PREV_PAGE);
	}

	player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "返回", GOSSIP_SENDER_MAIN, ACTION_MAINMENU_BACK);
	player->PlayerTalkClass->GetGossipMenu().SetMenuId(MENU_ID);
	player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, player->GetGUID());
}

void ItemMod::LookupOrBuyWeaponPermEnchant(Player* player, Item* item, uint32 sender)
{
	uint32 reqId = 0;

	uint32 len = FilterVec.size();

	for (size_t i = 0; i < len; i++)
		if (sender == FilterVec[i].enchantId)
		{
			reqId = FilterVec[i].enchantReqId;
			break;
		}

	player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "购买", sender, ACTION_WEAPONPERM_BUY, sReq->Notice(player, reqId, sCF->GetItemLink(item->GetEntry()), "附加光效"), sReq->Golds(reqId), 0);
	//HasPermEchant(item) ? player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "查看", sender, ACTION_WEAPONPERM_LOOKUP, "将会移除当前武器光效，确定吗？", 0, 0) : 
	player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "查看", sender, ACTION_WEAPONPERM_LOOKUP);
	player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "返回", GOSSIP_SENDER_MAIN, ACTION_WEAPON_PERMENCHANT_CURR_MENU_SHOW);
	player->PlayerTalkClass->GetGossipMenu().SetMenuId(MENU_ID);
	player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, player->GetGUID());
}



void ItemMod::LookupWeaponPermEnchant(Player* player, Item* item, uint32 enchantId)
{
	if (!item->IsEquipped())
	{
		ChatHandler(player->GetSession()).PSendSysMessage("请先装备这件武器!");
		return;
	}

	uint32 len = FilterVec.size();

	uint32 reqId = 0;
	for (size_t i = 0; i < len; i++)
		if (enchantId == FilterVec[i].enchantId)
		{
			reqId = FilterVec[i].enchantReqId;
			break;
		}
	player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "购买", enchantId, ACTION_WEAPONPERM_BUY, sReq->Notice(player, reqId, sCF->GetItemLink(item->GetEntry()), "附加光效"), sReq->Golds(reqId), 0);
	//HasPermEchant(item) ? player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "查看", enchantId, ACTION_WEAPONPERM_LOOKUP, "将会移除当前武器光效，确定吗？", 0, 0) : 
	player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "查看", enchantId, ACTION_WEAPONPERM_LOOKUP);
	player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "返回", GOSSIP_SENDER_MAIN, ACTION_WEAPON_PERMENCHANT_CURR_MENU_SHOW);
	player->PlayerTalkClass->GetGossipMenu().SetMenuId(MENU_ID);
	player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, player->GetGUID());

	bool invec = false;
	for (size_t i = 0; i < player->WeaponVec.size(); i++)
		if (item == player->WeaponVec[i].item)
			invec = true;
	
	if (!invec)
	{
		uint32 currEnchantId = item->GetEnchantmentId(PERM_ENCHANTMENT_SLOT);
		weapontemplate temp;
		temp.enchantId = currEnchantId;
		temp.item = item;
		player->WeaponVec.push_back(temp);

		ChatHandler(player->GetSession()).PSendSysMessage("pushback <------- item:%d -- currEnchantId:%d", item->GetEntry(), currEnchantId);
	}
	
	player->ApplyEnchantment(item, PERM_ENCHANTMENT_SLOT, false);
	item->SetEnchantment(PERM_ENCHANTMENT_SLOT, enchantId, LOOKUP_WEAPON_PERM_SECONDS * IN_MILLISECONDS, 0);
	player->ApplyEnchantment(item, PERM_ENCHANTMENT_SLOT, true);
	ChatHandler(player->GetSession()).PSendSysMessage("光效将在%d后或重新打开菜单时消失！", LOOKUP_WEAPON_PERM_SECONDS);

	player->isInLookupPermEnchant = true;
	player->lookupPermEnchantTimer = 0;
}

void ItemMod::BuyWeaponPermEnchant(Player* player, Item* item, uint32 enchantId)
{
	uint32 reqId = 0;

	uint32 len = FilterVec.size();

	for (size_t i = 0; i < len; i++)
		if (enchantId == FilterVec[i].enchantId)
		{
			reqId = FilterVec[i].enchantReqId;
			break;
		}

	if (sReq->Check(player, reqId))
	{
		player->ApplyEnchantment(item, PERM_ENCHANTMENT_SLOT, false);
		item->SetEnchantment(PERM_ENCHANTMENT_SLOT, enchantId, 0, 0);
		player->ApplyEnchantment(item, PERM_ENCHANTMENT_SLOT, true);
		player->CastSpell(player, VISUAL_SPELL_ID, true, NULL, NULL, player->GetGUID());
		sReq->Des(player, reqId);
		player->CLOSE_GOSSIP_MENU();
		
		//购买过FM的 item 将从vec中删除，这样等查看结束后将不会删除FM效果
		for (std::vector<weapontemplate>::iterator itr = player->WeaponVec.begin(); itr != player->WeaponVec.end();)
		{
			if (itr->item == item)
				itr = player->WeaponVec.erase(itr);
			else
				++itr;
		}
	}
}

