#pragma once
#pragma execution_character_set("utf-8")
#include "../CommonFunc/CommonFunc.h"
#include "../Requirement/Requirement.h"
#include "Reward.h"
#include "../DataLoader/DataLoader.h"
#include "../Command/CustomCommand.h"
#include "../Switch/Switch.h"
#include "../StatPoints/StatPoints.h"
#include "../String/myString.h"

std::unordered_map<uint32, RewTemplate> RewMap;

void Reward::Load()
{
	RewMap.clear();
	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		
		"SELECT ����ģ��ID,�������,��������,��������,��������,��������,��������,���ܻ�⻷��,GM������,"
		"��ƷID1,��Ʒ����1,��ƷID2,��Ʒ����2,��ƷID3,��Ʒ����3,��ƷID4,��Ʒ����4,��ƷID5,��Ʒ����5,"
		"��ƷID6,��Ʒ����6,��ƷID7,��Ʒ����7,��ƷID8,��Ʒ����8,��ƷID9,��Ʒ����9,��ƷID10,��Ʒ����10 FROM _ģ��_����" :

		"SELECT rewId,goldCount,tokenCount,xp,hrPoints,arenaPoints,statPoints,spellData,commandData,"
		"itemId1,itemCount1,itemId2,itemCount2,itemId3,itemCount3,itemId4,itemCount4,itemId5,itemCount5,"
		"itemId6,itemCount6,itemId7,itemCount7,itemId8,itemCount8,itemId9,itemCount9,itemId10,itemCount10 FROM _rew");
	if (!result)
		return;
	do
	{
		Field* fields = result->Fetch();
		uint32 rewId				= fields[0].GetUInt32();
		RewTemplate RewTemp;
		RewTemp.goldCount			= fields[1].GetUInt32() * GOLD;
		RewTemp.tokenCount			= fields[2].GetUInt32();
		RewTemp.xp					= fields[3].GetUInt32();
		RewTemp.hrPoints			= fields[4].GetUInt32();
		RewTemp.arenaPoints			= fields[5].GetUInt32();
		RewTemp.statPoints			= fields[6].GetUInt32();

		Tokenizer spellData(fields[7].GetString(), '#');

		for (Tokenizer::const_iterator itr = spellData.begin(); itr != spellData.end(); ++itr)
			if (SpellInfo const*  spellInfo = sSpellMgr->GetSpellInfo(abs(atoi(*itr))))
				RewTemp.SpellDataVec.push_back(atoi(*itr));

		Tokenizer commandData(fields[8].GetString(), '#');
		for (Tokenizer::const_iterator itr = commandData.begin(); itr != commandData.end(); ++itr)
		{
			Tokenizer commands(*itr, '$');
			RewCommandTemplate temp;
			if (commands.size() > 0)
				temp.command = commands[0];
			else
				temp.command = "";

			if (commands.size() > 1)
				temp.icon = commands[1];
			else
				temp.icon = "";

			if (commands.size() > 2)
				temp.des = commands[2];
			else
				temp.des = "";

			RewTemp.CommandDataVec.push_back(temp);
		}

		for (size_t i = 0; i < REW_ITEM_MAX; i++)
		{
			uint32 entry = fields[9 + 2 * i].GetUInt32();
			uint32 count = fields[10 + 2 * i].GetUInt32();

			ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(entry);

			if (pProto && count > 0)
			{
				RewItemTemplate temp;
				temp.itemId = entry;
				temp.itemCount = count;
				RewTemp.ItemDataVec.push_back(temp);
			}
		}
		
		RewMap.insert(std::make_pair(rewId, RewTemp));
	} while (result->NextRow());
}

void Reward::LoadDamCreToSend()
{
	VCreatureDamageSend.clear();
	QueryResult itemup = WorldDatabase.PQuery("select ������,����˺���,���н���,���е��� from _����_�˺�����");
	if (itemup)
	{
		int nCount = 0;
		do
		{
			CreatureDamageSend  tmpItem;
			tmpItem.creatureid = itemup->Fetch()[0].GetUInt32();
			tmpItem.mindamage = itemup->Fetch()[1].GetUInt32();
			Tokenizer senditems(itemup->Fetch()[2].GetString(), '#');
			tmpItem.sendgossipcount = itemup->Fetch()[3].GetUInt32();

			tmpItem.maxsend = senditems.size();
			if (tmpItem.maxsend)
			{
				for (uint32 i = 0; i < tmpItem.maxsend; i++)
				{
					tmpItem.itemsends[i] = atoi(senditems[i] ? senditems[i] : 0);
				}
			}

			VCreatureDamageSend.insert(CreatureDamageSend_t::value_type(tmpItem.creatureid, tmpItem));
			nCount++;

		} while (itemup->NextRow());
		sLog->outString(">> ��ȡ�Զ��幦�����ݱ� _����_�˺�����,��%u�����ݶ�ȡ����...", nCount);
	}
	else
		sLog->outString(">> ��ȡ�Զ��幦�����ݱ� _����_�˺�����,��0�����ݶ�ȡ����...");

}

void Reward::Rew(Player* player, uint32 rewId,uint32 muilt)
{
	if (rewId == 0)
		return;

	std::unordered_map<uint32, RewTemplate>::iterator iter = RewMap.find(rewId);
	if (iter != RewMap.end())
	{
		uint32 xp = muilt * iter->second.xp;
		uint32 goldCount = muilt * iter->second.goldCount;
		uint32 tokenCount = muilt * iter->second.tokenCount;
		uint32 hrPoints = muilt * iter->second.hrPoints;
		uint32 arenaPoints = muilt * iter->second.arenaPoints;
		uint32 statPoints = muilt * iter->second.statPoints;

		if (xp > 0)
		{

			uint32 curXP = player->GetUInt32Value(PLAYER_XP);
			uint32 nextLvlXP = player->GetUInt32Value(PLAYER_NEXT_LEVEL_XP);
			uint32 newXP = curXP + xp;
			uint32 level = player->getLevel();

			bool flag = false;

			while (newXP >= nextLvlXP && level < sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
			{
				newXP -= nextLvlXP;

				if (level < sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
				{
					if (level < sSwitch->GetValue(ST_XP_MAX_LV))
						player->GiveLevel(level + 1);
					else
						flag = true;
				}

				level = player->getLevel();
				nextLvlXP = player->GetUInt32Value(PLAYER_NEXT_LEVEL_XP);
			}

			player->SetUInt32Value(PLAYER_XP, newXP);

			if (flag)
				player->SetUInt32Value(PLAYER_XP, player->GetUInt32Value(PLAYER_NEXT_LEVEL_XP));

			std::ostringstream oss;
			oss << "���" << "|cFFFFCC00[����]|r X " << xp;
			player->GetSession()->SendAreaTriggerMessage(oss.str().c_str());
		}

		if (goldCount > 0)
		{
			player->ModifyMoney(goldCount);
			std::ostringstream oss;
			oss << "���" << "|cFFFFCC00[���]|r" << " X " << goldCount / GOLD;
			player->GetSession()->SendAreaTriggerMessage(oss.str().c_str());
		}

		if (tokenCount > 0)
		{
			sCF->UpdateTokenAmount(player, tokenCount, true, "[REW]����");
			std::ostringstream oss;
			oss << "���" << "|cFFFFCC00[" << sString->GetText(CORE_STR_TYPES(STR_TOKEN)) << "]|r" << " X " << tokenCount;
			player->GetSession()->SendAreaTriggerMessage(oss.str().c_str());
		}

		if (hrPoints > 0)
		{
			player->ModifyHonorPoints(hrPoints);
			std::ostringstream oss;
			oss << "���" << "|cFFFFCC00[����]|r" << " X " << hrPoints;
			player->GetSession()->SendAreaTriggerMessage(oss.str().c_str());
		}

		if (arenaPoints > 0)
		{
			player->ModifyArenaPoints(arenaPoints);
			std::ostringstream oss;
			oss << "���" << "|cFFFFCC00[������]|r" << " X " << arenaPoints;
			player->GetSession()->SendAreaTriggerMessage(oss.str().c_str());
		}

		if (statPoints > 0)
		{
			uint32 total = 0;

			for (size_t i = 0; i < MAX_STAT_POINTS_TYPE; i++)
				total += player->stat_points[i];

			if (total < sSwitch->GetValue(ST_SPS_LIMIT))
			{
				player->stat_points[SPT_TOTLAL] += statPoints;
				std::ostringstream oss;
				oss << "���" << "|cFFFFCC00[������]|r" << " X " << statPoints;
				player->GetSession()->SendAreaTriggerMessage(oss.str().c_str());
				sStatPoints->UpdateDB(player);
			}
		}


		for (auto itr = iter->second.ItemDataVec.begin(); itr != iter->second.ItemDataVec.end(); itr++)
			//player->AddItem(itr->itemId, itr->itemCount);
			RewItem(player, itr->itemId, itr->itemCount);

		for (auto itr = iter->second.SpellDataVec.begin(); itr != iter->second.SpellDataVec.end(); itr++)
		{
			if (*itr < 0)
				continue;

			uint32 spellId = *itr;

			SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
			if (spellInfo)
			{
				uint32 firstSpellId = sSpellMgr->GetFirstSpellInChain(spellId);

				PlayerSpellMap spellMap = player->GetSpellMap();

				for (PlayerSpellMap::const_iterator iter = spellMap.begin(); iter != spellMap.end(); ++iter)
					if (firstSpellId == sSpellMgr->GetFirstSpellInChain(iter->first))
						player->removeSpell(iter->first, SPEC_MASK_ALL, false);

				player->learnSpell(spellId);
			}
		}

		for (auto itr = iter->second.SpellDataVec.begin(); itr != iter->second.SpellDataVec.end(); itr++)
		{
			if (*itr > 0)
				continue;

			player->CastSpell(player, abs(*itr), false);
		}

		for (auto itr = iter->second.CommandDataVec.begin(); itr != iter->second.CommandDataVec.end(); itr++)
			if (!itr->command.empty())
				sCustomCommand->DoCommand(player, itr->command);
	}
	else
		sWorld->SendServerMessage(SERVER_MSG_STRING, sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? "���ݱ�[_rew]δ��������" : "���ݱ�[_ģ��_����]δ��������");
}

void Reward::RewItem(Player* player, uint32 itemId, uint32 itemCount)
{
	uint32 count = itemCount;
	uint32 noSpaceForCount = 0;
	ItemPosCountVec dest;
	InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemId, count, &noSpaceForCount);
	if (msg != EQUIP_ERR_OK)
		count -= noSpaceForCount;

	if (count == 0 || dest.empty())
	{}
	else
	{
		Item* item = player->StoreNewItem(dest, itemId, true, Item::GenerateItemRandomPropertyId(itemId));
		if (item)
			player->SendNewItem(item, count, true, false);
	}

	//bool add = player->AddItem(itemId, itemCount);

	itemCount -= count;

	if (itemCount > 0)
	{
		if (ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(itemId))
		{
			ChatHandler(player->GetSession()).PSendSysMessage("���յ����ʼ�:%s X %u", sCF->GetItemLink(itemId).c_str(), itemCount);
			MailSender sender(MAIL_NORMAL, 0, MAIL_STATIONERY_GM);
			MailDraft draft("��ʧ����Ʒ", "����з���������ʧ����Ʒ�����պá�");
			SQLTransaction trans = CharacterDatabase.BeginTransaction();

			uint32 stack = pProto->Stackable > 0 ? pProto->Stackable : 1;

			while (itemCount > stack)
			{
				if (Item* item = Item::CreateItem(itemId, stack))
				{
					item->SaveToDB(trans);
					draft.AddItem(item);
				}
				itemCount -= stack;
			}

			if (Item* item = Item::CreateItem(itemId, itemCount))
			{
				item->SaveToDB(trans);
				draft.AddItem(item);
			}

			draft.SendMailTo(trans, MailReceiver(NULL, player->GetGUIDLow()), sender);
			CharacterDatabase.CommitTransaction(trans);
		}
	}
}

void Reward::MailRew(Player* senderPlayer, uint32 receiverGuidLow, uint32 rewId, std::string titleText, std::string content)
{

	GlobalPlayerData const* receiverData = sWorld->GetGlobalPlayerData(receiverGuidLow);

	if (!receiverData)
		return;

	uint32 receiverAccId = receiverData->accountId;
	uint64 receiverGuid = MAKE_NEW_GUID(receiverGuidLow, 0, HIGHGUID_PLAYER);

	Player* receiver = ObjectAccessor::FindPlayerInOrOutOfWorld(receiverGuid);

	if (rewId == 0)
		return;

	std::unordered_map<uint32, RewTemplate>::iterator iter = RewMap.find(rewId);
	if (iter != RewMap.end())
	{
		uint32 xp = iter->second.xp;
		uint32 goldCount = iter->second.goldCount;
		uint32 tokenCount = iter->second.tokenCount;
		uint32 hrPoints = iter->second.hrPoints;
		uint32 arenaPoints = iter->second.arenaPoints;
		uint32 statPoints = iter->second.statPoints;

		if (tokenCount > 0)
		{
			if (receiver)
				sCF->UpdateTokenAmount(receiver, tokenCount, true, "��ļ����");
			else
				LoginDatabase.DirectPExecute("UPDATE account SET tokenAmount = tokenAmount +'%u' WHERE id = '%u'", tokenCount, receiverData->accountId);
		}
		if (arenaPoints > 0)
		{
			if (receiver)
				receiver->ModifyArenaPoints(arenaPoints);
			else
				CharacterDatabase.DirectPExecute("UPDATE characters SET arenaPoints = arenaPoints +'%u' WHERE guid = '%u'", arenaPoints, receiverGuidLow);
		}
		if (hrPoints > 0)
		{
			if (receiver)
				receiver->ModifyHonorPoints(hrPoints);
			else
				CharacterDatabase.DirectPExecute("UPDATE characters SET totalHonorPoints = totalHonorPoints +'%u' WHERE guid = '%u'", hrPoints, receiverGuidLow);
		}

		if (statPoints > 0)
		{
			if (receiver)
				receiver->stat_points[SPT_TOTLAL] += statPoints;
			else
				sStatPoints->UpdateDB(receiverGuidLow, statPoints);
		}

		MailSender sender(MAIL_NORMAL, senderPlayer->GetSession() ? senderPlayer->GetGUIDLow() : 0, MAIL_STATIONERY_GM);
		MailDraft draft(titleText, content);
		SQLTransaction trans = CharacterDatabase.BeginTransaction();

		if (goldCount > 0)
		{
			if (receiver)
				receiver->ModifyMoney(goldCount);
			else
				draft.AddMoney(goldCount);
		}

		for (auto itr = iter->second.ItemDataVec.begin(); itr != iter->second.ItemDataVec.end(); itr++)
		{
			ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(itr->itemId);
			uint32 itemCount = itr->itemCount;

			if (pProto && itemCount > 0)
			{
				uint32 stack = pProto->Stackable > 0 ? pProto->Stackable : 1;

				while (itemCount > stack)
				{
					if (Item* item = Item::CreateItem(itr->itemId, stack))
					{
						item->SaveToDB(trans);
						draft.AddItem(item);
					}
					itemCount -= stack;
				}

				if (Item* item = Item::CreateItem(itr->itemId, itemCount))
				{
					item->SaveToDB(trans);
					draft.AddItem(item);
				}
			}
		}

		draft.SendMailTo(trans, MailReceiver(NULL, receiverGuidLow), sender);
		CharacterDatabase.CommitTransaction(trans);

		if (receiver)
		{
			for (auto itr = iter->second.SpellDataVec.begin(); itr != iter->second.SpellDataVec.end(); itr++)
			{
				if (*itr < 0)
					continue;

				uint32 spellId = *itr;

				SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
				if (spellInfo)
				{
					uint32 firstSpellId = sSpellMgr->GetFirstSpellInChain(spellId);

					PlayerSpellMap spellMap = receiver->GetSpellMap();

					for (PlayerSpellMap::const_iterator iter = spellMap.begin(); iter != spellMap.end(); ++iter)
						if (firstSpellId == sSpellMgr->GetFirstSpellInChain(iter->first))
							receiver->removeSpell(iter->first, SPEC_MASK_ALL, false);

					receiver->learnSpell(spellId);
				}
			}

			for (auto itr = iter->second.SpellDataVec.begin(); itr != iter->second.SpellDataVec.end(); itr++)
			{
				if (*itr > 0)
					continue;

				receiver->CastSpell(receiver, abs(*itr), false);
			}

			for (auto itr = iter->second.CommandDataVec.begin(); itr != iter->second.CommandDataVec.end(); itr++)
				if (!itr->command.empty())
					sCustomCommand->DoCommand(receiver, itr->command);

			std::ostringstream oss;
			oss << "���յ����ʼ���" << titleText;
			receiver->GetSession()->SendAreaTriggerMessage(oss.str().c_str());
		}
	}
	else
		sWorld->SendServerMessage(SERVER_MSG_STRING, sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? "���ݱ�[_ģ��_����]δ��������" : "���ݱ�[_req]δ��������");
}

std::string Reward::GetDescription(uint32 rewId,bool quest)
{
	std::unordered_map<uint32, RewTemplate>::iterator iter = RewMap.find(rewId);
	if (iter != RewMap.end())
	{
		uint32 xp = iter->second.xp;
		uint32 goldCount = iter->second.goldCount;
		uint32 tokenCount = iter->second.tokenCount;
		uint32 hrPoints = iter->second.hrPoints;
		uint32 arenaPoints = iter->second.arenaPoints;
		uint32 statPoints = iter->second.statPoints;

		std::ostringstream oss;

		if (xp > 0)
			oss << "    |cFF0000FF[����]|r x |cFF0000FF" << xp << "|r\n";

		if (goldCount > 0)
			oss << "    |cFF0000FF[���]|r x |cFF0000FF" << goldCount / GOLD << "|r\n";

		if (tokenCount > 0)
			oss << "    |cFF0000FF[" << sString->GetText(CORE_STR_TYPES(STR_TOKEN)) << "]|r x |cFF0000FF" << tokenCount << "|r\n";

		if (hrPoints > 0)
			oss << "    |cFF0000FF[����]|r x |cFF0000FF" << hrPoints << "|r\n";

		if (arenaPoints > 0)
			oss << "    |cFF0000FF[������]|r x |cFF0000FF" << arenaPoints << "|r\n";

		if (statPoints > 0)
			oss << "    |cFF0000FF[������]|r x |cFF0000FF" << statPoints << "|r\n";


		for (auto itr = iter->second.ItemDataVec.begin(); itr != iter->second.ItemDataVec.end(); itr++)
			if(ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(itr->itemId))
				oss << "    |cFF0000FF[" << pProto->Name1 << "]|r x |cFF0000FF" << itr->itemCount << "|r\n";

		for (auto itr = iter->second.SpellDataVec.begin(); itr != iter->second.SpellDataVec.end(); itr++)
		{
			if (*itr < 0)
				continue;

			uint32 spellId = *itr;

			SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
			if (spellInfo)
				oss << "    |cFF0000FF[����] [" << spellInfo->SpellName[4] << "]|r\n";
		}

		for (auto itr = iter->second.SpellDataVec.begin(); itr != iter->second.SpellDataVec.end(); itr++)
		{
			if (*itr > 0)
				continue;

			SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(abs(*itr));
			if (spellInfo)
				oss << "    |cFF0000FF[����] [" << spellInfo->SpellName[4] << "]|r\n";
		}

		for (auto itr = iter->second.CommandDataVec.begin(); itr != iter->second.CommandDataVec.end(); itr++)
			if (!itr->command.empty())
				oss << "    |cFF0000FF[����] " << itr->des << "\n";

		return oss.str();
	}

	return "";
}


bool Reward::IsExist(uint32 rewId)
{
	std::unordered_map<uint32, RewTemplate>::iterator iter = RewMap.find(rewId);
	if (iter != RewMap.end())
		return true;
	return false;
}

std::string Reward::GetAnounceText(uint32 rewId)
{
	std::unordered_map<uint32, RewTemplate>::iterator iter = RewMap.find(rewId);
	if (iter != RewMap.end())
	{

		std::ostringstream oss;

		if (iter->second.goldCount > 0)
			oss << "|cFFFFCC00[���]|rx|cFFFFCC00" << iter->second.goldCount << " ";
		if (iter->second.tokenCount > 0)
			oss << "|cFFFFCC00[" << sString->GetText(CORE_STR_TYPES(STR_TOKEN)) << "]|rx|cFFFFCC00" << iter->second.tokenCount << " ";
		if (iter->second.xp > 0)
			oss << "|cFFFFCC00[����]|rx|cFFFFCC00" << iter->second.xp << " ";
		if (iter->second.hrPoints > 0)
			oss << "|cFFFFCC00[����]|rx|cFFFFCC00" << iter->second.hrPoints << " ";

		if (iter->second.arenaPoints > 0)
			oss << "|cFFFFCC00[������]|rx|cFFFFCC00" << iter->second.arenaPoints << " ";

		if (iter->second.statPoints > 0)
			oss << "|cFFFFCC00[������]|rx|cFFFFCC00" << iter->second.statPoints << " ";

		for (auto itr = iter->second.ItemDataVec.begin(); itr != iter->second.ItemDataVec.end(); itr++)
			oss << "" << sCF->GetItemLink(itr->itemId) << "x|cFFFFCC00" << itr->itemCount << " ";
		
		for (auto itr = iter->second.SpellDataVec.begin(); itr != iter->second.SpellDataVec.end(); itr++)
		{
			int32 id = *itr;
			if (id < 0)
				continue;

			oss << "ѧ�Ἴ��|cFFFFCC00[" << sSpellMgr->GetSpellInfo(abs(id))->SpellName[4] << "] ";
		}

		for (auto itr = iter->second.SpellDataVec.begin(); itr != iter->second.SpellDataVec.end(); itr++)
		{
			int32 id = *itr;
			if (id > 0)
				continue;

			oss << "�������|cFFFFCC00[" << sSpellMgr->GetSpellInfo(abs(id))->SpellName[4] << "] ";
		}

		oss << "|r";

		return oss.str();
	}
	
	
	return "";
}
