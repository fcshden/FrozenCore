#pragma execution_character_set("utf-8")
#include "MainFunc.h"
#include "../FunctionCollection/FunctionCollection.h"
#include "../Custom/HonorRank/HonorRank.h"
#include "../Lottery/Lottery.h"
#include "../CommonFunc/CommonFunc.h"
#include "../Recruit/Recruit.h"
#include "../CustomEvent/Event.h"
#include "Guild.h"
#include "../Requirement/Requirement.h"
#include "../Reward/Reward.h"
#include "../Quest/QuestMod.h"
#include "../VIP/VIP.h"
#include "Group.h"
#include "MapManager.h"
#include "../ResetInstance/ResetInstance.h"
#include "../Switch/Switch.h"
#include "../Skill/Skill.h"
#include "../Rank/Rank.h"
#include "../Faction/Faction.h"
#include "../Morph/Morph.h"
#include "../GCAddon/GCAddon.h"
#include "../LuckDraw/LuckDraw.h"
#include "../SignIn/SignIn.h"
#include "../Recovery/Recovery.h"
#include "../StatPoints/StatPoints.h"
#include "../Command/CustomCommand.h"
#include "../Reincarnation/Reincarnation.h"
#include "../String/myString.h"
#include "../UnitMod/CharMod/CharMod.h"
#include "../Talisman/Talisman.h"
#include "../AuthCheck/AuthCheck.h"
#include "../CDK/CDK.h"

//ALTER TABLE _function ADD PopText TEXT	DEFAULT NULL AFTER MenuText;

std::vector<MainFuncTemplate> MainFuncVec;
bool GreaterSort(MainFuncTemplate a, MainFuncTemplate b) { return (a.CurMenu <b.CurMenu); }

void MainFunc::Load()
{
	MainFuncVec.clear();
	
	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		//			0				1       2
		"SELECT 生物物品或物体ID, 菜单ID, 上级菜单ID, "
		//3			4			5			6			7		8		9				10			11	12		13		14
		"GM命令组, 需求模板ID, 奖励模板ID, 小图标ID, 大图标, 菜单文本, 联盟传送坐标ID, 部落传送坐标ID, 类型, 阵营, 功能选择,弹窗文本 FROM __主功能配置" :
		//			0     1       2
		"SELECT Entry, MenuId, PreMenuId, "
		//3        4		5		6			7		8		9		10		11		12		13		14
		"Command, ReqId, RewId, SmallIcon, BigIcon, MenuText, Pos_A, Pos_H, AgentType,TeamId, FuncType,PopText FROM _function");

	if (!result) 
		return;
	do
	{
		Field* fields = result->Fetch();
		MainFuncTemplate Temp;

		Temp.Entry		= fields[0].GetUInt32();
		Temp.CurMenu	= fields[1].GetUInt32();
		Temp.PreMenu	= fields[2].GetUInt32();
		Temp.Command	= fields[3].GetString();
		Temp.ReqId		= fields[4].GetUInt32();
		Temp.RewId		= fields[5].GetUInt32();
		Temp.SmallIcon	= fields[6].GetUInt8();
		Temp.BigIcon	= fields[7].GetString();
		Temp.Text		= fields[8].GetString();
		Temp.Pos_A		= fields[9].GetUInt32();
		Temp.Pos_H		= fields[10].GetUInt32();
		Temp.PopText	= fields[14].GetString();

		const char*  str = fields[11].GetCString();

		if (strcmp("Item", str) == 0)
			Temp.AgentType = MF_TYPE_ITEM;
		else if (strcmp("NPC", str) == 0)
			Temp.AgentType = MF_TYPE_CREATURE;
		else if (strcmp("GameObject", str) == 0)
			Temp.AgentType = MF_TYPE_GAMEOBJECT;
		else
			Temp.AgentType = MF_TYPE_NONE;


		str = fields[12].GetCString();

		if (strcmp("联盟", str) == 0)
			Temp.TeamId = TEAM_ALLIANCE;
		else if (strcmp("部落", str) == 0)
			Temp.TeamId = TEAM_HORDE;
		else
			Temp.TeamId = TEAM_NEUTRAL;


		str = fields[13].GetCString();

		if (strcmp("返回主菜单", str) == 0)
			Temp.FuncType = MF_MAIN_MENU;
		else if (strcmp("返回上级菜单", str) == 0)
			Temp.FuncType = MF_BACK_MENU;
		else if (strcmp("角色修改名字", str) == 0)
			Temp.FuncType = MF_MOD_CHAR_NAME;
		else if (strcmp("角色修改种族", str) == 0)
			Temp.FuncType = MF_MOD_CHAR_RACE;
		else if (strcmp("角色修改阵营", str) == 0)
			Temp.FuncType = MF_MOD_CHAR_FACTION;
		else if (strcmp("角色修改外观", str) == 0)
			Temp.FuncType = MF_MOD_CHAR_CUSTOMIZE;
		else if (strcmp("角色重置天赋", str) == 0)
			Temp.FuncType = MF_RESET_TALENTS;
		else if (strcmp("角色武器技能全满", str) == 0)
			Temp.FuncType = MF_UPGRADE_WEAPON_SKILLS;
		else if (strcmp("积分查询", str) == 0)
			Temp.FuncType = MF_QUERY_TOKEN;
		else if (strcmp("脱离战斗", str) == 0)
			Temp.FuncType = MF_COMBATE_STOP;
		else if (strcmp("泡点", str) == 0)
			Temp.FuncType = MF_ABTAIN_TIME_REWARD;
		else if (strcmp("彩票", str) == 0)
			Temp.FuncType = MF_BUY_LOTTERY;
		else if (strcmp("兑换码", str) == 0)
			Temp.FuncType = MF_CDK;
		else if (strcmp("活动列表", str) == 0)
			Temp.FuncType = MF_SHOW_ACTIVE_EVENTS;
		else if (strcmp("军衔", str) == 0)
			Temp.FuncType = MF_UPGRADE_HR;
		else if (strcmp("招募", str) == 0)
			Temp.FuncType = MF_RECRUIT;
		else if (strcmp("任务传送", str) == 0)
			Temp.FuncType = MF_QUEST_TELE;
		else if (strcmp("会员", str) == 0)
			Temp.FuncType = MF_UPGRADE_VIP;
		else if (strcmp("转生", str) == 0)
			Temp.FuncType = MF_REINCARNATION;
		else if (strcmp("重置特定副本", str) == 0)
			Temp.FuncType = MF_RESET_INSTANCE;
		else if (strcmp("自定义商业技能", str) == 0)
			Temp.FuncType = MF_CUSTOM_SKILL;
		else if (strcmp("自定义等级", str) == 0)
			Temp.FuncType = MF_RANK;
		else if (strcmp("自定义阵营", str) == 0)
			Temp.FuncType = MF_FACTION;
		else if (strcmp("重置日常任务", str) == 0)
			Temp.FuncType = MF_RESET_DAILY_QUEST;
		else if (strcmp("抽奖", str) == 0)
			Temp.FuncType = MF_LUCKDRAW;
		else if (strcmp("物品回收", str) == 0)
			Temp.FuncType = MF_RECOVERY;
		else if (strcmp("法宝", str) == 0)
			Temp.FuncType = MF_TALISMAN;
		else if (strcmp("斗气", str) == 0)
			Temp.FuncType = MF_STATPOINTS;
		else if (strcmp("随机任务", str) == 0)
			Temp.FuncType = MF_RANDOM_QUEST;
		else if (strcmp("重置所有副本", str) == 0)
			Temp.FuncType = MF_RESET_INSTANCE_ALL;
		else if (strcmp("绑定炉石点", str) == 0)
			Temp.FuncType = MF_HOME_BIND;
		else if (strcmp("传送炉石点", str) == 0)
			Temp.FuncType = MF_HOME_TELE;
		else if (strcmp("修理", str) == 0)
			Temp.FuncType = MF_REPAIR;
		else if (strcmp("银行", str) == 0)
			Temp.FuncType = MF_BANK;
		else if (strcmp("邮箱", str) == 0)
			Temp.FuncType = MF_MAIL;
		else if (strcmp("转职", str) == 0)
			Temp.FuncType = MF_ALT_CLASS;
		else
			Temp.FuncType = MF_NONE;

		MainFuncVec.push_back(Temp);
	} while (result->NextRow());

	sort(MainFuncVec.begin(), MainFuncVec.end(), GreaterSort);//升序排列
}

bool MainFunc::Pop(uint32 ReqId)
{
	auto  itr = ReqMap.find(ReqId);
	if (itr != ReqMap.end())
		return true;

	return false;
}

void MainFunc::ClearMenu(Player* pl)
{
	pl->PlayerTalkClass->ClearMenus();
}

void MainFunc::CloseMenu(Player* pl)
{
	pl->CLOSE_GOSSIP_MENU();
}

bool MainFunc::CheckTeamId(Player*  pl, TeamId teamId)
{
	if (teamId == TEAM_NEUTRAL || pl->GetTeamId() == teamId)
		return true;

	return false;
}

void MainFunc::TelePort(Player* pl, uint32 PosId)
{
	if (PosId == 0)
		return;

	auto itr = PosMap.find(PosId);

	if (itr != PosMap.end())
		pl->TeleportTo(itr->second.map, itr->second.x, itr->second.y, itr->second.z, itr->second.o);
}

MFAgentTypes MainFunc::GetAgentType(Object* obj)
{
	switch (obj->GetTypeId())
	{
	case TYPEID_ITEM:
		return MF_TYPE_ITEM;
	case TYPEID_UNIT:
		return MF_TYPE_CREATURE;
	case TYPEID_GAMEOBJECT:
		return MF_TYPE_GAMEOBJECT;
	}

	return MF_TYPE_NONE;
}


void MainFunc::GetExtraData(Player* pl, Object* obj, uint32 CurMenu, uint32 PreMenu, uint32 &RewId, std::string &Command, uint32 &PosId)
{
	MFAgentTypes AgentType = GetAgentType(obj);
	uint32 Entry = obj->GetEntry();

	for (auto itr = MainFuncVec.begin(); itr != MainFuncVec.end(); itr++)
		if (AgentType == itr->AgentType && Entry == itr->Entry && itr->CurMenu == CurMenu && itr->PreMenu == PreMenu)
		{
			RewId = itr->RewId;
			Command = itr->Command;
			if (pl->GetTeamId() == TEAM_ALLIANCE)
				PosId = itr->Pos_A;
			else
				PosId = itr->Pos_H;
			break;
		}		
}

uint32 MainFunc::GetPreMenu(Object* obj, uint32 CurMenu)
{
	MFAgentTypes AgentType = GetAgentType(obj);
	uint32 Entry = obj->GetEntry();

	for (auto itr = MainFuncVec.begin(); itr != MainFuncVec.end(); itr++)
		if (AgentType == itr->AgentType && Entry == itr->Entry && itr->CurMenu == CurMenu)
			return itr->PreMenu;

	return 0;
}

bool MainFunc::HasNextMenu(Object* obj, uint32 Menu)
{
	MFAgentTypes AgentType = GetAgentType(obj);
	uint32 Entry = obj->GetEntry();

	for (auto itr = MainFuncVec.begin(); itr != MainFuncVec.end(); itr++)
		if (AgentType == itr->AgentType && Entry == itr->Entry && itr->PreMenu == Menu)
			return true;

	return false;
}

void MainFunc::AddGossip(Player* pl, Object* obj, uint32 PreMenu)
{
	ClearMenu(pl);

	uint32 Entry = obj->GetEntry();
	MFAgentTypes AgentType = GetAgentType(obj);
	
	for (auto itr = MainFuncVec.begin(); itr != MainFuncVec.end(); itr++)
	{
		if (AgentType == itr->AgentType && Entry == itr->Entry && itr->PreMenu == PreMenu && CheckTeamId(pl, itr->TeamId))
		{
			uint32 ReqId = itr->ReqId;
			uint32 sender = HL(itr->PreMenu, itr->CurMenu);
			uint32 action = HL(itr->FuncType, ReqId);

			std::string text = itr->BigIcon.empty() ? itr->Text : "|TInterface/ICONS/" + itr->BigIcon + ":30:30:0:0|t" + itr->Text;

			if (Pop(itr->ReqId))
				pl->ADD_GOSSIP_ITEM_EXTENDED(itr->SmallIcon, text, sender, action, sReq->Notice(pl, ReqId, itr->Text, ""), sReq->Golds(ReqId), false);
			else if (!itr->PopText.empty())
				pl->ADD_GOSSIP_ITEM_EXTENDED(itr->SmallIcon, text, sender, action, itr->PopText, 0, false);
			else
				pl->ADD_GOSSIP_ITEM(itr->SmallIcon, text, sender, action);
		}
	}

	if (obj->ToCreature())
		pl->SEND_GOSSIP_MENU(obj->GetEntry(), obj->GetGUID());
	else
		pl->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, obj->GetGUID());
}

void MainFunc::DoAction(Player* pl, Object* obj, uint32 sender, uint32 action)
{
	ClearMenu(pl);

	switch (sender)
	{
	case SENDER_HR_MENU:
		sHR->AddNextHRMenu(pl, obj);
		return;
	case SENDER_HR_UP:
		sHR->SetHRTitle(pl, action);
		CloseMenu(pl);
		return;
	case SENDER_VIP_CURR:
		sVIP->AddGossip(pl, obj);
		return;
	case SENDER_VIP_UP:
		sVIP->Up(pl);
		CloseMenu(pl);
		return;
	case SENDER_QUEST_TELE:
		sQuestMod->Tele(pl, action);
		CloseMenu(pl);
		return;
	case SENDER_RESET_INS:
		sResetIns->Action(pl, action);
		CloseMenu(pl);
		return;
	case SENDER_REINCARNATION:
		sReincarnation->DoAction(pl, obj, action);
		return;
	case SENDER_CUSTOM_EVENT_ACTIVE:
		sEvent->AcceptInvitation(pl, action + 1000000);
		CloseMenu(pl);
		return;
	case SENDER_CUSTOM_EVENT_DEACTIVE:
		sEvent->AddEventList(pl, obj);
		return;
	case SENDER_CUSTOM_SKILL:
		sCustomSkill->Action(pl, action, obj);
		return;
	case SENDER_RANK:
		sRank->Action(pl, action, obj);
		return;
	case SENDER_FACTION:
		sFaction->Action(pl, action, obj);
		return;
	case SENDER_ALT_CLASS:
		sCharMod->ModClass(pl, action);
		CloseMenu(pl);
		return;
	}

	uint32 PreMenu = H(sender);
	uint32 CurMenu = L(sender);
	MFTypes FuncType = MFTypes(H(action));
	uint32 ReqId = L(action);

	//需求检测
	if (!sReq->Check(pl, ReqId))
	{
		//显示当前菜单
		AddGossip(pl, obj, PreMenu);
		return;
	}
	sReq->Des(pl, ReqId);

	//奖励 命令 传送
	uint32 RewId = 0;
	std::string Command = "";
	uint32 PosId = 0;
	GetExtraData(pl, obj, CurMenu, PreMenu, RewId, Command, PosId);
	sRew->Rew(pl, RewId);
	TelePort(pl, PosId);

	//if (!Command.empty())
		//return;

	//添加下级菜单
	AddGossip(pl, obj, CurMenu);

	//执行功能
	switch (FuncType)
	{
	case MF_NONE:
		//若没有下级菜单，显示当前菜单
		if (!HasNextMenu(obj, CurMenu))
			AddGossip(pl, obj, PreMenu);
		break;
	case MF_MAIN_MENU:
		AddGossip(pl, obj, 0);
		break;
	case MF_BACK_MENU:
		//返回上级菜单
		AddGossip(pl, obj, GetPreMenu(obj, PreMenu));
		return;
	case MF_MOD_CHAR_NAME:
		pl->SetAtLoginFlag(AT_LOGIN_RENAME);
		pl->GetSession()->SendAreaTriggerMessage("重新上线以更改[名字]");
		CloseMenu(pl);
		break;
	case MF_MOD_CHAR_RACE:
	{
		pl->SetAtLoginFlag(AT_LOGIN_CHANGE_RACE);
		pl->GetSession()->SendAreaTriggerMessage("重新上线以更改[种族]");
		if (Guild* guild = pl->GetGuild())
			guild->HandleLeaveMember(pl->GetSession());
		CloseMenu(pl);
	}
		break;
	case MF_MOD_CHAR_FACTION:
	{
		pl->SetAtLoginFlag(AT_LOGIN_CHANGE_FACTION);
		pl->GetSession()->SendAreaTriggerMessage("重新上线以更改[阵营]");
		if (Guild* guild = pl->GetGuild())
			guild->HandleLeaveMember(pl->GetSession());
		CloseMenu(pl);
	}
		break;
	case MF_MOD_CHAR_CUSTOMIZE:
	{
		pl->SetAtLoginFlag(AT_LOGIN_CUSTOMIZE);
		pl->GetSession()->SendAreaTriggerMessage("重新上线以更改[外观]");
		if (Guild* guild = pl->GetGuild())
			guild->HandleLeaveMember(pl->GetSession());
		CloseMenu(pl);
	}
		break;
	case MF_RESET_TALENTS:
		pl->resetTalents(true);
		pl->SendTalentsInfoData(false);
		pl->GetSession()->SendAreaTriggerMessage("已重置角色天赋");
		CloseMenu(pl);
		break;
	case MF_UPGRADE_WEAPON_SKILLS:
		pl->UpdateSkillsToMaxSkillsForLevel();
		pl->GetSession()->SendAreaTriggerMessage("武器技能已提升至最大值");
		CloseMenu(pl);
		break;
	case MF_QUERY_TOKEN:
	{
		std::ostringstream oss;
		oss << "你当前的" << sString->GetText(CORE_STR_TYPES(STR_TOKEN)) << "数量：" << sCF->GetTokenAmount(pl);
		pl->GetSession()->SendAreaTriggerMessage(oss.str().c_str());
		ChatHandler(pl->GetSession()).PSendSysMessage(oss.str().c_str());
		CloseMenu(pl);
	}
		break;
	case MF_COMBATE_STOP:
	{
		if (!pl->canStopCombat && sSwitch->GetValue(ST_STOP_COMBAT_CD) != 0)
		{
			uint32 cd = pl->stopCombatCD / IN_MILLISECONDS;
			std::ostringstream oss;
			oss << "脱战功能将在" << cd << "秒后冷却！";
			pl->GetSession()->SendNotification(oss.str().c_str());
		}
		else
		{
			pl->ClearInCombat();
			pl->GetSession()->SendAreaTriggerMessage("已清除战斗状态");
			pl->canStopCombat = false;
			pl->stopCombatCD = sSwitch->GetValue(ST_STOP_COMBAT_CD) * IN_MILLISECONDS;
		}
		CloseMenu(pl);
	}
		break;
	case MF_ABTAIN_TIME_REWARD:
	{
		if (PlayerTimeRewType != 1)
		{
			if (PlayerTimeRewType != 2)
				pl->GetSession()->SendNotification("泡点奖励已关闭！");
			else
				pl->GetSession()->SendNotification("领取泡点已关闭，请保持在线，等待系统发放奖励");
		}
		else
		{
			int32 unRewardNum = pl->GetTotalPlayedTime() / 3600 - pl->onlineRewardedCount;
			if (unRewardNum <= 0)
			{
				uint32 minu = pl->GetTotalPlayedTime() > 3600 ? uint32(60 - (float(pl->GetTotalPlayedTime()) / 3600 - pl->GetTotalPlayedTime() / 3600) * 60) : (60 - pl->GetTotalPlayedTime() / 60);
				std::ostringstream oss;
				oss << minu << "分钟后可领取泡点";
				pl->GetSession()->SendNotification(oss.str().c_str());
				ChatHandler(pl->GetSession()).PSendSysMessage(oss.str().c_str());
			}
			else
			{
				sCF->SetOnlineRewardedCount(pl, unRewardNum + pl->onlineRewardedCount);
				for (int32 i = 0; i < unRewardNum; i++)
					sRew->Rew(pl, pl->timeRewId);
			}		
		}
		CloseMenu(pl);
	}
		break;
	case MF_BUY_LOTTERY:
		sLottery->AddLotteryGossip(pl, obj);
		break;
	case MF_CDK:
		sCDK->AddGossip(pl, obj);
		break;
	case MF_SHOW_ACTIVE_EVENTS:
		sEvent->AddEventList(pl, obj);
		break;
	case MF_UPGRADE_HR:
		sHR->AddNextHRMenu(pl, obj);
		break;
	case MF_RECRUIT:
		sRecruit->AddMainMenu(pl, obj);
		break;
	case MF_QUEST_TELE:
		sQuestMod->AddTeleGossip(pl, obj);
		break;
	case MF_UPGRADE_VIP:
		sVIP->AddGossip(pl, obj);
		break;
	case MF_REINCARNATION:
		sReincarnation->AddGossip(pl, obj);
		break;
	case MF_RESET_INSTANCE:
		sResetIns->AddGossip(pl, obj);
		break;
	case MF_CUSTOM_SKILL:
		sCustomSkill->AddGossip(pl, obj);
		break;
	case MF_RANK:
		sRank->AddGossip(pl, obj);
		break;
	case MF_FACTION:
		sFaction->AddGossip(pl, obj);
		break;
	case MF_RESET_DAILY_QUEST:
		pl->ResetDailyQuestStatus();
		CloseMenu(pl);
		break;
	case MF_LUCKDRAW:
		sLuckDraw->OpenPanel(pl);
		CloseMenu(pl);
		break;
	case MF_TALISMAN:
		sTalisman->SendPacket(pl);
		CloseMenu(pl);
		break;
	case MF_RECOVERY:
		sRecovery->OpenPanel(pl);
		CloseMenu(pl);
		break;
	case MF_STATPOINTS:
		sStatPoints->OpenPanel(pl);
		CloseMenu(pl);
		break;
	case MF_RANDOM_QUEST:
		sQuestMod->AddAllRandomQuest(pl);
		CloseMenu(pl);
		break;
	case MF_RESET_INSTANCE_ALL:
		sCustomCommand->DoCommand(pl, ".instance unbind all");
		CloseMenu(pl);
		break;
	case MF_HOME_BIND:
		if (AreaTableEntry const* areaEntry = GetAreaEntryByAreaID(pl->GetAreaId()))
		{
			if (pl->GetMap()->Instanceable())
				ChatHandler(pl->GetSession()).PSendSysMessage("你处在副本、竞技场或战场中，炉石点不能绑定到当前位置：%s", areaEntry->area_name[4]);
			else
			{
				pl->SetHomebind(pl->GetWorldLocation(), pl->GetAreaId());
				ChatHandler(pl->GetSession()).PSendSysMessage("炉石点已经绑定到当前位置：%s", areaEntry->area_name[4]);
			}
		}
		CloseMenu(pl);
		break;
	case MF_HOME_TELE:
		pl->TeleportTo(pl->m_homebindMapId, pl->m_homebindX, pl->m_homebindY, pl->m_homebindZ, pl->GetOrientation());
		CloseMenu(pl);
		break;
	case MF_REPAIR:
		pl->DurabilityRepairAll(false, 0, false);
		pl->GetSession()->SendAreaTriggerMessage("修理完成");
		CloseMenu(pl);
		break;
	case MF_BANK:
		pl->GetSession()->SendShowBank(pl->GetGUID());
		CloseMenu(pl);
		break;
	case MF_MAIL:
		if (!pl->HasSpellCooldown(54710))
		{
			pl->CastSpell(pl, 54710, true);
			pl->AddSpellCooldown(54710, 0, 90 * IN_MILLISECONDS);
		}
		else
			pl->GetSession()->SendNotification("邮箱功能冷却剩余时间%d秒", pl->GetSpellCooldownDelay(54710) / IN_MILLISECONDS);
		CloseMenu(pl);
		break;
	case MF_ALT_CLASS:
		sCharMod->AddGossip(pl, obj);
		break;
	}

	sCustomCommand->DoCommand(pl, Command);
}


class Func_ItemScript : public ItemScript
{
public:
	Func_ItemScript() : ItemScript("Func_ItemScript") { }

	bool OnUse(Player* player, Item* item, SpellCastTargets const& targets) override
	{
		sMF->AddGossip(player, item, 0);
		return true;
	}

	void OnGossipSelect(Player* player, Item* item, uint32 sender, uint32 action) override
	{
		if (sRecruit->AddSubMenuOrDoAction(player, item, sender, action))
			return;

		sMF->DoAction(player, item, sender, action);
	}

	void OnGossipSelectCode(Player* player, Item* item, uint32 sender, uint32 action, const char* code) override
	{
		player->PlayerTalkClass->ClearMenus();

		if (!*code)
			return;

		if (sCDK->Redeem(player, sender, action, code))
			return;

		if (sRecruit->RecruitFriend(player, sender, code))
			return;

		if (sLottery->PlayerBuyLottery(player, LotteryType(sender), (uint32)atoi(code)))
			return;
	}

};

class Func_CreatureScript : public CreatureScript
{
public:
	Func_CreatureScript() : CreatureScript("Func_CreatureScript") { }


	bool OnGossipHello(Player* player, Creature* creature) override
	{
		sMF->AddGossip(player, creature, 0);
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
	{
		if (sRecruit->AddSubMenuOrDoAction(player, creature, sender, action))
			return true;
		
		sMF->DoAction(player, creature, sender, action);
        
		return true;
	}

	bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code) override
	{
		player->PlayerTalkClass->ClearMenus();

		if (!*code)
			return false;

		if (sCDK->Redeem(player, sender, action, code))
			return true;

		if (sRecruit->RecruitFriend(player, sender, code))
			return true;

		if (sLottery->PlayerBuyLottery(player, LotteryType(sender), (uint32)atoi(code)))
			return true;
		return false;
	}
};

class Func_GameObjectScript :public GameObjectScript
{
public:
	Func_GameObjectScript() : GameObjectScript("Func_GameObjectScript") { }

	bool OnGossipHello(Player* player, GameObject* go) override
	{
		sMF->AddGossip(player, go, 0);
		return true;
	}

	bool OnGossipSelect(Player* player, GameObject* go, uint32 sender, uint32 action) override
	{
		if (sRecruit->AddSubMenuOrDoAction(player, go, sender, action))
			return true;

		sMF->DoAction(player, go, sender, action);
		return true;
	}

	bool OnGossipSelectCode(Player* player, GameObject* go, uint32 sender, uint32 action, const char* code) override
	{
		player->PlayerTalkClass->ClearMenus();

		if (!*code)
			return false;

		if (sCDK->Redeem(player, sender, action, code))
			return true;

		if (sRecruit->RecruitFriend(player, sender, code))
			return true;

		if (sLottery->PlayerBuyLottery(player, LotteryType(sender), (uint32)atoi(code)))
			return true;
		return false;
	}
};

class Func_PlayerScript : PlayerScript
{
public:
	Func_PlayerScript() : PlayerScript("Func_PlayerScript") {}
	void OnLogin(Player* player)
	{
		sCF->SetCommercePoints(player);
	}
};

void AddSC_MainFunc()
{
	new Func_PlayerScript();
	new Func_ItemScript();
	new Func_CreatureScript();
	new Func_GameObjectScript();
}
