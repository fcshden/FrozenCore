#pragma execution_character_set("utf-8")
#include "SignIn.h"
#include "../Reward/Reward.h"
#include "../CommonFunc/CommonFunc.h"
#include "../GCAddon/GCAddon.h"

std::unordered_map<uint32, uint32> SignInMap;

void SignIn::Load()
{
	SignInMap.clear();
	//QueryResult result = WorldDatabase.PQuery("SELECT day,rewId from _signin");
	//if (!result)
	//	return;
	//do
	//{
	//	Field* fields = result->Fetch();
	//	uint32 day		= fields[0].GetUInt32();
	//	uint32 rewId	= fields[1].GetUInt32();
	//	SignInMap.insert(std::make_pair(day, rewId));
	//} while (result->NextRow());
}

uint32 SignIn::GetRewId(uint32 day)
{
	std::unordered_map<uint32, uint32>::iterator iter = SignInMap.find(day);

	if (iter != SignInMap.end())
		return iter->second;

	uint32 maxday = 0;

	for (std::unordered_map<uint32, uint32>::iterator iter = SignInMap.begin(); iter != SignInMap.end(); iter++)
		if (maxday < iter->first)
			maxday = iter->first;

	iter = SignInMap.find(maxday);

	if (iter != SignInMap.end())
		return iter->second;

	return 0;
}

void SignIn::GetInfo(Player* player, time_t &signinTime, uint32 &signinDays)
{
	QueryResult result = CharacterDatabase.PQuery("SELECT signin_time,signin_days FROM characters WHERE guid = '%u'", player->GetGUIDLow());
	if (!result)
		return;

	Field* fields = result->Fetch();

	signinTime = time_t(fields[0].GetUInt32());
	signinDays = fields[1].GetUInt32();
}

bool SignIn::CanSignIn(Player* player)
{
	return GetDiffDay(time(NULL), player->signinTime) != 0;
}

void SignIn::SendPacket(Player* player)
{
	std::string lastSignIn = "---";
	if (player->signinTime != 0)
		lastSignIn = sSignIn->GetTimeString(player->signinTime);

	if (sSignIn->GetDiffDay(player->signinTime, time(NULL)) > 1)
		player->signinDays = 0;

	uint32 canSignIn = CanSignIn(player) ? 1 : 0;

	std::ostringstream oss;
	oss << "GC_SMSG_OPC_SIGN_IN@" << lastSignIn << "#" << sSignIn->GetTimeString(time(NULL)) << "#" << player->signinDays << "#" << canSignIn << "#" << sGCAddon->GetRewString(player, GetRewId(player->signinDays + 1));;

	//sGCAddon->SendPacket(player, oss.str());
}

void SignIn::DoAction(Player* player)
{
	if (!CanSignIn(player))
		return;

	//签到间隔大于1时
	if (GetDiffDay(player->signinTime, time(NULL)) > 1)
		player->signinDays = 0;

	player->signinTime = time(NULL);
	player->signinDays++;

	sRew->Rew(player, GetRewId(player->signinDays));

	SQLTransaction trans = CharacterDatabase.BeginTransaction();
	PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_SIGN_IN);
	stmt->setUInt32(0, player->signinTime);
	stmt->setUInt32(1, player->signinDays);
	stmt->setUInt32(2, player->GetGUIDLow());
	trans->Append(stmt);
	CharacterDatabase.CommitTransaction(trans);
}

void SignIn::OpenPanel(Player* player)
{
	SendPacket(player);
	player->CLOSE_GOSSIP_MENU();
}

uint32 SignIn::GetDiffDay(time_t  time1, time_t  time2)
{
	uint32 nDaySec = 3600 * 24;
	//uint32 nDaySec = 60;

	int32 day1 = time1 / nDaySec;
	int32 day2 = time2 / nDaySec;

	return abs(day2 - day1);
}

std::string SignIn::GetTimeString(time_t time)
{
	tm localTime;
	localtime_r(&time, &localTime);

	int year = localTime.tm_year % 100;
	int month = localTime.tm_mon + 1;
	int day = localTime.tm_mday;
	int hour = localTime.tm_hour;
	int min = localTime.tm_min;
	int sec = localTime.tm_sec;

	std::ostringstream oss;

	oss << "20" << year << "-";
	
	if (month < 10)
		oss << "0" << month << "-";
	else
		oss << month << "-";

	if (day < 10)
		oss << "0" << day;
	else
		oss << day;

	return oss.str();
}

class SignInPlayerScript : PlayerScript
{
public:
	SignInPlayerScript() : PlayerScript("SignInPlayerScript") {}
	void OnLogin(Player* player) override
	{
		time_t signinTime = 0;
		uint32 signinDays = 0;

		sSignIn->GetInfo(player, signinTime, signinDays);

		//未连续签到
		if (sSignIn->GetDiffDay(signinTime, time(NULL)) > 1)
			signinDays = 0;

		player->signinTime = signinTime;
		player->signinDays = signinDays;
	}
};

void AddSC_SignIn()
{
	//new SignInPlayerScript();
}
