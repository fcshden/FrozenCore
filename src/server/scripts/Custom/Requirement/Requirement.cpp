#pragma execution_character_set("utf-8")
#include "../CommonFunc/CommonFunc.h"
#include "../HonorRank/HonorRank.h"
#include "../Requirement/Requirement.h"
#include "../Reward/Reward.h"
#include "../DataLoader/DataLoader.h"
#include "Chat.h"
#include "../VIP/VIP.h"
#include "../String/myString.h"
#include <iostream>
#include <iomanip>
#include "../Rank/Rank.h"
#include "../Faction/Faction.h"
#include "../Command/CustomCommand.h"
#include "../SpiritPower/SpiritPower.h"

std::unordered_map<uint32, ReqTemplate> ReqMap;

void Req::Load()
{
	ReqMap.clear();
	
	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		
		//			0		1		2		3			4		5		6		7
		"SELECT 需求模板ID,会员等级,军衔等级,成就点数,金币数量,积分数量,荣誉点数,竞技点数,"
		//	8		9		10		11			12			13		14		15			16		17		18		19			20		21			22		23			24		25			26		27
		"物品ID1,物品数量1,物品ID2,物品数量2,物品ID3,物品数量3,物品ID4,物品数量4,物品ID5,物品数量5,物品ID6,物品数量6,物品ID7,物品数量7,物品ID8,物品数量8,物品ID9,物品数量9,物品ID10,物品数量10,"
		//28		29		30		31			32		33		34		35		36		37			38		39
		"经验数量,是否在副本,等级,自定义等级,自定义阵营ID,GM命令组,灵力值,转生等级,地图组,技能或光环组,任务组,成就组 FROM _模板_需求" :

		//		0		1		2		3				4			5		6			7
		"SELECT reqId,vipLevel,hr,achievementPoints,goldCount,tokenCount,hrPoints,arenaPoints,"
		//	8		9		10		11			12			13		14		15			16		17		18		19			20		21			22		23			24		25			26		27
		"itemId1,itemCount1,itemId2,itemCount2,itemId3,itemCount3,itemId4,itemCount4,itemId5,itemCount5,itemId6,itemCount6,itemId7,itemCount7,itemId8,itemCount8,itemId9,itemCount9,itemId10,itemCount10,"
		//28	29		30		31		32		33		34			35				36		37		38			39
		"xp,inInstance,level,rankLevel,faction,command,spiriptPower,reincarnation,mapData,spellData,questData,achieveData FROM _req");

	if (!result)
		return;

	do
	{
		Field* fields = result->Fetch();
		uint32 reqId = fields[0].GetUInt32();
		ReqTemplate ReqTemp;
		ReqTemp.meetVipLevel			= fields[1].GetInt32();
		ReqTemp.meetHRRank				= fields[2].GetInt32();
		ReqTemp.meetAchievementPoints	= fields[3].GetUInt32();
		ReqTemp.desGoldCount			= fields[4].GetUInt32() * GOLD;
		ReqTemp.desTokenCount			= fields[5].GetUInt32();
		ReqTemp.desHRPoints				= fields[6].GetUInt32();
		ReqTemp.desArenaPoints			= fields[7].GetUInt32();

		for (size_t i = 0; i < REQ_ITEM_MAX; i++)
		{
			ReqTemp.desItem[i]			= fields[8 + 2 * i].GetUInt32();
			ReqTemp.desItemCount[i]		= fields[9 + 2 * i].GetUInt32();
		}

		ReqTemp.desXp					= fields[28].GetUInt32();
		ReqTemp.inInstance				= fields[29].GetBool();
		ReqTemp.meetLevel				= fields[30].GetInt32();
		ReqTemp.meetRankLevel			= fields[31].GetInt32();
		ReqTemp.meetFaction				= fields[32].GetInt32();

		Tokenizer commandData(fields[33].GetString(), '#');
		for (Tokenizer::const_iterator itr = commandData.begin(); itr != commandData.end(); ++itr)
		{
			Tokenizer commands(*itr, '$');
			ReqCommandTemplate temp;
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

			ReqTemp.CommandDataVec.push_back(temp);
		}

		ReqTemp.desSpiritPower			= fields[34].GetUInt32();
		ReqTemp.reincarnation			= fields[35].GetInt32();

		Tokenizer mapData(fields[36].GetString(), '#');

		for (Tokenizer::const_iterator itr = mapData.begin(); itr != mapData.end(); ++itr)
		{
			char* map = strtok((char*)(*itr), ",");
			char* zone = strtok(NULL, ",");
			char* area = strtok(NULL, ",");
			
			if (!map)
				continue;

			ReqMapData t;
			t.map = atoi(map);
			
			if (zone)
				t.zone = atoi(zone);
			else
				t.zone = 0;

			if (area)
				t.area = atoi(area);

			else
				t.area = 0;

			ReqTemp.MapDataVec.push_back(t);
		}

		Tokenizer spellData(fields[37].GetString(), '#');

		for (Tokenizer::const_iterator itr = spellData.begin(); itr != spellData.end(); ++itr)
			if (SpellInfo const*  spellInfo = sSpellMgr->GetSpellInfo(abs(atoi(*itr))))
				ReqTemp.SpellDataVec.push_back(atoi(*itr));


		Tokenizer questData(fields[38].GetString(), '#');

		for (Tokenizer::const_iterator itr = questData.begin(); itr != questData.end(); ++itr)
			if (Quest const* questProto = sObjectMgr->GetQuestTemplate(abs(atoi(*itr))))
				ReqTemp.QuestDataVec.push_back(atoi(*itr));

		Tokenizer achieveData(fields[39].GetString(), '#');
		for (Tokenizer::const_iterator itr = achieveData.begin(); itr != achieveData.end(); ++itr)
			if (AchievementEntry const* achieve = sAchievementStore.LookupEntry(atoi(*itr)))
				ReqTemp.AchieveDataVec.push_back(atoi(*itr));

		ReqMap.insert(std::make_pair(reqId, ReqTemp));

	} while (result->NextRow());
}

std::string padRight(std::string &oriStr, int len)
{
	int strlen = oriStr.length();
	if (strlen < len)
		for (int i = 0; i < len - strlen; i++)
			oriStr = oriStr + " ";

	return oriStr;
}

std::string padLeft(std::string &oriStr, int len)
{
	int strlen = oriStr.length();
	if (strlen < len)
		for (int i = 0; i < len - strlen; i++)
			oriStr = ' ' + oriStr;

	return oriStr;
}


std::string Req::Notice(Player* player, uint32 reqId, std::string generalText, std::string text, uint32 count, uint32 chance, uint32 vipRateType, uint32 hrRateType)
{
	uint32		meetLevel				= 0;
	int32		meetHRRank				= 0;
	int32		meetVipLevel			= 0;
	int32		meetRankLevel			= 0;
	int32		meetFaction				= 0;
	uint32		meetAchievementPoints	= 0;
	uint32		desXp					= 0;
	uint32		desGoldCount			= 0;
	uint32		desTokenCount			= 0;
	uint32		desHRPoints				= 0;
	uint32		desArenaPoints			= 0;
	uint32		desItem[REQ_ITEM_MAX];
	uint32		desItemCount[REQ_ITEM_MAX];
	uint32		desSpiritPower			= 0;
	uint32		areaId					= 0;
	int32		reincarnation			= 0;
	bool		inInstance				= false;
	

	std::unordered_map<uint32, ReqTemplate>::iterator iter = ReqMap.find(reqId);
	if (iter != ReqMap.end())
	{
			meetLevel				= iter->second.meetLevel				;
			meetHRRank				= iter->second.meetHRRank				;
			meetVipLevel			= iter->second.meetVipLevel				;
			meetRankLevel			= iter->second.meetRankLevel			;
			meetFaction				= iter->second.meetFaction				;
			meetAchievementPoints	= iter->second.meetAchievementPoints	;
			desXp					= iter->second.desXp					;
			desGoldCount			= count * iter->second.desGoldCount		;
			desTokenCount			= count * iter->second.desTokenCount	;
			desHRPoints				= count * iter->second.desHRPoints		;
			desArenaPoints			= count * iter->second.desArenaPoints	;

			for (size_t i = 0; i < REQ_ITEM_MAX; i++)
			{
				desItem[i] = iter->second.desItem[i];
				desItemCount[i] = count * iter->second.desItemCount[i];
			}

			desSpiritPower			= count * iter->second.desSpiritPower	;
			reincarnation			= iter->second.reincarnation			;
			inInstance				= iter->second.inInstance				;
	}
	else
		return "";

	
	std::ostringstream oss;
	oss << generalText << text;
	
	if (abs(meetVipLevel) > 0 || abs(meetHRRank) > 0 || meetAchievementPoints > 0 || meetLevel > 0 || abs(meetRankLevel) > 0 || abs(reincarnation) > 0 || abs(meetFaction) >0)
	{
		oss << "\n\n|cFFFF1717需要满足|r\n\n";

		if (meetLevel > 0)
			oss << "|cFFFFCC00[等级" << meetLevel << "]|r";

		if (abs(meetHRRank) > 0)
		{
			uint32 hrRank = player->GetTeamId() == TEAM_ALLIANCE ? abs(meetHRRank) : abs(meetHRRank) + 14;
			std::string title = "";
			std::string icon = "";
			sCF->GetHRTitle(NULL, title, icon, true, hrRank);

			if (meetHRRank > 0)
				oss << title;
			else
				oss << "当前是" << title;
		}

		if (abs(meetVipLevel) > 0)
		{
			std::string title = "";
			std::string icon = "";
			sVIP->GetVIPTitle(NULL, title, icon, true, abs(meetVipLevel));

			if (meetVipLevel > 0)
				oss << title;
			else
				oss << "当前是" << title;
		}

		if (abs(meetRankLevel) > 0)
		{
			if (meetRankLevel > 0)
				oss << "|cFFFFCC00" << sRank->GetName(meetRankLevel) << "|r";
			else
				oss << "当前是" << "|cFFFFCC00" << sRank->GetName(abs(meetRankLevel)) << "|r";
		}

		if (abs(reincarnation) > 0)
		{
			if (reincarnation > 0)
				oss << "|cFFFFCC00" << "转生等级" << reincarnation << "|r";
			else
				oss << "当前是" << "|cFFFFCC00" << "转生等级" << abs(reincarnation) << "|r";
		}


		if (abs(meetFaction) > 0)
		{
			if (meetFaction > 0)
				oss << "|cFFFFCC00" << sFaction->GetName(meetFaction) << "|r";
			else
				oss << "当前是|cFFFFCC00" << sFaction->GetName(meetFaction) << "|r";
		}
		
		if (meetAchievementPoints > 0)
			oss << "|cFFFFCC00[成就点" << meetAchievementPoints << "]|r";

		if (inInstance)
			oss << "|cFFFFCC00[在副本中]|r";

		if (AreaTableEntry const* areaEntry = GetAreaEntryByAreaID(areaId))
			oss << "|cFFFFCC00[在" << "areaEntry->area_name[4]" << "区域中]|r";

		oss << "\n";
	}

	ItemTemplate const* pProto[REQ_ITEM_MAX];
	for (size_t i = 0; i < REQ_ITEM_MAX; i++)
		pProto[i] = sObjectMgr->GetItemTemplate(desItem[i]);

	bool reqItem = false;

	for (size_t i = 0; i < REQ_ITEM_MAX; i++)
		if (pProto[i] && desItemCount[i] > 0)
			reqItem = true;

	if (desGoldCount > 0 || desXp > 0 || desTokenCount > 0 || desHRPoints > 0 || desArenaPoints > 0 || reqItem)
	{
		oss << "\n|cFFFF1717将消耗|r\n\n";

		if (desTokenCount > 0)
			oss << "|cFF0030FF" << desTokenCount << "|r|cFFFFCC00" << sString->GetText(CORE_STR_TYPES(STR_TOKEN)) << "|r";
		if (desHRPoints > 0)
			oss << "|cFF0030FF" << desHRPoints << "|r|cFFFFCC00荣誉|r";
		if (desArenaPoints > 0)
			oss << "|cFF0030FF" << desArenaPoints << "|r|cFFFFCC00竞技点|r";
		if (desXp > 0)
			oss << "|cFF0030FF" << desXp << "|r|cFFFFCC00经验|r";
		if (desGoldCount > 0)
			oss << "|cFF0030FF" << desGoldCount / GOLD << "|r|cFFFFCC00金币|r";


		if (desXp > 0 || desTokenCount > 0 || desHRPoints > 0 || desArenaPoints > 0 || desGoldCount > 0)
			oss << "\n\n";

		for (size_t i = 0; i < REQ_ITEM_MAX; i++)
			if (pProto[i] && desItemCount[i] > 0)
				oss << sCF->GetItemLink(desItem[i]) << " X |cFFFF1717" << desItemCount[i] << "|r\n\n";
		
		if (chance < 100)
		{
			oss << "成功几率是" << chance + sVIP->GetRate(player, VIPRateTypes(vipRateType)) + sHR->GetRate(player, HRRateTypes(hrRateType)) << "%";
			if (sVIP->GetRate(player, VIPRateTypes(vipRateType)) > 0 || sHR->GetRate(player, HRRateTypes(hrRateType)) > 0)
				oss << "(";
			if (sVIP->GetRate(player, VIPRateTypes(vipRateType)) > 0)
				oss << "会员 +" << sVIP->GetRate(player, VIPRateTypes(vipRateType)) << "%";
			if (sHR->GetRate(player, HRRateTypes(hrRateType)) > 0)
				oss << "军衔 + " << sHR->GetRate(player, HRRateTypes(hrRateType)) << "%";
			if (sVIP->GetRate(player, VIPRateTypes(vipRateType)) > 0 || sHR->GetRate(player, HRRateTypes(hrRateType)) > 0)
				oss << ")";
		}
			
		oss <<"\n\n";
	}
	
	return oss.str();
}
uint32 Req::Golds(uint32 reqId, uint32 count)
{
	std::unordered_map<uint32, ReqTemplate>::iterator iter = ReqMap.find(reqId);

	if (iter != ReqMap.end())
		return count* iter->second.desGoldCount;

	return 0;
}

bool Req::Check(Player* player, uint32 reqId, uint32 count, bool notice)
{
	if (reqId == 0)
		return true;

	bool		legal					= true;
	uint32		meetLevel				= 0;
	int32		meetHRRank				= 0;
	int32		meetVipLevel			= 0;
	int32		meetRankLevel			= 0;
	int32		reincarnation			= 0;
	int32		meetFaction				= 0;
	uint32		meetAchievementPoints	= 0;
	uint32		desXp					= 0;
	uint32		desGoldCount			= 0;
	uint32		desTokenCount			= 0;
	uint32		desHRPoints				= 0;
	uint32		desArenaPoints			= 0;
	uint32		desItem[REQ_ITEM_MAX];
	uint32		desItemCount[REQ_ITEM_MAX];
	uint32		desSpiritPower			= 0;
	uint32		areaId					= 0;
	bool		inInstance				= false;


	std::unordered_map<uint32, ReqTemplate>::iterator iter = ReqMap.find(reqId);
	if (iter != ReqMap.end())
	{
		meetLevel = iter->second.meetLevel;
		meetHRRank = iter->second.meetHRRank;
		meetVipLevel = iter->second.meetVipLevel;
		meetRankLevel = iter->second.meetRankLevel;
		reincarnation = iter->second.reincarnation;
		meetFaction = iter->second.meetFaction;
		meetAchievementPoints = iter->second.meetAchievementPoints;
		desXp = iter->second.desXp;
		desGoldCount = count * iter->second.desGoldCount;
		desTokenCount = count * iter->second.desTokenCount;
		desHRPoints = count * iter->second.desHRPoints;
		desArenaPoints = count * iter->second.desArenaPoints;
		for (size_t i = 0; i < REQ_ITEM_MAX; i++)
		{
			desItem[i] = iter->second.desItem[i];
			desItemCount[i] = count * iter->second.desItemCount[i];
		}
		desSpiritPower = count * iter->second.desSpiritPower;
		inInstance = iter->second.inInstance;
	}
	else
	{
		if (notice)
			ChatHandler(player->GetSession()).PSendSysMessage("|cFFFF1717数据库未正常配置|r");
		return false;
	}

	if (inInstance && !player->GetMap()->IsDungeon())
	{
		if (notice)
			ChatHandler(player->GetSession()).PSendSysMessage(sString->GetText(CORE_STR_TYPES(STR_REQ_INSTANCE)));
		legal = false;
	}

	if (areaId != 0 && player->GetAreaId() != areaId)
	{
		AreaTableEntry const* area = GetAreaEntryByAreaID(areaId);
		if (notice)
			ChatHandler(player->GetSession()).PSendSysMessage(sString->Format(sString->GetText(CORE_STR_TYPES(STR_REQ_ZONE)), area->area_name[player->GetSession()->GetSessionDbcLocale()]));
		legal = false;
	}

	if (meetLevel > 0)
	{
		if (player->getLevel() < meetLevel)
		{
			if (notice)
				ChatHandler(player->GetSession()).PSendSysMessage(sString->Format(sString->GetText(CORE_STR_TYPES(STR_REQ_LEVEL)), player->getLevel(), meetLevel));
			legal = false;
		}
	}

	if (abs(meetVipLevel) > 0)
	{
		if (meetVipLevel > 0)
		{
			if (player->vipLevel < meetVipLevel)
			{
				if (notice)
					ChatHandler(player->GetSession()).PSendSysMessage("当前%s，需达到%s", sVIP->GetVIPName(player->vipLevel).c_str(), sVIP->GetVIPName(meetVipLevel).c_str());
				legal = false;
			}
		}
		else
		{
			if (player->vipLevel != abs(meetVipLevel))
			{
				if (notice)
					ChatHandler(player->GetSession()).PSendSysMessage("当前%s，必须是%s", sVIP->GetVIPName(player->vipLevel).c_str(), sVIP->GetVIPName(abs(meetVipLevel)).c_str());
				legal = false;
			}
		}
	}

	if (abs(meetHRRank) > 0)
	{
		uint32 hrRank = sHR->GetHRRank(player);

		if (meetHRRank > 0)
		{
			if (hrRank < meetHRRank)
			{
				std::string title = "[无]";
				std::string icon = "";
				std::string nextTitle = "";

				if (player->realTeam == TEAM_HORDE)
				{
					hrRank += 14;
					meetHRRank += 14;
				}

				sCF->GetHRTitle(NULL, title, icon, true, hrRank);
				sCF->GetHRTitle(NULL, nextTitle, icon, true, meetHRRank);

				if (notice)
					ChatHandler(player->GetSession()).PSendSysMessage("当前%s,需达到%s", title.c_str(), nextTitle.c_str());
				legal = false;
			}
		}
		else
		{
			if (hrRank != abs(meetHRRank))
			{
				meetHRRank = abs(meetHRRank);

				std::string title = "[无]";
				std::string icon = "";
				std::string nextTitle = "";

				if (player->realTeam == TEAM_HORDE)
				{
					hrRank += 14;
					meetHRRank += 14;
				}

				sCF->GetHRTitle(NULL, title, icon, true, hrRank);
				sCF->GetHRTitle(NULL, nextTitle, icon, true, meetHRRank);
				if (notice)
					ChatHandler(player->GetSession()).PSendSysMessage("当前%s,必须是%s", title.c_str(), nextTitle.c_str());
				legal = false;
			}
		}
	}
	if (meetAchievementPoints > 0)
	{
		if (sCF->GetAchievementPoints(player) < meetAchievementPoints)
		{
			if (notice)
				ChatHandler(player->GetSession()).PSendSysMessage(sString->Format(sString->GetText(CORE_STR_TYPES(STR_REQ_ACHIEVE)), sCF->GetAchievementPoints(player), meetAchievementPoints));
			legal = false;
		}
	}

	if (abs(meetRankLevel) > 0)
	{
		if (meetRankLevel > 0)
		{
			if (player->rankLevel < meetRankLevel)
			{
				if (notice)
					ChatHandler(player->GetSession()).PSendSysMessage("当前%s，需达到%s", sRank->GetName(player->rankLevel).c_str(), sRank->GetName(meetRankLevel).c_str());
				legal = false;
			}
		}
		else
		{
			if (player->rankLevel != abs(meetRankLevel))
			{
				{
					if (notice)
						ChatHandler(player->GetSession()).PSendSysMessage("当前%s，必须是%s", sRank->GetName(player->rankLevel).c_str(), sRank->GetName(abs(meetRankLevel)).c_str());
					legal = false;
				}
			}
		}	
	}

	if (abs(reincarnation) > 0)
	{
		if (reincarnation > 0)
		{
			if (player->reincarnationLv < reincarnation)
			{
				if (notice)
					ChatHandler(player->GetSession()).PSendSysMessage("当前转生等级%u，需达到%u", player->reincarnationLv, reincarnation);
				legal = false;
			}
		}
		else
		{
			if (player->reincarnationLv != abs(reincarnation))
			{
				{
					if (notice)
						ChatHandler(player->GetSession()).PSendSysMessage("当前转生等级%u，必须是%s", player->reincarnationLv, abs(reincarnation));
					legal = false;
				}
			}
		}
	}

	if (meetFaction > 0)
	{
		if (player->faction != meetFaction)
		{
			if (notice)
				ChatHandler(player->GetSession()).PSendSysMessage("当前%s，需要%s", sFaction->GetName(player->faction).c_str(), sFaction->GetName(meetFaction).c_str());
			legal = false;
		}
	}

	if (desXp > 0)
	{
		if (player->GetUInt32Value(PLAYER_XP) < desXp)
		{
			if (notice)
				ChatHandler(player->GetSession()).PSendSysMessage(sString->Format(sString->GetText(CORE_STR_TYPES(STR_REQ_XP)), player->GetUInt32Value(PLAYER_XP), desXp));
			legal = false;
		}
	}

	if (desGoldCount > 0)
	{
		if (!player->HasEnoughMoney(desGoldCount))
		{
			if (notice)
				ChatHandler(player->GetSession()).PSendSysMessage(sString->Format(sString->GetText(CORE_STR_TYPES(STR_REQ_GOLD)), player->GetMoney() / GOLD, desGoldCount / GOLD));
			legal = false;
		}
	}

	if (desSpiritPower > 0)
	{
		if (player->SpiritPower < desSpiritPower)
		{
			if (notice)
				ChatHandler(player->GetSession()).PSendSysMessage("当前[灵力]%u，需消耗[灵力]%u", player->SpiritPower, desSpiritPower);
			legal = false;
		}
	}

	if (desTokenCount > 0)
	{
		uint32 tokenAmount = sCF->GetTokenAmount(player);
		if (tokenAmount < desTokenCount)
		{
			if (notice)
				ChatHandler(player->GetSession()).PSendSysMessage(sString->Format(sString->GetText(CORE_STR_TYPES(STR_REQ_TOKEN)), tokenAmount, desTokenCount));
			legal = false;
		}
	}

	if (desHRPoints > 0 && player->GetHonorPoints() < desHRPoints)
	{
		if (notice)
			ChatHandler(player->GetSession()).PSendSysMessage(sString->Format(sString->GetText(CORE_STR_TYPES(STR_REQ_HONOR)), player->GetHonorPoints(), desHRPoints));
		legal = false;
	}

	if (desArenaPoints > 0 && player->GetArenaPoints() < desArenaPoints)
	{
		if (notice)
			ChatHandler(player->GetSession()).PSendSysMessage(sString->Format(sString->GetText(CORE_STR_TYPES(STR_REQ_ARENA)), player->GetArenaPoints(), desArenaPoints));
		legal = false;
	}

	ItemTemplate const* pProto[REQ_ITEM_MAX];
	for (size_t i = 0; i < REQ_ITEM_MAX; i++)
		pProto[i] = sObjectMgr->GetItemTemplate(desItem[i]);

	for (size_t i = 0; i < REQ_ITEM_MAX; i++)
		if (pProto[i] && desItemCount[i] > 0)
			if (player->GetItemCount(desItem[i]) < desItemCount[i])
			{
				if (notice)
					ChatHandler(player->GetSession()).PSendSysMessage(sString->Format(sString->GetText(CORE_STR_TYPES(STR_REQ_ITEM)), sCF->GetItemLink(desItem[i]).c_str(), desItemCount[i] - player->GetItemCount(desItem[i])));
				legal = false;
			}

	if (!CheckQuest(player, reqId, notice))
		legal = false;

	if (!CheckMap(player, reqId, notice))
		legal = false;

	if (!CheckSpell(player, reqId, notice))
		legal = false;

	if (!CheckAcheive(player, reqId, notice))
		legal = false;

	if (!legal && notice)
		player->GetSession()->SendNotification(sString->GetText(CORE_STR_TYPES(STR_REQ_NOTICE)));
	return legal;
}

void Req::Des(Player* player, uint32 reqId, uint32 count, uint32 expItemId)
{
	if (reqId == 0)
		return;

	uint32		meetLevel				= 0;
	int32		meetHRRank				= 0;
	int32		meetVipLevel			= 0;
	int32		meetRankLevel			= 0;
	int32		meetFaction				= 0;
	uint32		meetAchievementPoints	= 0;
	uint32		desXp					= 0;
	uint32		desGoldCount			= 0;
	uint32		desTokenCount			= 0;
	uint32		desHRPoints				= 0;
	uint32		desArenaPoints			= 0;
	uint32		desItem[REQ_ITEM_MAX];
	uint32		desItemCount[REQ_ITEM_MAX];
	uint32		desSpiritPower			= 0;
	uint32		areaId					= 0;
	bool		inInstance				= false;


	std::unordered_map<uint32, ReqTemplate>::iterator iter = ReqMap.find(reqId);
	if (iter != ReqMap.end())
	{
		meetLevel = iter->second.meetLevel;
		meetHRRank = iter->second.meetHRRank;
		meetVipLevel = iter->second.meetVipLevel;
		meetRankLevel = iter->second.meetRankLevel;
		meetFaction = iter->second.meetFaction;
		meetAchievementPoints = iter->second.meetAchievementPoints;
		desXp = iter->second.desXp;
		desGoldCount = count * iter->second.desGoldCount;
		desTokenCount = count * iter->second.desTokenCount;
		desHRPoints = count * iter->second.desHRPoints;
		desArenaPoints = count * iter->second.desArenaPoints;
		for (size_t i = 0; i < REQ_ITEM_MAX; i++)
		{
			desItem[i] = iter->second.desItem[i];
			desItemCount[i] = count * iter->second.desItemCount[i];
		}
		desSpiritPower = count * iter->second.desSpiritPower;
		inInstance = iter->second.inInstance;

		for (auto itr = iter->second.CommandDataVec.begin(); itr != iter->second.CommandDataVec.end(); itr++)
			if (!itr->command.empty())
				sCustomCommand->DoCommand(player, itr->command);
	}
	else
		return;

	if (desXp > 0)	player->SetUInt32Value(PLAYER_XP, player->GetUInt32Value(PLAYER_XP) - desXp);

	if (desTokenCount > 0) sCF->UpdateTokenAmount(player, desTokenCount, false, "[REQ]扣除");

	if (desHRPoints > 0) player->ModifyHonorPoints(-int32(desHRPoints));

	if (desArenaPoints > 0) player->ModifyArenaPoints(-int32(desArenaPoints));

	if (desSpiritPower > 0) sSpiritPower->Update(player, desSpiritPower, false);

	if (expItemId != 0)
		for (size_t i = 0; i < REQ_ITEM_MAX; i++)
			if (expItemId == desItem[i] && desItemCount[i] > 0)
			{
				desItemCount[i]--;
				break;
			}

	ItemTemplate const* pProto[REQ_ITEM_MAX];
	for (size_t i = 0; i < REQ_ITEM_MAX; i++)
		pProto[i] = sObjectMgr->GetItemTemplate(desItem[i]);

	for (size_t i = 0; i < REQ_ITEM_MAX; i++)
		if (pProto[i] && desItemCount[i] > 0)
			player->DestroyItemCount(desItem[i], desItemCount[i], true, false);

	if (desGoldCount > 0) player->ModifyMoney(-int32(desGoldCount));
}

bool IsSpecialChar(char ch){
	//有几个字符与输入法有关；    
	const char *punc="0123456789ABCDEFabcdef|r[]";
	int len=strlen(punc);    
	for(int i=0; i<len; i++)        
		if( *(punc+i)== ch )            
			return true;    
	return false;
}

void RemovePunc(std::string &word)
{
	std::string::iterator first, second;

	first = find_if(word.begin(), word.end(), IsSpecialChar);    
	while (first != word.end())    
	{	
		second = word.erase(first, first + 1);       
		first = find_if(second, word.end(), IsSpecialChar); 
	} 
}

std::string Req::GetDescription(uint32 reqId, Player* player, uint32 count)
{
	uint32		meetLevel = 0;
	int32		meetHRRank = 0;
	int32		meetVipLevel = 0;
	int32		meetRankLevel = 0;
	int32		reincarnation = 0;
	int32		meetFaction = 0;
	uint32		meetAchievementPoints = 0;
	uint32		desXp = 0;
	uint32		desGoldCount = 0;
	uint32		desTokenCount = 0;
	uint32		desHRPoints = 0;
	uint32		desArenaPoints = 0;
	uint32		desItem[REQ_ITEM_MAX];
	uint32		desItemCount[REQ_ITEM_MAX];
	uint32		desSpiritPower = 0;
	uint32		areaId = 0;
	bool		inInstance = false;


	std::unordered_map<uint32, ReqTemplate>::iterator iter = ReqMap.find(reqId);
	if (iter != ReqMap.end())
	{
		meetLevel = iter->second.meetLevel;
		meetHRRank = iter->second.meetHRRank;
		meetVipLevel = iter->second.meetVipLevel;
		meetRankLevel = iter->second.meetRankLevel;
		reincarnation = iter->second.reincarnation;
		meetFaction = iter->second.meetFaction;
		meetAchievementPoints = iter->second.meetAchievementPoints;
		desXp = iter->second.desXp;
		desGoldCount = count * iter->second.desGoldCount;
		desTokenCount = count * iter->second.desTokenCount;
		desHRPoints = count * iter->second.desHRPoints;
		desArenaPoints = count * iter->second.desArenaPoints;
		for (size_t i = 0; i < REQ_ITEM_MAX; i++)
		{
			desItem[i] = iter->second.desItem[i];
			desItemCount[i] = count * iter->second.desItemCount[i];
		}
		desSpiritPower = count * iter->second.desSpiritPower;
		inInstance = iter->second.inInstance;
	}
	else
		return "";

	std::ostringstream oss;

	if (meetLevel > 0)
		oss << "    |cFF660066[等级]|r |cFF660066[" << meetLevel << "]|r\n";

	if (abs(meetRankLevel) > 0)
	{
		if (meetRankLevel > 0)
			oss << "    |cFF660066" << sRank->GetName(meetRankLevel) << "|r\n";
		else
			oss << "    |cFF660066[修炼]|r |cFF660066当前必须是[" << sRank->GetName(abs(meetRankLevel)) << "]|r\n";
	}

	if (abs(reincarnation) > 0)
	{
		if (reincarnation > 0)
			oss << "    |cFF660066" << "转生等级" << reincarnation << "|r\n";
		else
			oss << "    |cFF660066[修炼]|r |cFF660066当前必须是[" << "转生等级" << abs(reincarnation) << "]|r\n";
	}

	if (meetFaction > 0)
		oss << "    |cFF660066" << sFaction->GetName(meetFaction) << "|r\n";

	if (abs(meetVipLevel) > 0)
	{
		std::string title = sVIP->GetVIPName(abs(meetVipLevel));
		RemovePunc(title);

		if (meetVipLevel > 0)
			oss << "    |cFF660066[会员]|r |cFF660066[" << title << "]|r\n";
		else
			oss << "    |cFF660066[会员]|r |cFF660066当前会员必须是[" << title << "]|r\n";
	}

	if (abs(meetHRRank) > 0)
	{
		std::string title = sHR->GetHRTitle(player, abs(meetHRRank));

		if (meetHRRank > 0)
			oss << "    |cFF660066[军衔]|r |cFF660066[" << title << "]|r\n";
		else
			oss << "    |cFF660066[军衔]|r |cFF660066当前军衔必须是[" << title << "]|r\n";
	}
		

	if (desXp > 0)
		oss << "    |cFF660066[经验]|r x |cFF660066" << desXp << "|r\n";
	if (desGoldCount > 0)
		oss << "    |cFF660066[金币]|r x |cFF660066" << desGoldCount / GOLD << "|r\n";
	if (desTokenCount > 0)
		oss << "    |cFF660066[" << sString->GetText(CORE_STR_TYPES(STR_TOKEN)) << "]|r x |cFF660066" << desTokenCount << "|r\n";
	if (desHRPoints > 0)
		oss << "    |cFF660066[荣誉]|r x |cFF660066" << desHRPoints << "|r\n";
	if (desArenaPoints)
		oss << "    |cFF660066[竞技点]|r x |cFF660066" << desArenaPoints << "|r\n";
	if (meetAchievementPoints > 0)
		oss << "    |cFF660066[成就点]|r x |cFF660066" << meetAchievementPoints << "|r\n";
		
	ItemTemplate const* pProto[REQ_ITEM_MAX];
	for (size_t i = 0; i < REQ_ITEM_MAX; i++)
		pProto[i] = sObjectMgr->GetItemTemplate(desItem[i]);

	for (size_t i = 0; i < REQ_ITEM_MAX; i++)
		if (pProto[i] && desItemCount[i] > 0)
			oss << "    |cFF660066[" << pProto[i]->Name1 << "] x |cFF660066" << desItemCount[i] << "|r\n";

	return oss.str();
}

bool Req::IsExist(uint32 reqId)
{
	std::unordered_map<uint32, ReqTemplate>::iterator iter = ReqMap.find(reqId);
	if (iter != ReqMap.end())
		return true;

	return false;
}

std::string Req::GetExtraDes(uint32 reqId, uint32 count)
{
	uint32		meetLevel = 0;
	int32		meetHRRank = 0;
	int32		meetVipLevel = 0;
	int32		meetRankLevel = 0;
	int32		reincarnation = 0;
	int32		meetFaction = 0;
	uint32		meetAchievementPoints = 0;
	uint32		desXp = 0;
	uint32		desGoldCount = 0;
	uint32		desTokenCount = 0;
	uint32		desHRPoints = 0;
	uint32		desArenaPoints = 0;
	uint32		desItem[REQ_ITEM_MAX];
	uint32		desItemCount[REQ_ITEM_MAX];
	uint32		desSpiritPower = 0;
	uint32		areaId = 0;
	bool		inInstance = false;


	std::unordered_map<uint32, ReqTemplate>::iterator iter = ReqMap.find(reqId);
	if (iter != ReqMap.end())
	{
		meetLevel = iter->second.meetLevel;
		meetHRRank = iter->second.meetHRRank;
		meetVipLevel = iter->second.meetVipLevel;
		meetRankLevel = iter->second.meetRankLevel;
		reincarnation = iter->second.reincarnation;
		meetFaction = iter->second.meetFaction;
		meetAchievementPoints = iter->second.meetAchievementPoints;
		desXp = iter->second.desXp;
		desGoldCount = count * iter->second.desGoldCount;
		desTokenCount = count * iter->second.desTokenCount;
		desHRPoints = count * iter->second.desHRPoints;
		desArenaPoints = count * iter->second.desArenaPoints;
		for (size_t i = 0; i < REQ_ITEM_MAX; i++)
		{
			desItem[i] = iter->second.desItem[i];
			desItemCount[i] = count * iter->second.desItemCount[i];
		}
		desSpiritPower = count * iter->second.desSpiritPower;
		inInstance = iter->second.inInstance;
	}
	else
		return "";

	std::ostringstream oss;

	if (meetLevel > 0)
		oss << "|cFFFFCC00[等级" << meetLevel << "]|r";

	if (abs(reincarnation) > 0)
	{
		if (reincarnation > 0)
			oss << "   |cFFFFCC00" << "转生等级" << reincarnation << "|r\n";
		else
			oss << "   |cFFFFCC00当前转生等级必须是" << abs(reincarnation) << "|r\n";
	}

	if (abs(meetVipLevel) > 0)
	{
		std::string title = sVIP->GetVIPName(abs(meetVipLevel));
		RemovePunc(title);

		if (meetVipLevel > 0)
			oss << "   |cFFFFCC00[" << title << "]|r\n";
		else
			oss << "   |cFFFFCC00当前会员必须是[" << title << "]|r\n";
	}

	if (abs(meetHRRank) > 0)
	{
		if (meetHRRank > 0)
			oss << "   |cFFFFCC00[军衔" << sCF->GetHRTitle(abs(meetHRRank)) << "]|r\n";
		else
			oss << "   |cFFFFCC00当前军衔必须是[" << sCF->GetHRTitle(abs(meetHRRank)) << "]|r\n";
	}

	if (meetFaction > 0)
		oss << "   |cFFFFCC00" << sFaction->GetName(meetFaction) << "|r\n";
	if (desXp > 0)
		oss << "   |cFFFFCC00[经验]|r x |cFFFFCC00" << desXp << "|r\n";
	if (desGoldCount > 0)
		oss << "   |cFFFFCC00[金币]|r x |cFFFFCC00" << desGoldCount / GOLD << "|r\n";
	if (desTokenCount > 0)
		oss << "   |cFFFFCC00[" << sString->GetText(CORE_STR_TYPES(STR_TOKEN)) << "]|r x |cFFFFCC00" << desTokenCount << "|r\n";
	if (desHRPoints > 0)
		oss << "   |cFFFFCC00[荣誉]|r x |cFFFFCC00" << desHRPoints << "|r\n";
	if (desArenaPoints)
		oss << "   |cFFFFCC00[竞技点]|r x |cFFFFCC00" << desArenaPoints << "|r\n";

	ItemTemplate const* pProto[REQ_ITEM_MAX];
	for (size_t i = 0; i < REQ_ITEM_MAX; i++)
		pProto[i] = sObjectMgr->GetItemTemplate(desItem[i]);

	for (size_t i = 0; i < REQ_ITEM_MAX; i++)
		if (pProto[i] && desItemCount[i] > 0)
			oss << "   " << sCF->GetItemLink(desItem[i]) << " x |cFFFFCC00" << desItemCount[i] << "|r\n";
	
	return oss.str();
}


bool Req::CheckMap(Player* pl, uint32 reqId, bool notice)
{
	std::unordered_map<uint32, ReqTemplate>::iterator iter = ReqMap.find(reqId);
	if (iter == ReqMap.end())
		return true;

	if (iter->second.MapDataVec.empty())
		return true;

	uint32 map = pl->GetMapId();
	uint32 zone = pl->GetZoneId();
	uint32 area = pl->GetAreaId();

	for (auto itr = iter->second.MapDataVec.begin(); itr != iter->second.MapDataVec.end(); itr++)
		if (map == itr->map && 0 == itr->zone && 0 == itr->area ||
			map == itr->map && zone == itr->zone && 0 == itr->area ||
			map == itr->map && zone == itr->zone && area == itr->area)
			return true;

	if(notice)
		ChatHandler(pl->GetSession()).PSendSysMessage(sString->Format(sString->GetText(CORE_STR_TYPES(STR_REQ_ZONE)), "正确的区域"));
	return false;
}

bool Req::CheckSpell(Player* pl, uint32 reqId, bool notice)
{
	std::unordered_map<uint32, ReqTemplate>::iterator iter = ReqMap.find(reqId);
	if (iter == ReqMap.end())
		return true;

	if (iter->second.SpellDataVec.empty())
		return true;

	for (auto itr = iter->second.SpellDataVec.begin(); itr != iter->second.SpellDataVec.end(); itr++)
	{
		int32 spell = *itr;

		SpellInfo const*  spellInfo = sSpellMgr->GetSpellInfo(abs(spell));

		if (!spellInfo)
			continue;

		if (spell > 0)
		{
			if (pl->HasSpell(spell))
				return true;
		}
		else
		{
			if (pl->HasAura(abs(spell)))
				return true;
		}
	}

	for (auto itr = iter->second.SpellDataVec.begin(); itr != iter->second.SpellDataVec.end(); itr++)
	{
		int32 spell = *itr;

		SpellInfo const*  spellInfo = sSpellMgr->GetSpellInfo(abs(spell));

		if (!spellInfo)
			continue;

		if (spell > 0)
		{
			if (!pl->HasSpell(spell) && notice)
				ChatHandler(pl->GetSession()).PSendSysMessage("你不拥有技能[%s]", spellInfo->SpellName[4]);
		}
		else
		{
			if (!pl->HasAura(abs(spell)) && notice)
				ChatHandler(pl->GetSession()).PSendSysMessage("你不拥有光环[%s]", spellInfo->SpellName[4]);
		}
	}

	return false;
}

bool Req::CheckQuest(Player* pl, uint32 reqId, bool notice)
{
	std::unordered_map<uint32, ReqTemplate>::iterator iter = ReqMap.find(reqId);
	if (iter == ReqMap.end())
		return true;

	if (iter->second.QuestDataVec.empty())
		return true;

	for (auto itr = iter->second.QuestDataVec.begin(); itr != iter->second.QuestDataVec.end(); itr++)
	{
		int32 quest = *itr;
		Quest const* questProto = sObjectMgr->GetQuestTemplate(abs(quest));

		if (!questProto)
			continue;

		if (quest > 0)
		{
			if (pl->GetQuestStatus(abs(quest)) == QUEST_STATUS_INCOMPLETE)
				return true;
		}
		else
		{
			if (pl->GetQuestStatus(abs(quest)) == QUEST_STATUS_REWARDED)
				return true;
		}
	}

	for (auto itr = iter->second.QuestDataVec.begin(); itr != iter->second.QuestDataVec.end(); itr++)
	{
		int32 quest = *itr;
		Quest const* questProto = sObjectMgr->GetQuestTemplate(abs(quest));

		if (!questProto)
			continue;

		if (quest > 0)
		{
			if (!pl->hasQuest(quest) && notice)
				ChatHandler(pl->GetSession()).PSendSysMessage("你不拥有任务[%s]", questProto->GetTitle().c_str());
		}
		else
		{
			if (pl->GetQuestStatus(abs(quest)) != QUEST_STATUS_REWARDED && notice)
				ChatHandler(pl->GetSession()).PSendSysMessage("你未完成任务[%s]", questProto->GetTitle().c_str());
		}
	}

	return false;
}

bool Req::CheckAcheive(Player* pl, uint32 reqId, bool notice)
{
	std::unordered_map<uint32, ReqTemplate>::iterator iter = ReqMap.find(reqId);
	if (iter == ReqMap.end())
		return true;

	if (iter->second.AchieveDataVec.empty())
		return true;

	for (auto itr = iter->second.AchieveDataVec.begin(); itr != iter->second.AchieveDataVec.end(); itr++)
	{
		uint32 entryId = *itr;
		if (AchievementEntry const* achieve = sAchievementStore.LookupEntry(entryId))
			if (!achieve)
			continue;

		if (pl->HasAchieved(entryId))
				return true;
	}

	if (notice)
		ChatHandler(pl->GetSession()).PSendSysMessage("你未达成相关成就");
	return false;
}
