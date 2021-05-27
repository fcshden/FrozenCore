#pragma execution_character_set("utf-8")
#include "TalentReq.h"
#include "../Requirement/Requirement.h"
#include "../CommonFunc/CommonFunc.h"

std::unordered_map<uint32, uint32>TalentReqMap;

void TalentReq::Load()
{
	TalentReqMap.clear();
	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ?
		"SELECT 天赋技能ID,需求模板ID from _技能_天赋加点需求" :
		"SELECT spellid,reqId from _talent_req");
	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			TalentReqMap.insert(std::make_pair(fields[0].GetUInt32(), fields[1].GetUInt32()));
		} while (result->NextRow());
	}
}

bool TalentReq::SendAcceptOrCancel(Player* player, uint32 spellid)
{
	if (TalentReqMap.empty())
		return false;

	auto i = TalentReqMap.find(spellid);

	if (i == TalentReqMap.end())
		return false;

	if (std::find(player->BuyTalentVec.begin(), player->BuyTalentVec.end(), spellid) != player->BuyTalentVec.end())
		return false;

	const SpellInfo* spellInfo = sSpellMgr->GetSpellInfo(spellid);
	if (!spellInfo)
		return false;

	std::ostringstream oss;
	oss << "[" << spellInfo->SpellName[4] << "]";
	sCF->SendAcceptOrCancel(player, 995, sReq->Notice(player, i->second, "学习", oss.str()));

	return true;
}


void TalentReq::DoAction(Player* player)
{
	uint32 reqId = 0;

	auto i = TalentReqMap.find(player->buy_talentSpell);

	if (i != TalentReqMap.end())
		reqId = i->second;

	if (sReq->Check(player, reqId))
	{
		player->LearnTalentCustom(player->buy_talentId, player->buy_talentRank);
		player->SendTalentsInfoData(false);
		player->BuyTalentVec.push_back(player->buy_talentSpell);
		CharacterDatabase.PExecute("REPLACE INTO characters_talent_req (guid, spellid) VALUES (%u, %u)", player->GetGUIDLow(), player->buy_talentSpell);
		sReq->Des(player, reqId);
		player->buy_talentId = 0;
		player->buy_talentRank = 0;
		player->buy_talentSpell = 0;
	}
}

class TalentReqScript : PlayerScript
{
public:
	TalentReqScript() : PlayerScript("TalentReqScript") {}
	void OnLogin(Player* player) override
	{
		player->BuyTalentVec.clear();

		if (QueryResult result = CharacterDatabase.PQuery("SELECT spellid FROM characters_talent_req WHERE guid = '%u'", player->GetGUIDLow()))
		{
			do
			{
				Field* fields = result->Fetch();
				player->BuyTalentVec.push_back(fields[0].GetUInt32());
			} while (result->NextRow());
		}
	}
};

void AddSC_TalentReq()
{
	new TalentReqScript();
}
