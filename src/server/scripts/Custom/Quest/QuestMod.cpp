#pragma execution_character_set("utf-8")
#include "../CommonFunc/CommonFunc.h"
#include "../FunctionCollection/FunctionCollection.h"
#include "../Reward/Reward.h"
#include "../Requirement/Requirement.h"
#include "QuestMod.h"
#include "../CustomEvent/Event.h"
#include "Language.h"

std::unordered_map<uint32, QuestModTemplate> QuestModMap;
std::unordered_map<uint32, float> QuestRandomMap;

void QuestMod::Load()
{
	QuestModMap.clear();
	QuestRandomMap.clear();

	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ?
		//			0	1				2					3			4				5				6				7			8				9		10				11
		"SELECT 任务ID,接受任务需求模板ID,是否消耗需求, 任务传送坐标ID, 随机奖励模板ID1,随机奖励模板ID2,随机奖励模板ID3,随机奖励模板ID4,随机奖励几率1,随机奖励几率2,随机奖励几率3,随机奖励几率4 FROM _任务" :
		//			0	1		2				3			4				5		6		7			8		9			10			11
		"SELECT questId,reqId,desReq,telePosId,randomRewId1,randomRewId2,randomRewId3,randomRewId4,rewChance1,rewChance2,rewChance3,rewChance4 FROM _quest_mod");

	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			QuestModTemplate QuestModTemp;
			uint32 questId			= fields[0].GetUInt32();
			QuestModTemp.reqId		= fields[1].GetUInt32();
			QuestModTemp.desReq		= fields[2].GetBool();
			QuestModTemp.telePosId	= fields[3].GetUInt32();

			for (size_t i = 0; i < MAX_QUEST_MOD_REW_COUNT; i++)
				QuestModTemp.randomRewId[i] = fields[4 + i].GetUInt32();

			for (size_t i = 0; i < MAX_QUEST_MOD_REW_COUNT; i++)
				QuestModTemp.rewChance[i] = fields[8 + i].GetUInt32();
			
			QuestModMap.insert(std::make_pair(questId, QuestModTemp));
		} while (result->NextRow());
	}

	result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ?
		"SELECT 任务ID,接到几率 FROM _任务_随机配置" :
		"SELECT questId,chance FROM _quest_random");

	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			uint32 questId = fields[0].GetUInt32();
			float chance = fields[1].GetFloat();
			Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
			if (!quest)
				continue;
			QuestRandomMap.insert(std::make_pair(questId, chance));
		} while (result->NextRow());
	}
}

void QuestMod::AddTeleGossip(Player* player, Object* obj)
{
	for (auto itr = QuestModMap.begin(); itr != QuestModMap.end(); itr++)
	{
		Quest const* quest = sObjectMgr->GetQuestTemplate(itr->first);

		if (quest && player->IsActiveQuest(itr->first) && itr->second.telePosId != 0)
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, quest->GetTitle(), SENDER_QUEST_TELE, quest->GetQuestId());
	}

	if (obj->ToCreature())
		player->SEND_GOSSIP_MENU(obj->GetEntry(), obj->GetGUID());
	else
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, obj->GetGUID());
}


void QuestMod::Tele(Player* player, uint32 questId)
{
	auto itr = QuestModMap.find(questId);

	if (itr != QuestModMap.end())
	{
		auto it = PosMap.find(itr->second.telePosId);

		if (it != PosMap.end())
			player->TeleportTo(it->second.map, it->second.x, it->second.y, it->second.z, it->second.o);
	}
}

bool QuestMod::CanTakeQuest(Player* player, uint32 questId)
{
	auto itr = QuestModMap.find(questId);

	if (itr != QuestModMap.end())
	{
		if (sReq->Check(player, itr->second.reqId))
		{
			if (itr->second.desReq)
				sReq->Des(player, itr->second.reqId);

			return true;
		}
		else
			return false;
	}

	return true;
}

void QuestMod::RandomReward(Player* player, uint32 questId)
{
	auto itr = QuestModMap.find(questId);

	if (itr != QuestModMap.end())
	{
		for (size_t i = 0; i < MAX_QUEST_MOD_REW_COUNT; i++)
			if (CanRandomReward(itr->second.rewChance[i]))
				sRew->Rew(player, itr->second.randomRewId[i]);
	}
}

bool QuestMod::CanRandomReward(uint32 chance)
{
	if (urand(1, 100) <= chance)
		return true;

	return false;
}


std::string QuestMod::GetExtraDes(uint32 questId, Player* player)
{
	auto itr = QuestModMap.find(questId);

	if (itr != QuestModMap.end())
	{
		std::ostringstream oss;

		if (sReq->IsExist(itr->second.reqId))
		{
			if (itr->second.desReq)
				oss << "\n\n|cFF660066「接受任务需要满足并消耗」|r\n\n";
			else
				oss << "\n\n|cFF660066「接受任务需要满足」|r\n\n";

			oss << sReq->GetDescription(itr->second.reqId, player);
		}

		for (size_t i = 0; i < MAX_QUEST_MOD_REW_COUNT; i++)
		{
			if (sRew->IsExist(itr->second.randomRewId[i]) && itr->second.rewChance[i] > 0)
			{
				oss << "\n\n|cFF0000FF「完成任务有";
				oss << itr->second.rewChance[i];
				oss << "%几率获得」|r\n\n";
				oss << sRew->GetDescription(itr->second.randomRewId[i], true);
			}
		}

		return oss.str();
	}

	return "";
}

bool QuestMod::AddRandomQuest(Player* player)
{
	if (QuestRandomMap.empty())
	{
		sLog->outString("_quest_random未配置随机任务");
		return false;
	}


	std::unordered_map<uint32, float> randomMap;

	uint32 count = 1;

	for (auto it = QuestRandomMap.begin(); it != QuestRandomMap.end(); it++)
	{
		QuestStatusMap::iterator itr = player->getQuestStatusMap().find(it->first);

		if (itr != player->getQuestStatusMap().end())
		{
			if (itr->second.Status != QUEST_STATUS_REWARDED)
				count++;
			else
				randomMap.insert(std::make_pair(it->first, it->second));
		}
		else
			randomMap.insert(std::make_pair(it->first, it->second));
	}

	std::ostringstream oss;

	if (count > MAX_RANDOM_QUEST_COUNT)
	{
		oss << "你不能领取更多随机任务 (" << MAX_RANDOM_QUEST_COUNT << "/" << MAX_RANDOM_QUEST_COUNT << ")";
		player->GetSession()->SendNotification(oss.str().c_str());
		return false;
	}

	uint32 questId = 0;

	std::vector<std::pair<int, float>> vtMap;
	for (auto it = randomMap.begin(); it != randomMap.end(); it++)
		vtMap.push_back(std::make_pair(it->first, it->second));

	sort(vtMap.begin(), vtMap.end(),
		[](const std::pair<int, float> &x, const std::pair<int, float> &y) -> int {
		return x.second < y.second;
	});

	float sum = 0;

	for (auto it = vtMap.begin(); it != vtMap.end(); it++)
		sum += it->second;

	float rand = frand(0, sum);

	sum = 0;

	for (auto it = vtMap.begin(); it != vtMap.end(); it++)
	{
		sum += it->second;
		if (rand < sum)
		{
			questId = it->first;
			break;
		}
	}

	//uint32 questId = filterVec[urand(0, filterVec.size() - 1)];

	Quest const* quest = sObjectMgr->GetQuestTemplate(questId);

	if (!quest)
		return false;

	ItemTemplateContainer const* itc = sObjectMgr->GetItemTemplateStore();
	ItemTemplateContainer::const_iterator result = find_if(itc->begin(), itc->end(), Finder<uint32, ItemTemplate>(questId, &ItemTemplate::StartQuest));

	if (result != itc->end())
	{
		ChatHandler(player->GetSession()).PSendSysMessage(LANG_COMMAND_QUEST_STARTFROMITEM, questId, result->second.ItemId);
		return false;
	}

	if (player->CanAddQuest(quest, true))
		player->AddQuestAndCheckCompletion(quest, NULL);

	oss.str("");
	oss << "领取随机任务 [" << quest->GetTitle() << "] (" << count << "/" << MAX_RANDOM_QUEST_COUNT << ")";
	player->GetSession()->SendAreaTriggerMessage(oss.str().c_str());

	return true;
}

void QuestMod::AddAllRandomQuest(Player* player)
{
	for (size_t i = 0; i < MAX_RANDOM_QUEST_COUNT; i++)
		if (!AddRandomQuest(player))
			break;
}
