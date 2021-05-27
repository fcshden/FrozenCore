#pragma execution_character_set("utf-8")
#include "ZoneAura.h"
std::unordered_map<uint32/*zone*/, ZoneAuraTemplate> ZoneAuraMap;

void ZoneAura::Load()
{
	ZoneAuraMap.clear();										
	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 区域ID,光环ID,玩家最小血量 FROM _属性调整_区域平衡光环" :
		"SELECT zone,aura,limitHP FROM _pvp_gap_aura");

	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			uint32 zone = fields[0].GetUInt32();
			ZoneAuraTemplate Temp;
			Temp.aura		= fields[1].GetUInt32();
			Temp.limitHP	= fields[2].GetUInt32();
			ZoneAuraMap.insert(std::make_pair(zone,Temp));
		} while (result->NextRow());
	}
}

int32 ZoneAura::GetAuraStack(Map* map, uint32 zone, uint32 limitHP)
{
	if (!map)
		return 0;

	int32 A_Count = 0;
	int32 H_Count = 0;

	Map::PlayerList const& players = map->GetPlayers();

	if (players.isEmpty())
		return 0;

	for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
	{
		if (Player* player = i->GetSource())
		{
			if (player->GetZoneId() != zone || player->GetMaxHealth() < limitHP)
				continue;

			if (player->GetTeamId() == TEAM_ALLIANCE)
				A_Count++;
			else
				H_Count++;
		}
	}

	return A_Count - H_Count;
}
void ZoneAura::UpdateAura(Map* map)
{
	if (!map || ZoneAuraMap.empty())
		return;

	for (auto iter = ZoneAuraMap.begin(); iter != ZoneAuraMap.end(); iter++)
	{
		if (map->GetId() != GetMapIdByZone(iter->first))
			continue;

		int32 stack = GetAuraStack(map, iter->first, iter->second.limitHP);

		Map::PlayerList const& players = map->GetPlayers();

		if (players.isEmpty())
			continue;

		for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
		{
			if (Player* player = i->GetSource())
			{
				if (player->GetZoneId() != iter->first)
					continue;
					
				if (stack > 0)
				{
					if (player->GetTeamId() == TEAM_HORDE)
						player->SetAuraStack(iter->second.aura, player, stack);
					else
						player->RemoveAura(iter->second.aura);
										
				}
				else if (stack < 0)
				{
					if (player->GetTeamId() == TEAM_ALLIANCE)
						player->SetAuraStack(iter->second.aura, player, -stack);
					else
						player->RemoveAura(iter->second.aura);
						
				}else
					player->RemoveAura(iter->second.aura);
			}
		}		
	}
}

class ZoneAuraScript : PlayerScript
{
public:
	ZoneAuraScript() : PlayerScript("ZoneAuraScript") {}
	void OnUpdateZone(Player* player, uint32 /*newZone*/, uint32 /*newArea*/)
	{
		for (auto iter = ZoneAuraMap.begin(); iter != ZoneAuraMap.end(); iter++)
			player->RemoveAura(iter->second.aura);
	}
};

void AddSC_ZoneAura()
{
	new ZoneAuraScript();
}
