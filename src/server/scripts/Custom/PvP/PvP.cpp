#pragma execution_character_set("utf-8")
#include "PvP.h"
#include "../Switch/Switch.h"
#include "../CommonFunc/CommonFunc.h"
#include "../CharNameMod/CharNameMod.h"
#include "../Faction/Faction.h"
#include "../String/myString.h"
#include "GroupMgr.h"
#include "../GCAddon/GCAddon.h"
#include "../Reward/Reward.h"

std::vector<PvPTemplate> PvPVec;

void PvP::Load()
{
	PvPVec.clear();
	
	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		//		0			1		2			3				4			5	6			7				8			9
		"SELECT 地域ID, 区域属性, 进入提示文本,进入获得名字前缀,生命值下限,区域ID,生命值上限,击杀奖励模板ID,被杀奖励模板ID,获得奖励时满足的事件组,进入自动满血 FROM _属性调整_区域" :
		//		0		1		2		3	4	5		6		7		8			9
		"SELECT area, type, notice,prefix,minHp,zone,maxHp,killRewId,killedRewId,rewEventData FROM _pvp");

	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			PvPTemplate Temp;
			Temp.area			= fields[0].GetUInt32();

			const char*  type = fields[1].GetCString();

			if (strcmp("安全区", type) == 0)
				Temp.type = PVP_TYPE_SANCTUARY;
			else if (strcmp("自由PVP", type) == 0)
				Temp.type = PvP_TYPE_FFA;
			else if (strcmp("自由PVP - 禁止组队", type) == 0)
				Temp.type = PvP_TYPE_NO_GROUP_FFA;
			else if (strcmp("自定义阵营", type) == 0)
				Temp.type = PvP_TYPE_CUSTOM_FACTION;
			else if (strcmp("公会PVP", type) == 0)
				Temp.type = PvP_TYPE_GUILD;
			else
				Temp.type = PVP_TYPE_NONE;

			Temp.notice			= fields[2].GetString();
			Temp.prefix			= fields[3].GetString();
			Temp.minHp			= fields[4].GetInt32();
			Temp.zone			= fields[5].GetInt32();
			Temp.maxHp			= fields[6].GetInt32();
			Temp.killRewId		= fields[7].GetInt32();
			Temp.killedRewId	= fields[8].GetInt32();
			Tokenizer tokens(fields[9].GetString(), ' ');
			for (Tokenizer::const_iterator itr = tokens.begin(); itr != tokens.end(); ++itr)
				Temp.eventData.push_back(uint32(atol(*itr)));
			Temp.autoMaxHp		= fields[10].GetBool();
			PvPVec.push_back(Temp);
		} while (result->NextRow());
	}
}

PvPTypes PvP::GetType(uint32 zone, uint32 area)
{
	uint32 length = PvPVec.size();

	for (size_t i = 0; i < length; i++)
		if (PvPVec[i].zone == zone && PvPVec[i].area == 0 || PvPVec[i].zone == zone && PvPVec[i].area == area)
			return PvPVec[i].type;

	return PVP_TYPE_NONE;
}

uint32 PvP::GetMinHp(uint32 zone, uint32 area)
{
	uint32 length = PvPVec.size();

	for (size_t i = 0; i < length; i++)
		if (PvPVec[i].zone == zone && PvPVec[i].area == 0 || PvPVec[i].zone == zone && PvPVec[i].area == area)
			return PvPVec[i].minHp;

	return 0;
}

uint32 PvP::GetMaxHp(uint32 zone, uint32 area)
{
	uint32 length = PvPVec.size();

	for (size_t i = 0; i < length; i++)
		if (PvPVec[i].zone == zone && PvPVec[i].area == 0 || PvPVec[i].zone == zone && PvPVec[i].area == area)
			return PvPVec[i].maxHp;

	return 0;
}

bool PvP::AutoMaxHP(uint32 zone, uint32 area)
{
	uint32 length = PvPVec.size();

	for (size_t i = 0; i < length; i++)
		if (PvPVec[i].zone == zone && PvPVec[i].area == 0 || PvPVec[i].zone == zone && PvPVec[i].area == area)
			return PvPVec[i].autoMaxHp;

	return false;
}

void PvP::RewOnKill(Player *pKiller, Player *pVictim)
{
	uint32 zone = pKiller->GetZoneId();
	uint32 area = pKiller->GetAreaId();

	for (auto itr = PvPVec.begin(); itr != PvPVec.end(); itr++)
	{
		if (itr->zone == zone && itr->area == 0 || itr->zone == zone && itr->area == area)
		{
			bool valid = false;

			if (itr->eventData.empty())
				valid = true;

			for (auto i = itr->eventData.begin(); i != itr->eventData.end(); i++)
				if (sGameEventMgr->IsActiveEvent(*i))
					valid = true;

			if (valid)
			{
				sRew->Rew(pKiller, itr->killRewId);
				sRew->Rew(pVictim, itr->killedRewId);
			}

			return;
		}
	}
}

bool PvP::EnableGroup(uint32 zone, uint32 area)
{
	uint32 length = PvPVec.size();

	for (size_t i = 0; i < length; i++)
		if (PvPVec[i].zone == zone && PvPVec[i].area == 0 || PvPVec[i].zone == zone && PvPVec[i].area == area)
			if (PvPVec[i].type == PvP_TYPE_NO_GROUP_FFA)
				return false;

	return true;
}

void PvP::Update(Player* player, uint32 zone, uint32 area)
{
	bool automaxhp = AutoMaxHP(zone, area);

	player->UpdateMaxHealth();

	if (automaxhp)
		player->SetHealth(player->GetMaxHealth());

	if (Unit* pet = player->GetGuardianPet())
		pet->UpdateMaxHealth();
		
	player->SetUInt32Value(PLAYER_DUEL_TEAM, 0);

	std::string newName;
	bool exsit = false;


	uint32 length = PvPVec.size();

	for (size_t i = 0; i < length; i++)
	{
		if (PvPVec[i].zone == zone && PvPVec[i].area == 0 || PvPVec[i].zone == zone && PvPVec[i].area == area)
		{
			sWorld->UpdateNamePrefixSuffix(player, PvPVec[i].prefix + player->namePrefix, player->nameSuffix);
			
			if (!PvPVec[i].notice.empty())
				ChatHandler(player->GetSession()).PSendSysMessage(PvPVec[i].notice.c_str());

			switch (PvPVec[i].type)
			{
			case PvP_TYPE_NO_GROUP_FFA:
				player->UninviteFromGroup();
				if (player->GetGroup())
					player->RemoveFromGroup();
				break;
			case PvP_TYPE_CUSTOM_FACTION:
				player->SetUInt32Value(PLAYER_DUEL_TEAM, FACTION_DUEL + player->faction);
				break;
			case PvP_TYPE_GUILD:
				player->SetUInt32Value(PLAYER_DUEL_TEAM, GUILD_DUEL + player->GetGuildId());
				break;
			}

			exsit = true;
			break;
		}
	}

	if (!exsit)
		sWorld->UpdateNamePrefixSuffix(player, player->namePrefix, player->nameSuffix);
}

class FFAPvPPlayer : PlayerScript
{
public:
	FFAPvPPlayer() : PlayerScript("FFAPvPPlayer") {}

	void OnUpdateZone(Player* player, uint32 newZone, uint32 newArea)
	{
		
	}
};

void AddSC_FFAPvP()
{
	//new FFAPvPPlayer();
}
