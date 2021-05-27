#pragma execution_character_set("utf-8")
#include "Faction.h"
#include "../CommonFunc/CommonFunc.h"
#include "../Reward/Reward.h"
#include "../Requirement/Requirement.h"
#include "../Switch/Switch.h"
#include "../String/myString.h"
#include "../CharNameMod/CharNameMod.h"
#include "../FunctionCollection/FunctionCollection.h"
#include "../CustomEvent/Event.h"
#include "../MainFunc/MainFunc.h"

std::map<uint32, FactionTemplate> FactionDataMap;

void Faction::Load()
{
	FactionDataMap.clear();

	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		//		0				1				2		3					4		5
		"SELECT 自定义阵营ID, 自定义阵营名称, 菜单文本, 加入阵营奖励模板ID, 名字后缀,退出阵营需求模板ID FROM __自定义阵营" :
		//		0			1		2		3		4		5
		"SELECT faction, name, gossipText, rewId, suffix,quitReqId FROM _faction");
	
	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			uint32 faction = fields[0].GetUInt32();

			FactionTemplate Temp;
			Temp.name = fields[1].GetString();
			Temp.gossipText = fields[2].GetString();
			Temp.rewId = fields[3].GetUInt32();
			Temp.suffix = fields[4].GetString();
			Temp.quitReqId = fields[5].GetUInt32();
			FactionDataMap.insert(std::make_pair(faction, Temp));

		} while (result->NextRow());
	}
}

std::string Faction::GetName(uint32 faction)
{
	std::map<uint32, FactionTemplate>::iterator iter = FactionDataMap.find(faction);

	if (iter != FactionDataMap.end())
		return iter->second.name;

	return "[无]";
}

std::string Faction::GetGosstipText(uint32 faction)
{
	std::map<uint32, FactionTemplate>::iterator iter = FactionDataMap.find(faction);

	if (iter != FactionDataMap.end())
		return iter->second.gossipText;

	return "";
}

uint32 Faction::GetRewId(uint32 faction)
{
	std::map<uint32, FactionTemplate>::iterator iter = FactionDataMap.find(faction);

	if (iter != FactionDataMap.end())
		return iter->second.rewId;

	return 0;
}

uint32 Faction::GetReqId(uint32 faction)
{
	std::map<uint32, FactionTemplate>::iterator iter = FactionDataMap.find(faction);

	if (iter != FactionDataMap.end())
		return iter->second.quitReqId;

	return 0;
}

std::string Faction::GetSuffix(uint32 faction)
{
	std::map<uint32, FactionTemplate>::iterator iter = FactionDataMap.find(faction);

	if (iter != FactionDataMap.end())
		return iter->second.suffix;

	return "";
}

void Faction::Join(Player* player, uint32 faction)
{
	if (player->InEvent())
	{
		ChatHandler(player->GetSession()).PSendSysMessage("事件中不能加入门派！");
		return;
	}
	
	if (faction == 0)
	{
		//退出
		uint32 reqId = GetReqId(player->faction);

		if (!sReq->Check(player, reqId))
			return;

		sReq->Des(player, reqId);
	}
	else
	{
		//加入
		uint32 rewId = GetRewId(faction);
		sRew->Rew(player, rewId);
	}

	player->faction = faction;	
	CharacterDatabase.PExecute("UPDATE characters SET faction = %u WHERE guid = %u", faction, player->GetGUIDLow());
	sCharNameMod->UpdateSuffix(player, GetSuffix(faction));
}


void Faction::AddGossip(Player* player, Object* obj)
{
	std::map<uint32, FactionTemplate>::iterator iter = FactionDataMap.find(player->faction);

	if (iter != FactionDataMap.end())
	{
		std::string text = "|cff0033FF" + iter->second.name + "|r";
		player->ADD_GOSSIP_ITEM(0, text, SENDER_FACTION, GOSSIP_ACTION_INFO_DEF + iter->first);
	}	

	for (std::map<uint32, FactionTemplate>::iterator iter = FactionDataMap.begin(); iter != FactionDataMap.end(); iter++)
	{
		if (iter->first != player->faction)
		{
			std::string text = iter->second.name;

			if (player->faction != 0) 
				text = "|cffFF0000" + iter->second.name + "|r";

			player->ADD_GOSSIP_ITEM(0, text, SENDER_FACTION, GOSSIP_ACTION_INFO_DEF + iter->first);
		}	
	}

	player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, obj->GetGUID());
}

void Faction::Action(Player*player, uint32 action, Object*obj)
{
	if (action > 2 * GOSSIP_ACTION_INFO_DEF)
	{
		uint32 faction = action - 2 * GOSSIP_ACTION_INFO_DEF;

		if (faction == player->faction)
		{
			Join(player, 0);
			ChatHandler(player->GetSession()).PSendSysMessage("退出%s", GetName(faction).c_str());
		}
		else
		{
			Join(player, faction);
			ChatHandler(player->GetSession()).PSendSysMessage("加入%s", GetName(faction).c_str());
		}

		player->CLOSE_GOSSIP_MENU();
	}
	else if (action > GOSSIP_ACTION_INFO_DEF)
	{
		uint32 faction = action - GOSSIP_ACTION_INFO_DEF;
		std::string name = GetName(faction);

		player->ADD_GOSSIP_ITEM(0, GetGosstipText(faction), SENDER_FACTION, GOSSIP_ACTION_INFO_DEF);

		if (player->faction == faction)
		{
			std::string gossipText = ">> 退出" + name;
			std::string noticeText = "确定退出" + name + "吗？";
			player->ADD_GOSSIP_ITEM_EXTENDED(0, gossipText, SENDER_FACTION, 2 * GOSSIP_ACTION_INFO_DEF + player->faction, noticeText, 0, false);
		}
		else if (player->faction == 0)
		{
			std::string gossipText = ">> 加入" + name;
			std::string noticeText = "确定加入" + name + "吗？";
			player->ADD_GOSSIP_ITEM_EXTENDED(0, gossipText, SENDER_FACTION, 2 * GOSSIP_ACTION_INFO_DEF + action - GOSSIP_ACTION_INFO_DEF, noticeText, 0, false);
		}
			
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, obj->GetGUID());
	}
	else
		sMF->AddGossip(player, obj, 0);
}

class FactionScript : PlayerScript
{
public:
	FactionScript() : PlayerScript("FactionScript") {}

	void OnLogin(Player* player)
	{
		if (QueryResult result = CharacterDatabase.PQuery("SELECT faction FROM characters WHERE guid = %u", player->GetGUIDLow()))
			player->faction = result->Fetch()[0].GetUInt32();
	}
};

void AddSC_FactionScript()
{
	new FactionScript();
}
