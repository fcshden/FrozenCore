#pragma execution_character_set("utf-8")
#include "HonorRank.h"
#include "../DataLoader/DataLoader.h"
#include "../CommonFunc/CommonFunc.h"
#include "../Reward/Reward.h"
#include "../Requirement/Requirement.h"
#include "../Switch/Switch.h"
#include "../String/myString.h"
#include "../FunctionCollection/FunctionCollection.h"

std::vector<HRUpTemplate> HRUpVec;
void HonorRank::Load()
{
	HRUpVec.clear();
	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		//		0		1				2				3			4						5							6					7						8				
		"SELECT 军衔ID,升级需求模板ID,升级后奖励模板ID,掉落倍率,物品升级模式0增加成功百分比,物品升级模式1增加成功百分比,物品强化增加成功百分比,符印强化增加成功百分比,移除宝石增加成功百分比 FROM __军衔" :
		//		0		1	2		3		4				5				6			7				8
		"SELECT title,reqId,rewId,lootRate,exchangeRate_0,exchangeRate_1,strengthenRate,sigilRate,removeGemRate FROM _hr");
	if (!result) return;
	do
	{
		Field* fields = result->Fetch();
		HRUpTemplate HRUpTemp;
		HRUpTemp.title				= fields[0].GetUInt32();
		HRUpTemp.reqId				= fields[1].GetUInt32();
		HRUpTemp.rewId				= fields[2].GetUInt32();
		HRUpTemp.lootRate			= fields[3].GetFloat();
		HRUpTemp.exchangeRate_0		= fields[4].GetUInt32();
		HRUpTemp.exchangeRate_1		= fields[5].GetUInt32();
		HRUpTemp.strengthenRate		= fields[6].GetUInt32();
		HRUpTemp.sigilRate			= fields[7].GetUInt32();
		HRUpTemp.removeGemRate		= fields[8].GetUInt32();
		HRUpVec.push_back(HRUpTemp);
	} while (result->NextRow());
	
}
uint32 HonorRank::GetHRRank(Player* player)
{
	if (player->HasTitle(14) || player->HasTitle(28)) return 14;
	else if (player->HasTitle(13) || player->HasTitle(27)) return 13;
	else if (player->HasTitle(12) || player->HasTitle(26)) return 12;
	else if (player->HasTitle(11) || player->HasTitle(25)) return 11;
	else if (player->HasTitle(10) || player->HasTitle(24)) return 10;
	else if (player->HasTitle(9) || player->HasTitle(23)) return 9;
	else if (player->HasTitle(8) || player->HasTitle(22)) return 8;
	else if (player->HasTitle(7) || player->HasTitle(21)) return 7;
	else if (player->HasTitle(6) || player->HasTitle(20)) return 6;
	else if (player->HasTitle(5) || player->HasTitle(19)) return 5;
	else if (player->HasTitle(4) || player->HasTitle(18)) return 4;
	else if (player->HasTitle(3) || player->HasTitle(17)) return 3;
	else if (player->HasTitle(2) || player->HasTitle(16)) return 2;
	else if (player->HasTitle(1) || player->HasTitle(15)) return 1;
	else return 0;
}

uint32 HonorRank::GetHRTiteId(Player* player)
{
	if (player->GetTeamId() == TEAM_ALLIANCE)
	{
		if (player->HasTitle(14)) return 14;
		else if (player->HasTitle(13)) return 13;
		else if (player->HasTitle(12)) return 12;
		else if (player->HasTitle(11)) return 11;
		else if (player->HasTitle(10)) return 10;
		else if (player->HasTitle(9)) return 9;
		else if (player->HasTitle(8)) return 8;
		else if (player->HasTitle(7)) return 7;
		else if (player->HasTitle(6)) return 6;
		else if (player->HasTitle(5)) return 5;
		else if (player->HasTitle(4)) return 4;
		else if (player->HasTitle(3)) return 3;
		else if (player->HasTitle(2)) return 2;
		else if (player->HasTitle(1)) return 1;
		else return 0;
	}
	else
	{
		if (player->HasTitle(28)) return 28;
		else if (player->HasTitle(27)) return 27;
		else if (player->HasTitle(26)) return 26;
		else if (player->HasTitle(25)) return 25;
		else if (player->HasTitle(24)) return 24;
		else if (player->HasTitle(23)) return 23;
		else if (player->HasTitle(22)) return 22;
		else if (player->HasTitle(21)) return 21;
		else if (player->HasTitle(20)) return 20;
		else if (player->HasTitle(19)) return 19;
		else if (player->HasTitle(18)) return 18;
		else if (player->HasTitle(17)) return 17;
		else if (player->HasTitle(16)) return 16;
		else if (player->HasTitle(15)) return 15;
		else return 0;
	}
	
}
void HonorRank::AddCurrHRMenu(Player* player)
{
	std::string hr_name = "";
	if (player->GetTeamId() == TEAM_ALLIANCE)
	{
		if (player->HasTitle(14))		hr_name = "|TInterface/ICONS/Achievement_PVP_O_14:30:30:0:0|t当前的军阶【大元帅】";
		else if (player->HasTitle(13))	hr_name = "|TInterface/ICONS/Achievement_PVP_O_13:30:30:0:0|t当前的军阶【元帅】";
		else if (player->HasTitle(12))	hr_name = "|TInterface/ICONS/Achievement_PVP_O_12:30:30:0:0|t当前的军阶【统帅】";
		else if (player->HasTitle(11))	hr_name = "|TInterface/ICONS/Achievement_PVP_O_11:30:30:0:0|t当前的军阶【司令】";
		else if (player->HasTitle(10))	hr_name = "|TInterface/ICONS/Achievement_PVP_O_10:30:30:0:0|t当前的军阶【少校】";
		else if (player->HasTitle(9))	hr_name = "|TInterface/ICONS/Achievement_PVP_O_09:30:30:0:0|t当前的军阶【护卫骑士】";
		else if (player->HasTitle(8))	hr_name = "|TInterface/ICONS/Achievement_PVP_O_08:30:30:0:0|t当前的军阶【骑士队长】";
		else if (player->HasTitle(7))	hr_name = "|TInterface/ICONS/Achievement_PVP_O_07:30:30:0:0|t当前的军阶【骑士中尉】";
		else if (player->HasTitle(6))	hr_name = "|TInterface/ICONS/Achievement_PVP_O_06:30:30:0:0|t当前的军阶【骑士】";
		else if (player->HasTitle(5))	hr_name = "|TInterface/ICONS/Achievement_PVP_O_05:30:30:0:0|t当前的军阶【士官长】";
		else if (player->HasTitle(4))	hr_name = "|TInterface/ICONS/Achievement_PVP_O_04:30:30:0:0|t当前的军阶【军士长】";
		else if (player->HasTitle(3))	hr_name = "|TInterface/ICONS/Achievement_PVP_O_03:30:30:0:0|t当前的军阶【中士】";
		else if (player->HasTitle(2))	hr_name = "|TInterface/ICONS/Achievement_PVP_O_02:30:30:0:0|t当前的军阶【下士】";
		else if (player->HasTitle(1))	hr_name = "|TInterface/ICONS/Achievement_PVP_O_01:30:30:0:0|t当前的军阶【列兵】";
	}
	else
	{
		if (player->HasTitle(28)) hr_name = "|TInterface/ICONS/Achievement_PVP_P_14:30:30:0:0|t当前的军阶【高阶督军】";
		else if (player->HasTitle(27)) hr_name = "|TInterface/ICONS/Achievement_PVP_P_13:30:30:0:0|t当前的军阶【督军】";
		else if (player->HasTitle(26)) hr_name = "|TInterface/ICONS/Achievement_PVP_P_12:30:30:0:0|t当前的军阶【将军】";
		else if (player->HasTitle(25)) hr_name = "|TInterface/ICONS/Achievement_PVP_P_11:30:30:0:0|t当前的军阶【中将】";
		else if (player->HasTitle(24)) hr_name = "|TInterface/ICONS/Achievement_PVP_P_10:30:30:0:0|t当前的军阶【勇士】";
		else if (player->HasTitle(23)) hr_name = "|TInterface/ICONS/Achievement_PVP_P_09:30:30:0:0|t当前的军阶【百夫长】";
		else if (player->HasTitle(22)) hr_name = "|TInterface/ICONS/Achievement_PVP_P_08:30:30:0:0|t当前的军阶【军团士兵】";
		else if (player->HasTitle(21)) hr_name = "|TInterface/ICONS/Achievement_PVP_P_07:30:30:0:0|t当前的军阶【血卫士】";
		else if (player->HasTitle(20)) hr_name = "|TInterface/ICONS/Achievement_PVP_P_06:30:30:0:0|t当前的军阶【石头守卫】";
		else if (player->HasTitle(19)) hr_name = "|TInterface/ICONS/Achievement_PVP_P_05:30:30:0:0|t当前的军阶【一等军士长】";
		else if (player->HasTitle(18)) hr_name = "|TInterface/ICONS/Achievement_PVP_P_04:30:30:0:0|t当前的军阶【高阶军士】";
		else if (player->HasTitle(17)) hr_name = "|TInterface/ICONS/Achievement_PVP_P_03:30:30:0:0|t当前的军阶【中士】";
		else if (player->HasTitle(16)) hr_name = "|TInterface/ICONS/Achievement_PVP_P_02:30:30:0:0|t当前的军阶【步兵】";
		else if (player->HasTitle(15)) hr_name = "|TInterface/ICONS/Achievement_PVP_P_01:30:30:0:0|t当前的军阶【斥候】";
	}
	if (player->HasTitle(1) || player->HasTitle(15))
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, hr_name, SENDER_HR_MENU, 0);
}
std::string HonorRank::GetCurrHR(Player* player)
{
	std::string hr_name = "";
	if (player->GetTeamId() == TEAM_ALLIANCE)
	{
		if (player->HasTitle(14))		hr_name = "|TInterface/ICONS/Achievement_PVP_O_14:30:30:0:0|t当前的军阶【大元帅】";
		else if (player->HasTitle(13))	hr_name = "|TInterface/ICONS/Achievement_PVP_O_13:30:30:0:0|t当前的军阶【元帅】";
		else if (player->HasTitle(12))	hr_name = "|TInterface/ICONS/Achievement_PVP_O_12:30:30:0:0|t当前的军阶【统帅】";
		else if (player->HasTitle(11))	hr_name = "|TInterface/ICONS/Achievement_PVP_O_11:30:30:0:0|t当前的军阶【司令】";
		else if (player->HasTitle(10))	hr_name = "|TInterface/ICONS/Achievement_PVP_O_10:30:30:0:0|t当前的军阶【少校】";
		else if (player->HasTitle(9))	hr_name = "|TInterface/ICONS/Achievement_PVP_O_09:30:30:0:0|t当前的军阶【护卫骑士】";
		else if (player->HasTitle(8))	hr_name = "|TInterface/ICONS/Achievement_PVP_O_08:30:30:0:0|t当前的军阶【骑士队长】";
		else if (player->HasTitle(7))	hr_name = "|TInterface/ICONS/Achievement_PVP_O_07:30:30:0:0|t当前的军阶【骑士中尉】";
		else if (player->HasTitle(6))	hr_name = "|TInterface/ICONS/Achievement_PVP_O_06:30:30:0:0|t当前的军阶【骑士】";
		else if (player->HasTitle(5))	hr_name = "|TInterface/ICONS/Achievement_PVP_O_05:30:30:0:0|t当前的军阶【士官长】";
		else if (player->HasTitle(4))	hr_name = "|TInterface/ICONS/Achievement_PVP_O_04:30:30:0:0|t当前的军阶【军士长】";
		else if (player->HasTitle(3))	hr_name = "|TInterface/ICONS/Achievement_PVP_O_03:30:30:0:0|t当前的军阶【中士】";
		else if (player->HasTitle(2))	hr_name = "|TInterface/ICONS/Achievement_PVP_O_02:30:30:0:0|t当前的军阶【下士】";
		else if (player->HasTitle(1))	hr_name = "|TInterface/ICONS/Achievement_PVP_O_01:30:30:0:0|t当前的军阶【列兵】";
	}
	else
	{
		if (player->HasTitle(28)) hr_name = "|TInterface/ICONS/Achievement_PVP_P_14:30:30:0:0|t当前的军阶【高阶督军】";
		else if (player->HasTitle(27)) hr_name = "|TInterface/ICONS/Achievement_PVP_P_13:30:30:0:0|t当前的军阶【督军】";
		else if (player->HasTitle(26)) hr_name = "|TInterface/ICONS/Achievement_PVP_P_12:30:30:0:0|t当前的军阶【将军】";
		else if (player->HasTitle(25)) hr_name = "|TInterface/ICONS/Achievement_PVP_P_11:30:30:0:0|t当前的军阶【中将】";
		else if (player->HasTitle(24)) hr_name = "|TInterface/ICONS/Achievement_PVP_P_10:30:30:0:0|t当前的军阶【勇士】";
		else if (player->HasTitle(23)) hr_name = "|TInterface/ICONS/Achievement_PVP_P_09:30:30:0:0|t当前的军阶【百夫长】";
		else if (player->HasTitle(22)) hr_name = "|TInterface/ICONS/Achievement_PVP_P_08:30:30:0:0|t当前的军阶【军团士兵】";
		else if (player->HasTitle(21)) hr_name = "|TInterface/ICONS/Achievement_PVP_P_07:30:30:0:0|t当前的军阶【血卫士】";
		else if (player->HasTitle(20)) hr_name = "|TInterface/ICONS/Achievement_PVP_P_06:30:30:0:0|t当前的军阶【石头守卫】";
		else if (player->HasTitle(19)) hr_name = "|TInterface/ICONS/Achievement_PVP_P_05:30:30:0:0|t当前的军阶【一等军士长】";
		else if (player->HasTitle(18)) hr_name = "|TInterface/ICONS/Achievement_PVP_P_04:30:30:0:0|t当前的军阶【高阶军士】";
		else if (player->HasTitle(17)) hr_name = "|TInterface/ICONS/Achievement_PVP_P_03:30:30:0:0|t当前的军阶【中士】";
		else if (player->HasTitle(16)) hr_name = "|TInterface/ICONS/Achievement_PVP_P_02:30:30:0:0|t当前的军阶【步兵】";
		else if (player->HasTitle(15)) hr_name = "|TInterface/ICONS/Achievement_PVP_P_01:30:30:0:0|t当前的军阶【斥候】";
	}
	return hr_name;
}

uint32 HonorRank::GetReqId(uint32 title)
{
	for (size_t i = 0; i < HRUpVec.size(); i++)
		if (title == HRUpVec[i].title)
			return HRUpVec[i].reqId;

	return 0;
}

uint32 HonorRank::GetRewId(uint32 title)
{
	for (size_t i = 0; i < HRUpVec.size(); i++)
		if (title == HRUpVec[i].title)
			return HRUpVec[i].rewId;

	return 0;
}
std::string GetGossipText(uint32 title)
{
	switch (title)
	{
	case 1:
		return "|TInterface/ICONS/Achievement_PVP_O_01:30:30:0:0|t升级后军阶【列兵】";	
	case 2:
		return "|TInterface/ICONS/Achievement_PVP_O_02:30:30:0:0|t升级后军阶【下士】";		
	case 3:
		return "|TInterface/ICONS/Achievement_PVP_O_03:30:30:0:0|t升级后军阶【中士】";	
	case 4:		
		return "|TInterface/ICONS/Achievement_PVP_O_04:30:30:0:0|t升级后军阶【军士长】";	
	case 5:
		return "|TInterface/ICONS/Achievement_PVP_O_05:30:30:0:0|t升级后军阶【士官长】";	
	case 6:
		return "|TInterface/ICONS/Achievement_PVP_O_06:30:30:0:0|t升级后军阶【骑士】";	
	case 7:
		return "|TInterface/ICONS/Achievement_PVP_O_07:30:30:0:0|t升级后军阶【骑士中尉】";
	case 8:
		return "|TInterface/ICONS/Achievement_PVP_O_08:30:30:0:0|t升级后军阶【骑士队长】";	
	case 9:
		return "|TInterface/ICONS/Achievement_PVP_O_09:30:30:0:0|t升级后军阶【护卫骑士】";
	case 10:
		return "|TInterface/ICONS/Achievement_PVP_O_10:30:30:0:0|t升级后军阶【少校】";	
	case 11:
		return "|TInterface/ICONS/Achievement_PVP_O_11:30:30:0:0|t升级后军阶【司令】";
	case 12:
		return "|TInterface/ICONS/Achievement_PVP_O_12:30:30:0:0|t升级后军阶【统帅】";	
	case 13:
		return "|TInterface/ICONS/Achievement_PVP_O_13:30:30:0:0|t升级后军阶【元帅】";
	case 14:
		return "|TInterface/ICONS/Achievement_PVP_O_14:30:30:0:0|t升级后军阶【大元帅】";
	case 15:
		return "|TInterface/ICONS/Achievement_PVP_P_01:30:30:0:0|t升级后军阶【斥候】";
	case 16:
		return "|TInterface/ICONS/Achievement_PVP_P_02:30:30:0:0|t升级后军阶【步兵】";
	case 17:
		return "|TInterface/ICONS/Achievement_PVP_P_03:30:30:0:0|t升级后军阶【中士】";
	case 18:
		return "|TInterface/ICONS/Achievement_PVP_P_04:30:30:0:0|t升级后军阶【高阶军士】";
	case 19:
		return "|TInterface/ICONS/Achievement_PVP_P_05:30:30:0:0|t升级后军阶【一等军士长】";
	case 20:
		return "|TInterface/ICONS/Achievement_PVP_P_06:30:30:0:0|t升级后军阶【石头守卫】";
	case 21:
		return "|TInterface/ICONS/Achievement_PVP_P_07:30:30:0:0|t升级后军阶【血卫士】";
	case 22:
		return "|TInterface/ICONS/Achievement_PVP_P_08:30:30:0:0|t升级后军阶【军团士兵】";
	case 23:
		return "|TInterface/ICONS/Achievement_PVP_P_09:30:30:0:0|t升级后军阶【百夫长】";
	case 24:
		return "|TInterface/ICONS/Achievement_PVP_P_10:30:30:0:0|t升级后军阶【勇士】";
	case 25:
		return "|TInterface/ICONS/Achievement_PVP_P_11:30:30:0:0|t升级后军阶【中将】";
	case 26:
		return "|TInterface/ICONS/Achievement_PVP_P_12:30:30:0:0|t升级后军阶【将军】";
	case 27:
		return "|TInterface/ICONS/Achievement_PVP_P_13:30:30:0:0|t升级后军阶【督军】";
	case 28:
		return "|TInterface/ICONS/Achievement_PVP_P_14:30:30:0:0|t升级后军阶【高阶督军】";
	default:
		return "";
	}
}

void HonorRank::AddNextHRMenu(Player* player, Object* obj)
{
	AddCurrHRMenu(player);

	uint32 title = 0;

	if (player->GetTeamId() == TEAM_ALLIANCE)
	{
		if (!player->HasTitle(1))				title = 1;
		else    if (!player->HasTitle(2))   	title = 2;
		else	if (!player->HasTitle(3))   	title = 3;
		else	if (!player->HasTitle(4))   	title = 4;
		else	if (!player->HasTitle(5))   	title = 5;
		else	if (!player->HasTitle(6))   	title = 6;
		else	if (!player->HasTitle(7))   	title = 7;
		else	if (!player->HasTitle(8))   	title = 8;
		else	if (!player->HasTitle(9))   	title = 9;
		else	if (!player->HasTitle(10))  	title = 10;
		else	if (!player->HasTitle(11))  	title = 11;
		else	if (!player->HasTitle(12))  	title = 12;
		else	if (!player->HasTitle(13))  	title = 13;
		else	if (!player->HasTitle(14))  	title = 14;
	}
	else
	{
		if (!player->HasTitle(15))				title = 15;
		else    if (!player->HasTitle(16))   	title = 16;
		else	if (!player->HasTitle(17))   	title = 17;
		else	if (!player->HasTitle(18))   	title = 18;
		else	if (!player->HasTitle(19))   	title = 19;
		else	if (!player->HasTitle(20))   	title = 20;
		else	if (!player->HasTitle(21))   	title = 21;
		else	if (!player->HasTitle(22))   	title = 22;
		else	if (!player->HasTitle(23))   	title = 23;
		else	if (!player->HasTitle(24))  	title = 24;
		else	if (!player->HasTitle(25))  	title = 25;
		else	if (!player->HasTitle(26))  	title = 26;
		else	if (!player->HasTitle(27))  	title = 27;
		else	if (!player->HasTitle(28))  	title = 28;
	}

	if (title != 0 && !GetGossipText(title).empty())
		player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, GetGossipText(title), SENDER_HR_UP, title, sReq->Notice(player, GetReqId(title), GetGossipText(title), ""), sReq->Golds(GetReqId(title)), false);

	if (obj->ToCreature())
		player->SEND_GOSSIP_MENU(obj->GetEntry(), obj->GetGUID());
	else
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, obj->GetGUID());
}

std::string HonorRank::GetHRTitle(Player* player)
{
	std::string hr = "";

	if (player->GetTeamId() == TEAM_ALLIANCE)
	{
		if (player->HasTitle(14)) hr = "大元帅";
		else if (player->HasTitle(13)) hr = "元帅";
		else if (player->HasTitle(12)) hr = "统帅";
		else if (player->HasTitle(11)) hr = "司令";
		else if (player->HasTitle(10)) hr = "少校";
		else if (player->HasTitle(9)) hr = "护卫骑士";
		else if (player->HasTitle(8)) hr = "骑士队长";
		else if (player->HasTitle(7)) hr = "骑士中尉";
		else if (player->HasTitle(6)) hr = "骑士";
		else if (player->HasTitle(5)) hr = "士官长";
		else if (player->HasTitle(4)) hr = "军士长";
		else if (player->HasTitle(3)) hr = "中士";
		else if (player->HasTitle(2)) hr = "下士";
		else hr = "列兵";
	}
	else
	{
		if (player->HasTitle(28)) hr = "高阶督军";
		else if (player->HasTitle(27)) hr = "督军";
		else if (player->HasTitle(26)) hr = "将军";
		else if (player->HasTitle(25)) hr = "中将";
		else if (player->HasTitle(24)) hr = "勇士";
		else if (player->HasTitle(23)) hr = "百夫长";
		else if (player->HasTitle(22)) hr = "军团士兵";
		else if (player->HasTitle(21)) hr = "血卫士";
		else if (player->HasTitle(20)) hr = "石头守卫";
		else if (player->HasTitle(19)) hr = "一等军士长";
		else if (player->HasTitle(18)) hr = "高阶军士";
		else if (player->HasTitle(17)) hr = "中士";
		else if (player->HasTitle(16)) hr = "步兵";
		else hr = "斥候";
	}
	return hr;
}
std::string HonorRank::GetHRTitle(Player* player, uint32 rank)
{
	std::string hr = "";
	switch (rank)
	{
	case 14:
		player->GetTeamId() == TEAM_ALLIANCE ? hr = "大元帅" : hr = "高阶督军";
		break;
	case 13:
		player->GetTeamId() == TEAM_ALLIANCE ? hr = "元帅" : hr = "督军";
		break;
	case 12:
		player->GetTeamId() == TEAM_ALLIANCE ? hr = "统帅" : hr = "将军";
		break;
	case 11:
		player->GetTeamId() == TEAM_ALLIANCE ? hr = "司令" : hr = "中将";
		break;
	case 10:
		player->GetTeamId() == TEAM_ALLIANCE ? hr = "少校" : hr = "勇士";
		break;
	case 9:
		player->GetTeamId() == TEAM_ALLIANCE ? hr = "护卫骑士" : hr = "百夫长";
		break;
	case 8:
		player->GetTeamId() == TEAM_ALLIANCE ? hr = "骑士队长" : hr = "军团士兵";
		break;
	case 7:
		player->GetTeamId() == TEAM_ALLIANCE ? hr = "骑士中尉" : hr = "血卫士";
		break;
	case 6:
		player->GetTeamId() == TEAM_ALLIANCE ? hr = "骑士" : hr = "石头守卫";
		break;
	case 5:
		player->GetTeamId() == TEAM_ALLIANCE ? hr = "士官长" : hr = "一等军士长";
		break;
	case 4:
		player->GetTeamId() == TEAM_ALLIANCE ? hr = "军士长" : hr = "高阶军士";
		break;
	case 3:
		player->GetTeamId() == TEAM_ALLIANCE ? hr = "中士" : hr = "中士";
		break;
	case 2:
		player->GetTeamId() == TEAM_ALLIANCE ? hr = "下士" : hr = "步兵";
		break;
	case 1:
		player->GetTeamId() == TEAM_ALLIANCE ? hr = "列兵" : hr = "斥候";
		break;
	}


	return hr;
}
void HonorRank::SetHRTitle(Player* player, uint32 title)
{
	CharTitlesEntry const* titleInfo = sCharTitlesStore.LookupEntry(title);

	if (!titleInfo) 
		return;

	if (!sReq->Check(player, GetReqId(title))) 
		return;
	
	sReq->Des(player, GetReqId(title));
	sRew->Rew(player, GetRewId(title));

	player->SetTitle(titleInfo);

	if (sSwitch->GetOnOff(ST_HR_ACCOUNT_BIND))
		sCF->updateHRTitle(player);

	player->CastSpell(player, 61456, true, NULL, NULL, player->GetGUID());

	sCF->SetLootRate(player);

	const char*  text = sString->Format(sString->GetText(CORE_STR_TYPES(STR_HONORRANK_UP)), sCF->GetNameLink(player).c_str(), GetHRTitle(player).c_str());
	sWorld->SendScreenMessage(text);
}

uint32 HonorRank::GetRate(Player* player, HRRateTypes type)
{
	uint32 len = HRUpVec.size();
	for (size_t i = 0; i < len; i++)
	{
		if (GetHRTiteId(player) == HRUpVec[i].title)
		{
			switch (type)
			{
			case HR_RATE_LOOT:
				return HRUpVec[i].lootRate;
			case HR_RATE_ITEM_EXCHANGE_0:
				return HRUpVec[i].exchangeRate_0;
			case HR_RATE_ITEM_EXCHANGE_1:
				return HRUpVec[i].exchangeRate_1;
			case HR_RATE_ITEM_STRENGTHEN:
				return HRUpVec[i].strengthenRate;
			case HR_RATE_SIGIL:
				return HRUpVec[i].sigilRate;
			case HR_RATE_GEM_REMOVE:
				return HRUpVec[i].removeGemRate;
			default:
				return 0.0f;
			}
		}
	}
	return 0.0f;
}
