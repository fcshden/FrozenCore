//#pragma execution_character_set("utf-8")
//#include "ThunderbluffBattle.h"
//#include "../CustomEvent/Event.h"
//#include "../Reward/Reward.h"
//
//MotionTransport* gunship	= NULL;
//
//bool isGeneral_1_Alive		= true;
//bool isGeneral_2_Alive		= true;
//bool isGeneral_3_Alive		= true;
//bool isBaine_Alive			= true;
//std::list<GameObject*> ThunderbluffGobList;
//std::list<Creature*> ThunderbluffCreatureList;
//int32 gunshipHP = 0;
//
//
//void ThunderbluffBattle::Init(Creature* controller)
//{
//	DelAllCreatures();
//	DelAllGameObjects();
//
//	SetPhase(PHASE_ZERO);
//
//	gunshipHP = GunShipHP;
//
//	isGeneral_1_Alive		= true;
//	isGeneral_2_Alive		= true;
//	isGeneral_3_Alive		= true;
//	isBaine_Alive			= true;
//
//	////////////controller 召唤////////////
//	//飞艇
//	gunship = sTransportMgr->CreateTransport(GO_GUNSHIP, 0, controller->GetMap());
//	if (gunship)
//		ThunderbluffGobList.push_back(gunship);
//	//传送器
//	for (size_t i = 0; i < 4; i++)
//	{
//		if (GameObject* portal = controller->SummonGameObject(GO_PORTAL_HIGHLAND, HighLandPortalPos[i].GetPositionX(), HighLandPortalPos[i].GetPositionY(), HighLandPortalPos[i].GetPositionZ(), HighLandPortalPos[i].GetOrientation(), 0, 0, 0, 0, 0))
//			ThunderbluffGobList.push_back(portal);
//	}
//
//	//鱼叉炮台及触发器
//	for (size_t i = 0; i < 15; i++)
//	{
//		if (GameObject* harppon = controller->SummonGameObject(GO_HARPOON, HarpoonPos[i].GetPositionX(), HarpoonPos[i].GetPositionY(), HarpoonPos[i].GetPositionZ(), HarpoonPos[i].GetOrientation(), 0, 0, 0, 0, 0))
//			ThunderbluffGobList.push_back(harppon);
//
//		controller->SummonCreature(NPC_HAROON_TRIGGER, HarpoonPos[i]);
//	}
//
//	//墓地
//	for (size_t i = 0; i < 4; i++)
//	{
//		if (Creature* defenseHealer = controller->SummonCreature(NPC_SPIRITHEALER, DefenseHealerPos[i]))
//			defenseHealer->setFaction(DEFENSE_FACTION);
//	}
//	//传送触发器
//	for (size_t i = 0; i < 10; i++)
//		controller->SummonCreature(NPC_TELE_TRIGGER, TeleTriggerPos[i]);
//	//牛头人将军
//	controller->SummonCreature(NPC_GENERAL_1, GeneralPos[0]);
//	controller->SummonCreature(NPC_GENERAL_2, GeneralPos[1]);
//	controller->SummonCreature(NPC_GENERAL_3, GeneralPos[2]);
//	controller->SummonCreature(NPC_BAINE, BainePos);
//
//
//
//	////////////飞艇 召唤////////////
//	//墓地
//	if (Creature* offenseHealer = AddCreatureToShip(NPC_SPIRITHEALER, OffenseHealerPos.GetPositionX(), OffenseHealerPos.GetPositionY(), OffenseHealerPos.GetPositionZ(), OffenseHealerPos.GetOrientation(), gunship))
//		offenseHealer->setFaction(OFFENSE_FACTION);
//	//传送器
//	AddGameObjectToShip(GO_PORTAL_SHIP, GunShipPortalPos.GetPositionX(), GunShipPortalPos.GetPositionY(), GunShipPortalPos.GetPositionZ(), GunShipPortalPos.GetOrientation(), gunship);
//		
//	//飞船外壳
//	if (Creature* gunshipShell = AddCreatureToShip(NPC_GUNSHIP_SHELL, OffenseHealerPos.GetPositionX(), OffenseHealerPos.GetPositionY(), OffenseHealerPos.GetPositionZ(), OffenseHealerPos.GetOrientation(), gunship))
//		gunshipShell->setFaction(OFFENSE_FACTION);
//
//	//飞机
//	for (size_t i = 0; i < 7; i++)
//	{
//		AddCreatureToShip(NPC_VEHICLE_AIR, GunshipAirVehicle[i].GetPositionX(), GunshipAirVehicle[i].GetPositionY(), GunshipAirVehicle[i].GetPositionZ(), GunshipAirVehicle[i].GetOrientation(), gunship);
//	}
//}
//
//void ThunderbluffBattle::Stop()
//{
//	DelAllCreatures();
//	DelAllGameObjects();
//
//	//奖励玩家
//
//	if (isBaine_Alive)
//		sThunderbluffBattle->RewardByCreature(NPC_BAINE, false);
//}
//
//
//void ThunderbluffBattle::RewardByCreature(uint32 entry, bool kill)
//{
//	uint32 ThunderbluffBattleRewSettingId = 0;
//
//	switch (entry)
//	{
//	case NPC_GENERAL_1:
//		ThunderbluffBattleRewSettingId = RewSettingId_General_1;
//		break;
//	case NPC_GENERAL_2:
//		ThunderbluffBattleRewSettingId = RewSettingId_General_2;
//		break;
//	case NPC_GENERAL_3:
//		ThunderbluffBattleRewSettingId = RewSettingId_General_3;
//		break;
//	case NPC_BAINE:
//		ThunderbluffBattleRewSettingId = RewSettingId_Baine;
//		break;
//	default:
//		break;
//	}
//
//	if (ThunderbluffBattleRewSettingId == 0)
//		return;
//
//	uint32 killsForRew = 0;
//	uint32 killedsForRew = 0;
//	float damageForRew = 0;
//	float healForRew = 0;
//	uint32 winRewId = 0;
//	uint32 losRewId = 0;
//
//	for (std::vector<EventRewTemplate>::iterator itr = EventRewVec.begin(); itr != EventRewVec.end(); ++itr)
//	{
//		if (itr->eventId == ThunderbluffBattleRewSettingId)
//		{
//			killsForRew = itr->killsForRew;
//			killedsForRew = itr->killedsForRew;
//			damageForRew = itr->damageForRew;
//			healForRew = itr->healForRew;
//			winRewId = itr->winRewId;
//			losRewId = itr->losRewId;
//			break;
//		}
//	}
//
//	SessionMap const& smap = sWorld->GetAllSessions();
//	for (SessionMap::const_iterator iter = smap.begin(); iter != smap.end(); ++iter)
//		if (Player* pl = iter->second->GetPlayer())
//		{
//			if (pl->event_team == EVENT_TEAM_NONE)
//				continue;
//
//			if (pl->event_kills >= killsForRew || pl->event_killeds >= killedsForRew
//				|| pl->event_damage >= damageForRew || pl->event_heal >= healForRew)
//			{
//				if (kill)
//					pl->event_team == EVENT_TEAM_OFFENSE ? sRew->Rew(pl, winRewId) : sRew->Rew(pl, losRewId);
//				else
//					pl->event_team == EVENT_TEAM_DEFENSE ? sRew->Rew(pl, winRewId) : sRew->Rew(pl, losRewId);
//
//				pl->event_kills		= 0;
//				pl->event_killeds	= 0;
//				pl->event_damage	= 0;
//				pl->event_heal		= 0;
//			}
//		}
//
//}
//
//
//Creature* ThunderbluffBattle::AddCreatureToShip(uint32 entry, float x, float y, float z, float o, MotionTransport* transport)
//{
//	Map* map = transport->GetMap();
//	if (!map)
//		return NULL;
//
//	if (transport)
//	{
//		transport->CalculatePassengerPosition(x, y, z, &o);
//		if (Creature* creature = transport->SummonCreature(entry, x, y, z, o, TEMPSUMMON_MANUAL_DESPAWN))
//		{
//			ThunderbluffCreatureList.push_back(creature);
//			transport->AddPassenger(creature, true);
//			return creature;
//		}	
//	}
//
//	return NULL;
//}
//
//GameObject* ThunderbluffBattle::AddGameObjectToShip(uint32 entry, float x, float y, float z, float o, MotionTransport* transport)
//{
//	Map* map = transport->GetMap();
//	if (!map)
//		return NULL;
//
//	if (transport)
//	{
//		transport->CalculatePassengerPosition(x, y, z, &o);
//		if (GameObject* gob = transport->SummonGameObject(entry, x, y, z, o, 0, 0, 0, 0, 0))
//		{
//			ThunderbluffGobList.push_back(gob);
//			transport->AddPassenger(gob, true);
//			return gob;
//		}
//	}
//
//	return NULL;
//}
//
//void ThunderbluffBattle::TeleAllPlayers()
//{
//	SessionMap const& smap = sWorld->GetAllSessions();
//	for (SessionMap::const_iterator iter = smap.begin(); iter != smap.end(); ++iter)
//		if (Player* pl = iter->second->GetPlayer())
//			if (pl->event_team == EVENT_TEAM_OFFENSE)
//				TelePlayerToShip(pl, gunship);
//			else if (pl->event_team == EVENT_TEAM_DEFENSE)
//				TelePlayerToHighLand(pl);
//}
//
//
//void ThunderbluffBattle::TelePlayerToShip(Player* player, MotionTransport* transport)
//{
//	if (!transport)
//		return;
//
//	float x = GunShipPortalPos.GetPositionX();
//	float y = GunShipPortalPos.GetPositionY();
//	float z = GunShipPortalPos.GetPositionZ();
//	float o = GunShipPortalPos.GetOrientation();
//
//	transport->CalculatePassengerPosition(x, y, z);
//	player->TeleportTo(transport->GetMapId(), x, y, z, o, TELE_TO_NOT_LEAVE_TRANSPORT);
//}
//
//void ThunderbluffBattle::TelePlayerToHighLand(Player* player)
//{	
//	switch (GetPhase())
//	{
//	case PHASE_ZERO:
//	case PHASE_ONE:
//		player->TeleportTo(1, HighLandPortalPos[0].GetPositionX(), HighLandPortalPos[0].GetPositionY(), HighLandPortalPos[0].GetPositionZ(), HighLandPortalPos[0].GetOrientation());
//		break;
//	case PHASE_TWO:
//		player->TeleportTo(1, HighLandPortalPos[1].GetPositionX(), HighLandPortalPos[1].GetPositionY(), HighLandPortalPos[1].GetPositionZ(), HighLandPortalPos[1].GetOrientation());
//		break;
//	case PHASE_THREE:
//		player->TeleportTo(1, HighLandPortalPos[2].GetPositionX(), HighLandPortalPos[2].GetPositionY(), HighLandPortalPos[2].GetPositionZ(), HighLandPortalPos[2].GetOrientation());
//		break;
//	case PHASE_FINAL:
//		player->TeleportTo(1, HighLandPortalPos[3].GetPositionX(), HighLandPortalPos[3].GetPositionY(), HighLandPortalPos[3].GetPositionZ(), HighLandPortalPos[3].GetOrientation());
//		break;
//	default:
//		break;
//	}
//}
//
//void ThunderbluffBattle::TelePlayerToShipHS(Player* player, MotionTransport* transport)
//{
//	if (!transport)
//		return;
//
//	float x = OffenseHealerPos.GetPositionX();
//	float y = OffenseHealerPos.GetPositionY();
//	float z = OffenseHealerPos.GetPositionZ();
//	float o = OffenseHealerPos.GetOrientation();
//
//	transport->CalculatePassengerPosition(x, y, z);
//	player->TeleportTo(transport->GetMapId(), x, y, z, o, TELE_TO_NOT_LEAVE_TRANSPORT);
//}
//
//void ThunderbluffBattle::TelePlayerToHighLandHS(Player* player)
//{
//	switch (GetPhase())
//	{
//	case PHASE_ZERO:
//	case PHASE_ONE:
//		player->TeleportTo(1, DefenseHealerPos[0].GetPositionX(), DefenseHealerPos[0].GetPositionY(), DefenseHealerPos[0].GetPositionZ(), DefenseHealerPos[0].GetOrientation());
//		break;
//	case PHASE_TWO:
//		player->TeleportTo(1, DefenseHealerPos[1].GetPositionX(), DefenseHealerPos[1].GetPositionY(), DefenseHealerPos[1].GetPositionZ(), DefenseHealerPos[1].GetOrientation());
//		break;
//	case PHASE_THREE:
//		player->TeleportTo(1, DefenseHealerPos[2].GetPositionX(), DefenseHealerPos[2].GetPositionY(), DefenseHealerPos[2].GetPositionZ(), DefenseHealerPos[2].GetOrientation());
//		break;
//	case PHASE_FINAL:
//		player->TeleportTo(1, DefenseHealerPos[3].GetPositionX(), DefenseHealerPos[3].GetPositionY(), DefenseHealerPos[3].GetPositionZ(), DefenseHealerPos[3].GetOrientation());
//		break;
//	default:
//		break;
//	}
//}
//
//bool ThunderbluffBattle::DiedTele(Player* player)
//{
//	if (player->event_team == EVENT_TEAM_NONE)
//		return false;
//
//	if (player->IsInCombat())
//		return false;
//
//	player->event_team == EVENT_TEAM_OFFENSE ? TelePlayerToShipHS(player, gunship) : TelePlayerToHighLandHS(player);
//	return true;
//}
//
//void ThunderbluffBattle::JoinTele(Player* player)
//{
//	player->event_team == EVENT_TEAM_OFFENSE ? TelePlayerToShipHS(player, gunship) : TelePlayerToHighLand(player);
//}
//
//void ThunderbluffBattle::DelAllGameObjects()
//{
//	for (std::list<GameObject*>::const_iterator itr = ThunderbluffGobList.begin(); itr != ThunderbluffGobList.end(); ++itr)
//		if ((*itr) && (*itr)->IsInWorld())
//		{
//			(*itr)->CleanupsBeforeDelete();
//			(*itr)->Delete();
//		}
//			
//	ThunderbluffGobList.clear();
//	gunship = NULL;
//}
//
//void ThunderbluffBattle::DelAllCreatures()
//{
//	for (std::list<Creature*>::const_iterator itr = ThunderbluffCreatureList.begin(); itr != ThunderbluffCreatureList.end(); ++itr)
//		if ((*itr) && (*itr)->IsInWorld())
//			(*itr)->RemoveFromWorld();
//
//	ThunderbluffCreatureList.clear();
//}
//
//void ThunderbluffBattle::SetWorldState(WorldStatesGeneral type, uint32 value)
//{
//	SessionMap const& smap = sWorld->GetAllSessions();
//	for (SessionMap::const_iterator iter = smap.begin(); iter != smap.end(); ++iter)
//		if (Player* pl = iter->second->GetPlayer())
//				pl->SendUpdateWorldState(type, value);
//}
