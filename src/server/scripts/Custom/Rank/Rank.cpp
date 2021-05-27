#pragma execution_character_set("utf-8")
#include "Rank.h"
#include "../CommonFunc/CommonFunc.h"
#include "../Reward/Reward.h"
#include "../Requirement/Requirement.h"
#include "../Switch/Switch.h"
#include "../String/myString.h"
#include "../CharNameMod/CharNameMod.h"
#include "../FunctionCollection/FunctionCollection.h"
#include "../GCAddon/GCAddon.h"
#include "../MapMod/MapMod.h"

std::map<uint32, RankTemplate> RankDataMap;


void Rank::Load()
{
	RankDataMap.clear();
	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 等级,升级需达到的值,名称,菜单文本,升级奖励模板ID,玩家名字前缀 FROM __自定义等级" :
		"SELECT level,meetValue,name,gossipText,rewId,prefix FROM _rank");
	if (!result) return;
	do
	{
		Field* fields = result->Fetch();
		uint32 level = fields[0].GetUInt32();

		RankTemplate Temp;
		Temp.meetValue			= fields[1].GetUInt32();
		Temp.name				= fields[2].GetString();
		Temp.gossipText			= fields[3].GetString();
		Temp.rewId				= fields[4].GetUInt32();
		Temp.prefix				= fields[5].GetString();
		RankDataMap.insert(std::make_pair(level, Temp));
	} while (result->NextRow());

}

uint32 Rank::GetMeetValue(uint32 level)
{
	std::map<uint32, RankTemplate>::iterator iter = RankDataMap.find(level);

	if (iter != RankDataMap.end())
		return iter->second.meetValue;

	return 0;
}

std::string Rank::GetName(uint32 level)
{
	std::map<uint32, RankTemplate>::iterator iter = RankDataMap.find(level);

	if (iter != RankDataMap.end())
		return iter->second.name;

	return "[无]";
}

std::string Rank::GetGosstipText(uint32 level)
{
	std::map<uint32, RankTemplate>::iterator iter = RankDataMap.find(level);

	if (iter != RankDataMap.end())
		return iter->second.gossipText;

	return "";
}

uint32 Rank::GetRewId(uint32 level)
{
	std::map<uint32, RankTemplate>::iterator iter = RankDataMap.find(level);

	if (iter != RankDataMap.end())
		return iter->second.rewId;

	return 0;
}
std::string Rank::GetPrefix(uint32 level)
{
	std::map<uint32, RankTemplate>::iterator iter = RankDataMap.find(level);

	if (iter != RankDataMap.end())
		return iter->second.prefix;

	return "";
}

bool Rank::AccountBind()
{
	return sSwitch->GetOnOff(ST_RANK_ACCOUNT_BIND);
}

uint32 Rank::GetMaxLevel()
{
	return RankDataMap.size();
}

void Rank::SendPacket(Player* player)
{
	std::ostringstream oss;
	oss << player->rankValue << " ";
	oss << player->maxRankValue << " ";
	oss << GetName(player->rankLevel);
	sGCAddon->SendPacketTo(player, "GC_S_RANKVALUE", oss.str());
}

void Rank::Update(Player* player, uint32 value/* = 0*/, bool updateDB/* = false*/)
{
	if (player->rankLevel >= GetMaxLevel())
		return;
		
	player->rankValue += value;
	player->maxRankValue = GetMeetValue(player->rankLevel);

	if (player->rankValue > player->maxRankValue)
	{
		player->rankLevel++;
		ChatHandler(player->GetSession()).PSendSysMessage("|cffFF0000[修炼系统]|r 进阶%s", GetName(player->rankLevel).c_str());
		sRew->Rew(player, GetRewId(player->rankLevel));
		sMapMod->OnEnterMap(player);

		player->rankValue = player->rankValue - player->maxRankValue;
		player->maxRankValue = GetMeetValue(player->rankLevel);

		//更新前缀
		sCharNameMod->UpdatePrefix(player, GetPrefix(player->rankLevel));

		if (player->rankValue > player->maxRankValue)
			Update(player, 0, updateDB);
	}

	if (value > 0)
		SendPacket(player);

	if (updateDB)
		UpdateDBValue(player);
}

void Rank::UpdateDBValue(Player* player)
{
	if (AccountBind())
	{
		PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_RANKVALUE);
		stmt->setUInt32(0, player->GetSession()->GetAccountId());
		stmt->setUInt32(1, player->rankLevel);
		stmt->setUInt32(2, player->rankValue);
		LoginDatabase.Execute(stmt);
	}
	else
	{
		PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_RANKVALUE);
		stmt->setUInt32(0, player->GetGUIDLow());
		stmt->setUInt32(1, player->rankLevel);
		stmt->setUInt32(2, player->rankValue);
		CharacterDatabase.Execute(stmt);
	}	
}

void Rank::GetDBValue(Player* player, uint32 &rankLevel, uint32 &rankValue)
{
	if (AccountBind())
	{
		if (QueryResult result = LoginDatabase.PQuery("SELECT level, value FROM account_rank WHERE account = %u", player->GetSession()->GetAccountId()))
		{
			Field* fields = result->Fetch();
			rankLevel = fields[0].GetUInt32();
			rankValue = fields[1].GetUInt32();
		}		
	}
	else
	{
		if (QueryResult result = CharacterDatabase.PQuery("SELECT level, value FROM character_rank WHERE guid = %u", player->GetGUIDLow()))
		{
			Field* fields = result->Fetch();
			rankLevel = fields[0].GetUInt32();
			rankValue = fields[1].GetUInt32();
		}
	}
}

void Rank::AddGossip(Player* player, Object* obj)
{
	for (std::map<uint32, RankTemplate>::iterator iter = RankDataMap.begin(); iter != RankDataMap.end(); iter++)
	{
		std::string text = "";

		if (iter->first > player->rankLevel)
			text = "|cffFF0000" + GetName(iter->first) + "|r";
		else if (iter->first == player->rankLevel)
			text = "|cff0033FF" + GetName(iter->first) + "|r";
		else
			text = GetName(iter->first);

		player->ADD_GOSSIP_ITEM(0, text, SENDER_RANK, GOSSIP_ACTION_INFO_DEF + iter->first);
	}

	if (obj->ToCreature())
		player->SEND_GOSSIP_MENU(obj->GetEntry(), obj->GetGUID());
	else
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, obj->GetGUID());
}

void Rank::Action(Player*player, uint32 action, Object*obj)
{
	if (action == GOSSIP_ACTION_INFO_DEF)
		AddGossip(player, obj);
	else
	{
		player->ADD_GOSSIP_ITEM(0, GetGosstipText(action - GOSSIP_ACTION_INFO_DEF), SENDER_RANK, GOSSIP_ACTION_INFO_DEF);
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, obj->GetGUID());
	}
}

class RankScript : PlayerScript
{
public:
	RankScript() : PlayerScript("RankScript") {}

	void OnLogin(Player* player) 
	{ 
		uint32 rankLevel = 1;
		uint32 rankValue = 1;

		sRank->GetDBValue(player, rankLevel, rankValue);

		player->rankLevel = rankLevel;
		player->rankValue = rankValue;
		player->maxRankValue = sRank->GetMeetValue(player->rankLevel);

		uint32 maxRankLevel = sRank->GetMaxLevel();

		if (player->rankLevel >= maxRankLevel)
		{
			player->rankValue = 100;
			player->maxRankValue = 100;	
		}

		sCharNameMod->UpdatePrefix(player, sRank->GetPrefix(player->rankLevel));

		sGCAddon->SendCharData(player);

		sMapMod->OnEnterMap(player);
	}

	void OnLogout(Player* player) 
	{ 
		sRank->UpdateDBValue(player);
	}
};

void AddSC_RankScript()
{
	new RankScript();
}
