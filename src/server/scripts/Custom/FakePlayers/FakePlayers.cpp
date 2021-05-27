#pragma execution_character_set("utf-8")
#include "FakePlayers.h"
#include "../String/myString.h"
#include "../CommonFunc/CommonFunc.h"
#include "../Switch/Switch.h"
#include <random>
#include <algorithm>
#include <vector>

uint32 MAX_PLAYERS = 0;

uint32  START_PLAYERS = 0;

uint32 FakePlayerZones[MAX_FAKE_PLAYERS_ZONE]=
{
	4416, 3714, 33, 4742, 4131, 4812, 4493, 4809, 4265, 4603
};

std::vector<FakePlayersTemplate> FakePlayersVec;

void FakePlayers::Load()
{
	FakePlayersVec.clear();
	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		//		0		1		2	3	4	5	6	7			8
		"SELECT 名字,公会名称,等级,职业,种族,性别,ID,军衔等级,会员等级 FROM _假人_列表假人" :
		//		0		1	2	3		4	5	   6 7  8
		"SELECT name,gname,lvl,class_,race,gender,ID,hr,vip FROM _faker_list");
	if (!result) return;
	do
	{
		Field* fields = result->Fetch();
		FakePlayersTemplate Temp;
		Temp.pname		= fields[0].GetString();
		Temp.gname		= fields[1].GetString();
		Temp.lvl		= fields[2].GetUInt32();
		Temp.class_		= fields[3].GetUInt32();
		Temp.race		= fields[4].GetUInt32();
		Temp.gender		= fields[5].GetUInt8();
		Temp.pzoneid	= GenerateZoneId();
		Temp.online		= false;
		Temp.ID			= fields[6].GetUInt32();
		Temp.hr			= fields[7].GetUInt32();
		Temp.vip		= fields[8].GetUInt32();
		FakePlayersVec.push_back(Temp);
	} while (result->NextRow());
}

uint32 FakePlayers::GenerateZoneId()
{
	return FakePlayerZones[urand(0, MAX_FAKE_PLAYERS_ZONE - 1)];
}

void FakePlayers::LevelUp()
{
	if (FakePlayersVec.empty())
		return;

    std::default_random_engine generator{ std::random_device{}() };
    std::shuffle(std::begin(FakePlayersVec), std::end(FakePlayersVec), generator);

	std::vector<FakePlayersTemplate>::iterator itr;
	for (itr = FakePlayersVec.begin(); itr != FakePlayersVec.end(); ++itr)
	{
		if (itr->lvl >= 80)
			continue;
		if (!itr->online)
			continue;

		if (urand(0, 2) == 0)
		{
			itr->lvl++;

			WorldDatabase.PExecute("UPDATE _fake_players SET lvl = lvl + 1 WHERE ID = %u", itr->ID);
		}	
	}
}

void FakePlayers::LoginOrLogout()
{

	if (FakePlayersVec.empty())
		return;

	uint32 onlinecount = sWorld->GetPlayerCount();

	std::vector<FakePlayersTemplate>::iterator itr;
	for (itr = FakePlayersVec.begin(); itr != FakePlayersVec.end(); ++itr)
		if (itr->online)
			onlinecount++;


    std::default_random_engine generator{ std::random_device{}() };
    std::shuffle(std::begin(FakePlayersVec), std::end(FakePlayersVec), generator);

	for (itr = FakePlayersVec.begin(); itr != FakePlayersVec.end(); ++itr)
	{
		if (!itr->online)
		{
			if (onlinecount >= MAX_PLAYERS)
				break;

			if (urand(0, 1) == 0)
			{
				itr->online = true;

				if (sSwitch->GetOnOff(ST_LOG_IN))
				{
					std::string totalName = sCF->GetPlayerTotalName(NULL, sSwitch->GetFlag(ST_LOG_IN), true, itr->class_, itr->race, itr->hr, itr->pname, itr->vip, itr->gender);

					std::string text = sString->Format(sString->GetText(CORE_STR_TYPES(STR_LOGIN)), totalName.c_str());
					sWorld->SendGlobalText(text.c_str(), NULL);
				}
				
				break;
			}
		}
		else
		{
			if (urand(0, 3) == 0)
			{
				itr->online = false;

				if (sSwitch->GetOnOff(ST_LOG_OUT))
				{
					std::string totalName = sCF->GetPlayerTotalName(NULL, sSwitch->GetFlag(ST_LOG_OUT), true, itr->class_, itr->race, itr->hr, itr->pname, itr->vip);

					std::string text = sString->Format(sString->GetText(CORE_STR_TYPES(STR_LOGOUT)), totalName.c_str());
					sWorld->SendGlobalText(text.c_str(), NULL);
				}

				break;
			}
		}
	}
}

void FakePlayers::UpdateZone()
{
	if (FakePlayersVec.empty())
		return;

    std::default_random_engine generator{ std::random_device{}() };
    std::shuffle(std::begin(FakePlayersVec), std::end(FakePlayersVec), generator);

	std::vector<FakePlayersTemplate>::iterator itr;
	for (itr = FakePlayersVec.begin(); itr != FakePlayersVec.end(); ++itr)
	{
		if (!itr->online)
			continue;
		if (urand(0, 3) == 0)
			itr->pzoneid = GenerateZoneId();
	}		
}

bool FakePlayers::Logout(std::string name)
{
	if (FakePlayersVec.empty())
		return false;

	std::vector<FakePlayersTemplate>::iterator itr;
	for (itr = FakePlayersVec.begin(); itr != FakePlayersVec.end(); ++itr)
		if (strcmp(name.c_str(), itr->pname.c_str()) == 0)
		{
			itr->online = false;
			return true;
		}

	return false;
}

bool FakePlayers::isSameTeamId(Player* player,std::string name)
{
	std::vector<FakePlayersTemplate>::iterator itr;
	for (itr = FakePlayersVec.begin(); itr != FakePlayersVec.end(); ++itr)
		if (player->GetTeamId() == Player::TeamIdForRace(itr->race))
			return true;

	return false;
}


class FakePlayerScript : public WorldScript
{
public:
	FakePlayerScript() : WorldScript("FakePlayerScript") {}

	void OnAfterConfigLoad(bool)
	{
		if (!sSwitch->GetOnOff(ST_FAKE_PLAYERS))
			return;

		START_PLAYERS = atoi(sSwitch->GetFlagByIndex(ST_FAKE_PLAYERS, 1).c_str());
		MAX_PLAYERS = atoi(sSwitch->GetFlagByIndex(ST_FAKE_PLAYERS, 2).c_str());

		sFakePlayers->Load();

        std::default_random_engine generator{ std::random_device{}() };
        std::shuffle(std::begin(FakePlayersVec), std::end(FakePlayersVec), generator);

		uint32 count = 0;

		std::vector<FakePlayersTemplate>::iterator itr;
		for (itr = FakePlayersVec.begin(); itr != FakePlayersVec.end(); ++itr)
			if (!itr->online)
				if (count <= START_PLAYERS)
				{
						itr->online = true;
						count++;
				}			
	}
};

void AddSC_FakePlayerScript()
{
	new FakePlayerScript();
}
