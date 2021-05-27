#pragma execution_character_set("utf-8")
#include "Faker.h"
#include "../String/myString.h"
#include "../CommonFunc/CommonFunc.h"
#include "../Switch/Switch.h"
#include "Battleground.h"
#include "BattlegroundMgr.h"
#include "WaypointManager.h"
#include "WorldSession.h"

std::unordered_map<uint32, bool>FakerMap;
std::vector<FakerLocationTemplate> FakerLocation;
std::vector<WorldSession *> FakerSessions;

void Faker::Load()
{
	FakerMap.clear();
	FakerLocation.clear();

	QueryResult result;

	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 账号ID FROM _假人_在线假人" :
		"SELECT accountId FROM _faker_online"))
	{
		do
		{
			if (QueryResult charResult = CharacterDatabase.PQuery("SELECT guid FROM characters WHERE account = %u", result->Fetch()[0].GetUInt32()))
			{
				do
				{
					FakerMap.insert(std::make_pair(charResult->Fetch()[0].GetUInt32(), false));
				} while (charResult->NextRow());
			}
		} while (result->NextRow());
	}

	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 

		"SELECT 地图ID,X坐标,Y坐标,Z坐标,O坐标 FROM _假人_在线假人_坐标" :
		"SELECT Map,X,Y,Z,O FROM _faker_online_postion"))
	{
		do
		{
			Field* fields = result->Fetch();
			FakerLocationTemplate Temp;
			Temp.Map		= fields[0].GetUInt32();
			Temp.X			= fields[1].GetFloat();
			Temp.Y			= fields[2].GetFloat();
			Temp.Z			= fields[3].GetFloat();
			Temp.O			= fields[4].GetFloat();
			FakerLocation.push_back(Temp);
		} while (result->NextRow());
	}
}

void Faker::Add(bool ingnoreLimit)
{
	uint32 count = 0;

	for (auto itr = FakerMap.begin(); itr != FakerMap.end(); itr++)
		if (itr->second)
			count++;

	if (!ingnoreLimit && count >= sSwitch->GetValue(ST_FAKER_ONLINE))
		return;

	//std::random_shuffle(FakerMap.begin(), FakerMap.end());
	
	for (auto itr = FakerMap.begin(); itr != FakerMap.end(); itr++)
	{
		if (itr->second)
			continue;

		AddFaker(itr->first);
		break;
	}
}


DWORD WINAPI FakerThread(LPVOID i)
{
	int guid = reinterpret_cast<int>(i);

	QueryResult result = CharacterDatabase.PQuery("SELECT account FROM characters WHERE guid = %u", guid);

	if (!result)
		return 0;

	uint32 account = result->Fetch()[0].GetUInt32();

	WorldSession *FakerSession = new WorldSession(account, NULL, SEC_PLAYER, 2, time(NULL), LOCALE_zhCN, 0, false, true, 0);

	if (!FakerSession)
	{
		delete FakerSession;
		return 0;
	}

	FakerSession->SetPlayerLoading(true);

	LoginQueryHolder *holder = new LoginQueryHolder(account, MAKE_NEW_GUID(guid, 0, HIGHGUID_PLAYER));

	if (!holder->Initialize())
	{
		delete holder;
		FakerSession->SetPlayerLoading(false);
		return 0;
	}

	QueryResultHolderFuture future = CharacterDatabase.DelayQueryHolder((SQLQueryHolder*)holder);

	while (true)
	{
		if (future.ready())
		{
			SQLQueryHolder* param;
			future.get(param);
			LoginQueryHolder* holder = (LoginQueryHolder*)param;
			FakerSession->HandleFakerLoginFromDB((LoginQueryHolder*)param);
			future.cancel();
			
			FakerSessions.push_back(FakerSession);
			auto itr = FakerMap.find(GUID_LOPART(holder->GetGuid()));
			if (itr != FakerMap.end())
				itr->second = true;

			return 0;
		}
	}

	return 0;
}


void AddFaker(int i)
{
	CloseHandle(CreateThread(NULL, 0, FakerThread, reinterpret_cast<LPVOID>(i), 0, NULL));
}

void Faker::Remove(uint64 guid)
{
	for (auto itr = FakerSessions.begin(); itr != FakerSessions.end();)
	{
		if ((*itr)->GetPlayer()->GetGUID() == guid)
		{
			WorldSession* s = *itr;
			s->LogoutPlayer(true);
			delete s;
			itr = FakerSessions.erase(itr);
		}
		else
			itr++;
	}
}
		
		
void Faker::UpdateAllSessions(uint32 diff)
{
	for (auto itr = FakerSessions.begin(); itr != FakerSessions.end(); itr++)
	{
		WorldSession * s = *itr;
		if (s && s->GetPlayer()->IsBeingTeleportedFar())
			s->HandleMoveWorldportAckOpcode();
	}
}

void Faker::Update(Player* faker, uint32 diff)
{
	if (!faker->IsFaker)
		return;

	if (faker->IsBeingTeleportedNear())
	{
		WorldPacket p = WorldPacket(MSG_MOVE_TELEPORT_ACK, 8 + 4 + 4);
		p.appendPackGUID(faker->GetGUID());
		p << (uint32)0;
		p << (uint32)time(0);
		faker->GetSession()->HandleMoveTeleportAck(p);
	}
	else if (faker->IsInWorld())
		faker->GetSession()->HandleFakerPackets();


	faker->FakerMoveTimer += diff;

	if (faker->FakerMoveTimer >= 120 * IN_MILLISECONDS)
	{
		faker->FakerMoveTimer = 0;
		
		if (faker->isMoving() || faker->IsBeingTeleported() || FakerLocation.empty())
			return;

		FakerLocationTemplate location = FakerLocation[urand(0, FakerLocation.size() - 1)];

		if (faker->GetMapId() != location.Map)
			faker->TeleportTo(location.Map, location.X + frand(-1, 1), location.Y + frand(-1, 1), location.Z, location.O);		
		else
			faker->GetMotionMaster()->MoveCharge(location.X + frand(-1, 1), location.Y + frand(-1, 1), location.Z, faker->GetSpeed(MOVE_RUN));
	}
}


class FakerLogin : PlayerScript
{
public:
	FakerLogin() : PlayerScript("FakerLogin") {}

	void OnLogin(Player* player)
	{
		if (player->IsFaker)
			return;
		
		sFaker->Add(false);
	}
};
void AddSC_FakerLogin()
{
	new FakerLogin();
}
