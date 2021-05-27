//#pragma execution_character_set("utf-8")
//#include "Event.h"
//#include "GameEventMgr.h"
//#include "Battleground.h"
//#include "BattlegroundMgr.h"
//#include "../CustomEvent/FixedTimeBG/FixedTimeBG.h"
//#include "../Reward/Reward.h"
//#include "GroupMgr.h"
//#include "Group.h"
//#include "../FakePlayers/FakePlayers.h"
//#include "ThunderbluffBattle/ThunderbluffBattle.h"
//#include "../AuthCheck/AuthCheck.h"
//#include "../CommonFunc/CommonFunc.h"
//#include "SACity/SA.h"
//#include "../QuickResponse/QuickResponse.h"
//#include "../Talisman/Talisman.h"
//#include "../FunctionCollection/FunctionCollection.h"
//#include "UnknownBG/UnknownBG.h"
//#include "../Command/CustomCommand.h"
//#include "WaypointManager.h"
//#include "MapManager.h"
//// load db
//
//std::unordered_map<uint32, EventDataTemplate> EventDataMap;
//std::vector<EventActiveTemplate> EventActiveVec;
//
//
///*击杀生物*/
//std::unordered_map<uint32/*creature entry*/, uint32> KillCreatureMap;
///*摧毁物体*/
//std::unordered_map<uint32/*gob entry*/, uint32> DestroyGobMap;
///*击杀玩家*/
//std::unordered_map<TeamTypes/*event team*/, uint32> KillPlayerMap;
///*玩家信息*/
//std::unordered_map<uint32/*guid*/, EventPlayerTemplate> PlayerDataMap;
///*队伍信息*/
//std::unordered_map<uint32/*guid*/, TeamTypes> GroupDataMap;
//
//void CustomEvent::Load()
//{
//	EventDataMap.clear();
//	EventActiveVec.clear();
//
//	QueryResult result = WorldDatabase.PQuery("SELECT eventEntry,description,gossipText,noticeText,event_type,group_type,rejoin_type,start_defense_pos,start_offense_pos,defenseName,offenseName,zone_id FROM game_event WHERE eventEntry >= 70");
//	if (result)
//	{
//		do
//		{
//			//加载data
//			Field* fields = result->Fetch();
//			uint32 eventId = fields[0].GetUInt32();
//			EventDataTemplate Temp;
//			Temp.eventName			= fields[1].GetString();
//			Temp.gossipText			= fields[2].GetString();
//			Temp.noticeText			= fields[3].GetString();
//			Temp.eventType			= EventTypes(fields[4].GetUInt32());
//			Temp.groupType			= GroupTypes(fields[5].GetUInt32());
//			Temp.rejoinType			= RejoinTypes(fields[6].GetUInt32());
//			Temp.startDefensePos	= fields[7].GetUInt32();
//			Temp.startOffensePos	= fields[8].GetUInt32();
//			Temp.defenseName		= fields[9].GetString();
//			Temp.offenseName		= fields[10].GetString();
//			Temp.zoneId				= fields[11].GetUInt32();
//
//			Temp.eventPhaseMap.clear();
//
//			Temp.rew_winRewId = 0;
//			Temp.rew_losRewId = 0;
//			Temp.rew_killsForRew = 0;
//			Temp.rew_killedsForRew = 0;
//			Temp.rew_damageForRew = 0;
//			Temp.rew_healForRew = 0;
//			Temp.stop_creature1 = 0;
//			Temp.stop_creatureCount1 = 0;
//			Temp.stop_creature2 = 0;
//			Temp.stop_creatureCount2 = 0;
//			Temp.stop_creature3 = 0;
//			Temp.stop_creatureCount3 = 0;
//			Temp.stop_creature4 = 0;
//			Temp.stop_creatureCount4 = 0;
//			Temp.stop_creature5 = 0;
//			Temp.stop_creatureCount5 = 0;
//			Temp.stop_gameobject1 = 0;
//			Temp.stop_gameobjectCount1 = 0;
//			Temp.stop_gameobject2 = 0;
//			Temp.stop_gameobjectCount2 = 0;
//			Temp.stop_gameobject3 = 0;
//			Temp.stop_gameobjectCount3 = 0;
//			Temp.stop_gameobject4 = 0;
//			Temp.stop_gameobjectCount4 = 0;
//			Temp.stop_gameobject5 = 0;
//			Temp.stop_gameobjectCount5 = 0;
//			Temp.stop_defenseKills = 0;
//			Temp.stop_offenseKills = 0;
//			Temp.stop_killGaps = 0;
//			Temp.state_creature1 = 0;
//			Temp.state_creatureWorldState1 = 0;
//			Temp.state_creature2 = 0;
//			Temp.state_creatureWorldState2 = 0;
//			Temp.state_creature3 = 0;
//			Temp.state_creatureWorldState3 = 0;
//			Temp.state_gameobject1 = 0;
//			Temp.state_gameobjectWorldState1 = 0;
//			Temp.state_gameobject2 = 0;
//			Temp.state_gameobjectWorldState2 = 0;
//			Temp.state_gameobject3 = 0;
//			Temp.state_gameobjectWorldState3 = 0;
//			Temp.state_defenseWorldState = 0;
//			Temp.state_offenseWorldState = 0;
//			EventDataMap.insert(std::make_pair(eventId, Temp));
//
//		} while (result->NextRow());
//	}
//
//	//加载WorldStateVec
//	result = WorldDatabase.PQuery("SELECT eventId,creature1,creatureWorldState1,creature2,creatureWorldState2,creature3,creatureWorldState3,gameobject1,gameobjectWorldState1,gameobject2,gameobjectWorldState2,gameobject3,gameobjectWorldState3,defenseWorldState,offenseWorldState FROM _game_event_world_state");
//	if (result)
//	{
//		do
//		{
//			Field* fields = result->Fetch();
//			uint32 eventId = fields[0].GetUInt32();
//
//			std::unordered_map<uint32, EventDataTemplate>::iterator iter = EventDataMap.find(eventId);
//		
//			if (iter != EventDataMap.end())
//			{
//				iter->second.state_creature1 = fields[1].GetUInt32();
//				iter->second.state_creatureWorldState1 = fields[2].GetUInt32();
//				iter->second.state_creature2 = fields[3].GetUInt32();
//				iter->second.state_creatureWorldState2 = fields[4].GetUInt32();
//				iter->second.state_creature3 = fields[5].GetUInt32();
//				iter->second.state_creatureWorldState3 = fields[6].GetUInt32();
//				iter->second.state_gameobject1 = fields[7].GetUInt32();
//				iter->second.state_gameobjectWorldState1 = fields[8].GetUInt32();
//				iter->second.state_gameobject2 = fields[9].GetUInt32();
//				iter->second.state_gameobjectWorldState2 = fields[10].GetUInt32();
//				iter->second.state_gameobject3 = fields[11].GetUInt32();
//				iter->second.state_gameobjectWorldState3 = fields[12].GetUInt32();
//				iter->second.state_defenseWorldState = fields[13].GetUInt32();
//				iter->second.state_offenseWorldState = fields[14].GetUInt32();
//			}
//		} while (result->NextRow());
//	}
//
//	//加载RewVec
//	result = WorldDatabase.PQuery("SELECT eventId,winRewId,losRewId,killsForRew,killedsForRew,damageForRew,healForRew FROM _game_event_rew_setting");
//	if (result)
//	{
//		do
//		{
//			Field* fields = result->Fetch();
//			uint32 eventId = fields[0].GetUInt32();
//
//			std::unordered_map<uint32, EventDataTemplate>::iterator iter = EventDataMap.find(eventId);
//
//			if (iter != EventDataMap.end())
//			{
//				iter->second.rew_winRewId = fields[1].GetUInt32();
//				iter->second.rew_losRewId = fields[2].GetUInt32();
//				iter->second.rew_killsForRew = fields[3].GetUInt32();
//				iter->second.rew_killedsForRew = fields[4].GetUInt32();
//				iter->second.rew_damageForRew = fields[5].GetFloat();
//				iter->second.rew_healForRew = fields[6].GetFloat();
//			}
//
//		} while (result->NextRow());
//	}
//	
//
//	//加载StopConditionVec
//	result = WorldDatabase.PQuery("SELECT eventId,creature1,creatureCount1,creature2,creatureCount2,creature3,creatureCount3,creature4,creatureCount4,creature5,creatureCount5, gameobject1, gameobjectCount1, gameobject2, gameobjectCount2, gameobject3, gameobjectCount3, gameobject4, gameobjectCount4, gameobject5, gameobjectCount5, defenseKills, offenseKills, killGaps FROM _game_event_stop_condition");
//	if (result)
//	{
//		do
//		{
//			Field* fields = result->Fetch();
//			uint32 eventId = fields[0].GetUInt32();
//
//			std::unordered_map<uint32, EventDataTemplate>::iterator iter = EventDataMap.find(eventId);
//
//			if (iter != EventDataMap.end())
//			{
//				iter->second.stop_creature1 = fields[1].GetUInt32();
//				iter->second.stop_creatureCount1 = fields[2].GetUInt32();
//				iter->second.stop_creature2 = fields[3].GetUInt32();
//				iter->second.stop_creatureCount2 = fields[4].GetUInt32();
//				iter->second.stop_creature3 = fields[5].GetUInt32();
//				iter->second.stop_creatureCount3 = fields[6].GetUInt32();
//				iter->second.stop_creature4 = fields[7].GetUInt32();
//				iter->second.stop_creatureCount4 = fields[8].GetUInt32();
//				iter->second.stop_creature5 = fields[9].GetUInt32();
//				iter->second.stop_creatureCount5 = fields[10].GetUInt32();
//				iter->second.stop_gameobject1 = fields[11].GetUInt32();
//				iter->second.stop_gameobjectCount1 = fields[12].GetUInt32();
//				iter->second.stop_gameobject2 = fields[13].GetUInt32();
//				iter->second.stop_gameobjectCount2 = fields[14].GetUInt32();
//				iter->second.stop_gameobject3 = fields[15].GetUInt32();
//				iter->second.stop_gameobjectCount3 = fields[16].GetUInt32();
//				iter->second.stop_gameobject4 = fields[17].GetUInt32();
//				iter->second.stop_gameobjectCount4 = fields[18].GetUInt32();
//				iter->second.stop_gameobject5 = fields[19].GetUInt32();
//				iter->second.stop_gameobjectCount5 = fields[20].GetUInt32();
//				iter->second.stop_defenseKills = fields[21].GetUInt32();
//				iter->second.stop_offenseKills = fields[22].GetUInt32();
//				iter->second.stop_killGaps = fields[23].GetUInt32();
//			}
//		} while (result->NextRow());
//	}
//	
//
//
//	//加载eventPhaseMap
//
//	for (auto iter = EventDataMap.begin(); iter != EventDataMap.end(); iter++)
//	{
//		result = WorldDatabase.PQuery("SELECT phase,stop_creature_guid,stop_gameobject_guid,stop_player_kills,"
//			"stop_defense_tele_pos,stop_offense_tele_pos,defense_graveyard_pos,offense_graveyard_pos,stop_defense_notice,stop_offense_notice,"
//			"defense_graveyard_safe,offense_graveyard_safe,defense_graveyard_healer,offense_graveyard_healer FROM _game_event_phase WHERE eventId = '%u';", iter->first);
//		if (result)
//		{
//			do
//			{
//				Field* fields = result->Fetch();
//				uint32 phase = fields[0].GetUInt32();
//				EventPhaseTemplate Temp;
//				Temp.stop_creature			= fields[1].GetUInt32();
//				Temp.stop_gameobject		= fields[2].GetUInt32();
//				Temp.stop_kills				= fields[3].GetUInt32();
//				Temp.stop_defense_tele_pos	= fields[4].GetUInt32();
//				Temp.stop_offense_tele_pos	= fields[5].GetUInt32();
//				Temp.defense_graveyard_pos	= fields[6].GetUInt32();
//				Temp.offense_graveyard_pos	= fields[7].GetUInt32();
//				Temp.stop_defense_notice	= fields[8].GetString();
//				Temp.stop_offense_notice	= fields[9].GetString();
//				Temp.defense_graveyard_safe = fields[10].GetFloat();
//				Temp.offense_graveyard_safe = fields[11].GetFloat();
//				Temp.defense_graveyard_healer = fields[12].GetUInt32();
//				Temp.offense_graveyard_healer = fields[13].GetUInt32();
//				iter->second.eventPhaseMap.insert(std::make_pair(phase, Temp));
//
//			} while (result->NextRow());
//		}
//	}
//
//	//加载ActiveVec
//	result = WorldDatabase.PQuery("SELECT eventId,activeGuid,activeType,creatureEntry,killCount,gameobjectEntry,destroyCount FROM _game_event_active_condition");
//	if (result)
//	{
//		do
//		{
//			Field* fields = result->Fetch();
//			EventActiveTemplate Temp;
//			Temp.eventId = fields[0].GetUInt32();
//			Temp.activeGUID = fields[1].GetUInt32();
//			Temp.activeType = fields[2].GetUInt32();
//			Temp.creatureEntry = fields[3].GetUInt32();
//			Temp.killCount = fields[4].GetUInt32();
//			Temp.gameobjectEntry = fields[5].GetUInt32();
//			Temp.destroyCount = fields[6].GetUInt32();
//			EventActiveVec.push_back(Temp);
//		} while (result->NextRow());
//	}
//}
//
//std::unordered_map<uint32, PosTemplate> PosMap;
//
//void CustomEvent::LoadPos()
//{
//	PosMap.clear();
//	QueryResult result = WorldDatabase.PQuery("SELECT map,x,y,z,o,posId FROM _position");
//	if (!result) return;
//	do
//	{
//		Field* fields = result->Fetch();
//		PosTemplate Temp;
//		Temp.map = fields[0].GetUInt32();
//		Temp.x = fields[1].GetFloat();
//		Temp.y = fields[2].GetFloat();
//		Temp.z = fields[3].GetFloat();
//		Temp.o = fields[4].GetFloat();
//		uint32 ID = fields[5].GetUInt32();
//		PosMap.insert(std::make_pair(ID, Temp));
//	} while (result->NextRow());
//}
//
//void CustomEvent::GetTelePos(uint32 eventId, PosTypes type, uint32 &map, float &x, float &y, float &z, float &o)
//{
//	uint32 posId = 0;
//
//	std::unordered_map<uint32, EventDataTemplate>::iterator iter = EventDataMap.find(eventId);
//
//	if (iter != EventDataMap.end())
//	{
//		switch (type)
//		{
//		case C_PT_START_DEFENSE:
//			posId = iter->second.startDefensePos;
//			break;
//		case C_PT_START_OFFENSE:
//			posId = iter->second.startOffensePos;
//			break;
//		}
//	}
//
//	std::unordered_map<uint32, PosTemplate>::iterator it = PosMap.find(posId);
//
//	if (it != PosMap.end())
//	{
//		map = it->second.map;
//		x	= it->second.x;
//		y	= it->second.y;
//		z	= it->second.z;
//		o	= it->second.o;
//	}
//
//}
//
//void CustomEvent::GetTelePos(uint32 posId, uint32 &map, float &x, float &y, float &z, float &o)
//{
//	std::unordered_map<uint32, PosTemplate>::iterator it = PosMap.find(posId);
//
//	if (it != PosMap.end())
//	{
//		map = it->second.map;
//		x = it->second.x;
//		y = it->second.y;
//		z = it->second.z;
//		o = it->second.o;
//	}
//}
//
//void CustomEvent::SummonSoulHealer()
//{
//	if (Map* map = sMapMgr->FindMap(_mapId, 0))
//	{
//		auto it = PosMap.find(_graveyard_defense_pos);
//
//		if (it != PosMap.end())
//		{
//			if (Creature* healer = map->GetCreature(_soulhealer_defense))
//				healer->RemoveFromWorld();
//
//			Position pos;
//			pos.Relocate(it->second.x, it->second.y, it->second.z, it->second.o);
//			if (Creature* healer = map->SummonCreature(_defense_graveyard_healer, pos, 0, 60 * 60 * IN_MILLISECONDS))
//			{
//				_soulhealer_defense = healer->GetGUID();
//				healer->setFaction(DEFENSE_FACTION);
//				healer->setDeathState(DEAD);
//				healer->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, healer->GetGUID());
//				healer->SetUInt32Value(UNIT_CHANNEL_SPELL, SPELL_SPIRIT_HEAL_CHANNEL);
//				healer->SetFloatValue(UNIT_MOD_CAST_SPEED, 1.0f);
//			}		
//		}
//
//		it = PosMap.find(_graveyard_offense_pos);
//
//		if (it != PosMap.end())
//		{
//			if (Creature* healer = map->GetCreature(_soulhealer_offense))
//				healer->RemoveFromWorld();
//
//			Position pos;
//			pos.Relocate(it->second.x, it->second.y, it->second.z, it->second.o);
//			if (Creature* healer = map->SummonCreature(_offense_graveyard_healer, pos, 0, 60 * 60 * IN_MILLISECONDS))
//			{
//				_soulhealer_offense = healer->GetGUID();
//				healer->setFaction(OFFENSE_FACTION);
//				healer->setDeathState(DEAD);
//				healer->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, healer->GetGUID());
//				healer->SetUInt32Value(UNIT_CHANNEL_SPELL, SPELL_SPIRIT_HEAL_CHANNEL);
//				healer->SetFloatValue(UNIT_MOD_CAST_SPEED, 1.0f);
//			}			
//		}
//	}
//}
//
//void CustomEvent::InitParams(uint32 eventId)
//{
//	std::unordered_map<uint32, EventDataTemplate>::iterator iter = EventDataMap.find(eventId);
//
//	if (iter != EventDataMap.end())
//	{
//		_eventId = eventId;
//		_winTeam = C_TEAM_NONE;
//		_expired = false;
//		KillCreatureMap.clear();
//		DestroyGobMap.clear();
//		KillPlayerMap.clear();
//		PlayerDataMap.clear();
//		GroupDataMap.clear();
//
//		/*事件信息*/
//		_eventType = iter->second.eventType;
//		_groupType = iter->second.groupType;
//		_rejoinType = iter->second.rejoinType;
//		_eventName = iter->second.eventName;
//		_gossipText = iter->second.gossipText;
//		_noticeText = iter->second.noticeText;
//		_defenseName = iter->second.defenseName;
//		_offenseName = iter->second.offenseName;
//		_startDefensePos = iter->second.startDefensePos;
//		_startOffensePos = iter->second.startOffensePos;
//
//		_zoneId = iter->second.zoneId;
//		_mapId = GetMapIdByZone(_zoneId);
//		
//		
//		_eventPhase = 1;
//		_stop_creature_flag = false;
//		_stop_gameobject_flag = false;
//		_stop_kills_flag = false;
//
//		_graveyard_defense_pos = 0;
//		_graveyard_offense_pos = 0;
//		_soulhealer_defense = 0;
//		_soulhealer_offense = 0;
//		_defense_graveyard_safe = 0;
//		_offense_graveyard_safe = 0;
//		_defense_graveyard_healer = 0;
//		_offense_graveyard_healer = 0;
//
//		auto iter = EventDataMap.find(_eventId);
//
//		if (iter != EventDataMap.end())
//		{
//			auto i = iter->second.eventPhaseMap.find(_eventPhase);
//
//			if (i != iter->second.eventPhaseMap.end())
//			{
//				_graveyard_defense_pos = i->second.defense_graveyard_pos;
//				_graveyard_offense_pos = i->second.offense_graveyard_pos;
//				_defense_graveyard_safe = i->second.defense_graveyard_safe;
//				_offense_graveyard_safe = i->second.offense_graveyard_safe;
//				_defense_graveyard_healer = i->second.defense_graveyard_healer;
//				_offense_graveyard_healer = i->second.offense_graveyard_healer;
//				SummonSoulHealer();
//			}
//		}
//
//
//		_reviveUpdateTimer = REVIVE_INTERVALS;
//		_safeAreaUpdateTimer = IN_MILLISECONDS;
//
//		_rew_winRewId = iter->second.rew_winRewId;
//		_rew_losRewId = iter->second.rew_losRewId;
//		_rew_killsForRew = iter->second.rew_killsForRew;
//		_rew_killedsForRew = iter->second.rew_killedsForRew;
//		_rew_damageForRew = iter->second.rew_damageForRew;
//		_rew_healForRew = iter->second.rew_healForRew;
//
//		_stop_creature1 = iter->second.stop_creature1;
//		_stop_creatureCount1 = iter->second.stop_creatureCount1;
//		_stop_creature2 = iter->second.stop_creature2;
//		_stop_creatureCount2 = iter->second.stop_creatureCount2;
//		_stop_creature3 = iter->second.stop_creature3;
//		_stop_creatureCount3 = iter->second.stop_creatureCount3;
//		_stop_creature4 = iter->second.stop_creature4;
//		_stop_creatureCount4 = iter->second.stop_creatureCount4;
//		_stop_creature5 = iter->second.stop_creature5;
//		_stop_creatureCount5 = iter->second.stop_creatureCount5;
//
//		_stop_gameobject1 = iter->second.stop_gameobject1;
//		_stop_gameobjectCount1 = iter->second.stop_gameobjectCount1;
//		_stop_gameobject2 = iter->second.stop_gameobject2;
//		_stop_gameobjectCount2 = iter->second.stop_gameobjectCount2;
//		_stop_gameobject3 = iter->second.stop_gameobject3;
//		_stop_gameobjectCount3 = iter->second.stop_gameobjectCount3;
//		_stop_gameobject4 = iter->second.stop_gameobject4;
//		_stop_gameobjectCount4 = iter->second.stop_gameobjectCount4;
//		_stop_gameobject5 = iter->second.stop_gameobject5;
//		_stop_gameobjectCount5 = iter->second.stop_gameobjectCount5;
//
//		_stop_defenseKills = iter->second.stop_defenseKills;
//		_stop_offenseKills = iter->second.stop_offenseKills;
//
//		_state_creature1 = iter->second.state_creature1;
//		_state_creatureWorldState1 = iter->second.state_creatureWorldState1;
//		_state_creature2 = iter->second.state_creature2;
//		_state_creatureWorldState2 = iter->second.state_creatureWorldState2;
//		_state_creature3 = iter->second.state_creature3;
//		_state_creatureWorldState3 = iter->second.state_creatureWorldState3;
//		_state_gameobject1 = iter->second.state_gameobject1;
//		_state_gameobjectWorldState1 = iter->second.state_gameobjectWorldState1;
//		_state_gameobject2 = iter->second.state_gameobject2;
//		_state_gameobjectWorldState2 = iter->second.state_gameobjectWorldState2;
//		_state_gameobject3 = iter->second.state_gameobject3;
//		_state_gameobjectWorldState3 = iter->second.state_gameobjectWorldState3;
//		_state_defenseWorldState = iter->second.state_defenseWorldState;
//		_state_offenseWorldState = iter->second.state_offenseWorldState;
//	}
//}
//
//void CustomEvent::Reset()
//{
//	_eventId = 0;
//	_winTeam = C_TEAM_NONE;
//	_expired = false;
//	_AllyAsOffense = true;
//	KillCreatureMap.clear();
//	DestroyGobMap.clear();
//	KillPlayerMap.clear();
//	PlayerDataMap.clear();
//	GroupDataMap.clear();
//
//	/*事件信息*/
//	_eventType = C_ET_GROUP;
//	_groupType = C_GT_OFFENSE_PLAYER_RANDOM;
//	_rejoinType = C_RT_KEEP_GROUP;
//	_eventName = "";
//	_gossipText = "";
//	_noticeText = "";
//	_defenseName = "";
//	_offenseName = "";
//	_startDefensePos = 0;
//	_startOffensePos = 0;
//
//
//	_eventPhase = 1;
//	_stop_creature_flag = false;
//	_stop_gameobject_flag = false;
//	_stop_kills_flag = false;
//
//	_graveyard_defense_pos = 0;
//	_graveyard_offense_pos = 0;
//	_soulhealer_defense = 0;
//	_soulhealer_offense = 0;
//	_defense_graveyard_safe = 0;
//	_offense_graveyard_safe = 0;
//	_defense_graveyard_safe = 0;
//	_offense_graveyard_safe = 0;
//
//	auto iter = EventDataMap.find(_eventId);
//
//	if (iter != EventDataMap.end())
//	{
//		auto i = iter->second.eventPhaseMap.find(_eventPhase);
//
//		if (i != iter->second.eventPhaseMap.end())
//		{
//			_graveyard_defense_pos = i->second.defense_graveyard_pos;
//			_graveyard_offense_pos = i->second.offense_graveyard_pos;
//			_defense_graveyard_safe = i->second.defense_graveyard_safe;
//			_offense_graveyard_safe = i->second.offense_graveyard_safe;
//		}
//	}
//
//	_zoneId = 0;
//	_mapId = -1;
//	
//	_reviveUpdateTimer = REVIVE_INTERVALS;
//	_safeAreaUpdateTimer = IN_MILLISECONDS;
//
//	_rew_winRewId = 0;
//	_rew_losRewId = 0;
//	_rew_killsForRew = 0;
//	_rew_killedsForRew = 0;
//	_rew_damageForRew = 0;
//	_rew_healForRew = 0;
//
//	_stop_creature1 = 0;
//	_stop_creatureCount1 = 0;
//	_stop_creature2 = 0;
//	_stop_creatureCount2 = 0;
//	_stop_creature3 = 0;
//	_stop_creatureCount3 = 0;
//	_stop_creature4 = 0;
//	_stop_creatureCount4 = 0;
//	_stop_creature5 = 0;
//	_stop_creatureCount5 = 0;
//	_stop_gameobject1 = 0;
//	_stop_gameobjectCount1 = 0;
//	_stop_gameobject2 = 0;
//	_stop_gameobjectCount2 = 0;
//	_stop_gameobject3 = 0;
//	_stop_gameobjectCount3 = 0;
//	_stop_gameobject4 = 0;
//	_stop_gameobjectCount4 = 0;
//	_stop_gameobject5 = 0;
//	_stop_gameobjectCount5 = 0;
//	_stop_defenseKills = 0;
//	_stop_offenseKills = 0;
//	_stop_killGaps = 0;
//
//	_state_creature1 = 0;
//	_state_creatureWorldState1 = 0;
//	_state_creature2 = 0;
//	_state_creatureWorldState2 = 0;
//	_state_creature3 = 0;
//	_state_creatureWorldState3 = 0;
//	_state_gameobject1 = 0;
//	_state_gameobjectWorldState1 = 0;
//	_state_gameobject2 = 0;
//	_state_gameobjectWorldState2 = 0;
//	_state_gameobject3 = 0;
//	_state_gameobjectWorldState3 = 0;
//	_state_defenseWorldState = 0;
//	_state_offenseWorldState = 0;
//}
//
//void CustomEvent::Start(uint32 eventId)
//{
//	if (eventId < 70)
//		return;
//
//	if (eventId == EVENT_ID_QUICK_RESPONSE)
//	{
//		sQuickResponse->Anounce();
//		return;
//	}
//
//	//在此做假人上下线 切换区域等事件处理
//
//	if (eventId == EVENT_ID_FAKE_PLAYER_LOGIN_OR_LOGOUT)
//	{
//		sFakePlayers->LoginOrLogout();
//		return;
//	}
//
//	if (eventId == EVENT_ID_FAKE_PLAYER_UPDATE_ZONE_ID)
//	{
//		sFakePlayers->UpdateZone();
//		return;
//	}
//
//	if (eventId == EVENT_ID_FAKE_PLAYER_LEVEL_UP)
//	{
//		sFakePlayers->LevelUp();
//		return;
//	}
//
//	
//	InitParams(eventId);
//	InitWorldState(eventId);
//
//	std::ostringstream oss;
//	oss << _eventName << "开始！";
//	sWorld->SendServerMessage(SERVER_MSG_STRING, oss.str().c_str());
//
//	if (_noticeText.empty())
//		return;
//
//	SendInvitationToAll(_noticeText);
//
//	if (eventId == UKBG_EVENT_ID)
//	{
//		sUkBG->Start();
//		return;
//	}
//
//	if (_eventType != C_ET_GROUP)
//		return;
//
//	oss.str("");
//	oss << _eventName;
//
//	switch (_groupType)
//	{
//	case C_GT_OFFENSE_ALL:
//		oss << "所有玩家将加入" << _offenseName << "！";
//		break;
//	case C_GT_OFFENSE_ALLY:
//		oss << "联盟将加入" << _offenseName << "，部落将加入" << _defenseName << "！";
//		break;
//	case C_GT_OFFENSE_HORDE:
//		oss << "联盟将加入" << _defenseName << "，部落将加入" << _offenseName << "！";
//		break;
//	case C_GT_OFFENSE_FACTION_RANDOM:
//		_AllyAsOffense ? oss << "联盟将加入" << _offenseName << "，部落将加入" << _defenseName << "！" : oss << "联盟将加入" << _defenseName << "，部落将加入" << _offenseName << "！";
//		break;
//	case C_GT_OFFENSE_PLAYER_RANDOM:
//		oss << "玩家将随机加入" << _defenseName << "或" << _offenseName << "！";
//		break;
//	}
//
//	sWorld->SendServerMessage(SERVER_MSG_STRING, oss.str().c_str());
//
//	oss.str("");
//	oss << _eventName;
//
//	switch (_rejoinType)
//	{
//	case C_RT_BANED:
//		oss << "重新加入活动将被禁止！";
//		break;
//	case C_RT_KEEP_GROUP:
//		oss << "重新加入活动不会改变分组！";
//		break;
//	case C_RT_REDIS_GROUP:
//		oss << "重新加入活动时，系统会重新分组！";
//		break;
//	}
//
//	sWorld->SendServerMessage(SERVER_MSG_STRING, oss.str().c_str());
//}
//
//void CustomEvent::Stop(uint32 eventId, bool expired)
//{
//	if (eventId < 70)
//		return;
//
//	//抢答
//	if (eventId == EVENT_ID_QUICK_RESPONSE)
//	{
//		sQuickResponse->Reward();
//		return;
//	}
//
//
//	//战场
//	if (_eventType == C_ET_WS ||
//		_eventType == C_ET_AB ||
//		_eventType == C_ET_EY ||
//		_eventType == C_ET_SA ||
//		_eventType == C_ET_AV ||
//		_eventType == C_ET_IC)
//	{
//		Reset();
//		return;
//	}
//
//
//
//	if (_eventType != C_ET_GROUP)
//		return;
//
//	std::ostringstream oss;
//	oss << _eventName << "结束！";
//
//	if (expired)
//		_winTeam = C_TEAM_DEFENSE;
//	else
//		_winTeam = C_TEAM_OFFENSE;
//
//	//此处做特殊殊事件输赢判断
//	if (eventId == NARROW_EVENT_ID)
//	{
//		uint32 defenseKills = GetKillPlayerCount(C_TEAM_DEFENSE);
//		uint32 offenseKills = GetKillPlayerCount(C_TEAM_OFFENSE);
//
//		if (defenseKills == offenseKills)
//			_winTeam = C_TEAM_NONE;
//		else if (defenseKills > offenseKills)
//			_winTeam = C_TEAM_DEFENSE;
//		else
//			_winTeam = C_TEAM_OFFENSE;
//	}
//
//	switch (_winTeam)
//	{
//	case C_TEAM_NONE:
//		oss << _offenseName << _defenseName << "都没有取得最后的胜利！";
//		break;
//	case C_TEAM_DEFENSE:
//		oss << _defenseName << "获胜！";
//		break;
//	case C_TEAM_OFFENSE:
//		oss << _offenseName << "获胜！";
//		break;
//	}
//
//	sWorld->SendScreenMessage(oss.str().c_str());
//	RewardAllPlayers();
//	ResetAllPlayers();
//	Reset();
//}
//
//void CustomEvent::SendInvitationToAll(std::string text)
//{
//	uint32 menuId = _eventId + 1000000;
//
//	SessionMap const& smap = sWorld->GetAllSessions();
//	for (SessionMap::const_iterator iter = smap.begin(); iter != smap.end(); ++iter)
//		if (Player* player = iter->second->GetPlayer())
//		{
//			if (player->GetSession())
//			{
//				player->CLOSE_GOSSIP_MENU();
//
//				if (player->IsSpectator())
//					continue;
//
//				if (player->isDead())
//				{
//					player->ResurrectPlayer(1.0f);
//					player->SpawnCorpseBones();
//				}
//
//				WorldPacket data(SMSG_GOSSIP_MESSAGE, 100);
//				data << uint64(player->GetGUID());
//				data << uint32(menuId);
//				data << uint32(1);
//				data << uint32(1);
//				data << uint32(1);
//				data << uint8(1);
//				data << uint8(0);
//				data << uint32(0);
//				data << "";
//				data << text;
//				player->GetSession()->SendPacket(&data);
//			}
//		}
//}
//
//void CustomEvent::Process(Player* player, Object* obj)
//{
//	uint32 entry = obj->GetEntry();
//	TeamTypes team = GetTeam(player);
//
//	std::unordered_map<uint32, EventDataTemplate>::iterator iter = EventDataMap.find(_eventId);
//
//	if (iter != EventDataMap.end())
//	{
//		bool endFlag = false;
//		bool announce = false;
//		std::ostringstream oss;
//		std::string playerName = sCF->GetNameLink(player);
//
//		switch (obj->GetTypeId())
//		{
//		case TYPEID_PLAYER:
//		{
//			std::unordered_map<TeamTypes/*event team*/, uint32>::iterator i = KillPlayerMap.find(team);
//
//			if (i != KillPlayerMap.end())
//				i->second++;
//			else
//				KillPlayerMap.insert(std::make_pair(team, 1));
//
//			uint32 defenseKills = GetKillPlayerCount(C_TEAM_DEFENSE);
//			uint32 offenseKills = GetKillPlayerCount(C_TEAM_OFFENSE);
//
//			if (team == C_TEAM_DEFENSE && iter->second.stop_defenseKills > 0)
//			{
//				oss << _eventName << _defenseName << "击杀玩家" << defenseKills << "/" << iter->second.stop_defenseKills;
//				announce = true;
//			}
//				
//			if (team == C_TEAM_OFFENSE && iter->second.stop_offenseKills > 0)
//			{
//				oss << _eventName << _offenseName << "击杀玩家" << offenseKills << "/" << iter->second.stop_offenseKills;
//				announce = true;
//			}
//				
//			if (iter->second.stop_killGaps > 0)
//			{
//				int32 killGaps = defenseKills - offenseKills;
//
//				if (killGaps > 0)
//					oss << "，" << _defenseName << "领先击杀" << killGaps << "/" << iter->second.stop_killGaps;
//				else if (killGaps < 0)
//					oss << "，" << _offenseName << "领先击杀" << abs(killGaps) << "/" << iter->second.stop_killGaps;
//
//				announce = true;
//			}
//		}
//		break;
//		case TYPEID_UNIT:
//		{
//			std::unordered_map<uint32/*creature entry*/, uint32>::iterator i = KillCreatureMap.find(entry);
//
//			if (i != KillCreatureMap.end())
//				i->second++;
//			else
//				KillCreatureMap.insert(std::make_pair(entry, 1));
//
//			uint32 creatureKills = GetKillCreatureCount(entry);
//
//			oss << _eventName << playerName << "击杀了[" << obj->ToCreature()->GetName() << "]   ";
//
//			if (entry == iter->second.stop_creature1 && iter->second.stop_creatureCount1 > 0)
//			{
//				if (creatureKills < iter->second.stop_creatureCount1)
//					oss << creatureKills << "/" << iter->second.stop_creatureCount1;
//				else
//					oss << "[目标达成]";
//				announce = true;
//			}
//			else if (entry == iter->second.stop_creature2 && iter->second.stop_creatureCount2 != 0)
//			{
//				if (creatureKills < iter->second.stop_creatureCount2)
//					oss << creatureKills << "/" << iter->second.stop_creatureCount2;
//				else
//					oss << "[目标达成]";
//				announce = true;
//			}
//			else if (entry == iter->second.stop_creature3 && iter->second.stop_creatureCount3 != 0)
//			{
//				if (creatureKills < iter->second.stop_creatureCount3)
//					oss << creatureKills << "/" << iter->second.stop_creatureCount3;
//				else
//					oss << "[目标达成]";
//				announce = true;
//			}
//			else if (entry == iter->second.stop_creature4 && iter->second.stop_creatureCount4 != 0)
//			{
//				if (creatureKills < iter->second.stop_creatureCount4)
//					oss << creatureKills << "/" << iter->second.stop_creatureCount4;
//				else
//					oss << "[目标达成]";
//				announce = true;
//			}
//			else if (entry == iter->second.stop_creature5 && iter->second.stop_creatureCount5 != 0)
//			{
//				if (creatureKills < iter->second.stop_creatureCount5)
//					oss << creatureKills << "/" << iter->second.stop_creatureCount5;
//				else
//					oss << "[目标达成]";
//				announce = true;
//			}
//		}
//		break;
//		case TYPEID_GAMEOBJECT:
//		{
//			std::unordered_map<uint32/*gob entry*/, uint32>::iterator i = DestroyGobMap.find(entry);
//
//			if (i != DestroyGobMap.end())
//				i->second++;
//			else
//				DestroyGobMap.insert(std::make_pair(entry, 1));
//
//			uint32 count = GetDestroyGobCount(entry);
//
//			oss << _eventName << playerName << "摧毁了[" << obj->ToGameObject()->GetName() << "]   ";
//
//			if (entry == iter->second.stop_gameobject1 && iter->second.stop_gameobjectCount1 != 0)
//			{
//				if (count < iter->second.stop_gameobjectCount1)
//					oss << count << "/" << iter->second.stop_gameobjectCount1;
//				else
//					oss << "[目标达成]";
//				announce = true;
//			}
//			else if (entry == iter->second.stop_gameobject2 && iter->second.stop_gameobjectCount2 != 0)
//			{
//				if (count < iter->second.stop_gameobjectCount2)
//					oss << count << "/" << iter->second.stop_gameobjectCount2;
//				else
//					oss << "[目标达成]";
//				announce = true;
//			}
//			else if (entry == iter->second.stop_gameobject3 && iter->second.stop_gameobjectCount3 != 0)
//			{
//				if (count < iter->second.stop_gameobjectCount3)
//					oss << count << "/" << iter->second.stop_gameobjectCount3;
//				else
//					oss << "[目标达成]";
//				announce = true;
//			}
//			else if (entry == iter->second.stop_gameobject4 && iter->second.stop_gameobjectCount4 != 0)
//			{
//				if (count < iter->second.stop_gameobjectCount4)
//					oss << count << "/" << iter->second.stop_gameobjectCount4;
//				else
//					oss << "[目标达成]";
//				announce = true;
//			}
//			else if (entry == iter->second.stop_gameobject5 && iter->second.stop_gameobjectCount5 != 0)
//			{
//				if (count < iter->second.stop_gameobjectCount5)
//					oss << count << "/" << iter->second.stop_gameobjectCount5;
//				else
//					oss << "[目标达成]";
//				announce = true;
//			}
//		}
//		break;
//		}
//
//		if (announce)
//			sWorld->SendScreenMessage(oss.str().c_str());
//
//		if (
//			iter->second.stop_creatureCount1 <= GetKillCreatureCount(iter->second.stop_creature1) &&
//			iter->second.stop_creatureCount2 <= GetKillCreatureCount(iter->second.stop_creature2) &&
//			iter->second.stop_creatureCount3 <= GetKillCreatureCount(iter->second.stop_creature3) &&
//			iter->second.stop_creatureCount4 <= GetKillCreatureCount(iter->second.stop_creature4) &&
//			iter->second.stop_creatureCount5 <= GetKillCreatureCount(iter->second.stop_creature5) &&
//			iter->second.stop_gameobjectCount1 <= GetDestroyGobCount(iter->second.stop_gameobject1) &&
//			iter->second.stop_gameobjectCount2 <= GetDestroyGobCount(iter->second.stop_gameobject2) &&
//			iter->second.stop_gameobjectCount3 <= GetDestroyGobCount(iter->second.stop_gameobject3) &&
//			iter->second.stop_gameobjectCount4 <= GetDestroyGobCount(iter->second.stop_gameobject4) &&
//			iter->second.stop_gameobjectCount5 <= GetDestroyGobCount(iter->second.stop_gameobject5) &&
//			(
//			iter->second.stop_defenseKills <= GetKillPlayerCount(C_TEAM_DEFENSE) ||
//			iter->second.stop_offenseKills <= GetKillPlayerCount(C_TEAM_OFFENSE)
//			)
//			&&
//			iter->second.stop_killGaps <= uint32(GetKillPlayerCount(C_TEAM_DEFENSE) - GetKillPlayerCount(C_TEAM_OFFENSE))
//			)
//			endFlag = true;
//
//		if (endFlag)
//			sGameEventMgr->StopEvent(_eventId, false, false);
//	}
//}
//
////WorldState
//void CustomEvent::_UpdateWorldState(uint32 index, uint32 value)
//{
//	if (index == 0)
//		return;
//
//	sWorld->setWorldState(index, value);
//	SessionMap const& smap = sWorld->GetAllSessions();
//	for (SessionMap::const_iterator i = smap.begin(); i != smap.end(); ++i)
//		if (Player* pl = i->second->GetPlayer())
//			pl->SendUpdateWorldState(index, value);
//}
//
//void CustomEvent::InitWorldState(uint32 eventId)
//{
//	std::unordered_map<uint32, EventDataTemplate>::iterator iter = EventDataMap.find(eventId);
//
//	if (iter != EventDataMap.end())
//	{
//		_UpdateWorldState(iter->second.state_creatureWorldState1, 0);
//		_UpdateWorldState(iter->second.state_creatureWorldState2, 0);
//		_UpdateWorldState(iter->second.state_creatureWorldState3, 0);
//		_UpdateWorldState(iter->second.state_gameobjectWorldState1, 0);
//		_UpdateWorldState(iter->second.state_gameobjectWorldState2, 0);
//		_UpdateWorldState(iter->second.state_gameobjectWorldState3, 0);
//		_UpdateWorldState(iter->second.state_defenseWorldState, iter->second.stop_defenseKills);
//		_UpdateWorldState(iter->second.state_offenseWorldState, iter->second.stop_offenseKills);
//	}
//}
//
//void CustomEvent::UpdateWorldState(Object* obj, Player* player)
//{
//	uint32 entry = obj->GetEntry();
//
//	GameEventMgr::ActiveEvents const& activeEvents = sGameEventMgr->GetActiveEventList();
//
//	for (GameEventMgr::ActiveEvents::const_iterator i = activeEvents.begin(); i != activeEvents.end(); ++i)
//	{
//		uint32 eventId = *i;	
//		uint32 worldstateIndex = 0;
//		bool increase = true;
//
//		std::unordered_map<uint32, EventDataTemplate>::iterator iter = EventDataMap.find(eventId);
//
//		if (iter != EventDataMap.end())
//		{
//			if (obj->GetTypeId() == TYPEID_UNIT)
//			{
//				if (entry == iter->second.state_creature1)
//					worldstateIndex = iter->second.state_creatureWorldState1;
//				else if (entry == iter->second.state_creature2)
//					worldstateIndex = iter->second.state_creatureWorldState2;
//				else if (entry == iter->second.state_creature3)
//					worldstateIndex = iter->second.state_creatureWorldState3;
//			}
//			else if (obj->GetTypeId() == TYPEID_GAMEOBJECT)
//			{
//				if (entry == iter->second.state_gameobject1)
//					worldstateIndex = iter->second.state_gameobjectWorldState1;
//				else if (entry == iter->second.state_gameobject2)
//					worldstateIndex = iter->second.state_gameobjectWorldState2;
//				else if (entry == iter->second.state_gameobject3)
//					worldstateIndex = iter->second.state_gameobjectWorldState3;
//			}
//			else if (obj->GetTypeId() == TYPEID_PLAYER)
//			{
//				if (Player* victim = obj->ToPlayer())
//				{
//					TeamTypes team = GetTeam(victim);
//
//					if (team == C_TEAM_DEFENSE)
//						worldstateIndex = iter->second.state_offenseWorldState;
//					else if (team == C_TEAM_OFFENSE)
//						worldstateIndex = iter->second.state_defenseWorldState;
//
//					increase = false;
//				}
//			}
//
//			if (worldstateIndex == 0)
//				continue;
//
//			if (increase)
//				sWorld->setWorldState(worldstateIndex, sWorld->getWorldState(worldstateIndex) + 1);
//			else
//				sWorld->setWorldState(worldstateIndex, sWorld->getWorldState(worldstateIndex) - 1);
//
//			uint32 value = sWorld->getWorldState(worldstateIndex);
//
//			SessionMap const& smap = sWorld->GetAllSessions();
//			for (SessionMap::const_iterator i = smap.begin(); i != smap.end(); ++i)
//				if (Player* pl = i->second->GetPlayer())
//					pl->SendUpdateWorldState(worldstateIndex, value);
//		}
//	}
//}
//
////激活
//void CustomEvent::UpdateActive(Object *obj)
//{
//	bool valid = false;
//	uint32 entry = obj->GetEntry();
//	TypeID typeId = obj->GetTypeId();
//
//	uint32 len = EventActiveVec.size();
//
//	for (uint32 i = 0; i < len; i++)
//	{
//		if (typeId == TYPEID_UNIT && entry == EventActiveVec[i].creatureEntry)
//		{
//			valid = true;
//			break;
//		}
//		else if (typeId == TYPEID_GAMEOBJECT && entry == EventActiveVec[i].gameobjectEntry)
//		{
//			valid = true;
//			break;
//		}
//	}
//
//	if (!valid)
//		return;
//
//	for (uint32 i = 0; i < len; i++)
//	{
//		if (_eventId == EventActiveVec[i].eventId)
//		{
//			if (GetKillCreatureCount(EventActiveVec[i].creatureEntry) >= EventActiveVec[i].killCount && GetDestroyGobCount(EventActiveVec[i].gameobjectEntry) >= EventActiveVec[i].destroyCount)
//			{
//				uint32 activeGUID = EventActiveVec[i].activeGUID;
//				uint32 activeType = EventActiveVec[i].activeType;
//
//				if (activeType == 0)
//				{
//					if (CreatureData const* data = sObjectMgr->GetCreatureData(activeGUID))
//					{
//						if (Creature* target = ObjectAccessor::GetObjectInWorld(MAKE_NEW_GUID(activeGUID, data->id, HIGHGUID_UNIT), (Creature*)NULL))
//						{
//							if (target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE) || target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
//							{
//								std::ostringstream oss;
//								target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
//								oss << _eventName << "[" << target->GetName() << "]现在可以攻击了！";
//								sWorld->SendScreenMessage(oss.str().c_str());
//							}
//						}
//					}
//				}
//				else
//				{
//					if (GameObjectData const* data = sObjectMgr->GetGOData(activeGUID))
//					{
//						if (GameObject* target = ObjectAccessor::GetObjectInWorld(MAKE_NEW_GUID(activeGUID, data->id, HIGHGUID_GAMEOBJECT), (GameObject*)NULL))
//						{
//							if (target->HasFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE))
//							{
//								std::ostringstream oss;
//								target->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
//								oss << _eventName << "[" << target->GetName() << "]已经激活！";
//								sWorld->SendScreenMessage(oss.str().c_str());
//							}		
//						}
//					}
//				}
//			}
//		}
//	}
//}
//
////玩家
//bool CustomEvent::AcceptInvitation(Player* player, uint32 menuId)
//{
//	if (menuId < 1000000)
//		return false;
//
//	uint32 eventId = menuId - 1000000;
//
//	if (!sGameEventMgr->IsActiveEvent(eventId))
//	{
//		player->GetSession()->SendNotification("该事件已经结束！");
//		return true;
//	}
//
//	if (eventId == UKBG_EVENT_ID)
//	{
//		sUkBG->AddPlayer(player);
//		return true;
//	}
//
//	switch (_eventType)
//	{
//	case C_ET_NONE:
//		break;
//	case C_ET_WS:
//		player->GetSession()->FixedBGJoin(BATTLEGROUND_WS);
//		break;
//	case C_ET_AB:
//		player->GetSession()->FixedBGJoin(BATTLEGROUND_AB);
//		break;
//	case C_ET_EY:
//		player->GetSession()->FixedBGJoin(BATTLEGROUND_EY);
//		break;
//	case C_ET_SA:
//		player->GetSession()->FixedBGJoin(BATTLEGROUND_SA);
//		break;
//	case C_ET_AV:
//		player->GetSession()->FixedBGJoin(BATTLEGROUND_AV);
//		break;
//	case C_ET_IC:
//		player->GetSession()->FixedBGJoin(BATTLEGROUND_IC);
//		break;
//	case C_ET_TELE:
//	{
//		uint32 map = 0; float x = 0; float y = 0; float z = 0; float o = 0;
//		player->GetTeamId() == TEAM_ALLIANCE ? GetTelePos(eventId, C_PT_START_DEFENSE, map, x, y, z, o) : GetTelePos(eventId, C_PT_START_OFFENSE, map, x, y, z, o);
//
//		if (map == 0 && x == 0 && y == 0 && z == 0)
//			return true;
//
//		player->TeleportTo(map, x, y, z, o);
//	}
//	break;
//	case C_ET_GROUP:
//		AddNewToGroup(player);
//		break;
//	default:
//		break;
//	}
//
//	return true;
//}
//
//uint32 CustomEvent::GetTeamMembers(TeamTypes team)
//{
//	uint32 count = 0;
//
//	std::unordered_map<uint32, EventPlayerTemplate>::iterator iter;
//	for (iter = PlayerDataMap.begin(); iter != PlayerDataMap.end(); iter++)
//		if (team == iter->second.team)
//			count++;
//
//	return count;
//}
//
//TeamTypes CustomEvent::GetTeam(Player* player)
//{
//	std::unordered_map<uint32, EventPlayerTemplate>::iterator ii = PlayerDataMap.find(player->GetGUIDLow());
//	if (ii != PlayerDataMap.end())
//		return ii->second.team;
//
//	return C_TEAM_NONE;
//}
//
//TeamTypes CustomEvent::SetTeam(Player* player)
//{
//	TeamTypes team = C_TEAM_NONE;
//
//	switch (_groupType)
//	{
//	case C_GT_OFFENSE_ALL:
//		team = C_TEAM_OFFENSE;
//		break;
//	case C_GT_OFFENSE_ALLY:
//		if (player->GetTeamId() == TEAM_ALLIANCE)
//			team = C_TEAM_OFFENSE;
//		else
//			team = C_TEAM_DEFENSE;
//		break;
//	case C_GT_OFFENSE_HORDE:
//		if (player->GetTeamId() == TEAM_HORDE)
//			team = C_TEAM_OFFENSE;
//		else
//			team = C_TEAM_DEFENSE;
//		break;
//	case C_GT_OFFENSE_FACTION_RANDOM:
//
//		if (_AllyAsOffense)
//		{
//			if (player->GetTeamId() == TEAM_ALLIANCE)
//				team = C_TEAM_OFFENSE;
//			else
//				team = C_TEAM_DEFENSE;
//		}
//		else
//		{
//			if (player->GetTeamId() == TEAM_HORDE)
//				team = C_TEAM_OFFENSE;
//			else
//				team = C_TEAM_DEFENSE;
//		}
//		break;
//	case C_GT_OFFENSE_PLAYER_RANDOM:
//	{
//		uint32 defenseCount = GetTeamMembers(C_TEAM_DEFENSE);
//		uint32 offenseCount = GetTeamMembers(C_TEAM_OFFENSE);
//
//		if (offenseCount > defenseCount)
//			team = C_TEAM_DEFENSE;
//		else if (offenseCount < defenseCount)
//			team = C_TEAM_OFFENSE;
//		else
//		{
//			if (urand(0, 1) == 0)
//				team = C_TEAM_OFFENSE;
//			else
//				team = C_TEAM_DEFENSE;
//		}
//	}
//	break;
//	}
//
//	std::unordered_map<uint32, EventPlayerTemplate>::iterator ii = PlayerDataMap.find(player->GetGUIDLow());
//	if (ii != PlayerDataMap.end())
//		ii->second.team = team;
//
//	return team;
//}
//
//void CustomEvent::SetFaction(Player* player, TeamTypes team)
//{
//	if (team == C_TEAM_DEFENSE)
//		player->setFaction(DEFENSE_FACTION);
//	else if (team == C_TEAM_OFFENSE)
//		player->setFaction(OFFENSE_FACTION);
//}
//
//void CustomEvent::SetValid(Player* player, bool valid)
//{
//	std::unordered_map<uint32, EventPlayerTemplate>::iterator iter = PlayerDataMap.find(player->GetGUIDLow());
//
//	if (iter != PlayerDataMap.end())
//		iter->second.valid = valid;
//}
//
//bool CustomEvent::CheckValid(Player* player)
//{
//	std::unordered_map<uint32, EventPlayerTemplate>::iterator iter = PlayerDataMap.find(player->GetGUIDLow());
//
//	if (iter != PlayerDataMap.end())
//		return iter->second.valid;
//
//	return false;
//}
//
//TeamTypes CustomEvent::AddPlayer(Player* player)
//{
//	TeamTypes team = C_TEAM_NONE;
//
//	std::unordered_map<uint32, EventPlayerTemplate>::iterator ii = PlayerDataMap.find(player->GetGUIDLow());
//
//	if (ii != PlayerDataMap.end())
//	{
//		switch (_rejoinType)
//		{
//		case C_RT_BANED:
//			break;
//		case C_RT_KEEP_GROUP:
//			team = GetTeam(player);
//			SetFaction(player, team);
//			SetValid(player, true);
//			break;
//		case C_RT_REDIS_GROUP:
//			team = SetTeam(player);
//			SetFaction(player, team);
//			SetValid(player, true);
//			break;
//		}
//	}
//	else
//	{
//		team = SetTeam(player);
//		SetFaction(player, team);
//
//		std::unordered_map<uint32, EventPlayerTemplate>::iterator iter = PlayerDataMap.find(player->GetGUIDLow());
//
//		if (iter == PlayerDataMap.end())
//		{
//			EventPlayerTemplate Temp;
//			Temp.team = team;
//			Temp.damage = 0;
//			Temp.heal = 0;
//			Temp.kills = 0;
//			Temp.killeds = 0;
//			Temp.valid = true;
//			PlayerDataMap.insert(std::make_pair(player->GetGUIDLow(), Temp));
//		}
//	}
//
//	return team;
//}
//
//void CustomEvent::AddNewToGroup(Player* player)
//{
//	//已经在事件中
//	if (CheckValid(player))
//	{
//		ChatHandler(player->GetSession()).PSendSysMessage("%s重新加入事件失败！你已经加入事件！", _eventName.c_str());
//		return;
//	}
//	
//	//超过IP限制
//	if (!sCF->joinEventIsAllowed(_eventId, player))
//		return;
//
//	TeamTypes team = AddPlayer(player);
//
//	//禁止重新加入
//	if (team == C_TEAM_NONE)
//	{
//		ChatHandler(player->GetSession()).PSendSysMessage("%s加入失败！无法重新加入！", _eventName.c_str());
//		return;
//	}
//
//	if (team == C_TEAM_DEFENSE)
//		ChatHandler(player->GetSession()).PSendSysMessage("%s你将为%s而战！", _eventName.c_str(), _defenseName.c_str());
//	else
//		ChatHandler(player->GetSession()).PSendSysMessage("%s你将为%s而战！", _eventName.c_str(), _offenseName.c_str());
//
//	if (player->isDead())
//	{
//		player->ResurrectPlayer(1.0f);
//		player->SpawnCorpseBones();
//	}
//
//	player->UpdateMaxHealth();
//	player->UninviteFromGroup();
//	if (player->GetGroup())
//		player->RemoveFromGroup();
//
//	TelePort(player);
//
//	bool groupExist = false;
//
//	for (std::unordered_map<uint32, TeamTypes>::iterator ii = GroupDataMap.begin(); ii != GroupDataMap.end(); ii++)
//	{
//		if (Group* group = sGroupMgr->GetGroupByGUID(ii->first))
//			if (!group->IsFull() && ii->second == team)
//			{
//				group->AddMember(player);
//				groupExist = true;
//				break;
//			}
//	}
//
//	if (!groupExist)
//	{
//		Group* group = new Group;
//		group->Create(player);
//		group->ConvertToRaid();
//		sGroupMgr->AddGroup(group);
//		GroupDataMap.insert(std::make_pair(group->GetLowGUID(), team));
//	}
//}
//
//void CustomEvent::TelePort(Player* player)
//{
//	uint32 map = 0;
//	float x = 0;
//	float y = 0;
//	float z = 0;
//	float o = 0;
//
//	TeamTypes team = GetTeam(player);
//
//	if (team == C_TEAM_DEFENSE)
//		GetTelePos(_eventId, C_PT_START_DEFENSE, map, x, y, z, o);
//	else if (team == C_TEAM_OFFENSE)
//		GetTelePos(_eventId, C_PT_START_OFFENSE, map, x, y, z, o);
//
//
//	if (map == 0 && x == 0 && y == 0 && z == 0)
//		return;
//
//	player->TeleportTo(map, x, y, z, 0);
//}
//
//void CustomEvent::Revive(Player* player)
//{
//	player->GetSession()->SendAreaTriggerMessage("%s拿出你的勇气，继续战斗！", _eventName.c_str());
//	player->CastSpell(player, 24171, true);
//	player->ResurrectPlayer(1.0f);
//	player->CastSpell(player, 6962, true);
//	player->CastSpell(player, 44535, true);
//	player->SpawnCorpseBones();
//}
//
//bool CustomEvent::DiedTele(Player* player)
//{
//	if (!CheckValid(player))
//		return false;
//
//	TeamTypes team = GetTeam(player);
//
//	if (team == C_TEAM_NONE)
//		return false;
//
//	if (team == C_TEAM_DEFENSE)
//		Teleport(player, _graveyard_defense_pos);
//	else if (team == C_TEAM_OFFENSE)
//		Teleport(player, _graveyard_offense_pos);
//
//	return true;
//}
//
//bool CustomEvent::SendAreaSpiritHealerQueryOpcode(Player* player)
//{
//	if (!player || player->IsAlive())
//		return false;
//
//	if (_eventId == 0)
//		return false;
//
//	if (_reviveUpdateTimer <= 0)
//		return false;
//
//	TeamTypes team = GetTeam(player);
//
//	if (team == C_TEAM_NONE)
//		return false;
//
//	WorldPacket data(SMSG_AREA_SPIRIT_HEALER_TIME, 12);
//
//	if (team == C_TEAM_DEFENSE)
//		data << _soulhealer_defense;
//	else
//		data << _soulhealer_offense;
//
//	data << _reviveUpdateTimer;
//	player->GetSession()->SendPacket(&data);
//	return true;
//}
//
//void CustomEvent::RemovePlayer(Player* player)
//{
//	if (!CheckValid(player))
//		return;
//
//	std::unordered_map<uint32, EventPlayerTemplate>::iterator ii = PlayerDataMap.find(player->GetGUIDLow());
//
//	if (ii != PlayerDataMap.end())
//		ii->second.valid = false;
//
//	ChatHandler(player->GetSession()).PSendSysMessage("你已经离开%s", _eventName.c_str());
//	player->setFactionForRace(player->getRace());
//	player->UpdateMaxHealth();
//	player->UninviteFromGroup();
//	if (player->GetGroup())
//		player->RemoveFromGroup();
//	player->TeleportTo(player->m_homebindMapId, player->m_homebindX, player->m_homebindY, player->m_homebindZ, 0);
//}
//
//void CustomEvent::ResetAllPlayers()
//{
//	//重置player
//	for (std::unordered_map<uint32, EventPlayerTemplate>::iterator ii = PlayerDataMap.begin(); ii != PlayerDataMap.end(); ii++)
//		if (Player* player = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(ii->first, 0, HIGHGUID_PLAYER)))
//		{
//			if (!ii->second.valid)
//				continue;
//
//			player->setFactionForRace(player->getRace());
//
//			if (player->isDead())
//			{
//				player->ResurrectPlayer(1.0f);
//				player->SpawnCorpseBones();
//			}
//
//			player->UpdateMaxHealth();
//			//清除队伍状态
//			player->UninviteFromGroup();
//			if (player->GetGroup())
//				player->RemoveFromGroup();
//		}
//}
//
//void CustomEvent::RewardAllPlayers()
//{
//	for (std::unordered_map<uint32, EventPlayerTemplate>::iterator ii = PlayerDataMap.begin(); ii != PlayerDataMap.end(); ii++)
//	{
//		if (Player* player = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(ii->first, 0, HIGHGUID_PLAYER)))
//		{
//			ChatHandler(player->GetSession()).PSendSysMessage("%s活跃度统计", _eventName.c_str());
//			ChatHandler(player->GetSession()).PSendSysMessage("%s击杀：%u/%u", _eventName.c_str(), ii->second.kills, _rew_killsForRew);
//			ChatHandler(player->GetSession()).PSendSysMessage("%s被杀：%u/%u", _eventName.c_str(), ii->second.killeds, _rew_killedsForRew);
//			ChatHandler(player->GetSession()).PSendSysMessage("%s伤害：%u/%u", _eventName.c_str(), ii->second.damage, _rew_damageForRew);
//			ChatHandler(player->GetSession()).PSendSysMessage("%s治疗：%u/%u", _eventName.c_str(), ii->second.heal, _rew_healForRew);
//
//			if (!ii->second.valid)
//			{
//				ChatHandler(player->GetSession()).PSendSysMessage("%s你中途离开活动，无法获得奖励", _eventName.c_str());
//				continue;
//			}
//
//			if (ii->second.kills < _rew_killsForRew && ii->second.killeds < _rew_killedsForRew && ii->second.damage < _rew_damageForRew && ii->second.heal < _rew_healForRew)
//			{
//				ChatHandler(player->GetSession()).PSendSysMessage("%s活跃度未达到要求，无法获得奖励", _eventName.c_str(), ii->second.heal);
//				continue;
//			}
//
//			if (player && player->IsInWorld())
//				ii->second.team == _winTeam ? sRew->Rew(player, _rew_winRewId) : sRew->Rew(player, _rew_losRewId);
//		}	
//	}
//}
//
//void CustomEvent::IncreaseDamage(Player* player,Unit* target, uint32 damage)
//{
//	std::unordered_map<uint32, EventPlayerTemplate>::iterator ii = PlayerDataMap.find(player->GetGUIDLow());
//
//	if (ii != PlayerDataMap.end())
//		if (target->GetTypeId() != TYPEID_PLAYER)
//			ii->second.damage += damage;
//}
//
//void CustomEvent::IncreaseHeal(Player* player, Unit* target, uint32 heal)
//{
//	std::unordered_map<uint32, EventPlayerTemplate>::iterator ii = PlayerDataMap.find(player->GetGUIDLow());
//
//	if (ii != PlayerDataMap.end())
//		if (target->GetTypeId() != TYPEID_PLAYER)
//			ii->second.heal += heal;
//}
//
//void CustomEvent::IncreaseKills(Player* player)
//{
//	std::unordered_map<uint32, EventPlayerTemplate>::iterator ii = PlayerDataMap.find(player->GetGUIDLow());
//
//	if (ii != PlayerDataMap.end())
//			ii->second.kills ++;
//}
//
//void CustomEvent::IncreaseKilleds(Player* player)
//{
//	std::unordered_map<uint32, EventPlayerTemplate>::iterator ii = PlayerDataMap.find(player->GetGUIDLow());
//
//	if (ii != PlayerDataMap.end())
//			ii->second.killeds++;
//}
//
//void CustomEvent::Teleport(Player* player, uint32 posId)
//{
//	std::unordered_map<uint32, PosTemplate>::iterator it = PosMap.find(posId);
//
//	if (it != PosMap.end())
//		player->TeleportTo(it->second.map, it->second.x, it->second.y, it->second.z, it->second.o);
//}
//
//void CustomEvent::Update(Player* player)
//{
//	if (!CheckValid(player))
//		return;
//
//	switch (_eventId)
//	{
//	case NARROW_EVENT_ID:			//狭路相逢
//	{
//		uint32 posId = 0;
//
//		GetTeam(player) == C_TEAM_OFFENSE ? posId = NARROW_EVENT_OFFENSE_POSID : posId = NARROW_EVENT_DEFENSE_POSID;
//
//		if (!player->IsAlive())
//		{
//			Teleport(player, posId);
//			Revive(player);
//		}
//
//		float distance = GetDistance(player, posId);
//
//		if (distance < 127.3)
//			player->RemoveAura(NARROW_EVENT_DEBUFF_ID);
//		else if (!player->HasAura(NARROW_EVENT_DEBUFF_ID))
//			player->AddAura(NARROW_EVENT_DEBUFF_ID, player);
//	}
//	break;
//	case PORT_EVENT_ID:
//
//	default:
//		break;
//	}
//}
//
//void CustomEvent::UpdatePhase(Object* obj)
//{
//	uint32 guid = obj->GetGUIDLow();
//	TypeID typeId = obj->GetTypeId();
//
//	auto iter = EventDataMap.find(_eventId);
//
//	if (iter != EventDataMap.end())
//	{
//		auto i = iter->second.eventPhaseMap.find(_eventPhase);
//
//		if (i != iter->second.eventPhaseMap.end())
//		{
//			if (i->second.stop_creature == 0)
//				_stop_creature_flag = true;
//
//			if (i->second.stop_gameobject == 0)
//				_stop_gameobject_flag = true;
//
//			if (i->second.stop_kills == 0)
//				_stop_kills_flag = true;
//
//			if (typeId == TYPEID_UNIT && guid == i->second.stop_creature)
//				_stop_creature_flag = true;
//
//			if (typeId == TYPEID_GAMEOBJECT && guid == i->second.stop_gameobject)
//				_stop_gameobject_flag = true;
//
//			if (typeId == TYPEID_PLAYER && (GetKillPlayerCount(C_TEAM_DEFENSE) >= i->second.stop_kills || GetKillPlayerCount(C_TEAM_OFFENSE) >= i->second.stop_kills))
//				_stop_kills_flag = true;
//		}
//	}
//	else
//		return;
//
//	if (_stop_creature_flag && _stop_gameobject_flag && _stop_kills_flag)
//	{
//		_stop_creature_flag = false;
//		_stop_gameobject_flag = false;
//		_stop_kills_flag = false;
//
//		auto i = iter->second.eventPhaseMap.find(_eventPhase);
//
//		if (i != iter->second.eventPhaseMap.end())
//		{
//			std::unordered_map<uint32, EventPlayerTemplate>::iterator iter;
//			for (iter = PlayerDataMap.begin(); iter != PlayerDataMap.end(); iter++)
//			{
//				if (Player* pl = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(iter->first, 0, HIGHGUID_PLAYER)))
//				{
//					if (!iter->second.valid)
//						continue;
//
//					if (!pl->IsInWorld())
//						continue;
//
//					if (!pl->IsAlive())
//					{
//						pl->CastSpell(pl, 22012, true);
//						Revive(pl);
//					}
//
//					if (GetTeam(pl) == C_TEAM_DEFENSE)
//						Teleport(pl, i->second.stop_defense_tele_pos);
//					if (GetTeam(pl) == C_TEAM_OFFENSE)
//						Teleport(pl, i->second.stop_offense_tele_pos);
//				}
//			}
//
//			if (!i->second.stop_defense_notice.empty())
//			{
//				std::string text = _eventName + i->second.stop_defense_notice;
//				SendScreenMessage(text.c_str(), C_TEAM_DEFENSE);
//			}
//				
//			if (!i->second.stop_offense_notice.empty())
//			{
//				std::string text = _eventName + i->second.stop_offense_notice;
//				SendScreenMessage(text.c_str(), C_TEAM_OFFENSE);
//			}
//		}
//
//		_eventPhase++;
//
//		i = iter->second.eventPhaseMap.find(_eventPhase);
//
//		if (i != iter->second.eventPhaseMap.end())
//		{
//			_graveyard_defense_pos = i->second.defense_graveyard_pos;
//			_graveyard_offense_pos = i->second.offense_graveyard_pos;
//			_defense_graveyard_safe = i->second.defense_graveyard_safe;
//			_offense_graveyard_safe = i->second.offense_graveyard_safe;
//			_defense_graveyard_healer = i->second.defense_graveyard_healer;
//			_offense_graveyard_healer = i->second.offense_graveyard_healer;
//			SummonSoulHealer();
//		}
//	}
//}
//
//void CustomEvent::SendScreenMessage(const char *text, TeamTypes team, bool onlydead)
//{
//	WorldPacket data;
//	ChatHandler::BuildChatPacket(data, CHAT_MSG_RAID_BOSS_EMOTE, LANG_UNIVERSAL,
//		NULL, NULL, text);
//
//	std::unordered_map<uint32, EventPlayerTemplate>::iterator iter;
//	for (iter = PlayerDataMap.begin(); iter != PlayerDataMap.end(); iter++)
//	{
//		if (team == iter->second.team || team == C_TEAM_NONE)
//		{
//			if (Player* player = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(iter->first, 0, HIGHGUID_PLAYER)))
//			{
//				if (!iter->second.valid)
//					continue;
//
//				if (player->IsAlive() && onlydead)
//					continue;
//
//				if (WorldSession* ss = player->GetSession())
//					player->GetSession()->SendPacket(&data);
//			}
//		}
//	}	
//}
//
//void CustomEvent::UpdateRevive(uint32 diff)
//{
//	if (_graveyard_defense_pos == 0 && _graveyard_offense_pos == 0)
//		return;
//
//	if (_reviveUpdateTimer > 0)
//		_reviveUpdateTimer -= diff;
//	else
//	{
//		_reviveUpdateTimer = REVIVE_INTERVALS;
//
//		std::unordered_map<uint32, EventPlayerTemplate>::iterator iter;
//		for (iter = PlayerDataMap.begin(); iter != PlayerDataMap.end(); iter++)
//		{
//			if (Player* pl = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(iter->first, 0, HIGHGUID_PLAYER)))
//			{
//				if (!iter->second.valid)
//					continue;
//
//				if (!pl->IsInWorld())
//					continue;
//
//				if (pl->IsAlive())
//					continue;
//
//				if (GetTeam(pl) == C_TEAM_DEFENSE && GetDistance(pl, _graveyard_defense_pos) > 20.0f)
//					continue;
//
//				if (GetTeam(pl) == C_TEAM_OFFENSE && GetDistance(pl, _graveyard_offense_pos) > 20.0f)
//					continue;
//
//				pl->CastSpell(pl, 22012, true);
//				Revive(pl);
//			}
//		}
//	}
//}
//
//void CustomEvent::UpdateSafeArea(uint32 diff)
//{
//	if (_graveyard_defense_pos == 0 && _graveyard_offense_pos == 0)
//		return;
//
//	if (_safeAreaUpdateTimer > 0)
//		_safeAreaUpdateTimer -= diff;
//	else
//	{
//		_safeAreaUpdateTimer = IN_MILLISECONDS;
//
//		std::unordered_map<uint32, EventPlayerTemplate>::iterator iter;
//		for (iter = PlayerDataMap.begin(); iter != PlayerDataMap.end(); iter++)
//		{
//			if (Player* pl = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(iter->first, 0, HIGHGUID_PLAYER)))
//			{
//				if (!iter->second.valid)
//					continue;
//
//				if (!pl->IsInWorld())
//					continue;
//
//				if (!pl->IsAlive())
//					continue;
//				
//				if (GetTeam(pl) == C_TEAM_OFFENSE && GetDistance(pl, _graveyard_defense_pos) <= _defense_graveyard_safe)
//				{
//					std::ostringstream oss;
//					oss << _eventName << "你已进入敌方墓地保护区域" << _defense_graveyard_safe << "，强制传送";
//					pl->GetSession()->SendNotification(oss.str().c_str());
//					Teleport(pl, _graveyard_offense_pos);
//				}
//
//				if (GetTeam(pl) == C_TEAM_DEFENSE && GetDistance(pl, _graveyard_offense_pos) < _offense_graveyard_safe)
//				{
//					std::ostringstream oss;
//					oss << _eventName << "你已进入敌方墓地保护区域" << _offense_graveyard_safe << "，强制传送";
//					pl->GetSession()->SendNotification(oss.str().c_str());
//					Teleport(pl, _graveyard_defense_pos);
//				}		
//			}
//		}
//	}
//}
//
//void CustomEvent::Update(Map* map, uint32 diff)
//{
//	if (!map)
//		return;
//
//	if (map->GetId() != _mapId)
//		return;
//
//	UpdateRevive(diff);
//	UpdateSafeArea(diff);
//}
//
////战场
//bool CustomEvent::IsFixedTimeBgActive(BattlegroundTypeId bgTypeId)
//{
//	EventTypes eventType = C_ET_NONE;
//
//	switch (bgTypeId)
//	{
//	case BATTLEGROUND_AV:
//		eventType = C_ET_AV;
//		break;
//	case BATTLEGROUND_WS:
//		eventType = C_ET_WS;
//		break;
//	case BATTLEGROUND_AB:
//		eventType = C_ET_AB;
//		break;
//	case BATTLEGROUND_EY:
//		eventType = C_ET_EY;
//		break;
//	case BATTLEGROUND_SA:
//		eventType = C_ET_SA;
//		break;
//	case BATTLEGROUND_IC:
//		eventType = C_ET_IC;
//		break;
//	}
//
//	if (eventType == _eventType)
//		return true;
//
//	return false;
//}
//
//void CustomEvent::AddBgList(Player* player)
//{
//	GameEventMgr::ActiveEvents const& activeEvents = sGameEventMgr->GetActiveEventList();
//
//	for (GameEventMgr::ActiveEvents::const_iterator itr = activeEvents.begin(); itr != activeEvents.end(); ++itr)
//	{
//		std::unordered_map<uint32, EventDataTemplate>::iterator iter = EventDataMap.find(*itr);
//
//		if (iter != EventDataMap.end())
//		{
//			if (iter->second.eventType == C_ET_NONE || iter->second.eventType > C_ET_IC)
//				continue;
//
//			const BattlegroundContainer& bgList = sBattlegroundMgr->GetBattlegroundList();
//			for (BattlegroundContainer::const_iterator ii = bgList.begin(); ii != bgList.end(); ++ii)
//			{
//				Battleground* bg = ii->second;
//				if (!bg)
//					continue;
//
//				if (!iter->second.gossipText.empty())
//					player->ADD_GOSSIP_ITEM(0, iter->second.gossipText, SENDER_CUSTOM_EVENT, *itr);
//			}
//		}
//	}
//}
//
//void CustomEvent::AddEventList(Player* player, Object* obj, bool isBG)
//{
//	player->PlayerTalkClass->ClearMenus();
//
//	GameEventMgr::ActiveEvents const& activeEvents = sGameEventMgr->GetActiveEventList();
//
//	for (GameEventMgr::ActiveEvents::const_iterator itr = activeEvents.begin(); itr != activeEvents.end(); ++itr)
//	{
//		std::unordered_map<uint32, EventDataTemplate>::iterator iter = EventDataMap.find(*itr);
//
//		if (iter != EventDataMap.end())
//		{
//			if (iter->second.eventType == C_ET_NONE)
//				continue;
//
//			if (!iter->second.gossipText.empty())
//				player->ADD_GOSSIP_ITEM(0, iter->second.gossipText, SENDER_CUSTOM_EVENT, *itr);
//		}
//	}
//
//	player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, obj->GetGUID());
//}
//
////Unk1 = 1时 检测阵营是否一致 一致则通过 可以释放技能
//bool CustomEvent::GobUnk1Check(Unit* target, GameObject *const gob)
//{
//	if (!target)
//		return true;
//
//	GameObjectTemplate const* temp = sObjectMgr->GetGameObjectTemplate(gob->GetEntry());
//
//	if (std::strcmp(temp->unk1.c_str(), "1") != 0)
//		return true;
//
//	if (temp->faction != target->getFaction())
//		return false;
//
//	return true;
//}
//
//void CustomEvent::PrintPos(Player* player,uint32 posId1, uint32 posId2, uint32 count, bool addPos)
//{
//	float x1, x2, y1, y2, z1, z2, o1, o2;
//	uint32 map1, map2;
//
//	bool exsit1 = false;
//	bool exsit2 = false;
//
//	std::unordered_map<uint32, PosTemplate>::iterator it = PosMap.find(posId1);
//
//	if (it != PosMap.end())
//	{
//		map1 = it->second.map;
//		x1 = it->second.x;
//		y1 = it->second.y;
//		z1 = it->second.z;
//		o1 = it->second.o;
//		exsit1 = true;
//	}
//
//	it = PosMap.find(posId2);
//
//	if (it != PosMap.end())
//	{
//		map2 = it->second.map;
//		x2 = it->second.x;
//		y2 = it->second.y;
//		z2 = it->second.z;
//		o2 = it->second.o;
//		exsit2 = true;
//	}
//
//	if (exsit1 && exsit2 && map1 == map2)
//	{
//		for (uint8 i = 1; i <= count; i++)
//		{
//			uint32 map = map1;
//
//			float x = (x1 * i + x2 * (count + 1 - i)) / (count + 1);
//			float y = (y1 * i + y2 * (count + 1 - i)) / (count + 1);
//			float z = (z1 * i + z2 * (count + 1 - i)) / (count + 1);
//			float o = (o1 * i + o2 * (count + 1 - i)) / (count + 1);
//
//			ChatHandler(player->GetSession()).PSendSysMessage("%u------x:%f,y:%f,z%f", x, y, z);
//
//			if (addPos)
//			{
//				std::ostringstream oss;
//				oss << "posId" << posId1 << "与" << posId2 << "之间采集点" << uint32(i);
//
//				uint32 posId = 1;
//
//				if (QueryResult result = WorldDatabase.PQuery("SELECT max(posId) FROM _position"))
//					posId = result->Fetch()[0].GetUInt32() + 1;
//
//				WorldDatabase.DirectPExecute("INSERT INTO _position(comment,posId,map,x,y,z,o) VALUES ('%s','%u','%u','%f','%f','%f','%f')", oss.str().c_str(), posId,map, x, y, z, o);
//
//				std::ostringstream oss1;
//				oss1 << "_position新增ID:" << posId << "备注:" << oss.str().c_str();
//				ChatHandler(player->GetSession()).PSendSysMessage(oss1.str().c_str());
//			}
//		}
//	}
//}
//
//float CustomEvent::GetDistance(Player* player, uint32 posId)
//{
//	std::unordered_map<uint32, PosTemplate>::iterator it = PosMap.find(posId);
//
//	if (it != PosMap.end())
//		if (it->second.map == player->GetMapId())
//			return player->GetDistance(it->second.x, it->second.y, it->second.z);
//		
//	return 1000.0f;
//}
//
//float CustomEvent::GetDistance(uint32 posId1, uint32 posId2)
//{
//	float x1, x2, y1, y2, z1, z2;
//	uint32 map1, map2;
//
//	bool exsit1 = false;
//	bool exsit2 = false;
//
//	std::unordered_map<uint32, PosTemplate>::iterator it = PosMap.find(posId1);
//
//	if (it != PosMap.end())
//	{
//		map1 = it->second.map;
//		x1 = it->second.x;
//		y1 = it->second.y;
//		z1 = it->second.z;
//		exsit1 = true;
//	}
//
//	it = PosMap.find(posId2);
//
//	if (it != PosMap.end())
//	{
//		map2 = it->second.map;
//		x2 = it->second.x;
//		y2 = it->second.y;
//		z2 = it->second.z;
//		exsit2 = true;
//	}
//
//	if (exsit1 && exsit2 && map1 == map2)
//		return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) + (z1 - z2) * (z1 - z2));
//
//	return 0;
//}
//
//class EventPlayerScript : PlayerScript
//{
//public:
//	EventPlayerScript() : PlayerScript("EventPlayerScript") {}
//
//	void OnCreatureKill(Player* player, Creature* creature) override
//	{
//		if (!sEvent->IsValid() || !sEvent->CheckValid(player))
//			return;
//
//		sEvent->UpdateWorldState(creature, player);
//		sEvent->Process(player, creature);
//		sEvent->UpdateActive(creature);
//		sEvent->UpdatePhase(creature);
//	}
//
//	void OnUseGameObject(Player* player, GameObject* gob) override
//	{
//		if (!sEvent->IsValid() || !sEvent->CheckValid(player))
//			return;
//
//		sEvent->UpdateWorldState(gob, player);
//		sEvent->Process(player, gob);
//		sEvent->UpdateActive(gob);
//		sEvent->UpdatePhase(gob);
//	}
//
//	void OnPVPKill(Player* killer, Player* killed) override
//	{
//		if (!sEvent->IsValid() || !sEvent->CheckValid(killer) || !sEvent->CheckValid(killed))
//			return;
//
//		sEvent->UpdateWorldState(killed, killer);
//		sEvent->Process(killer,killed);
//		sEvent->UpdatePhase(killed);
//	}
//
//	void OnUpdateZone(Player* player, uint32 newZone, uint32 /*newArea*/) 
//	{
//		player->RemoveAura(NARROW_EVENT_DEBUFF_ID);
//
//		if (!sEvent->IsValid() || sEvent->GetZoneId() == newZone || !sEvent->CheckValid(player))
//			return;
//
//		sEvent->RemovePlayer(player);
//	}
//};
//
//
////野外挑战脚本
//class StageTrigger : public CreatureScript
//{
//public:
//	StageTrigger() : CreatureScript("StageTrigger") { }
//
//	struct StageTriggerAI : public ScriptedAI
//	{
//		StageTriggerAI(Creature* creature) : ScriptedAI(creature), Summons(me)
//		{
//			stage = 1;
//			active = true;
//		}
//
//		SummonList Summons;
//		uint32 stage;
//		bool active;
//
//		void JustSummoned(Creature* summon) override
//		{
//			summon->GetMotionMaster()->MoveRandom(5.0f);
//			Summons.Summon(summon);
//		}
//
//		void Reset() override
//		{
//			stage = 1;
//			active = true;
//			Summons.DespawnAll();
//		}
//
//		void MoveInLineOfSight(Unit* who)
//		{
//			if (Summons.empty() && active)
//			{
//				std::ostringstream oss;
//				oss << "[胜者为王]：第" << stage << "关开始!";
//				sWorld->SendScreenMessage(oss.str().c_str());
//				me->SummonCreatureGroup(stage);
//			}
//		}
//
//		void SummonedCreatureDies(Creature* summon, Unit* killer)  override
//		{	
//			std::ostringstream oss;
//			oss << "[胜者为王]：[" << killer->GetName() << "]击杀[" << summon->GetCreatureTemplate()->Name << "]，挑战成功！";
//			sWorld->SendScreenMessage(oss.str().c_str());
//			Summons.Despawn(summon);
//
//			me->SummonGameObject(350005, -13216.9, 232.457, 190.98, 1.29123, 0, 0, 0, 0, 0);
//
//			stage++;
//
//			if (stage > 10)
//				active = false;
//		}
//	};
//
//	CreatureAI* GetAI(Creature* creature) const override
//	{
//		return new StageTriggerAI(creature);
//	}
//};
//
//
//#define MAX_MOUNT_SPELLS 8
//
//uint32 MountSpells[MAX_MOUNT_SPELLS] = { 95001, 95002, 95003, 95004, 95005, 95006, 95007, 95008 };
//
////HitSpellClone
//class HitSpellClone : public CreatureScript
//{
//public:
//	HitSpellClone() : CreatureScript("HitSpellClone") { }
//
//	struct HitSpellCloneAI : public ScriptedAI
//	{
//		HitSpellCloneAI(Creature* creature) : ScriptedAI(creature), Summons(me)
//		{
//			MoveTimer = 0;
//			SpellTimer = 0;
//			MoveInterval = 20 * IN_MILLISECONDS;
//		}
//
//		SummonList Summons;
//		uint32 MoveTimer;
//		uint32 SpellTimer;
//		uint32 MoveInterval;
//		std::unordered_map<uint32, int32>SpellMap;
//		std::vector<const SpellInfo*> SpellVec;
//		
//		void Reset() override
//		{
//			if (MAX_MOUNT_SPELLS > 1)
//			{
//				for (size_t i = 0; i < MAX_MOUNT_SPELLS; i++)
//					me->RemoveAura(MountSpells[i]);
//			
//				me->AddAura(MountSpells[urand(0, MAX_MOUNT_SPELLS - 1)], me);
//			}
//
//			SpellVec.clear();
//			MoveSpline(MoveInterval);
//			Summons.DespawnAll();
//		}
//
//		void SpellHit(Unit* caster, const SpellInfo* spell) override
//		{
//			if (!caster)
//				return;
//
//			std::unordered_map<uint32, int32>::iterator iter = SpellMap.find(spell->Id);
//			if (iter != SpellMap.end())
//				return;
//
//			SpellMap.insert(std::make_pair(spell->Id, 0));
//
//			if (std::find(SpellVec.begin(), SpellVec.end(), spell) != SpellVec.end())
//				return;
//
//			SpellVec.push_back(spell);
//		}
//
//		void SpellCooldown()
//		{
//			for (std::unordered_map<uint32, int32>::iterator iter = SpellMap.begin(); iter != SpellMap.end(); iter++)
//				if (iter->second > 0)
//					iter->second--;
//		}
//
//		void CastSpell(uint32 diff)
//		{
//			SpellTimer += diff;
//
//			if (SpellTimer > 2 * IN_MILLISECONDS)
//			{
//				SpellTimer = 0;
//
//				std::vector<uint32/*spellId*/> SpellVec;
//
//				for (std::unordered_map<uint32, int32>::iterator iter = SpellMap.begin(); iter != SpellMap.end(); iter++)
//					if (iter->second <= 0)
//						SpellVec.push_back(iter->first);
//
//				if (SpellVec.empty())
//					return;
//
//				uint32 spellId = SpellVec[urand(0, SpellVec.size() - 1)];
//
//				DoCast(spellId);
//
//				const SpellInfo* spellInfo = sSpellMgr->GetSpellInfo(spellId);
//
//				std::unordered_map<uint32, int32>::iterator iter = SpellMap.find(spellId);
//
//				if (iter != SpellMap.end())
//					iter->second = spellInfo->GetRecoveryTime();
//			}	
//		}
//
//		void MoveSpline(uint32 diff)
//		{
//			if (me->IsInCombat())
//				return;
//
//			MoveTimer += diff;
//
//			if (MoveTimer >= MoveInterval)
//			{
//				MoveTimer = 0;
//
//				if (WaypointPath const* i_path = sWaypointMgr->GetPath(me->GetWaypointPath()))
//				{
//					me->StopMoving();
//
//					float x = me->GetPositionX();
//					float y = me->GetPositionY();
//					float z = me->GetPositionZ();
//
//					Movement::PointsArray pathPoints;
//					pathPoints.push_back(G3D::Vector3(x, y, z));
//
//					WaypointData const* node;
//
//					uint8 _i = 0;
//					float _dist = 100000.0f;
//
//					for (uint8 i = 0; i < i_path->size(); ++i)
//					{
//						node = i_path->at(i);
//
//						float dist = sqrt((node->x - x) * (node->x - x) + (node->y - y) * (node->y - y) + (node->z - z) * (node->z - z));
//
//						if (dist < _dist)
//						{
//							_dist = dist;
//							_i = i;
//						}
//					}
//
//					for (uint8 i = _i; i < i_path->size(); ++i)
//					{
//						node = i_path->at(i);
//						pathPoints.push_back(G3D::Vector3(node->x, node->y, node->z));
//					}
//
//					for (uint8 i = 0; i < i_path->size(); ++i)
//					{
//						node = i_path->at(i);
//						pathPoints.push_back(G3D::Vector3(node->x, node->y, node->z));
//					}
//
//					me->GetMotionMaster()->MoveSplinePath(&pathPoints);
//				}
//			}
//		}
//
//		bool StopOnCasting()
//		{
//			if (me->HasUnitState(UNIT_STATE_CASTING))
//			{
//				me->StopMoving();
//				return true;
//			}
//			return false;
//		}
//
//		void UpdateAI(uint32 diff)
//		{
//			SpellCooldown();
//			MoveSpline(diff);
//
//			if (!UpdateVictim() || StopOnCasting())
//				return;
//
//			CastSpell(diff);
//			DoMeleeAttackIfReady();
//		}
//
//		void AttackStart(Unit* who)
//		{
//			ScriptedAI::AttackStartCaster(who, 10.0f);
//			ScriptedAI::AttackStart(who);
//		}
//
//		void EnterCombat(Unit* victim) 
//		{
//			
//		}
//
//		void MoveInLineOfSight(Unit* who)
//		{
//			if (who->IsHostileTo(me) && who->GetDistance(me) < 10.0f && !me->IsInCombat())
//				AttackStart(who);
//		}
//
//		void JustSummoned(Creature* summon) override
//		{
//			Summons.Summon(summon);
//		}
//
//		void SummonedCreatureDies(Creature* summon, Unit* killer)  override
//		{
//			
//		}
//	};
//
//	CreatureAI* GetAI(Creature* creature) const override
//	{
//		return new HitSpellCloneAI(creature);
//	}
//};
//
//void AddSC_CustomEventScript()
//{
//	new EventPlayerScript();
//	new StageTrigger();
//	new HitSpellClone();
//}
