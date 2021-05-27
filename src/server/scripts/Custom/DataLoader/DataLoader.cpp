#pragma execution_character_set("utf-8")
#include "LFGMgr.h"
#include "../Requirement/Requirement.h"
#include "../Reward/Reward.h"
#include "../DataLoader/DataLoader.h"
#include "../CommonFunc/CommonFunc.h"
#include "../HonorRank/HonorRank.h"
#include "../Recruit/Recruit.h"
#include "../SellReward/SellReward.h"
#include "../FunctionCollection/FunctionCollection.h"
#include "../ItemMod/ItemMod.h"
#include "../Quest/QuestMod.h"
#include "../MapMod/MapMod.h"
#include "../AntiCheat/AntiCheat.h"
#include "../UnitMod/CreatureMod/CreatureMod.h"
#include "../UnitMod/CharMod/CharMod.h"
#include "../Challenge/challenge.h"
#include "../Challenge/stage.h"
#include "../VIP/VIP.h"
#include "../Switch/Switch.h"
#include "../Lottery/Lottery.h"
#include "../MountVendor/MountVendor.h"
#include "../ExtraEquipment/ExtraEquipment.h"
#include "../StatPoints/StatPoints.h"
#include "../String/myString.h"
#include "../ResetInstance/ResetInstance.h"
#include "../CustomEvent/FixedTimeBG/FixedTimeBG.h"
#include "../ServerAnnounce/ServerAnnounce.h"
#include "../CustomEvent/Event.h"
#include "../scripts/CustomScripts.h"
#include "../FakePlayers/FakePlayers.h"
#include "../SpellMod/SpellMod.h"
#include "../Deadline/Deadline.h"
#include "../LeaderReward/LeaderReward.h"
#include "../Trigger/Trigger.h"
#include "../QuickResponse/QuickResponse.h"
#include "../Command/CustomCommand.h"
#include "../Instance/InstanceDieTele.h"
#include "../GateWay/GateWay.h"
#include "../GCAddon/GCAddon.h"
#include "../Armory/Armory.h"
#include "../Gift/Gift.h"
#include "../SignIn/SignIn.h"
#include "../ItemSet/ItemSet.h"
#include "../Talisman/Talisman.h"
#include "../Skill/Skill.h"
#include "../Rank/Rank.h"
#include "../Faction/Faction.h"
#include "../Morph/Morph.h"
#include "../LuckDraw/LuckDraw.h"
#include "../Recovery/Recovery.h"
#include "../PvP/PvP.h"
#include "../PetMod/PetMod.h"
#include "../AntiFarm/AntiFarm.h"
#include "../Faker/Faker.h"
#include "../RandomEnchant/RandomEnchant.h"
#include "../ItemMod/NoPatchItem.h"
#include "../NPC/NPC.h"
#include "../Reincarnation/Reincarnation.h"
#include "../TalentReq/TalentReq.h"
#include "../EquipmentManager/EquipmentManager.h"
#include "../ZoneAura/ZoneAura.h"
#include "../PlayerLoot/PlayerLoot.h"
#include "../UI/Rune/Rune.h"
#include "../MainFunc/MainFunc.h"
#include "../GS/GS.h"
#include "../AuthCheck/AuthCheck.h"
#include "../CustomEvent/ArenaDuel/ArenaDuel.h"
#include "../CDK/CDK.h"
#include "../CharPvpTop/CharPvpTop.h"
#include "../GvgSys/GvgSys.h"
/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
/*-------------------数据加载区--------------------------------*/
/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/

std::unordered_map<uint32, float> PlayerDropRateMap;
std::unordered_map<uint32, KillerStreakTemplate> KillerStreakMap;
std::unordered_map<uint32, MountAllowedTemplate> MountAllowedMap;
std::unordered_map<uint32, uint32> BuyTransItemMap;
std::vector<std::string> AddonBanVec;
std::unordered_map<uint32, float> AreaVisibilityDistMap;
std::vector<std::string> DirtyWordVector;

void DataLoader::AddLootPl(Player* target, uint32 mod)
{
    std::unordered_map<uint32, float>::iterator iter = PlayerDropRateMap.find(target->GetGUIDLow());
    if (iter != PlayerDropRateMap.end())
    {
        iter->second = mod;
        WorldDatabase.DirectPExecute("UPDATE `_属性调整_玩家掉率修改` SET `掉落倍率`='%u' WHERE (`玩家GUID`='%u')", mod, target->GetGUIDLow());
    }
    else
    {
        PlayerDropRateMap.insert(std::make_pair(target->GetGUIDLow(), mod));
        WorldDatabase.DirectPExecute("INSERT INTO `_属性调整_玩家掉率修改` (`玩家GUID`, `掉落倍率`) VALUES ('%u', '%u')", target->GetGUIDLow(), mod);
    }

}


void DataLoader::Load()
{
	QueryResult result;

	MountAllowedMap.clear();
	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		//		0			1				2					3
		"SELECT 坐骑技能ID,是否允许室内使用,是否允许副本使用,是否允许战场使用,"
		//4				5			6			7		8				9		10			11			12			13
		"禁用地图ID1,禁用地图ID2,禁用地图ID3,禁用地图ID4,禁用地图ID5,禁用地图ID6,禁用地图ID7,禁用地图ID8,禁用地图ID9,禁用地图ID10 FROM _坐骑_使用区域" :	
		//		0			 1			2			3
		"SELECT mountSpellId,indoor,instance,battleground,"
		//4			5		6		7		8		9		10		11		12		13
		"banMap1,banMap2,banMap3,banMap4,banMap5,banMap6,banMap7,banMap8,banMap9,banMap10 FROM _mount_allowed"))
	{
		do
		{
			Field* fields = result->Fetch();
			uint32 mountSpellId = fields[0].GetUInt32();
			MountAllowedTemplate Temp;
			Temp.indoor = fields[1].GetBool();
			Temp.instance = fields[2].GetBool();
			Temp.battleground = fields[3].GetBool();

			for (size_t i = 0; i < BAN_MAP_MAX; i++)
				Temp.BanMap[i] = fields[4 + i].GetInt32();

			MountAllowedMap.insert(std::make_pair(mountSpellId, Temp));
		} while (result->NextRow());
	}

	KillerStreakMap.clear();
	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		//		0		1				2			3		4			5
		"SELECT 连杀数量, 连杀奖励模板ID,终结奖励模板ID,提示类型,连杀奖励类型,终结奖励类型 FROM __连杀" :
		//		0		1		2		3			4		5
		"SELECT count, rewId,endRewId,announceFlag,rewFlag,endRewFlag FROM _killerstreak"))
	{
		do
		{
			Field* fields = result->Fetch();
			KillerStreakTemplate Temp;
			uint32 num = fields[0].GetUInt32();
			Temp.rewId = fields[1].GetUInt32();
			Temp.endRewId = fields[2].GetUInt32();
			Temp.announceFlag = fields[3].GetUInt32();
			Temp.rewFlag = fields[4].GetUInt32();
			Temp.endRewFlag = fields[5].GetUInt32();
			KillerStreakMap.insert(std::make_pair(num, Temp));
		} while (result->NextRow());
	}
	
	BuyTransItemMap.clear();
	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 物品ID, 需求模板ID FROM _物品_购买幻化" :
		"SELECT entry, reqId FROM _itemmod_buy_trans"))
	{
		do
		{
			Field* fields = result->Fetch();
			BuyTransItemMap.insert(std::make_pair(fields[0].GetUInt32(), fields[1].GetUInt32()));
		} while (result->NextRow());
	}
	
	PlayerDropRateMap.clear();
	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 玩家GUID,掉落倍率 FROM _属性调整_玩家掉率修改" :
		"SELECT guid,rate FROM _rate_charaters"))
	{
		do
		{
			Field* fields = result->Fetch();
			PlayerDropRateMap.insert(std::make_pair(fields[0].GetUInt32(), fields[1].GetFloat()));
		} while (result->NextRow());
	}

	AreaVisibilityDistMap.clear();
	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 地域ID,可视距离 FROM _属性调整_地域可视距离" :
		"SELECT Area,Distance FROM _area_visibility"))
	{
		do
		{
			Field* fields = result->Fetch();
			AreaVisibilityDistMap.insert(std::make_pair(fields[0].GetUInt32(), fields[1].GetFloat()));
		} while (result->NextRow());
	}

	DirtyWordVector.clear();
	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ?
		"SELECT 屏蔽词 FROM __聊天屏蔽词列表" :
		"SELECT DirtyWord FROM _dirtyword"))
	{
		do
		{
			Field* fields = result->Fetch();
			DirtyWordVector.push_back(fields[0].GetString());
		} while (result->NextRow());
	}
}

void DataLoader::LoadAll()
{
	sFixtimeBG->Load();
	sSpellMod->Load();
	sRandomEnchant->Load();
	sSwitch->Load();
	sItemMod->Load();
	sRew->Load();
	sRew->LoadDamCreToSend();
	sReq->Load();
	//sFC->Load();
	sMF->Load();
	sHR->Load();
	sRecruit->Load();
	sQuestMod->Load();
	sMapMod->Load();
	sCreatureMod->Load();
	sCharMod->Load();
	sChallengeMod->Load();
	sVIP->Load();
	sLottery->Load();
	sMountVendor->Load();
	sStage->Load();
	sExtraEquipment->Load();
	sStatPoints->Load();
	sString->Load();
	sResetIns->Load();
	sFTB->Load();
	sServerAnnounce->Load();
	sCustomScript->LoadGobScripts();
	sCustomScript->LoadCreautreScripts();
	sCustomScript->LoadCircleData();
	sDeadline->Load();
	sLeaderReward->Load();
	sTrigger->Load();
	sQuickResponse->Load();
	sCustomCommand->Load();
	sInstanceDieTele->Load();
	sGateWay->Load();
	sGCAddon->Load();
	sArmory->Load();
	sGift->Load();
	sSignIn->Load();
	sTalisman->Load();
	sCustomSkill->Load();
	sRank->Load();
	sFaction->Load();
	sEvent->LoadPos();
	//sMorph->Load();
	sLuckDraw->Load();
	sRecovery->Load();
	sPvP->Load();
	sPetMod->Load();
	sAntiFarm->SetParams();
	Load();
	//sObjectMgr->LoadVendors();
	sNoPatchItem->Load();
	sStory->Load();
	sReincarnation->Load();
	sTalentReq->Load();
	sEM->Load();
	sZoneAura->Load();
	sPlayerLoot->Load();
	sRune->Load();
	sGS->Load();
	sItemMod->InitUIItemEntryData();
	sLFGMgr->LoadRewards();
	sCDK->Load();
	sCharPvpTop->LoadTopSys();
	sGvgSys->LoadgvgSys();
}

class DataLoaderWorldScript : public WorldScript
{
public:
	DataLoaderWorldScript() : WorldScript("DataLoaderWorldScript") {}

	void OnAfterConfigLoad(bool)
	{
		sDataLoader->LoadAll();
		sFaker->Load();
		sArenaDuel->Load();
        sLog->outString("加载自定义数据库完成");
	}
};

class DataReloaderCommand : public CommandScript
{
public:
	DataReloaderCommand() : CommandScript("DataReloaderCommand") { }

    std::vector<ChatCommand> GetCommands() const
	{
		static std::vector<ChatCommand> reloaderCommandTable =
		{
			{ "all", SEC_CONSOLE, true, &HandleRlALLCommand, "" },
			{ "item", SEC_CONSOLE, true, &HandleRlItemCommand, "" },
			{ "spell", SEC_CONSOLE, true, &HandleRlSpellCommand, "" }
		};

		static std::vector<ChatCommand> commandTable =
		{
			{ "rl", SEC_CONSOLE, true, NULL, "", reloaderCommandTable }
		};

		return commandTable;
	}

	static bool HandleRlALLCommand(ChatHandler* handler, const char* /*args*/)
	{
		sDataLoader->LoadAll();

		if (!handler->GetSession())
		{
			sLog->outString(">> [数据库]重载完毕...\n");
			return true;
		}

		if (handler->GetSession()->GetPlayer())
			handler->SendSysMessage("[数据库]重载完毕...");

		return true;
	}

	static bool HandleRlItemCommand(ChatHandler* handler, const char* /*args*/)
	{
		sObjectMgr->LoadItemTemplates();

		if (!handler->GetSession())
		{
			sLog->outString(">> [装备修改数据表]重载完毕...\n");
			return true;
		}

		if (handler->GetSession()->GetPlayer())
			handler->SendSysMessage(">> [装备修改数据表]重载完毕...");

		return true;
	}

	static bool HandleRlSpellCommand(ChatHandler* handler, const char* /*args*/)
	{
		sSpellMgr->LoadDbcDataCorrections();

		if (!handler->GetSession())
		{
			sLog->outString(">> [SpellMod数据表]重载完毕...\n");
			return true;
		}

		if (handler->GetSession()->GetPlayer())
			handler->SendSysMessage("[SpellMod数据表]重载完毕...");

		return true;
	}
};

class LoaderDataOnLogin : PlayerScript
{
public:
	LoaderDataOnLogin() : PlayerScript("LoaderDataOnLogin") {}
	void OnLogin(Player* player) override
	{
		//初始化onlineRewardedCount		
		QueryResult result = CharacterDatabase.PQuery("SELECT onlineRewardedCount FROM characters WHERE guid = '%u'", player->GetGUIDLow());
		if (!result)
			player->onlineRewardedCount = 10000;
		else
		{
			Field* fields = result->Fetch();
			player->onlineRewardedCount = fields[0].GetInt32();
		}
	}
};

void AddSC_Data_Loader()
{
	new DataLoaderWorldScript();
	new DataReloaderCommand();
	new LoaderDataOnLogin();
}
