#pragma execution_character_set("utf-8")
#include "FixedTimeBG.h"
#include "BattlegroundMgr.h"
#include "Battleground.h"
#include "../../CommonFunc/CommonFunc.h"
#include "../Event.h"
#include "../../DataLoader/DataLoader.h"
#include "../../Reward/Reward.h"
#include "../../Requirement/Requirement.h"

std::unordered_map<BattlegroundTypeId, FixedTimeBGTemplate> FixedTimeBGMap;

void FixedTimeBG::Load()
{
	FixedTimeBGMap.clear();
	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		//			0		1			2				3			4					5				6					7				8			9		10
		"SELECT 战场类型,胜利奖励模板ID, 失败奖励模板ID,结束击杀人数,获得奖励最小击杀数,获得奖励最小被杀数,获得奖励最小伤害值,获得奖励最小治疗值,资源或旗帜,是否跨阵营,最低血量,"
		//11				12			13			14			15			16		17
		"随机光环间隔时间, 随机光环ID1,随机光环ID2,随机光环ID3,随机光环ID4,随机光环ID5,战场名称 FROM __战场" :
		//			0		1			2		3		4		5			6		7		8	9	10
		"SELECT bgTypeId,winRewId, losRewId,endKills,RewKills,RewdKilleds,RewDmg,RewHeal,maxRes,cf,limitHP,"
		//11					12			13			14			15			16			17
		"RandomBuffInterval, RandomBuff1,RandomBuff2,RandomBuff3,RandomBuff4,RandomBuff5,bgName FROM _battleground");
	if (!result)
	{
		return;
	}
	do
	{
		Field* fields = result->Fetch();

		BattlegroundTypeId bgTypeId = BattlegroundTypeId(fields[0].GetUInt32());

		FixedTimeBGTemplate FixedTimeBGTemp;
		FixedTimeBGTemp.winRewardTemplateId		= fields[1].GetUInt32();
		FixedTimeBGTemp.loseRewardTemplateId	= fields[2].GetUInt32();
		FixedTimeBGTemp.killsForEnd				= fields[3].GetUInt32();
		FixedTimeBGTemp.killsForReward			= fields[4].GetUInt32();
		FixedTimeBGTemp.killedsForReward		= fields[5].GetUInt32();
		FixedTimeBGTemp.damageForReward			= fields[6].GetUInt32();
		FixedTimeBGTemp.healForReward			= fields[7].GetUInt32();
		FixedTimeBGTemp.resourcesMax			= fields[8].GetUInt32();
		FixedTimeBGTemp.crossFaction			= fields[9].GetBool();
		FixedTimeBGTemp.limitHP					= fields[10].GetUInt32();
		FixedTimeBGTemp.RandomBuffInterval		= fields[11].GetUInt32();

		for (size_t i = 0; i < 5; i++)
			FixedTimeBGTemp.randomBuffs[i]		= fields[12 + i].GetUInt32();

		FixedTimeBGTemp.bgName					= fields[17].GetString();

		FixedTimeBGMap.insert(std::make_pair(bgTypeId, FixedTimeBGTemp));
	} while (result->NextRow());
}


uint32 FixedTimeBG::GetLimitHP(BattlegroundTypeId bgTypeId)
{
	std::unordered_map<BattlegroundTypeId, FixedTimeBGTemplate>::iterator iter = FixedTimeBGMap.find(bgTypeId);

	if (iter != FixedTimeBGMap.end())
		return iter->second.limitHP;

	return 0;
}

uint32 FixedTimeBG::GetMaxRes(BattlegroundTypeId bgTypeId)
{
	std::unordered_map<BattlegroundTypeId, FixedTimeBGTemplate>::iterator iter = FixedTimeBGMap.find(bgTypeId);

	if (iter != FixedTimeBGMap.end())
		return iter->second.resourcesMax;

	return 0;
}
bool FixedTimeBG::GetCFFlag(BattlegroundTypeId bgTypeId)
{
	std::unordered_map<BattlegroundTypeId, FixedTimeBGTemplate>::iterator iter = FixedTimeBGMap.find(bgTypeId);

	if (iter != FixedTimeBGMap.end())
		return iter->second.crossFaction;

	return false;
}

uint32 FixedTimeBG::GetWinRewId(BattlegroundTypeId bgTypeId)
{
	std::unordered_map<BattlegroundTypeId, FixedTimeBGTemplate>::iterator iter = FixedTimeBGMap.find(bgTypeId);

	if (iter != FixedTimeBGMap.end())
		return iter->second.winRewardTemplateId;

	return 0;
}
uint32 FixedTimeBG::GetLosRewId(BattlegroundTypeId bgTypeId)
{
	std::unordered_map<BattlegroundTypeId, FixedTimeBGTemplate>::iterator iter = FixedTimeBGMap.find(bgTypeId);

	if (iter != FixedTimeBGMap.end())
		return iter->second.loseRewardTemplateId;

	return 0;
}
uint32 FixedTimeBG::GetEndKilss(BattlegroundTypeId bgTypeId)
{
	std::unordered_map<BattlegroundTypeId, FixedTimeBGTemplate>::iterator iter = FixedTimeBGMap.find(bgTypeId);

	if (iter != FixedTimeBGMap.end())
		return iter->second.killsForEnd;

	return 0;
}
uint32 FixedTimeBG::GetRewHeal(BattlegroundTypeId bgTypeId)
{
	std::unordered_map<BattlegroundTypeId, FixedTimeBGTemplate>::iterator iter = FixedTimeBGMap.find(bgTypeId);

	if (iter != FixedTimeBGMap.end())
		return iter->second.healForReward;

	return 0;
}
uint32 FixedTimeBG::GetRewDmg(BattlegroundTypeId bgTypeId)
{
	std::unordered_map<BattlegroundTypeId, FixedTimeBGTemplate>::iterator iter = FixedTimeBGMap.find(bgTypeId);

	if (iter != FixedTimeBGMap.end())
		return iter->second.damageForReward;

	return 0;
}
uint32 FixedTimeBG::GetRewKills(BattlegroundTypeId bgTypeId)
{
	std::unordered_map<BattlegroundTypeId, FixedTimeBGTemplate>::iterator iter = FixedTimeBGMap.find(bgTypeId);

	if (iter != FixedTimeBGMap.end())
		return iter->second.killsForReward;

	return 0;
}
uint32 FixedTimeBG::GetRewKilleds(BattlegroundTypeId bgTypeId)
{
	std::unordered_map<BattlegroundTypeId, FixedTimeBGTemplate>::iterator iter = FixedTimeBGMap.find(bgTypeId);

	if (iter != FixedTimeBGMap.end())
		return iter->second.killedsForReward;

	return 0;
}

void FixedTimeBG::RewardPlayer(Player* player, BattlegroundTypeId bgTypeId, bool win)
{
	if (win)
		sRew->Rew(player, GetWinRewId(bgTypeId));
	else
		sRew->Rew(player, GetLosRewId(bgTypeId));
}

uint32 FixedTimeBG::GetRandomBuffInterval(BattlegroundTypeId bgTypeId)
{
	std::unordered_map<BattlegroundTypeId, FixedTimeBGTemplate>::iterator iter = FixedTimeBGMap.find(bgTypeId);

	if (iter != FixedTimeBGMap.end())
		return iter->second.RandomBuffInterval * IN_MILLISECONDS;

	return 0;
}

void FixedTimeBG::RemoveRandomBuff(Battleground* bg, Player* player)
{
	BattlegroundTypeId bgTypeId = bg->GetBgTypeID();

	std::unordered_map<BattlegroundTypeId, FixedTimeBGTemplate>::iterator iter = FixedTimeBGMap.find(bgTypeId);

	if (iter != FixedTimeBGMap.end())
		for (size_t i = 0; i < 5; i++)
			player->RemoveAura(iter->second.randomBuffs[i]);
}

void FixedTimeBG::AddRandomBuff(Battleground* bg, uint32 diff)
{
	bg->RandomBuffInterval -= diff;

	if (bg->RandomBuffInterval <= 0)
	{
		BattlegroundTypeId bgTypeId = bg->GetBgTypeID();

		bg->RandomBuffInterval = GetRandomBuffInterval(bgTypeId);

		if (bg->RandomBuffInterval == 0)
			return;

		std::unordered_map<BattlegroundTypeId, FixedTimeBGTemplate>::iterator iter = FixedTimeBGMap.find(bgTypeId);

		if (iter != FixedTimeBGMap.end())
		{
			std::vector<uint32> buffVec;

			for (size_t i = 0; i < 5; i++)
				if (const SpellInfo* spell = sSpellMgr->GetSpellInfo(iter->second.randomBuffs[i]))
					buffVec.push_back(iter->second.randomBuffs[i]);

			if (buffVec.empty())
				return;

			uint32 spellId = buffVec[urand(0, buffVec.size() - 1)];

			std::ostringstream oss;

			if (const SpellInfo* spell = sSpellMgr->GetSpellInfo(spellId))
				oss << iter->second.bgName << " 所有玩家获得随机效果 " << spell->SpellName[4];

			Battleground::BattlegroundPlayerMap const& pl = bg->GetPlayers();
			for (Battleground::BattlegroundPlayerMap::const_iterator itr = pl.begin(); itr != pl.end(); ++itr)
			{
				Player* player = itr->second;

				if (!player || !player->IsInWorld())
					continue;

				for (size_t i = 0; i < buffVec.size(); i++)
					player->RemoveAura(buffVec[i]);

				player->AddAura(spellId, player);

				player->GetSession()->SendNotification(oss.str().c_str());
			}
		}
	}
}
