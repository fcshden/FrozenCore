#pragma execution_character_set("utf-8")
#include "Recovery.h"
#include "../GCAddon/GCAddon.h"
#include "../CommonFunc/CommonFunc.h"
#include "../String/myString.h"
#include "..\..\server\scripts\Custom\Reward\Reward.h"
std::unordered_map<uint32/*entry*/, RecoveryTemplate> RecoveryMap;
std::unordered_map<uint32/*categoryId*/, std::string/*categoryName*/> RecoveryCategoryMap;

void Recovery::Load()
{
	RecoveryMap.clear();
	RecoveryCategoryMap.clear();

	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ?
        "SELECT 物品ID,分组ID,奖励积分数量,物品,数量,奖励模板ID FROM _物品_回收" :
		"SELECT entry,categoryId,rewToken FROM _recovery");
	if (result)
	{
		do
		{
            Field* fields = result->Fetch();
            uint32 entry = fields[0].GetUInt32();
            RecoveryTemplate Temp;
            Temp.categoryId = fields[1].GetUInt32();
            Temp.rewToken = fields[2].GetFloat();
            Temp.rewitem = fields[3].GetUInt32();
            Temp.rewcunt = fields[4].GetUInt32();
            Temp.rewid = fields[5].GetUInt32();
			RecoveryMap.insert(std::make_pair(entry, Temp));
		} while (result->NextRow());
	}
	
	result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ?
		"SELECT 分组ID,分组名称 FROM _物品_回收分组" :
		"SELECT categoryId,categoryName FROM _recovery_category");
	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			uint32 categoryId			= fields[0].GetUInt32();
			std::string categoryName	= fields[1].GetString();
			RecoveryCategoryMap.insert(std::make_pair(categoryId, categoryName));
		} while (result->NextRow());
	}
}

std::string Recovery::GetDes(uint32 entry)
{
	const ItemTemplate * temp = sObjectMgr->GetItemTemplate(entry);
	if (!temp)
		return "";

	std::ostringstream oss;
	std::unordered_map<uint32, RecoveryTemplate>::iterator iter = RecoveryMap.find(entry);

	if (iter != RecoveryMap.end())
		return "|cFF00FF00「可回收」|r\n";

	return "";
}

void Recovery::GetItemInfo(uint32 entry, uint32 count, uint32 &categoryId, float &tokenAmount)
{
	const ItemTemplate * temp = sObjectMgr->GetItemTemplate(entry);
	if (!temp)
		return;

	std::unordered_map<uint32, RecoveryTemplate>::iterator iter = RecoveryMap.find(entry);

	if (iter != RecoveryMap.end())
	{
		categoryId = iter->second.categoryId;
		tokenAmount = count * iter->second.rewToken;
	}
}

uint32 Recovery::GetCategoryId(uint32 entry)
{
	const ItemTemplate * temp = sObjectMgr->GetItemTemplate(entry);
	if (!temp)
		return 0;

	std::unordered_map<uint32, RecoveryTemplate>::iterator iter = RecoveryMap.find(entry);

	if (iter != RecoveryMap.end())
		return iter->second.categoryId;

	return 0;
}

uint32 Recovery::GetTokenAmount(Player* player, uint32 categoryId)
{
	float tokenAmount = 0;

	//主背包
	for (uint8 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++)
		if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
		{
			uint32 entry = item->GetEntry();
			uint32 count = item->GetCount();
			uint32 _categoryId = 0;
			float _tokenAmount = 0;
			GetItemInfo(entry, count, _categoryId, _tokenAmount);
			if (_categoryId == categoryId)
				tokenAmount += _tokenAmount;
		}

	//额外三个背包
	for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
		if (Bag* pBag = player->GetBagByPos(i))
			for (uint32 j = 0; j < pBag->GetBagSize(); j++)
				if (Item* item = player->GetItemByPos(i, j))
				{
					uint32 entry = item->GetEntry();
					uint32 count = item->GetCount();
					uint32 _categoryId = 0;
					float _tokenAmount = 0;
					GetItemInfo(entry, count, _categoryId, _tokenAmount);
					if (_categoryId == categoryId)
						tokenAmount += _tokenAmount;
				}

	return uint32(tokenAmount);
}

bool Recovery::HasCategoryItem(Player* player, uint32 categoryId)
{
	uint32 flag = false;

	//主背包
	for (uint8 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++)
		if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
		{
			uint32 entry = item->GetEntry();
			uint32 count = item->GetCount();
			uint32 _categoryId = 0;
			float _tokenAmount = 0;
			GetItemInfo(entry, count, _categoryId, _tokenAmount);
			if (_categoryId == categoryId)
				flag = true;
		}

	if (flag)
		return true;

	//额外三个背包
	for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
		if (Bag* pBag = player->GetBagByPos(i))
			for (uint32 j = 0; j < pBag->GetBagSize(); j++)
				if (Item* item = player->GetItemByPos(i, j))
				{
					uint32 entry = item->GetEntry();
					uint32 count = item->GetCount();
					uint32 _categoryId = 0;
					float _tokenAmount = 0;
					GetItemInfo(entry, count, _categoryId, _tokenAmount);
					if (_categoryId == categoryId)
						flag = true;
				}

	return flag;
}

void Recovery::OpenPanel(Player* player)
{
	std::ostringstream oss;
	oss << "0#";
	for (std::unordered_map<uint32, std::string>::iterator iter = RecoveryCategoryMap.begin(); iter != RecoveryCategoryMap.end(); iter++)
		if (HasCategoryItem(player, iter->first))
			oss << iter->first << "-" << iter->second << ":";
	
	sGCAddon->SendPacketTo(player, "GC_S_RECOVERY", oss.str());
}

void Recovery::SendCategoryMsg(Player* player, uint32 categoryId)
{
	std::ostringstream oss;
	oss << "1#" << GetTokenAmount(player, categoryId) << "#";

	//主背包
	for (uint8 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++)
		if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
		{
			uint32 entry = item->GetEntry();
			uint32 count = item->GetCount();
			if (GetCategoryId(entry) == categoryId)
				oss << entry << "-" << count << ":";
		}

	//额外三个背包
	for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
		if (Bag* pBag = player->GetBagByPos(i))
			for (uint32 j = 0; j < pBag->GetBagSize(); j++)
				if (Item* item = player->GetItemByPos(i, j))
				{
					uint32 entry = item->GetEntry();
					uint32 count = item->GetCount();
					if (GetCategoryId(entry) == categoryId)
						oss << entry << "-" << count << ":";
				}

	sGCAddon->SendPacketTo(player, "GC_S_RECOVERY", oss.str());
}

void Recovery::Action(Player* player, uint32 categoryId)
{
	uint32 tokenAmount = 0;

	//主背包
	for (uint8 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++)
		if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
		{
			uint32 entry = item->GetEntry();
			uint32 count = item->GetCount();
			uint32 _categoryId = 0;
			float _tokenAmount = 0;
			GetItemInfo(entry, count, _categoryId, _tokenAmount);
			if (_categoryId == categoryId)
			{
                std::unordered_map<uint32, RecoveryTemplate>::iterator iter = RecoveryMap.find(entry);
                if (iter != RecoveryMap.end())
                {
                    if (iter->second.rewitem && iter->second.rewcunt)
                        player->AddItem(iter->second.rewitem, iter->second.rewcunt * count);
                    if (iter->second.rewid)
                        sRew->Rew(player, iter->second.rewid, count);
                }
				player->DestroyItem(INVENTORY_SLOT_BAG_0, i, true);
				tokenAmount += _tokenAmount;
			}
		}

	//额外三个背包
	for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
		if (Bag* pBag = player->GetBagByPos(i))
			for (uint32 j = 0; j < pBag->GetBagSize(); j++)
				if (Item* item = player->GetItemByPos(i, j))
				{
					uint32 entry = item->GetEntry();
					uint32 count = item->GetCount();
					uint32 _categoryId = 0;
					float _tokenAmount = 0;
					GetItemInfo(entry, count, _categoryId, _tokenAmount);
					if (_categoryId == categoryId)
					{
                        std::unordered_map<uint32, RecoveryTemplate>::iterator iter = RecoveryMap.find(entry);
                        if (iter != RecoveryMap.end())
                        {
                            if (iter->second.rewitem && iter->second.rewcunt)
                                player->AddItem(iter->second.rewitem, iter->second.rewcunt * count);
                            if (iter->second.rewid)
                                sRew->Rew(player, iter->second.rewid, count);
                        }

						player->DestroyItem(i, j, true);
						tokenAmount += _tokenAmount;
					}
				}

	OpenPanel(player);

	ChatHandler(player->GetSession()).PSendSysMessage("回收物品获得[%s] X %u", sString->GetText(CORE_STR_TYPES(STR_TOKEN)), tokenAmount);
	sCF->UpdateTokenAmount(player, tokenAmount, true, "[回收]奖励");

	sCF->CompleteQuest(player, 30002);
}
