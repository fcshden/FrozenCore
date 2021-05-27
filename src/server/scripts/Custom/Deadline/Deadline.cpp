#pragma execution_character_set("utf-8")
#include "Deadline.h"
#include "Group.h"
#include "../Reward/Reward.h"
#include "../String/myString.h"
#include "../Switch/Switch.h"

std::vector<DeadlineTemplate> DeadlineVec;

void Deadline::Load()
{
	DeadlineVec.clear();
	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 地图ID,时间,通关奖励模板ID,通关需击杀生物ID,副本挑战等级,事件状态ID,通关召唤物体ID,副本难度 FROM _副本_限时模式" :
		"SELECT mapId,countDown,rewId,killedEntry,challengeLv,worldStat,gobEntry,diff FROM _deadline");

	if (!result) return;
	do
	{
		Field* fields = result->Fetch();
		DeadlineTemplate Temp;
		Temp.mapId			= fields[0].GetUInt32();
		Temp.countDown		= fields[1].GetUInt32()  * MINUTE * IN_MILLISECONDS;
		Temp.rewId			= fields[2].GetUInt32();
		Temp.killedEntry	= fields[3].GetUInt32();
		Temp.challengeLv	= fields[4].GetInt32();
		Temp.worldStat		= fields[5].GetUInt32();
		Temp.gobEntry		= fields[6].GetUInt32();
		Temp.diff			= fields[7].GetUInt8();
		DeadlineVec.push_back(Temp);
	} while (result->NextRow());
}

void Deadline::SetActive(Map* map)
{
	//if (map->GetDifficulty() != DUNGEON_DIFFICULTY_HEROIC)
		//return;

	//已激活 或 已失败
	if (map->deadlineActive || map->deadlineFailed)
		return;

	for (std::vector<DeadlineTemplate>::iterator iter = DeadlineVec.begin(); iter != DeadlineVec.end(); ++iter)
		if (iter->mapId == map->GetId() && iter->challengeLv == map->challengeLv && map->GetDifficulty() == iter->diff)
		{
			map->countDown = iter->countDown;
			map->deadlineActive = true;
			Announce(map, sString->GetText(CORE_STR_TYPES(STR_DEADLINE_START)));
			SetWorldState(map, map->countDown / (MINUTE * IN_MILLISECONDS));
			break;
		}
}

void Deadline::Announce(Map* map, const char* text)
{
	Map::PlayerList const &players = map->GetPlayers();
	for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
	{
		Player* player = i->GetSource();
		if (!player)
			continue;

		ChatHandler(player->GetSession()).PSendSysMessage(text);
	}
}

void Deadline::Reward(Player* killer)
{
	uint32 rewId = 0;
	uint32 mapId = killer->GetMapId();
	Map* map = killer->GetMap();

	if (!map)
		return;

	for (std::vector<DeadlineTemplate>::iterator iter = DeadlineVec.begin(); iter != DeadlineVec.end(); ++iter)
		if (iter->mapId == mapId && killer->GetMap()->challengeLv == iter->challengeLv && map->GetDifficulty() == iter->diff)
		{
			rewId = iter->rewId;
			break;
		}

	if (rewId == 0)
		return;

	Group* group = killer->GetGroup();
	if (group)
	{
		for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
			if (Player* member = itr->GetSource())
				if (member->IsInWorld())
					sRew->Rew(member, rewId);
	}
	else
		sRew->Rew(killer, rewId);
}

void Deadline::SetWorldState(Map* map, uint32 value)
{
	uint32 worldStatType = 0;

	for (std::vector<DeadlineTemplate>::iterator iter = DeadlineVec.begin(); iter != DeadlineVec.end(); ++iter)
		if (iter->mapId == map->GetId() && map->challengeLv == iter->challengeLv && map->GetDifficulty() == iter->diff)
		{
			worldStatType = iter->worldStat;
			break;
		}

	if (worldStatType == 0)
		return;

	Map::PlayerList const &players = map->GetPlayers();
	for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
	{
		Player* player = i->GetSource();
		if (!player)
			continue;

		player->SendUpdateWorldState(worldStatType, value);
	}
}

class DeadlineKill : PlayerScript
{
public:
	DeadlineKill() : PlayerScript("DeadlineKill") {}

	void OnCreatureKill(Player* killer, Creature* killed) override
	{
		if (killer->GetMap()->deadlineFailed)
			return;

		for (std::vector<DeadlineTemplate>::iterator iter = DeadlineVec.begin(); iter != DeadlineVec.end(); ++iter)
			if (killed->GetEntry() == iter->killedEntry && killer->GetMap()->challengeLv == iter->challengeLv)
			{
				sDeadline->Reward(killer);
				killer->SummonGameObject(iter->gobEntry, killed->GetPositionX(), killed->GetPositionY(), killed->GetPositionZ(), killed->GetOrientation(), 0, 0, 0, 0, 0);
				sDeadline->Announce(killer->GetMap(), sString->GetText(CORE_STR_TYPES(STR_DEADLINE_SUCCESS)));
				break;
			}
	}

	void OnMapChanged(Player* player) 
	{
		Map* map = player->GetMap();

		if (!map || map->countDown == 0)
			return;

		if (map->countDown < 0)
		{
			sDeadline->Announce(map, sString->GetText(CORE_STR_TYPES(STR_DEADLINE_FAILED)));
			sDeadline->SetWorldState(map, 0);
		}
		else
		{
			uint32 leftTime = map->countDown / (60 * IN_MILLISECONDS) + 1;
			sDeadline->SetWorldState(map, leftTime);

			if (sSwitch->GetOnOff(ST_DEADLINE_TIME_NOTICE))
				sDeadline->Announce(map, sString->Format(sString->GetText(CORE_STR_TYPES(STR_DEADLINE_TIME_LEFT)), leftTime));
		}	
	}
};

void AddSC_DeadLine()
{
	new DeadlineKill();
}
