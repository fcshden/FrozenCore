#pragma execution_character_set("utf-8")
#include "../PrecompiledHeaders/ScriptPCH.h"
#include "../DataLoader/DataLoader.h"
#include "../CommonFunc/CommonFunc.h"
#include "../Reward/Reward.h"
#include "../String/myString.h"
#include "../Switch/Switch.h"
#include "../Talisman/Talisman.h"
#include "../PvP/PvP.h"


//announceFlag		//0 - 不提示   1 - 连杀提示		2 - 终结连杀提示		3 - 都提示
//rewFlag			//0 - 野外     1 - 战场			2 - 野外和战场
//endRewFlag		//0 - 野外     1 - 战场			2 - 野外和战场

const int32 KillerStreak1 = 1;
const int32 KillerStreak2 = 2;
const int32 KillerStreak3 = 3;
const int32 KillerStreak4 = 4;
const int32 KillerStreak5 = 5;
const int32 KillerStreak6 = 6;
const int32 KillerStreak7 = 7;
const int32 KillerStreak8 = 8;
const int32 KillerStreak9 = 9;
const int32 KillerStreak10 = 10;

struct SystemInfo
{
	uint64 KillStreak;
	uint64 LastGUIDKill;
};

static std::map<uint32, SystemInfo> KillingStreak;

class System_OnPVPKill : public PlayerScript
{
public:
	System_OnPVPKill() : PlayerScript("System_OnPVPKill") {}

	void GetRewAndNotice(uint32 num, uint32 &rewId, uint32 &endRewId, uint32 &announceFlag, uint32 &rewFlag, uint32 &endRewFlag)
	{
		std::unordered_map<uint32, KillerStreakTemplate>::iterator iter = KillerStreakMap.find(num);
		if (iter != KillerStreakMap.end())
		{
			rewId = iter->second.rewId;
			endRewId = iter->second.endRewId;
			announceFlag = iter->second.announceFlag;
			rewFlag = iter->second.rewFlag;
			endRewFlag = iter->second.endRewFlag;
		}
	}

	void KillAnnounceAndRew(Player* pKiller, Player* pVictim, uint32 num)
	{
		uint32 rewId = 0;
		uint32 endRewId = 0;
		uint32 announceFlag = 0;
		uint32 rewFlag = 0;
		uint32 endRewFlag = 0;
		GetRewAndNotice(num, rewId, endRewId, announceFlag,rewFlag,endRewFlag);

		CORE_STR_TYPES type = STR_NONE;

		switch (num)
		{
		case KillerStreak1:	type = STR_KILL_STREAK_1;	break;
		case KillerStreak2:	type = STR_KILL_STREAK_2;	break;
		case KillerStreak3:	type = STR_KILL_STREAK_3;	break;
		case KillerStreak4:	type = STR_KILL_STREAK_4;	break;
		case KillerStreak5:	type = STR_KILL_STREAK_5;	break;
		case KillerStreak6:	type = STR_KILL_STREAK_6;	break;
		case KillerStreak7:	type = STR_KILL_STREAK_7;	break;
		case KillerStreak8:	type = STR_KILL_STREAK_8;	break;
		case KillerStreak9:	type = STR_KILL_STREAK_9;	break;
		case KillerStreak10:type = STR_KILL_STREAK_10; break;
		default:
			type = STR_NONE;
		}

		if (AreaTableEntry const* areaEntry = GetAreaEntryByAreaID(pKiller->GetAreaId()))
		{
			if (type != STR_NONE && (announceFlag == 1 || announceFlag == 3))
				sWorld->SendServerMessage(SERVER_MSG_STRING, sString->Format(sString->GetText(type), areaEntry->area_name[4], pKiller->GetName().c_str(), pVictim->GetName().c_str()));

			if (rewId != 0)
			{
				if (rewFlag == 2 || !pKiller->InBattleground() && rewFlag == 0 || pKiller->InBattleground() && rewFlag == 1)
				{
					sRew->Rew(pKiller, rewId);
					sWorld->SendServerMessage(SERVER_MSG_STRING, sString->Format(sString->GetText(CORE_STR_TYPES(STR_KILL_STREAK_REW)), pKiller->GetName().c_str(), sRew->GetAnounceText(rewId).c_str()));
				}
			}

			if (endRewId != 0)
				sWorld->SendServerMessage(SERVER_MSG_STRING, sString->Format(sString->GetText(CORE_STR_TYPES(STR_KILL_STREAK_TO_REW)), pKiller->GetName().c_str(), sRew->GetAnounceText(endRewId).c_str()));
		}	
	}

	void KilledAnnounceAndRew(Player* pKiller, Player* pVictim, uint32 num)
	{
		uint32 rewId = 0;
		uint32 endRewId = 0;
		uint32 announceFlag = 0;
		uint32 rewFlag = 0;
		uint32 endRewFlag = 0;
		GetRewAndNotice(num, rewId, endRewId, announceFlag, rewFlag, endRewFlag);

		uint32 type = STR_NONE;

		switch (num)
		{
		case KillerStreak1:	type = STR_END_KILL_STREAK_1;	break;
		case KillerStreak2:	type = STR_END_KILL_STREAK_2;	break;
		case KillerStreak3:	type = STR_END_KILL_STREAK_3;	break;
		case KillerStreak4:	type = STR_END_KILL_STREAK_4;	break;
		case KillerStreak5:	type = STR_END_KILL_STREAK_5;	break;
		case KillerStreak6:	type = STR_END_KILL_STREAK_6;	break;
		case KillerStreak7:	type = STR_END_KILL_STREAK_7;	break;
		case KillerStreak8:	type = STR_END_KILL_STREAK_8;	break;
		case KillerStreak9:	type = STR_END_KILL_STREAK_9;	break;
		case KillerStreak10:type = STR_END_KILL_STREAK_10;  break;
		default:
			break;
		}

		if (AreaTableEntry const* areaEntry = GetAreaEntryByAreaID(pKiller->GetAreaId()))
		{
			if (type != STR_NONE && (announceFlag == 2 || announceFlag == 3))
				sWorld->SendServerMessage(SERVER_MSG_STRING, sString->Format(sString->GetText(CORE_STR_TYPES(type)), areaEntry->area_name[4], pVictim->GetName().c_str(), pKiller->GetName().c_str()));

			if (endRewId != 0)
			{
				if (endRewFlag == 2 || !pKiller->InBattleground() && endRewFlag == 0 || pKiller->InBattleground() && endRewFlag == 1)
				{
					sRew->Rew(pKiller, endRewId);
					sWorld->SendServerMessage(SERVER_MSG_STRING, sString->Format(sString->GetText(CORE_STR_TYPES(STR_END_KILL_STREAK_REW)), pKiller->GetName().c_str(), sRew->GetAnounceText(endRewId).c_str()));

				}
			}
		}
	}


	void OnPVPKill(Player *pKiller, Player *pVictim)
	{
		if (pVictim->HasAura(2479) || pVictim->HasAura(46705))
			return;

		//虚弱复活
		if (pVictim->HasAura(15007))
			return;

		//同IP
		std::string kIP = pKiller->GetSession()->GetRemoteAddress();
		std::string vIP = pVictim->GetSession()->GetRemoteAddress();
		if (strcmp(kIP.c_str(), vIP.c_str()) == 0)
			return;

		uint32 kGUID = pKiller->GetGUID();
		uint32 vGUID = pVictim->GetGUID();

		//自杀
		if (kGUID == vGUID)
			return;

		//连续击杀同一玩家
		if (KillingStreak[kGUID].LastGUIDKill == vGUID)
			return;

		//连续击杀同一IP下的玩家
		if (Player* lastVictim = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(KillingStreak[kGUID].LastGUIDKill, 0, HIGHGUID_PLAYER)))
		{
			if (WorldSession* session = lastVictim->GetSession())
			{
				std::string lastIp = session->GetRemoteAddress();
		
				if (strcmp(lastIp.c_str(), vIP.c_str()) == 0)
					return;
			}
		}

		sPvP->RewOnKill(pKiller, pVictim);

		//sTalisman->OnKillStreak(pKiller);

		KilledAnnounceAndRew(pKiller, pVictim, KillingStreak[vGUID].KillStreak);
		//连杀

		KillingStreak[kGUID].KillStreak++;
		KillingStreak[vGUID].KillStreak = 0;
		KillingStreak[kGUID].LastGUIDKill = vGUID;
		KillingStreak[vGUID].LastGUIDKill = 0;

		KillAnnounceAndRew(pKiller, pVictim, KillingStreak[kGUID].KillStreak);
	}
};

void AddSC_PvP_System()
{
	new System_OnPVPKill();
}
