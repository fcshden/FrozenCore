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
		"SELECT ������Ʒ������ID, �˵�ID, �ϼ��˵�ID, "
		//3			4			5			6			7		8		9				10			11	12		13		14
		"GM������, ����ģ��ID, ����ģ��ID, Сͼ��ID, ��ͼ��, �˵��ı�, ���˴�������ID, ���䴫������ID, ����, ��Ӫ, ����ѡ��,�����ı� FROM __����������" :
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

		if (strcmp("����", str) == 0)
			Temp.TeamId = TEAM_ALLIANCE;
		else if (strcmp("����", str) == 0)
			Temp.TeamId = TEAM_HORDE;
		else
			Temp.TeamId = TEAM_NEUTRAL;


		str = fields[13].GetCString();

		if (strcmp("�������˵�", str) == 0)
			Temp.FuncType = MF_MAIN_MENU;
		else if (strcmp("�����ϼ��˵�", str) == 0)
			Temp.FuncType = MF_BACK_MENU;
		else if (strcmp("��ɫ�޸�����", str) == 0)
			Temp.FuncType = MF_MOD_CHAR_NAME;
		else if (strcmp("��ɫ�޸�����", str) == 0)
			Temp.FuncType = MF_MOD_CHAR_RACE;
		else if (strcmp("��ɫ�޸���Ӫ", str) == 0)
			Temp.FuncType = MF_MOD_CHAR_FACTION;
		else if (strcmp("��ɫ�޸����", str) == 0)
			Temp.FuncType = MF_MOD_CHAR_CUSTOMIZE;
		else if (strcmp("��ɫ�����츳", str) == 0)
			Temp.FuncType = MF_RESET_TALENTS;
		else if (strcmp("��ɫ��������ȫ��", str) == 0)
			Temp.FuncType = MF_UPGRADE_WEAPON_SKILLS;
		else if (strcmp("���ֲ�ѯ", str) == 0)
			Temp.FuncType = MF_QUERY_TOKEN;
		else if (strcmp("����ս��", str) == 0)
			Temp.FuncType = MF_COMBATE_STOP;
		else if (strcmp("�ݵ�", str) == 0)
			Temp.FuncType = MF_ABTAIN_TIME_REWARD;
		else if (strcmp("��Ʊ", str) == 0)
			Temp.FuncType = MF_BUY_LOTTERY;
		else if (strcmp("�һ���", str) == 0)
			Temp.FuncType = MF_CDK;
		else if (strcmp("��б�", str) == 0)
			Temp.FuncType = MF_SHOW_ACTIVE_EVENTS;
		else if (strcmp("����", str) == 0)
			Temp.FuncType = MF_UPGRADE_HR;
		else if (strcmp("��ļ", str) == 0)
			Temp.FuncType = MF_RECRUIT;
		else if (strcmp("������", str) == 0)
			Temp.FuncType = MF_QUEST_TELE;
		else if (strcmp("��Ա", str) == 0)
			Temp.FuncType = MF_UPGRADE_VIP;
		else if (strcmp("ת��", str) == 0)
			Temp.FuncType = MF_REINCARNATION;
		else if (strcmp("�����ض�����", str) == 0)
			Temp.FuncType = MF_RESET_INSTANCE;
		else if (strcmp("�Զ�����ҵ����", str) == 0)
			Temp.FuncType = MF_CUSTOM_SKILL;
		else if (strcmp("�Զ���ȼ�", str) == 0)
			Temp.FuncType = MF_RANK;
		else if (strcmp("�Զ�����Ӫ", str) == 0)
			Temp.FuncType = MF_FACTION;
		else if (strcmp("�����ճ�����", str) == 0)
			Temp.FuncType = MF_RESET_DAILY_QUEST;
		else if (strcmp("�齱", str) == 0)
			Temp.FuncType = MF_LUCKDRAW;
		else if (strcmp("��Ʒ����", str) == 0)
			Temp.FuncType = MF_RECOVERY;
		else if (strcmp("����", str) == 0)
			Temp.FuncType = MF_TALISMAN;
		else if (strcmp("����", str) == 0)
			Temp.FuncType = MF_STATPOINTS;
		else if (strcmp("�������", str) == 0)
			Temp.FuncType = MF_RANDOM_QUEST;
		else if (strcmp("�������и���", str) == 0)
			Temp.FuncType = MF_RESET_INSTANCE_ALL;
		else if (strcmp("��¯ʯ��", str) == 0)
			Temp.FuncType = MF_HOME_BIND;
		else if (strcmp("����¯ʯ��", str) == 0)
			Temp.FuncType = MF_HOME_TELE;
		else if (strcmp("����", str) == 0)
			Temp.FuncType = MF_REPAIR;
		else if (strcmp("����", str) == 0)
			Temp.FuncType = MF_BANK;
		else if (strcmp("����", str) == 0)
			Temp.FuncType = MF_MAIL;
		else if (strcmp("תְ", str) == 0)
			Temp.FuncType = MF_ALT_CLASS;
		else
			Temp.FuncType = MF_NONE;

		MainFuncVec.push_back(Temp);
	} while (result->NextRow());

	sort(MainFuncVec.begin(), MainFuncVec.end(), GreaterSort);//��������
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

	//������
	if (!sReq->Check(pl, ReqId))
	{
		//��ʾ��ǰ�˵�
		AddGossip(pl, obj, PreMenu);
		return;
	}
	sReq->Des(pl, ReqId);

	//���� ���� ����
	uint32 RewId = 0;
	std::string Command = "";
	uint32 PosId = 0;
	GetExtraData(pl, obj, CurMenu, PreMenu, RewId, Command, PosId);
	sRew->Rew(pl, RewId);
	TelePort(pl, PosId);

	//if (!Command.empty())
		//return;

	//����¼��˵�
	AddGossip(pl, obj, CurMenu);

	//ִ�й���
	switch (FuncType)
	{
	case MF_NONE:
		//��û���¼��˵�����ʾ��ǰ�˵�
		if (!HasNextMenu(obj, CurMenu))
			AddGossip(pl, obj, PreMenu);
		break;
	case MF_MAIN_MENU:
		AddGossip(pl, obj, 0);
		break;
	case MF_BACK_MENU:
		//�����ϼ��˵�
		AddGossip(pl, obj, GetPreMenu(obj, PreMenu));
		return;
	case MF_MOD_CHAR_NAME:
		pl->SetAtLoginFlag(AT_LOGIN_RENAME);
		pl->GetSession()->SendAreaTriggerMessage("���������Ը���[����]");
		CloseMenu(pl);
		break;
	case MF_MOD_CHAR_RACE:
	{
		pl->SetAtLoginFlag(AT_LOGIN_CHANGE_RACE);
		pl->GetSession()->SendAreaTriggerMessage("���������Ը���[����]");
		if (Guild* guild = pl->GetGuild())
			guild->HandleLeaveMember(pl->GetSession());
		CloseMenu(pl);
	}
		break;
	case MF_MOD_CHAR_FACTION:
	{
		pl->SetAtLoginFlag(AT_LOGIN_CHANGE_FACTION);
		pl->GetSession()->SendAreaTriggerMessage("���������Ը���[��Ӫ]");
		if (Guild* guild = pl->GetGuild())
			guild->HandleLeaveMember(pl->GetSession());
		CloseMenu(pl);
	}
		break;
	case MF_MOD_CHAR_CUSTOMIZE:
	{
		pl->SetAtLoginFlag(AT_LOGIN_CUSTOMIZE);
		pl->GetSession()->SendAreaTriggerMessage("���������Ը���[���]");
		if (Guild* guild = pl->GetGuild())
			guild->HandleLeaveMember(pl->GetSession());
		CloseMenu(pl);
	}
		break;
	case MF_RESET_TALENTS:
		pl->resetTalents(true);
		pl->SendTalentsInfoData(false);
		pl->GetSession()->SendAreaTriggerMessage("�����ý�ɫ�츳");
		CloseMenu(pl);
		break;
	case MF_UPGRADE_WEAPON_SKILLS:
		pl->UpdateSkillsToMaxSkillsForLevel();
		pl->GetSession()->SendAreaTriggerMessage("�������������������ֵ");
		CloseMenu(pl);
		break;
	case MF_QUERY_TOKEN:
	{
		std::ostringstream oss;
		oss << "�㵱ǰ��" << sString->GetText(CORE_STR_TYPES(STR_TOKEN)) << "������" << sCF->GetTokenAmount(pl);
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
			oss << "��ս���ܽ���" << cd << "�����ȴ��";
			pl->GetSession()->SendNotification(oss.str().c_str());
		}
		else
		{
			pl->ClearInCombat();
			pl->GetSession()->SendAreaTriggerMessage("�����ս��״̬");
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
				pl->GetSession()->SendNotification("�ݵ㽱���ѹرգ�");
			else
				pl->GetSession()->SendNotification("��ȡ�ݵ��ѹرգ��뱣�����ߣ��ȴ�ϵͳ���Ž���");
		}
		else
		{
			int32 unRewardNum = pl->GetTotalPlayedTime() / 3600 - pl->onlineRewardedCount;
			if (unRewardNum <= 0)
			{
				uint32 minu = pl->GetTotalPlayedTime() > 3600 ? uint32(60 - (float(pl->GetTotalPlayedTime()) / 3600 - pl->GetTotalPlayedTime() / 3600) * 60) : (60 - pl->GetTotalPlayedTime() / 60);
				std::ostringstream oss;
				oss << minu << "���Ӻ����ȡ�ݵ�";
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
				ChatHandler(pl->GetSession()).PSendSysMessage("�㴦�ڸ�������������ս���У�¯ʯ�㲻�ܰ󶨵���ǰλ�ã�%s", areaEntry->area_name[4]);
			else
			{
				pl->SetHomebind(pl->GetWorldLocation(), pl->GetAreaId());
				ChatHandler(pl->GetSession()).PSendSysMessage("¯ʯ���Ѿ��󶨵���ǰλ�ã�%s", areaEntry->area_name[4]);
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
		pl->GetSession()->SendAreaTriggerMessage("�������");
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
			pl->GetSession()->SendNotification("���书����ȴʣ��ʱ��%d��", pl->GetSpellCooldownDelay(54710) / IN_MILLISECONDS);
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
