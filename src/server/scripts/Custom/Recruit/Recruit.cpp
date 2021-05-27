#pragma execution_character_set("utf-8")
#include "../PrecompiledHeaders/ScriptPCH.h"
#include "Recruit.h"
#include "Pet.h"
#include "GuildMgr.h"
#include "../Custom/Requirement/Requirement.h"
#include "../Custom/Reward/Reward.h"
#include "../CommonFunc/CommonFunc.h"

#define RECRUIT_MENU_ID 8000

std::vector <RecruitTemplate> RecruitInfo;
std::vector<RecruitOrDissTemplate> RecruitOrDissVec;
std::unordered_map<uint32, RucruitLootShareTemplate> RecruitLootMap;

void Recruit::PopMsg(Player* player, uint32 menuId, std::string text)
{
	WorldPacket data(SMSG_GOSSIP_MESSAGE, 100);
	data << uint64(player->GetGUID());
	data << uint32(menuId);
	data << uint32(1);
	data << uint32(1);
	data << uint32(1);
	data << uint8(1);
	data << uint8(0);
	data << uint32(0);
	data << "����";
	data << text;
	player->GetSession()->SendPacket(&data);
}

bool Recruit::IsRecruited(Player* player)
{
	for (size_t i = 0; i < RecruitInfo.size(); i++)
	{
		if (player->GetGUIDLow() == RecruitInfo[i].friendGUIDLow)
			return true;
	}

	return false;
}

bool Recruit::IsRecruitYourRecruiter(uint32 recruiterGUIDLow, uint32 friendGUIDLow)
{
	for (size_t i = 0; i < RecruitInfo.size(); i++)
	{
		if (friendGUIDLow == RecruitInfo[i].recruiterGUIDLow)
		{
			for (size_t j = 0; j < RecruitInfo.size(); j++)
			{
				if (recruiterGUIDLow == RecruitInfo[i].friendGUIDLow)
				{
					return false;
				}
			}
		}
	}

	return true;
}

uint32 Recruit::GetFriendAmount(Player* recruiter)
{
	uint32 friendAmount = 0;
	for (size_t i = 0; i < RecruitInfo.size(); i++)
	{
		if (recruiter->GetGUIDLow() == RecruitInfo[i].recruiterGUIDLow)
			friendAmount++;
	}
	return friendAmount;
}

bool Recruit::RecruitAcceptOrCancel(Player*player, uint32 menuId)
{
	if (menuId != RECRUIT_MENU_ID)
		return false;

	Player* recruiter = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(player->recruiterGUIDLow, 0, HIGHGUID_PLAYER));
	if (player && recruiter)
	{
		sReq->Des(recruiter, GetRecReqId());
		sRew->Rew(recruiter, GetRewId1());
		sRew->Rew(player, GetRewId2());

		RecruitTemplate RecruitTemp;
		RecruitTemp.recruiterGUIDLow = player->recruiterGUIDLow;
		RecruitTemp.friendGUIDLow = player->GetGUIDLow();

		RecruitInfo.push_back(RecruitTemp);

		if (UpdateRecruitDB(player->recruiterGUIDLow, player->GetGUIDLow()))
			if (player->getLevel() < GetInsLevel())
				player->SetLevel(GetInsLevel(), true);

		std::ostringstream oss;
		oss << "|cFFFF1717[��ļϵͳ]|r������������飬[��ļ��]|cFF0177EC" << sCF->GetNameLink(recruiter) << "|r��|cFF0177EC" << sCF->GetNameLink(player) << "|r��������ϵ";
		sWorld->SendScreenMessage(oss.str().c_str());
		sCF->CompleteQuest(recruiter, 30003);
	}

	return true;
}

bool Recruit::UpdateRecruitDB(uint32 recruiterGUIDLow, uint32 friendGUIDLow)
{
	Player* recruiter = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(recruiterGUIDLow, 0, HIGHGUID_PLAYER));
	Player* pFriend = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(friendGUIDLow, 0, HIGHGUID_PLAYER));

	if (!pFriend || !recruiter) 
		return false;

	CharacterDatabase.DirectPExecute("INSERT INTO character_recruit(recruiter,friend,recruiterGUID,friendGUID,friendPlayedTime) VALUES ('%s','%s','%u','%u','%u')", recruiter->GetName().c_str(), pFriend->GetName().c_str(), recruiterGUIDLow, friendGUIDLow, pFriend->GetTotalPlayedTime());

	return true;
}

void Recruit::Load()
{
	RecruitInfo.clear();
	QueryResult result1 = CharacterDatabase.PQuery("SELECT recruiterGUID,friendGUID,friendPlayedTime,timeRewarded1,timeRewarded2,timeRewarded3 from character_recruit");
	if (result1)
	{
		do
		{
			Field* fields = result1->Fetch();
			RecruitTemplate RecruitTemp;
			RecruitTemp.recruiterGUIDLow = fields[0].GetUInt32();
			RecruitTemp.friendGUIDLow = fields[1].GetUInt32();
			RecruitTemp.friendPlayedTime = fields[2].GetUInt32();
			RecruitTemp.timeRewarded1 = fields[3].GetBool();
			RecruitTemp.timeRewarded2 = fields[4].GetBool();
			RecruitTemp.timeRewarded3 = fields[5].GetBool();
			RecruitInfo.push_back(RecruitTemp);
		} while (result1->NextRow());
	}

	RecruitOrDissVec.clear();
	QueryResult result2 = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		//			0				1				2				3						4						5			6					7					8					9					10						11						12					13
		"SELECT ��ļ����ģ��ID,��ļ�߽���ģ��ID,����ļ�߽���ģ��ID,��ļ�߽����ļ����ģ��ID,����ļ�߽����ļ����ģ��ID,��ļ��������,�Ƿ��������Ӫ��ļ,����ļ�����������ȼ�,��ļ�߻�ȡ�����ۼ�ʱ��1,��ļ�߻�ȡ����ģ��ID1,��ļ�߻�ȡ�����ۼ�ʱ��2,��ļ�߻�ȡ����ģ��ID2,��ļ�߻�ȡ�����ۼ�ʱ��3,��ļ�߻�ȡ����ģ��ID3 FROM __��ļ" :
		//			0	1		2		3		4			5			6				7			8			9			10			11				12			13
		"SELECT reqId,rewId1,rewId2,disReqId1,disReqId2,playersLimit,allowCrossFaction,insLevel,timeForRew1,timeForRewId1,timeForRew2,timeForRewId2,timeForRew3,timeForRewId3 FROM _recruit");
	if (result2)
	{
		do
		{
			Field* fields = result2->Fetch();
			RecruitOrDissTemplate RecruitOrDissTemp;
			RecruitOrDissTemp.recruitReqId = fields[0].GetUInt32();
			RecruitOrDissTemp.rewId1 = fields[1].GetUInt32();
			RecruitOrDissTemp.rewId2 = fields[2].GetUInt32();
			RecruitOrDissTemp.disReqId1 = fields[3].GetUInt32();
			RecruitOrDissTemp.disReqId2 = fields[4].GetUInt32();
			RecruitOrDissTemp.playersLimit = fields[5].GetUInt32();
			RecruitOrDissTemp.crossFaction = fields[6].GetBool();
			RecruitOrDissTemp.insLevel = fields[7].GetUInt32();
			RecruitOrDissTemp.timeForRew1 = fields[8].GetUInt32();
			RecruitOrDissTemp.timeForRewId1 = fields[9].GetUInt32();
			RecruitOrDissTemp.timeForRew2 = fields[10].GetUInt32();
			RecruitOrDissTemp.timeForRewId2 = fields[11].GetUInt32();
			RecruitOrDissTemp.timeForRew3 = fields[12].GetUInt32();
			RecruitOrDissTemp.timeForRewId3 = fields[13].GetUInt32();
			RecruitOrDissVec.push_back(RecruitOrDissTemp);
		} while (result2->NextRow());
	}

	RecruitLootMap.clear();
	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ?
		"SELECT ��ƷID,������������, ������,����ļ��ͬʱ��ȡ��������,����ļ��ͬʱ��ȡ������Ʒ���� FROM __��ļ_���乲��" :
		"SELECT entry,shareCountLimit, shareChance,rewChanceOnShare,rewCountLimit FROM _recruit_lootshare");
	if (result)
	{
		do
		{
			Field* fields = result->Fetch();

			uint32 entry = fields[0].GetUInt32();

			RucruitLootShareTemplate Temp;
			Temp.shareCountLimit	= fields[1].GetUInt32();
			Temp.shareChance		= fields[2].GetFloat();
			Temp.rewChanceOnShare	= fields[3].GetFloat();
			Temp.rewCountLimit		= fields[4].GetUInt32();
			RecruitLootMap.insert(std::make_pair(entry, Temp));
		} while (result->NextRow());
	}

}

GlobalPlayerData const* Recruit::GetRecruiterData(Player* player)
{
	if (!player)
		return NULL;

	uint32 recruiterGUIDLow = 0;

	for (uint32 i = 0; i < RecruitInfo.size(); i++)
	{
		if (RecruitInfo[i].friendGUIDLow == player->GetGUIDLow())
		{
			recruiterGUIDLow = RecruitInfo[i].recruiterGUIDLow;
			break;
		}
	}

	GlobalPlayerData const* recruiterPlayerData = sWorld->GetGlobalPlayerData(recruiterGUIDLow);

	if (!recruiterPlayerData)
		return NULL;

	return recruiterPlayerData;
}

void Recruit::GetFriendsDataList(std::vector<GlobalPlayerData const*> &friendsDataList, Player* player)
{
	friendsDataList.clear();

	if (!player)
		return;

	for (uint32 i = 0; i < RecruitInfo.size(); i++)
	{
		if (RecruitInfo[i].recruiterGUIDLow == player->GetGUIDLow())
		{
			GlobalPlayerData const* friendPlayerData = sWorld->GetGlobalPlayerData(RecruitInfo[i].friendGUIDLow);
			if (friendPlayerData)
				friendsDataList.push_back(friendPlayerData);
		}
	}
}

void Recruit::DismissRecruit(uint32 recruiterGUIDLow, uint32 friendGUIDLow)
{
	CharacterDatabase.DirectPExecute("DELETE FROM character_recruit WHERE recruiterGUID = %u AND friendGUID = %u", recruiterGUIDLow, friendGUIDLow);

	for (std::vector<RecruitTemplate>::iterator itr = RecruitInfo.begin(); itr != RecruitInfo.end();)
	{
		if (itr->friendGUIDLow == friendGUIDLow && itr->recruiterGUIDLow == recruiterGUIDLow)
			itr = RecruitInfo.erase(itr);	
		else
			++itr;
	}
}

void Recruit::RecruitItemReward(Player* player, Item* newItem, uint32 count)
{
	if (!newItem)
		return;

	uint32 entry = newItem->GetEntry();
	uint32 recruiterItemCount = 0;
	uint32 followerItemCount = 0;

	GetItemCount(entry, count, recruiterItemCount, followerItemCount);

	if (!recruiterItemCount)
		return;

	GlobalPlayerData const* recruiterPlayerData = sRecruit->GetRecruiterData(player);

	if (!recruiterPlayerData) 
		return;

	Player* recruiter = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(recruiterPlayerData->guidLow, 0, HIGHGUID_PLAYER));

	if (recruiter)
	{
		std::string itemlink = sCF->GetItemLink(newItem->GetEntry());
		ChatHandler(recruiter->GetSession()).PSendSysMessage("|cFFFF1717[��ļϵͳ]|r����|cFF0177EC%s|r�Ĺ������%s X %u", sCF->GetNameLink(player).c_str(), itemlink.c_str(), recruiterItemCount);
		recruiter->AddItem(entry, recruiterItemCount);

		if (followerItemCount)
		{
			player->AddItem(entry, followerItemCount);
			ChatHandler(player->GetSession()).PSendSysMessage("|cFFFF1717[��ļϵͳ]|r|cFF0177EC%s|r�����Ĺ������%s X %u���������ö��⽱��%s X %u", sCF->GetNameLink(recruiter).c_str(), itemlink.c_str(), recruiterItemCount, itemlink.c_str(), followerItemCount);
		}
	}		
}

void Recruit::RecruitMoneyReward(Player* player, uint32 count)
{
	uint32 recruiterItemCount = 0;
	uint32 followerItemCount = 0;

	GetItemCount(0, count, recruiterItemCount, followerItemCount);

	if (!recruiterItemCount)
		return;

	GlobalPlayerData const* recruiterPlayerData = sRecruit->GetRecruiterData(player);
	if (!recruiterPlayerData) return;

	Player* recruiter = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(recruiterPlayerData->guidLow, 0, HIGHGUID_PLAYER));

	if (recruiter)
	{
		recruiter->ModifyMoney(count);

		WorldPacket data(SMSG_LOOT_MONEY_NOTIFY, 4 + 1);
		data << uint32(count);
		data << uint8(1);
		recruiter->GetSession()->SendPacket(&data);
	}

}

void Recruit::RecruitXPReward(Player* player, uint32 xp, Unit* _victim, float _groupRate, Group* _group)
{
	uint32 recruiterItemCount = 0;
	uint32 followerItemCount = 0;

	GetItemCount(2, xp, recruiterItemCount, followerItemCount);

	if (!recruiterItemCount)
		return;

	GlobalPlayerData const* recruiterPlayerData = sRecruit->GetRecruiterData(player);
	if (!recruiterPlayerData) 
		return;

	Player* recruiter = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(recruiterPlayerData->guidLow, 0, HIGHGUID_PLAYER));

	if (recruiter)
	{
		recruiter->GiveXP(xp, _victim, _groupRate);
		if (Pet* pet = recruiter->GetPet())
			pet->GivePetXP(_group ? xp / 2 : xp);
	}
}

void Recruit::RecruitHonorReward(Player* player, uint32 honor)
{
	uint32 recruiterItemCount = 0;
	uint32 followerItemCount = 0;

	GetItemCount(1, honor, recruiterItemCount, followerItemCount);

	if (!recruiterItemCount)
		return;

	GlobalPlayerData const* recruiterPlayerData = sRecruit->GetRecruiterData(player);
	if (!recruiterPlayerData) return;

	Player* recruiter = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(recruiterPlayerData->guidLow, 0, HIGHGUID_PLAYER));

	if (recruiter)
	{
		recruiter->ModifyHonorPoints(honor);
		recruiter->ApplyModUInt32Value(PLAYER_FIELD_TODAY_CONTRIBUTION, honor, true);
	}
}


void Recruit::GetItemCount(uint32 entry, uint32 itemCount, uint32 &recruiterItemCount, uint32 &followerItemCount)
{
	std::unordered_map<uint32, RucruitLootShareTemplate>::iterator iter = RecruitLootMap.find(entry);

	if (iter != RecruitLootMap.end())
		if (frand(0, 100) <= iter->second.shareChance)
		{
			recruiterItemCount = std::min(itemCount, iter->second.shareCountLimit);

			if (frand(0, 100) <= iter->second.rewChanceOnShare)
				followerItemCount = std::min(itemCount, iter->second.rewCountLimit);
		}

}

void Recruit::RecruitTelePort(Player* player, uint32 targetGUIDLow)
{
	if (Player* target = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(targetGUIDLow, 0, HIGHGUID_PLAYER)))
	{
		if (target->InBattleground())
		{
			player->GetSession()->SendNotification("����ʧ�ܣ�Ŀ����ս���У�");
			return;
		}
			
		if (target->GetMap()->IsDungeon())
		{
			player->GetSession()->SendNotification("����ʧ�ܣ�Ŀ���ڸ����У�");
			return;
		}

		player->TeleportTo(target->GetWorldLocation());
	}else
		player->GetSession()->SendNotification("����Ŀ�겻���ߣ�");
}


uint32 Recruit::GetRecReqId()
{
	if (!RecruitOrDissVec.empty())
		return RecruitOrDissVec[0].recruitReqId;
	return 0;
}
uint32 Recruit::GetRewId1()
{
	if (!RecruitOrDissVec.empty())
		return RecruitOrDissVec[0].rewId1;
	return 0;
}
uint32 Recruit::GetRewId2()
{
	if (!RecruitOrDissVec.empty())
		return RecruitOrDissVec[0].rewId2;
	return 0;
}
uint32 Recruit::GetDissReqId1()
{
	if (!RecruitOrDissVec.empty())
		return RecruitOrDissVec[0].disReqId1;
	return 0;
}
uint32 Recruit::GetDissReqId2()
{
	if (!RecruitOrDissVec.empty())
		return RecruitOrDissVec[0].disReqId2;
	return 0;
}
uint32 Recruit::GetPlayersLimit()
{
	if (!RecruitOrDissVec.empty())
		return RecruitOrDissVec[0].playersLimit;
	return 0;
}
bool Recruit::CrossFaction()
{
	if (!RecruitOrDissVec.empty())
		return RecruitOrDissVec[0].crossFaction;
	return false;
}
uint32 Recruit::GetInsLevel()
{
	if (!RecruitOrDissVec.empty())
		return RecruitOrDissVec[0].insLevel;
	return 0;
}
uint32 Recruit::GetTimeForRew1()
{
	if (!RecruitOrDissVec.empty())
		return RecruitOrDissVec[0].timeForRew1;
	return 0;
}
uint32 Recruit::GetTimeForRewId1()
{
	if (!RecruitOrDissVec.empty())
		return RecruitOrDissVec[0].timeForRewId1;
	return 0;
}

uint32 Recruit::GetTimeForRew2()
{
	if (!RecruitOrDissVec.empty())
		return RecruitOrDissVec[0].timeForRew2;
	return 0;
}
uint32 Recruit::GetTimeForRewId2()
{
	if (!RecruitOrDissVec.empty())
		return RecruitOrDissVec[0].timeForRewId2;
	return 0;
}
uint32 Recruit::GetTimeForRew3()
{
	if (!RecruitOrDissVec.empty())
		return RecruitOrDissVec[0].timeForRew3;
	return 0;
}
uint32 Recruit::GetTimeForRewId3()
{
	if (!RecruitOrDissVec.empty())
		return RecruitOrDissVec[0].timeForRewId3;
	return 0;
}

class RecruitWorldScript : public WorldScript
{
public:
	RecruitWorldScript() : WorldScript("RecruitWorldScript") {}

	void OnAfterConfigLoad(bool /*reload*/)
	{
		sRecruit->Load();
	}
};

class RecruitPlayerScript : PlayerScript
{
public:
	RecruitPlayerScript() : PlayerScript("RecruitPlayerScript") {}
	void OnLogin(Player* player) override
	{

		GlobalPlayerData const* recruiterPlayerData = sRecruit->GetRecruiterData(player);

		if (!recruiterPlayerData)
			return;

		uint32 playedTime = 0;
		bool hasReward1 = false;
		bool hasReward2 = false;
		bool hasReward3 = false;
		uint32 recruiterGUIDLow = 0;
		for (uint32 i = 0; i < RecruitInfo.size(); i++)
		{
			if (RecruitInfo[i].friendGUIDLow == player->GetGUIDLow())
			{
				playedTime = RecruitInfo[i].friendPlayedTime;
				hasReward1 = RecruitInfo[i].timeRewarded1;
				hasReward2 = RecruitInfo[i].timeRewarded2;
				hasReward3 = RecruitInfo[i].timeRewarded3;

				recruiterGUIDLow = RecruitInfo[i].recruiterGUIDLow;

				break;
			}
		}
		
		if (!hasReward1 && (player->GetTotalPlayedTime() - playedTime > sRecruit->GetTimeForRew1()) && (sRecruit->GetTimeForRew1() > 0))
		{	
			sRecruit->UpdateHasRewad(player, 1);
			std::ostringstream oss;
			oss << "\n\n�����������[|cFFFF1717" << player->GetName() << "|r]�ĵ�һ����ļ����\n\nף���ڰ�����˹��ð��֮�ó�����Ȥ��";
			sRew->MailRew(player, recruiterGUIDLow, sRecruit->GetTimeForRewId1(), "��ļ����(һ)", oss.str());
		}
		if (!hasReward2 && (player->GetTotalPlayedTime() - playedTime > sRecruit->GetTimeForRew2()) && (sRecruit->GetTimeForRew2() > 0))
		{
			sRecruit->UpdateHasRewad(player, 2);
			std::ostringstream oss;
			oss << "\n\n�����������[|cFFFF1717" << player->GetName() << "|r]�ĵڶ�����ļ����\n\nף���ڰ�����˹��ð��֮�ó�����Ȥ��";		
			sRew->MailRew(player, recruiterGUIDLow, sRecruit->GetTimeForRewId2(), "��ļ����(��)", oss.str());
		}
		if (!hasReward3 && (player->GetTotalPlayedTime() - playedTime > sRecruit->GetTimeForRew3()) && (sRecruit->GetTimeForRew3() > 0))
		{
			sRecruit->UpdateHasRewad(player, 3);
			std::ostringstream oss;
			oss << "n\n�����������[|cFFFF1717" << player->GetName() << "|r]�ĵ�������ļ����\n\nף���ڰ�����˹��ð��֮�ó�����Ȥ��";	
			sRew->MailRew(player, recruiterGUIDLow, sRecruit->GetTimeForRewId3(), "��ļ����(��)", oss.str());
		}
	}
};


void Recruit::UpdateHasRewad(Player* player, uint32 flag)
{
	uint32 len = RecruitInfo.size();
	for (uint32 i = 0; i < len; i++)
		if (RecruitInfo[i].friendGUIDLow == player->GetGUIDLow())
		{
			player->GetSession()->SendNotification("|cFFFF1717[��ļϵͳ]|r��ļ������ѻ�ý���");
			switch (flag)
			{
			case 1:
				CharacterDatabase.DirectPExecute("UPDATE character_recruit SET timeRewarded1 = %d WHERE friendGUID = %d", 1, player->GetGUIDLow());
				RecruitInfo[i].timeRewarded1 = true;
				break;
			case 2:
				CharacterDatabase.DirectPExecute("UPDATE character_recruit SET timeRewarded2 = %d WHERE friendGUID = %d", 1, player->GetGUIDLow());
				RecruitInfo[i].timeRewarded2 = true;
				break;
			case 3:

				CharacterDatabase.DirectPExecute("UPDATE character_recruit SET timeRewarded3 = %d WHERE friendGUID = %d", 1, player->GetGUIDLow());
				RecruitInfo[i].timeRewarded3 = true;
				break;
			default:
				break;
			}
		}
}

void AddSC_RECRUIT()
{
	new RecruitWorldScript();
	new RecruitPlayerScript();
}


void Recruit::AddMainMenu(Player* player, Object* obj)
{
	player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "��ļ�µĻ��", SENDER_RECRUIT_NEW, GOSSIP_ACTION_INFO_DEF, "", 0, true);
	player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "�����ļ��ϵ", SENDER_RECRUIT_CHAR_DISS_LIST, GOSSIP_ACTION_INFO_DEF);
	player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "�鿴��ļ��Ϣ", SENDER_RECRUIT_CHAR_INFO_LIST, GOSSIP_ACTION_INFO_DEF);
	player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "֧Ԯ���߻��", SENDER_RECRUIT_CHAR_TELE_LIST, GOSSIP_ACTION_INFO_DEF);
	if (obj->ToCreature())
		player->SEND_GOSSIP_MENU(obj->GetEntry(), obj->GetGUID());
	else
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, obj->GetGUID());
}
bool Recruit::AddSubMenuOrDoAction(Player* player, Object* obj, uint32 sender, uint32 action)
{
	player->PlayerTalkClass->ClearMenus();
	switch (sender)
	{
	case SENDER_RECRUIT_CHAR_INFO_LIST:
	{
		if (GetRecruiterData(player))
		{
			std::ostringstream oss;

			uint64 recruiterGUID = MAKE_NEW_GUID(GetRecruiterData(player)->guidLow, 0, HIGHGUID_PLAYER);

			if (ObjectAccessor::FindPlayerInOrOutOfWorld(recruiterGUID))
				oss << "[|cFF0177EC����|r]";
			else
				oss << "[|cFFFF1717����|r]";
			oss << "|cFFFF1717";
			oss << GetRecruiterData(player)->name;
			oss << "|r";
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, oss.str(), SENDER_RECRUIT_CHAR_INFO, GetRecruiterData(player)->guidLow);
		}

		std::vector<GlobalPlayerData const*> friendsDataList;
		GetFriendsDataList(friendsDataList, player);

		for (size_t i = 0; i < friendsDataList.size(); i++)
		{
			std::ostringstream oss;
			uint64 friendGUID = MAKE_NEW_GUID(friendsDataList[i]->guidLow, 0, HIGHGUID_PLAYER);

			if (ObjectAccessor::FindPlayerInOrOutOfWorld(friendGUID))
				oss << "[|cFF0177EC����|r]";
			else
				oss << "[|cFFFF1717����|r]";

			oss << "|cFF0177EC";
			oss << friendsDataList[i]->name;
			oss << "|r";
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, oss.str(), SENDER_RECRUIT_CHAR_INFO, friendsDataList[i]->guidLow);
		}
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, obj->GetGUID());
	}
	break;

	case SENDER_RECRUIT_CHAR_INFO:
	{
		GlobalPlayerData const* playerData = sWorld->GetGlobalPlayerData(action);

		if (playerData)
		{
			std::ostringstream oss;
			oss << "���֣�" << playerData->name << "\n";
			oss << "�ȼ� :" << playerData->level << "\n";;
			oss << "���� :" << sGuildMgr->GetGuildNameById(playerData->guildId) << "\n";
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, oss.str(), SENDER_RECRUIT_CHAR_INFO, action);
		}
	}
	break;

	case SENDER_RECRUIT_CHAR_DISS_LIST:
	{
		if (GetRecruiterData(player))
		{
			std::ostringstream oss;

			uint64 recruiterGUID = MAKE_NEW_GUID(GetRecruiterData(player)->guidLow, 0, HIGHGUID_PLAYER);

			if (ObjectAccessor::FindPlayerInOrOutOfWorld(recruiterGUID))
				oss << "[|cFF0177EC����|r]";
			else
				oss << "[|cFFFF1717����|r]";

			oss << "|cFFFF1717";
			oss << GetRecruiterData(player)->name;
			oss << "|r";
			player->ADD_GOSSIP_ITEM_EXTENDED(0, oss.str(), SENDER_RECRUIT_CHAR_DISMISS, 0, sReq->Notice(player, GetDissReqId2(), "�Ƴ�", "��ļ��ϵ"), sReq->Golds(GetDissReqId2()), false);
		}

		std::vector<GlobalPlayerData const*> friendsDataList;

		GetFriendsDataList(friendsDataList, player);

		for (size_t i = 0; i < friendsDataList.size(); i++)
		{
			std::ostringstream oss;
			uint64 friendGUID = MAKE_NEW_GUID(friendsDataList[i]->guidLow, 0, HIGHGUID_PLAYER);

			if (ObjectAccessor::FindPlayerInOrOutOfWorld(friendGUID))
				oss << "[|cFF0177EC����|r]";
			else
				oss << "[|cFFFF1717����|r]";

			oss << "|cFF0177EC";
			oss << friendsDataList[i]->name;
			oss << "|r";

			player->ADD_GOSSIP_ITEM_EXTENDED(0, oss.str(), SENDER_RECRUIT_CHAR_DISMISS, friendsDataList[i]->guidLow, sReq->Notice(player, GetDissReqId1(), "�Ƴ�", "��ļ��ϵ"), sReq->Golds(GetDissReqId1()), false);
		}
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, obj->GetGUID());
	}
	break;
	case SENDER_RECRUIT_CHAR_DISMISS:
	{
		if (action == 0)
		{
			if (sReq->Check(player, GetDissReqId2()))
			{
				sReq->Des(player, GetDissReqId2());
				DismissRecruit(GetRecruiterData(player)->guidLow, player->GetGUID());
			}
		}
		else
		{
			if (sReq->Check(player, GetDissReqId1()))
			{
				sReq->Des(player, GetDissReqId1());
				DismissRecruit(player->GetGUIDLow(), action);
			}
		}

		player->CLOSE_GOSSIP_MENU();
	}
	break;
	case SENDER_RECRUIT_CHAR_TELE_LIST:
	{
		if (GetRecruiterData(player))
		{
			std::ostringstream oss;

			uint64 recruiterGUID = MAKE_NEW_GUID(GetRecruiterData(player)->guidLow, 0, HIGHGUID_PLAYER);

			if (ObjectAccessor::FindPlayerInOrOutOfWorld(recruiterGUID))
			{
				oss << "[|cFF0177EC����|r]";
				oss << "|cFFFF1717";
				oss << GetRecruiterData(player)->name;
				oss << "|r";
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, oss.str(), SENDER_RECRUIT_TELE, GetRecruiterData(player)->guidLow);
			}
		}

		std::vector<GlobalPlayerData const*> friendsDataList;
		GetFriendsDataList(friendsDataList, player);

		for (size_t i = 0; i < friendsDataList.size(); i++)
		{
			std::ostringstream oss;
			uint64 friendGUID = MAKE_NEW_GUID(friendsDataList[i]->guidLow, 0, HIGHGUID_PLAYER);

			if (ObjectAccessor::FindPlayerInOrOutOfWorld(friendGUID))
			{
				oss << "[|cFF0177EC����|r]";
				oss << "|cFF0177EC";
				oss << friendsDataList[i]->name;
				oss << "|r";
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, oss.str(), SENDER_RECRUIT_TELE, friendsDataList[i]->guidLow);
			}
		}
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, obj->GetGUID());
	}
	break;
	case SENDER_RECRUIT_TELE:
	{
		RecruitTelePort(player, action);
		player->CLOSE_GOSSIP_MENU();
	}
	break;
	default:
		return false;
	}

	return true;
}

bool Recruit::RecruitFriend(Player* player, uint32 sender, const char* name)
{
	if (sender != SENDER_RECRUIT_NEW)
		return false;

	player->PlayerTalkClass->ClearMenus();

	if (!*name)
		return true;

	player->CLOSE_GOSSIP_MENU();

	std::string pName = name;

	if (!normalizePlayerName(pName))
	{
		ChatHandler(player->GetSession()).PSendSysMessage("|cFFFF1717[��ļϵͳ]|r�������������");
		return true;
	}

	uint32 GUIDLow = sWorld->GetGlobalPlayerGUID(pName);
	if (!GUIDLow)
	{
		ChatHandler(player->GetSession()).PSendSysMessage("|cFFFF1717[��ļϵͳ]|r��ļ����Ҳ�����");
		return true;
	}

	uint64 friendGuid = MAKE_NEW_GUID(GUIDLow, 0, HIGHGUID_PLAYER);

	Player* pFriend = ObjectAccessor::FindPlayerInOrOutOfWorld(friendGuid);

	if (!pFriend)
	{
		ChatHandler(player->GetSession()).PSendSysMessage("|cFFFF1717[��ļϵͳ]|r��ļ����Ҳ�����");
		return true;
	}

	std::string ip1 = pFriend->GetSession()->GetRemoteAddress();
	std::string ip2 = player->GetSession()->GetRemoteAddress();

	if (strcmp(ip1.c_str(), ip2.c_str()) == 0 && player->GetSession()->GetSecurity() < SEC_ADMINISTRATOR)
	{
		ChatHandler(player->GetSession()).PSendSysMessage("|cFFFF1717[��ļϵͳ]|rͬһIP����Ҳ�����ļ");
		return true;
	}
	
	if (!CrossFaction() && player->GetTeamId() != pFriend->GetTeamId())
	{
		ChatHandler(player->GetSession()).PSendSysMessage("|cFFFF1717[��ļϵͳ]|rֻ����ļͬһ��Ӫ���");
		return true;
	}

	if (IsRecruited(pFriend))
	{
		ChatHandler(player->GetSession()).PSendSysMessage("|cFFFF1717[��ļϵͳ]|r|cFF0177EC%s|r�ѱ���ļ", sCF->GetNameLink(pFriend).c_str());
		return true;
	}

	if (GetFriendAmount(player) >= GetPlayersLimit())
	{
		ChatHandler(player->GetSession()).PSendSysMessage("|cFFFF1717[��ļϵͳ]|r�㲻����ļ�������");
		return true;
	}

	if (!IsRecruitYourRecruiter(player->GetGUIDLow(), pFriend->GetGUIDLow()))
	{
		ChatHandler(player->GetSession()).PSendSysMessage("|cFFFF1717[��ļϵͳ]|r�㲻����ļ�����ļ��");
		return true;
	}

	if (!sReq->Check(player, GetRecReqId()))
		return true;

	pFriend->recruiterGUIDLow = player->GetGUIDLow();

	std::ostringstream oss;
	oss << "|cFF0177EC";
	oss << sCF->GetNameLink(player);
	oss << "|r������ļ��";
	PopMsg(pFriend, RECRUIT_MENU_ID, oss.str());

	ChatHandler(player->GetSession()).PSendSysMessage("|cFFFF1717[��ļϵͳ]|r��|cFF0177EC%s|r������ļ����ɹ���",sCF->GetNameLink(pFriend).c_str());

	return true;
}
