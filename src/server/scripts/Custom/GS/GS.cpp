#pragma execution_character_set("utf-8")
#include "GS.h"
#include "../ItemMod/ItemMod.h"
#include "../GCAddon/GCAddon.h"

std::vector<GSTemplate> GSVec;

void GS::Load()
{
	GSVec.clear();
	
	//QueryResult result = WorldDatabase.PQuery("SELECT Id, Type, GS FROM _gs");

	//if (!result)
	//	return;
	//do
	//{
	//	Field* fields = result->Fetch();
	//	GSTemplate Temp;
	//	Temp.id = fields[0].GetUInt32();
	//
	//	const char*  str = fields[1].GetCString();
	//
	//	if (strcmp("装备", str) == 0)
	//		Temp.type = GS_TYPE_ITEM_EQUIP;
	//	else if (strcmp("技能", str) == 0)
	//		Temp.type = GS_TYPE_SPELL;
	//	else if (strcmp("隐藏物品", str) == 0)
	//		Temp.type = GS_TYPE_ITEM_HIDDEN;
	//
	//	Temp.gs = fields[2].GetUInt32();
	//
	//	GSVec.push_back(Temp);
	//} while (result->NextRow());
}

uint32 GS::GetGS(uint32 id, GSTypes type)
{
	if (id == 0)
		return 0;

	for (auto itr = GSVec.begin(); itr != GSVec.end(); itr++)
		if (id == itr->id && type == itr->type)
			return itr->gs;

	return 0;
}

void GS::UpdateGS(Player* pl)
{
	uint32 gs = 0;

	//技能
	PlayerSpellMap spellMap = pl->GetSpellMap();
	for (PlayerSpellMap::const_iterator iter = spellMap.begin(); iter != spellMap.end(); ++iter)
		gs += GetGS(iter->first, GS_TYPE_SPELL); 

	//装备栏
	for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; i++)
		if (Item* item = pl->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
			gs += GetGS(item->GetEntry(), GS_TYPE_ITEM_EQUIP);

	//主背包
	for (uint8 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++)
		if (Item* item = pl->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
			for (uint32 k = 0; k < HiddenItemInfo.size(); k++)
				if (item->GetEntry() == HiddenItemInfo[k].entry)
					gs += GetGS(item->GetEntry(), GS_TYPE_ITEM_HIDDEN);

	//额外三个背包
	for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
		if (Bag* pBag = pl->GetBagByPos(i))
			for (uint32 j = 0; j < pBag->GetBagSize(); j++)
				if (Item* item = pl->GetItemByPos(i, j))
					for (uint32 k = 0; k < HiddenItemInfo.size(); k++)
						if (item->GetEntry() == HiddenItemInfo[k].entry)
							gs += GetGS(item->GetEntry(), GS_TYPE_ITEM_HIDDEN);

	//银行
	for (uint8 i = BANK_SLOT_ITEM_START; i < BANK_SLOT_BAG_END; ++i)
		if (Item* item = pl->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
			for (uint32 k = 0; k < HiddenItemInfo.size(); k++)
				if (item->GetEntry() == HiddenItemInfo[k].entry)
					gs += GetGS(item->GetEntry(), GS_TYPE_ITEM_HIDDEN);
	//银行背包
	for (uint8 i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; ++i)
		if (Bag* pBag = pl->GetBagByPos(i))
			for (uint32 j = 0; j < pBag->GetBagSize(); j++)
				if (Item* item = pl->GetItemByPos(i, j))
					for (uint32 k = 0; k < HiddenItemInfo.size(); k++)
						if (item->GetEntry() == HiddenItemInfo[k].entry)
							gs += GetGS(item->GetEntry(), GS_TYPE_ITEM_HIDDEN);

	pl->GS = gs;
}

void GS::UpdateGS(Player* pl, uint32 id, GSTypes type, bool add)
{
	uint32 gs = GetGS(id, type);

	if (gs == 0)
		return;

	add ? pl->GS += gs : pl->GS -= gs;

	SendVisableGSData(pl, false);
}

void GS::SendGSData(Player* player, Player* target)
{
	std::ostringstream oss;
	oss << target->namePrefix + target->GetName() + target->nameSuffix << " ";
	oss << target->GetName() << " ";
	oss << target->GS;
	sGCAddon->SendPacketTo(player, "GC_S_GS", oss.str());
}

void GS::SendVisableGSData(Player* player, bool sendtoself)
{
	std::list<Player*> playersNearby;
	player->GetPlayerListInGrid(playersNearby, player->GetVisibilityRange(), false);

	if (!playersNearby.empty())
		for (std::list<Player*>::iterator iter = playersNearby.begin(); iter != playersNearby.end(); ++iter)
			if (Player* tar = *iter)
				sendtoself ? SendGSData(player, tar) : SendGSData(tar, player);
}


uint32 GS::GetItemGS(uint32 id)
{
	for (auto itr = GSVec.begin(); itr != GSVec.end(); itr++)
		if (id == itr->id && GS_TYPE_ITEM_EQUIP == itr->type && GS_TYPE_ITEM_HIDDEN == itr->type)
			return itr->gs;
}

void GS::SendSpellGSData(Player* pl)
{
	for (auto itr = GSVec.begin(); itr != GSVec.end(); itr++)
		if (GS_TYPE_SPELL == itr->type)
			sGCAddon->SendPacketTo(pl, "GC_S_GS_SPELLDATA", std::to_string(itr->id) + " " + std::to_string(itr->gs));
}
