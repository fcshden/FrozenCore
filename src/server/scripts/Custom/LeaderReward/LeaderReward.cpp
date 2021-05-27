#pragma execution_character_set("utf-8")
#include "LeaderReward.h"
#include "../CommonFunc/CommonFunc.h"
#include "Group.h"
#include "../Reward/Reward.h"

std::vector<LeaderRewardTemplate> LeaderRewardVec;
std::vector<LearderDataTemplate> LeaderDataVec;

void LeaderReward::Load()
{
	return;
	LeaderRewardVec.clear();
	QueryResult result = WorldDatabase.PQuery("SELECT mapIdOrZoneId,isInstance,bossInfo,rewId,distance,nearbyPlayers,rewType,diff,ipMaxCount FROM _leader_reward");
	if (!result) return;
	do
	{
		Field* fields = result->Fetch();
		LeaderRewardTemplate Temp;
		Temp.mapIdOrZoneId	= fields[0].GetUInt32();
		Temp.isInstance		= fields[1].GetBool();

		std::string bossInfo = fields[2].GetString();
		std::vector<std::string> strBossInfo = sCF->SplitStr(bossInfo, "|");
		for (size_t j = 0; j < strBossInfo.size(); j++)
			Temp.bossInfo.push_back(atoi(strBossInfo[j].c_str()));

		Temp.rewId			= fields[3].GetUInt32();
		Temp.range			= fields[4].GetUInt32();
		Temp.nearbyPlayers	= fields[5].GetUInt32();
		Temp.rewType		= fields[6].GetUInt32();
		Temp.diff			= fields[7].GetUInt8();
		Temp.ipMaxCount		= fields[8].GetUInt32();
		LeaderRewardVec.push_back(Temp);
	} while (result->NextRow());
}

void LeaderReward::GetLeaderInfo(uint32 entry,uint32 mapIdOrZoneId, bool isInstance, uint8 diff, std::vector<uint32/*bossId*/> &bossVec, uint32 &rewId, uint32 &range, uint32 &nearbyPlayers, uint32 &rewType, uint32 &ipMaxCount)
{
	uint32 len = LeaderRewardVec.size();
	for (size_t i = 0; i < len; i++)
	{
		if (mapIdOrZoneId == LeaderRewardVec[i].mapIdOrZoneId && isInstance == LeaderRewardVec[i].isInstance && diff == LeaderRewardVec[i].diff)
		{
			bossVec = LeaderRewardVec[i].bossInfo;

			if (std::find(bossVec.begin(), bossVec.end(), entry) == bossVec.end())
			{
				bossVec.clear();
				continue;
			}

			rewId = LeaderRewardVec[i].rewId;
			range = LeaderRewardVec[i].range;
			nearbyPlayers = LeaderRewardVec[i].nearbyPlayers;
			rewType = LeaderRewardVec[i].rewType;
			ipMaxCount = LeaderRewardVec[i].ipMaxCount;

			return;
		}
	}
}

uint32 LeaderReward::GetNearbyPlayers(Player* leader, float range)
{
	Group* group = leader->GetGroup();

	if (!group)
		return 0;

	uint32 count = 0;

	for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
		if (Player* member = itr->GetSource())
			if (member->GetDistance2d(leader) <= range)
				count += 1;
	
	return count - 1;
}

uint32 LeaderReward::GetSameIpCount(Player* leader)
{
	Group* group = leader->GetGroup();

	if (!group)
		return 1;

	std::vector<std::string> ipVec;

	for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
		if (Player* member = itr->GetSource())
		{
			std::string ipAdress = member->GetSession()->GetRemoteAddress();
				ipVec.push_back(ipAdress);
		}

	uint32 max = 0;

	for (std::vector<std::string>::iterator iter = ipVec.begin(); iter != ipVec.end(); ++iter)
	{
		uint32 count = std::count(ipVec.begin(), ipVec.end(), (*iter));

		if (count > max)
			max = count;
	}

	return max;
}

void LeaderReward::GetBossInfoVec(uint32 mapIdOrZoneId,bool isInstance,uint8 diff, uint32 entry, std::vector<uint32> &bossinfovec)
{
	for (std::vector<LeaderRewardTemplate>::iterator iLeaderRewardVec = LeaderRewardVec.begin(); iLeaderRewardVec != LeaderRewardVec.end(); ++iLeaderRewardVec)
	{
		if (mapIdOrZoneId != iLeaderRewardVec->mapIdOrZoneId || isInstance != iLeaderRewardVec->isInstance || diff != iLeaderRewardVec->diff)
			continue;

		for (std::vector<uint32>::iterator ii = iLeaderRewardVec->bossInfo.begin(); ii != iLeaderRewardVec->bossInfo.end(); ++ii)
			if (entry == (*ii))
			{
				bossinfovec = iLeaderRewardVec->bossInfo;
				break;
			}
	}
		
}

void LeaderReward::GetPlayerBossIdVec(uint32 guid, bool isInstance, uint8 diff, uint32 mapIdOrZoneId, std::vector<uint32> &playerbossidvec)
{
	for (std::vector<LearderDataTemplate>::iterator i = LeaderDataVec.begin(); i != LeaderDataVec.end(); ++i)
	{
		if (guid == i->guid && diff == i->diff && isInstance == i->isInstance)
		{
			playerbossidvec.push_back(i->bossId);
		}
	}
}

bool LeaderReward::CheckLeaderData(Player* killer, Creature* boss)
{
	bool isInstance = killer->GetMap()->Instanceable();
	uint8 diff = killer->GetMap()->GetDifficulty();
	uint32 mapIdOrZoneId = 0;

	if (isInstance)
		mapIdOrZoneId = killer->GetMapId();
	else
		mapIdOrZoneId = killer->GetZoneId();


	Group* group = killer->GetGroup();

	uint32 entry = boss->GetEntry();

	Player* leader;

	if (group)
		leader = ObjectAccessor::FindPlayerInOrOutOfWorld(group->GetLeaderGUID());
	else
		leader = killer;


	std::vector<uint32/*bossId*/> bossVec;
	uint32 rewId = 0;
	uint32 range = 0;
	uint32 nearbyPlayers = 0;
	uint32 rewType = 0;
	uint32 ipMaxCount = 0;

	GetLeaderInfo(entry,mapIdOrZoneId, isInstance, diff, bossVec, rewId, range, nearbyPlayers, rewType, ipMaxCount);
	
	//该地图或区域不在表中
	if (bossVec.empty() || rewId == 0)
		return false;

	bool isLegalBoss = false;

	for (std::vector<uint32>::iterator i = bossVec.begin(); i != bossVec.end(); ++i)
		if (entry == (*i))
			isLegalBoss = true;

	if (!isLegalBoss)
		return false;

	uint32 count = GetNearbyPlayers(leader, range);
	uint32 ipCount = GetSameIpCount(leader);

	if (count < nearbyPlayers && group)
	{
		ChatHandler(leader->GetSession()).PSendSysMessage("共有 %d 人在 %d 码范围内，需要 %d 人，将无法获得奖励", count, range, nearbyPlayers);
		return false;
	}

	if (ipCount > ipMaxCount && group)
	{
		ChatHandler(leader->GetSession()).PSendSysMessage("共有 %d 人同一IP，超过限制 %d，将无法获得奖励", ipCount, ipMaxCount);
		return false;
	}

	std::vector<uint32> bossinfovec;
	GetBossInfoVec(mapIdOrZoneId, isInstance, diff, entry, bossinfovec);
	std::vector<uint32> playerbossidvec;
	GetPlayerBossIdVec(leader->GetGUIDLow(), isInstance, diff, mapIdOrZoneId, playerbossidvec);

	bool exsist = false;

	for (std::vector<uint32>::iterator iter = playerbossidvec.begin(); iter != playerbossidvec.end(); ++iter)
		if (entry == (*iter))
			exsist = true;

	if (!exsist)
	{
		LearderDataTemplate Temp;
		Temp.guid = leader->GetGUIDLow();
		Temp.mapIdOrZoneId = mapIdOrZoneId;
		Temp.isInstance = isInstance;
		Temp.diff = diff;
		Temp.bossId = entry;
		LeaderDataVec.push_back(Temp);
	}	

	GetPlayerBossIdVec(leader->GetGUIDLow(), isInstance, diff, mapIdOrZoneId, playerbossidvec);

	bool allKillFlag = true;

	for (std::vector<uint32>::iterator iter = bossinfovec.begin(); iter != bossinfovec.end(); ++iter)
	{
		
		if (std::find(playerbossidvec.begin(), playerbossidvec.end(), (*iter)) == playerbossidvec.end())
		{
			allKillFlag = false;
			break;
		}
			
	}


	if (!allKillFlag)
		return false;

	if (rewType == 0)
		sRew->Rew(leader, rewId);
	else
	{
		if (group)
			for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
				if (Player* member = itr->GetSource())
					sRew->Rew(member, rewId);
	}

	//奖励之后清空
	for (std::vector<LearderDataTemplate>::iterator iter = LeaderDataVec.begin(); iter != LeaderDataVec.end(); ++iter)
	{
		if (leader->GetGUIDLow() == iter->guid && mapIdOrZoneId == iter->mapIdOrZoneId && isInstance == iter->isInstance && diff == iter->diff)
		{
			if (std::find(bossinfovec.begin(), bossinfovec.end(), iter->bossId) != bossinfovec.end())
				iter->bossId = 0;
		}
	}


	for (std::vector<LearderDataTemplate>::iterator iter = LeaderDataVec.begin(); iter != LeaderDataVec.end();)
		if (iter->bossId == 0)
			iter = LeaderDataVec.erase(iter);
		else
			++iter;

	return true;
}

class LeaderRewardPlayerScript : PlayerScript
{
public:
	LeaderRewardPlayerScript() : PlayerScript("LeaderRewardPlayerScript") {}

	void OnCreatureKill(Player* killer, Creature* boss) override
	{
		sLeaderReward->CheckLeaderData(killer, boss);
	}
};
void AddSC_LeaderReward()
{
	//new LeaderRewardPlayerScript();
}
