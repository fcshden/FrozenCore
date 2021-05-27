#pragma execution_character_set("utf-8")
#include "InstanceDieTele.h"
#include "../CustomEvent/Event.h"

std::unordered_map<uint32, InstanceDieTeleTemplate> InstanceDieTeleMap;

void InstanceDieTele::Load()
{
	InstanceDieTeleMap.clear();
	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 地图ID,联盟死亡传送坐标ID,部落死亡传送坐标ID,是否唯一 FROM _副本_死亡传送" :
		"SELECT MapId,DieTelePos_A,DieTelePos_H,IsUnique FROM _instance");
	if (!result) return;
	do
	{
		Field* fields = result->Fetch();
		uint32 MapId = fields[0].GetUInt32();
		InstanceDieTeleTemplate Temp;
		Temp.DieTelePos_A = fields[1].GetUInt32();
		Temp.DieTelePos_H = fields[2].GetUInt32();
		Temp.Unique		  = fields[3].GetBool();
		InstanceDieTeleMap.insert(std::make_pair(MapId, Temp));
	} while (result->NextRow());
}

bool InstanceDieTele::Tele(Player* player)
{
	if (!player->GetMap()->Instanceable())
		return false;

	uint32 posId = 0;

	auto itr = InstanceDieTeleMap.find(player->GetMap()->GetId());
	if (itr != InstanceDieTeleMap.end())
		posId = player->GetTeamId() == TEAM_ALLIANCE ? itr->second.DieTelePos_A : itr->second.DieTelePos_H;

	auto it = PosMap.find(posId);

	if (it != PosMap.end())
	{
		player->TeleportTo(it->second.map, it->second.x, it->second.y, it->second.z, it->second.o);
		player->ResurrectPlayer(1.0);
		player->SpawnCorpseBones();
		return true;
	}

	return false;
}

bool InstanceDieTele::Unique(uint32 MapId)
{
	auto itr = InstanceDieTeleMap.find(MapId);
	if (itr != InstanceDieTeleMap.end())
		return itr->second.Unique;

	return false;
}
