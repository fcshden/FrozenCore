#pragma execution_character_set("utf-8")
#include "AntiCheat.h"
#include "../Switch/Switch.h"
#include "AccountMgr.h"
#include "../Command/CustomCommand.h"
#include "../CommonFunc/CommonFunc.h"
#include "../CustomEvent/Event.h"
#include "Configuration/Config.h"

void AntiCheat::CreatureReset(Creature* creature,uint32 diff)
{
	if (creature->C_ResetDistance <= 0 || !creature->IsInCombat())
		return;

	creature->reset_timer += diff;

	if (creature->reset_timer > 3000)
	{
		creature->reset_timer = 0;

		if (creature->GetHomePosition().GetExactDist2d(creature) > creature->C_ResetDistance)
		{
			creature->CombatStop(true);
			creature->DeleteThreatList();
			creature->GetMotionMaster()->MoveTargetedHome();
			creature->GetAI()->Reset();
		}
	}
}

std::string AntiCheat::GetTimeString()
{
	time_t t = time(NULL);
	tm localTime;
	ACE_OS::localtime_r(&t, &localTime);

	int year = localTime.tm_year % 100;
	int month = localTime.tm_mon + 1;
	int day = localTime.tm_mday;
	int hour = localTime.tm_hour;
	int min = localTime.tm_min;
	int sec = localTime.tm_sec;
	int wday = localTime.tm_wday;
	std::ostringstream oss;

	oss << "20" << year << "年" << month << "月" << day << "日" << "星期";

	switch (wday)
	{
	case 1:
		oss << "一";
		break;
	case 2:
		oss << "二";
		break;
	case 3:
		oss << "三";
		break;
	case 4:
		oss << "四";
		break;
	case 5:
		oss << "五";
		break;
	case 6:
		oss << "六";
		break;
	default:
		oss << "日";
		break;
	}
	
	oss << " " << hour << "时" << min << "分";

	return oss.str();
}

void AntiCheat::SaveAndNotice(Player* player, AC_TYPES type, float z)
{
	std::string text = "";
	bool save = false;
	AC_ACTION_TYPES actionType = AC_ACTION_TYPES(AntiCheatType);

	switch (type)
	{
	case AC_FLY:
		if (urand(0, 3) == 1)
			save = true;
		text = "飞天";
		actionType = AC_ACTION_FALL;
		break;
	case AC_OTHER:
		text = "其他";
		save = false;
		actionType = AC_ACTION_FALL;
		break;
	case AC_SPEED:
		if (actionType == AC_ACTION_IGNORE_SPEED)
			return;
		if (urand(0, 3) == 1)
			save = true;
		text = "加速";
		actionType = AC_ACTION_PULL;
		break;
	case AC_TELE:
		text = "传送";
		save = true;
		break;
	}

	std::ostringstream oss;
	oss << "|cFFFFFC00<外挂检测>|r |cFF60FF00" << sCF->GetNameLink(player) << "|r |cFFFFFC00使用|r<" << text << ">";
	WorldPacket data(SMSG_NOTIFICATION, (oss.str().size() + 1));
	data << oss.str();
	sWorld->SendGlobalGMMessage(&data);
	//sWorld->SendGlobalGMText(oss.str().c_str());

	player->SetOrientation(frand(-3, 3));

	switch (actionType)
	{	
	case AC_ACTION_PULL:
		player->TeleportTo(player->GetMapId(), player->m_positionX, player->m_positionY, player->m_positionZ, player->GetOrientation());
		break;
	case AC_ACTION_TELE_HOME:
		player->TeleportTo(player->m_homebindMapId, player->m_homebindX, player->m_homebindY, player->m_homebindZ, player->GetOrientation());
		break;
	case AC_ACTION_PRISION:
		player->TeleportTo(1, 16230.157227, 16406.478516, -64.37920, player->GetOrientation());
		break;
	case AC_ACTION_KICK:
		player->GetSession()->KickPlayer(true);
		break;
	case AC_ACTION_BAN_CHAR:
		break;
	case AC_ACTION_BAN_ACC:
		break;
	case AC_ACTION_FALL:
		player->TeleportTo(player->GetMapId(), player->m_positionX, player->m_positionY, z, player->GetOrientation());
		break;
	}

	if (save)
	{
		SQLTransaction trans = CharacterDatabase.BeginTransaction();
		PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_AC);
		stmt->setString(0, player->GetName());
		stmt->setUInt32(1, player->GetSession()->GetAccountId());
		stmt->setString(2, text);
		stmt->setString(3, GetTimeString());
		trans->Append(stmt);
		CharacterDatabase.CommitTransaction(trans);
	}
}

bool AntiCheat::CheckMovementInfo(Unit* unit, MovementInfo const& movementInfo)
{
    if (!sConfigMgr->GetBoolDefault("Frozen.FG", false))
        return false;

	if (!unit)
		return false;

	Player* pl = unit->ToPlayer();

	if (!pl)
		return false;

	time_t t = time(NULL);
	if (t - unit->GetSpeedHackCooldown() < 1)
		return false;
	unit->SetSpeedHackCooldown(t);

	if (float timer = pl->GetLastMoveClientTimestamp())
	{
		if (pl->IsFalling() ||
			pl->IsInFlight() || 
			pl->GetTransport() ||
			pl->HasUnitMovementFlag(MOVEMENTFLAG_ONTRANSPORT) ||
			pl->HasUnitState(UNIT_STATE_IGNORE_ANTISPEEDHACK) ||
			pl->IsGameMaster()
			)
			return false;

		if (pl->IsSkipOnePacket())
		{
			pl->SetSkipOnePacket(false);
			return false;
		}

		float realping = pl->GetSession()->GetLatency();
		float ping = realping < 60.0f ? 60.0f : realping;
		float speed = (pl->IsFlying() || pl->CanFly()) ? pl->GetSpeed(MOVE_FLIGHT) : pl->GetSpeed(MOVE_RUN);
		float difftime = (movementInfo.time - timer) * 0.001f + (getMSTime() - movementInfo.time) / 10000000000 + (ping * 0.001f);
		float normaldistance = speed * difftime + 0.1f;

		Position npos = movementInfo.pos;
		float distance = pl->GetExactDist2d(&npos);

		float diffdist = distance - normaldistance;

		if (diffdist < 0 || distance < 5.6f)
			return false;

		if (pl->UnderACKmount())
		{
			if (diffdist > 70.0f)
			{
				SaveAndNotice(pl, AC_TELE);
				return true;
			}
		}
		else
		{
			if (diffdist > 70.0f)
				SaveAndNotice(pl, AC_TELE);
			else
				SaveAndNotice(pl, AC_SPEED);
			return true;
		}	
	}

	return false;
}

void AntiCheat::CheckMovementInfo(Player* pl)
{

    if (!sConfigMgr->GetBoolDefault("Frozen.FG", false))
        return;

	if (pl->IsFalling() ||
		pl->IsInFlight() ||
		pl->GetTransport() ||
		pl->HasUnitMovementFlag(MOVEMENTFLAG_ONTRANSPORT) ||
		pl->HasUnitState(UNIT_STATE_IGNORE_ANTISPEEDHACK) ||
		pl->UnderACKmount() ||
		pl->IsGameMaster() || 
		pl->IsInWater()
		)
		return;

	const bool no_fly_auras = !(pl->HasAuraType(SPELL_AURA_FLY) || pl->HasAuraType(SPELL_AURA_MOD_INCREASE_VEHICLE_FLIGHT_SPEED)
		|| pl->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED) || pl->HasAuraType(SPELL_AURA_MOD_INCREASE_FLIGHT_SPEED)
		|| pl->HasAuraType(SPELL_AURA_MOD_MOUNTED_FLIGHT_SPEED_ALWAYS) || pl->HasAuraType(SPELL_AURA_MOD_FLIGHT_SPEED_NOT_STACK));
	const bool no_fly_flags = (pl->GetUnitMovementFlags() & (MOVEMENTFLAG_DESCENDING | MOVEMENTFLAG_ASCENDING | MOVEMENTFLAG_CAN_FLY | MOVEMENTFLAG_FLYING)) == 0;

	float z = pl->GetMap()->GetHeight(pl->GetPhaseMask(), pl->m_positionX, pl->m_positionY, pl->m_positionZ + 1.8f, true);

	if (no_fly_auras && !no_fly_flags && !pl->GMFlyON)
	{
		SaveAndNotice(pl, AC_FLY, z);
		return;
	}
	//if (!pl->HasUnitMovementFlag(MOVEMENTFLAG_SWIMMING) && no_fly_auras && !pl->GMFlyON && !pl->HasAuraType(SPELL_AURA_WATER_WALK))
	//if (pl->m_positionZ - z > 4.8f && !pl->IsInWater() && no_fly_auras)
	//		if (!pl->GetMap()->IsInWater(pl->m_positionX, pl->m_positionX, z))
	//			SaveAndNotice(pl, AC_OTHER, z);
}


std::unordered_map<BattlegroundTypeId, std::vector<uint32>>FixtimeBGMap;

void FixtimeBG::Load()
{
	FixtimeBGMap.clear();
	return;
	QueryResult result = WorldDatabase.PQuery("SELECT id, events from xy_battleground_reward");
	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			
			BattlegroundTypeId bgt = BattlegroundTypeId(fields[0].GetUInt32());
			std::vector<uint32> events;
			Tokenizer data(fields[1].GetString(), ' ');
			for (Tokenizer::const_iterator itr = data.begin(); itr != data.end(); ++itr)
				events.push_back(atoi(*itr));
			FixtimeBGMap.insert(std::make_pair(bgt, events));
		} while (result->NextRow());
	}
}

bool FixtimeBG::IsActive(Player* pl, BattlegroundTypeId bgt)
{
	auto itr = FixtimeBGMap.find(bgt);
	if (itr == FixtimeBGMap.end())
		return true;

	if (itr->second.empty())
		return true;

	for (auto it = itr->second.begin(); it != itr->second.end(); it++)
		if (sGameEventMgr->IsActiveEvent(*it))
			return true;

	ChatHandler(pl->GetSession()).PSendSysMessage("该战场未激活！");
	return false;
}
