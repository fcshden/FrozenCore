#pragma execution_character_set("utf-8")
#include "ArenaDuel.h"
#include "../Custom/Reward/Reward.h"
#include "../Custom/Requirement/Requirement.h"
#include "../Custom/MainFunc/MainFunc.h"
#include "../Custom/CommonFunc/CommonFunc.h"
#include "../Custom/Command/CustomCommand.h"
#include "Pet.h"

WorldLocation ARENA_DUEL_CIRCLE_LOCATIONS[10] =
{
	WorldLocation(0, -13258.1, 257.313, 33.241, 0.376306),
	WorldLocation(0, -13260.6, 286.052, 33.346, 6.05081),
	WorldLocation(0, -13254.4, 302.142, 33.6155, 5.78377),
	WorldLocation(0, -13215.1, 328.655, 33.2434, 5.06514),
	WorldLocation(0, -13178.9, 321.976, 33.2434, 4.26796),
	WorldLocation(0, -13152.9, 295.351, 33.3038, 3.46685),
	WorldLocation(0, -13150.8, 262.599, 33.2429, 3.01524),
	WorldLocation(0, -13158.4, 241.41, 33.2958, 2.544),
	WorldLocation(0, -13180.7, 222.091, 33.2439, 2.18664),
	WorldLocation(0, -13199.7, 217.598, 33.2417, 1.83321),
};

WorldLocation ARENA_DUEL_MID_LOCATIONS[2] =
{
	WorldLocation(0, -13191.5, 303.531, 21.8581, 4.17468),
	WorldLocation(0, -13220.2, 246.299, 21.8581, 0.87599),
};


WorldLocation ARENA_DUEL_PILLAR_LOCATIONS[4] =
{
	WorldLocation(0, -13210.4, 262.851, 18.4, 1.16667),
	WorldLocation(0, -13197.5, 290.454, 18.4, 4.33035),
	WorldLocation(0, -13219.8, 284.421, 18.4, 5.82800),
	WorldLocation(0, -13189.0, 268.273, 18.4, 2.69405),
};

WorldLocation ARENA_DUEL_EYE_LOCATIONS[4] =
{
	WorldLocation(0, -13207.7, 287.798, 21.8574, 4.95382),
	WorldLocation(0, -13191.5, 279.775, 21.8574, 3.41052),
	WorldLocation(0, -13199.9, 264.983, 21.8574, 1.94182),
	WorldLocation(0, -13217.4, 272.533, 21.8574, 0.390663),
};

std::map<uint32, ArenaDuelTemplate> ArenaDuelMap;
std::unordered_map<uint64, int64> AreaDuelGambleMap;

void ArenaDuel::Load()
{
	ArenaDuelMap.clear();
	QueryResult result = CharacterDatabase.PQuery("SELECT guid,selected,winnum FROM characters_arena_duel");
	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			uint32 guid = fields[0].GetUInt32();
			ArenaDuelTemplate temp;
			temp.selected = fields[1].GetBool();
			temp.winNum = fields[2].GetUInt32();
			ArenaDuelMap.insert(std::make_pair(guid, temp));
		} while (result->NextRow());
	}
}

void ArenaDuel::Save(Player* pl)
{
	auto itr = ArenaDuelMap.find(pl->GetGUIDLow());
	if (itr == ArenaDuelMap.end())
		return;

	PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_REP_ARENA_DUEL);
	stmt->setUInt32(0, pl->GetGUIDLow());
	stmt->setBool(1, itr->second.selected);
	stmt->setUInt32(2, itr->second.winNum);
	CharacterDatabase.Execute(stmt);
}

void ArenaDuel::Signup(Player* pl)
{
	ArenaDuelTemplate temp;
	temp.selected = false;
	temp.winNum = 0;
	ArenaDuelMap.insert(std::make_pair(pl->GetGUIDLow(), temp));

	Save(pl);
}

void ArenaDuel::InitPlayer(Player* pl)
{
	pl->RemoveAllSpellCooldown();

	if (!pl->IsAlive())
	{
		pl->ResurrectPlayer(1.0f);
		pl->SpawnCorpseBones();
	}

	pl->RemoveAura(ARENA_DUEL_SEC_DEBUFF);
	pl->RemoveAura(ARENA_DUEL_FLAG_1);
	pl->RemoveAura(ARENA_DUEL_FLAG_2);
}

void ArenaDuel::TelePortAll()
{
	//std::random_shuffle(ArenaDuelMap.begin(), ArenaDuelMap.end());

	for (auto itr = ArenaDuelMap.begin(); itr != ArenaDuelMap.end(); itr++)
		if (Player* pl = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(itr->first, 0, HIGHGUID_PLAYER)))
		{
			InitPlayer(pl);
			TelePort(pl);
		}
}

void ArenaDuel::TelePort(Player* pl)
{
	pl->TeleportTo(ARENA_DUEL_CIRCLE_LOCATIONS[urand(0, 9)]);
}

void ArenaDuel::TeleportDueler()
{
	InitPlayer(_DUELER_1);
	InitPlayer(_DUELER_2);
	_DUELER_1->TeleportTo(ARENA_DUEL_MID_LOCATIONS[0]);
	_DUELER_2->TeleportTo(ARENA_DUEL_MID_LOCATIONS[1]);
}

Player* ArenaDuel::GenerateDueler()
{
	for (auto itr = ArenaDuelMap.begin(); itr != ArenaDuelMap.end(); itr++)
		if (!itr->second.selected)
			if (Player* pl = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(itr->first, 0, HIGHGUID_PLAYER)))
			{
				itr->second.selected = true;
				return pl;
			}

	return NULL;
}

bool ArenaDuel::HasDueler()
{
	for (auto itr = ArenaDuelMap.begin(); itr != ArenaDuelMap.end(); itr++)
		if (!itr->second.selected && ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(itr->first, 0, HIGHGUID_PLAYER)))
			return true;

	return false;
}

bool ArenaDuel::IsDueler(Player* pl)
{
	if (CheckDueler(_DUELER_1) && _DUELER_1->GetGUID() == pl->GetGUID())
		return true;

	if (CheckDueler(_DUELER_2) && _DUELER_2->GetGUID() == pl->GetGUID())
		return true;

	return false;
}

void ArenaDuel::StartDuel()
{
	//CREATE DUEL FLAG OBJECT
	uint32 gameobject_id = 21680;
	GameObject* pGameObj = new GameObject();

	Map* map = _DUELER_1->GetMap();
	if (!pGameObj->Create(sObjectMgr->GenerateLowGuid(HIGHGUID_GAMEOBJECT), gameobject_id,
		map, _DUELER_1->GetPhaseMask(),
		ARENA_DUEL_MID_LOCATIONS[0].GetPositionX() + (ARENA_DUEL_MID_LOCATIONS[1].GetPositionX() - ARENA_DUEL_MID_LOCATIONS[0].GetPositionX()) / 2,
		ARENA_DUEL_MID_LOCATIONS[0].GetPositionY() + (ARENA_DUEL_MID_LOCATIONS[1].GetPositionY() - ARENA_DUEL_MID_LOCATIONS[0].GetPositionY()) / 2,
		ARENA_DUEL_MID_LOCATIONS[0].GetPositionZ(),
		ARENA_DUEL_MID_LOCATIONS[0].GetOrientation(), G3D::Quat(), 0, GO_STATE_READY))
	{
		delete pGameObj;
		return;
	}

	pGameObj->SetUInt32Value(GAMEOBJECT_FACTION, _DUELER_1->getFaction());
	pGameObj->SetUInt32Value(GAMEOBJECT_LEVEL, _DUELER_1->getLevel() + 1);
	pGameObj->SetRespawnTime(0);
	pGameObj->SetSpellId(7266);


	_DUELER_1->AddGameObject(pGameObj);
	map->AddToMap(pGameObj, true);
	//END

	// create duel-info
	DuelInfo* duel = new DuelInfo;
	duel->initiator = _DUELER_1;
	duel->opponent = _DUELER_2;
	duel->startTime = 0;
	duel->startTimer = 0;
	duel->isMounted = false;
	_DUELER_1->duel = duel;

	DuelInfo* duel2 = new DuelInfo;
	duel2->initiator = _DUELER_1;
	duel2->opponent = _DUELER_1;
	duel2->startTime = 0;
	duel2->startTimer = 0;
	duel2->isMounted = false;
	_DUELER_2->duel = duel2;

	_DUELER_1->SetUInt64Value(PLAYER_DUEL_ARBITER, pGameObj->GetGUID());
	_DUELER_2->SetUInt64Value(PLAYER_DUEL_ARBITER, pGameObj->GetGUID());
}

void ArenaDuel::EndDuel(Player* winner, Player* loser)
{
	CloseGamble();

	if (CheckDueler(_DUELER_1) && _DUELER_1->GetGUID() == winner->GetGUID())
		EndGamble(true);
	else
		EndGamble(false);

	InitPlayer(winner);
	InitPlayer(loser);
	TelePort(loser);

	if (winner)
	{
		winner->UpdateMaxHealth();
		if (winner->getClass() == CLASS_SHAMAN)
			winner->UnsummonAllTotems();
	}

	if (loser)
	{
		loser->UpdateMaxHealth();
		if (loser->getClass() == CLASS_SHAMAN)
			loser->UnsummonAllTotems();
	}


	auto itr = ArenaDuelMap.find(winner->GetGUIDLow());
	if (itr != ArenaDuelMap.end())
		itr->second.winNum++;

	Save(winner);

	sRew->Rew(winner, ARENA_DUEL_WIN_REW_ID);
	sRew->Rew(loser, ARENA_DUEL_LOS_REW_ID);

	loser->CastSpell(loser, ARENA_DUEL_LOSER_SPELL);


	_DUELER_1 = winner;
	_DUELER_2 = NULL;


	std::ostringstream oss;
	oss << "决斗结束，";
	oss << sCF->GetNameLink(winner) << "胜利，";
	oss << sCF->GetNameLink(loser) << "失败！";
	sWorld->SendScreenMessage(oss.str().c_str());

	if (!HasDueler())
		SetState(ARENA_DUEL_STATE_END);
	else
		SetState(ARENA_DUEL_STATE_WAIT);
}

void ArenaDuel::SetState(ArenaDuelStates state)
{
	_State = state;

	switch (state)
	{
	case ARENA_DUEL_STATE_SIGNUP:
		break;
	case ARENA_DUEL_STATE_START:
		_DUELER_1 = NULL;
		_DUELER_2 = NULL;
		_PillarTimer = 0;
		for (size_t i = 0; i < 4; i++)
			_Pillar[i] = 0;
		break;
	case ARENA_DUEL_STATE_SELECT:
		break;
	case ARENA_DUEL_STATE_GAMBLE:
		sWorld->SendScreenMessage("产生决斗双方！决斗即将开始！现在开始下注！");
		_DUELER_1->AddAura(ARENA_DUEL_FLAG_1, _DUELER_1);
		_DUELER_2->AddAura(ARENA_DUEL_FLAG_2, _DUELER_2);
		_DUELER_1->AddAura(ARENA_DUEL_FREEZE_DEBUFF, _DUELER_1);
		_DUELER_2->AddAura(ARENA_DUEL_FREEZE_DEBUFF, _DUELER_2);
		//弹窗下注
		PopGamble();
		break;
	case ARENA_DUEL_STATE_DUEL:
		_DuelTimer = 0;
		_DuelSedcs = 0;
		_DuelTimer = 0;
		break;
	case ARENA_DUEL_STATE_WAIT:
		_WaitTimer = 0;
		_WaitSecs = ARENA_DUEL_WAIT_SECS;
		break;
	case ARENA_DUEL_STATE_END:
		_DUELER_1 = NULL;
		_DUELER_2 = NULL;
		_RewTimer = 0;
		_DuelTimer = 0;
		_DuelSedcs = 0;
		_WaitTimer = 0;
		_WaitSecs = ARENA_DUEL_WAIT_SECS;
		ArenaDuelMap.clear();
		CharacterDatabase.Execute(CharacterDatabase.GetPreparedStatement(CHAR_DEL_ARENA_DUEL));
		break;
	default:
		break;
	}
}

void ArenaDuel::TogglePillar(Map * map)
{
	for (size_t i = 0; i < 4; i++)
	{
		if (GameObject *go = map->GetGameObject(_Pillar[i]))
		{
			if (go->GetGoState() == GO_STATE_READY)
				go->SetGoState(GO_STATE_ACTIVE);
			else if (go->GetGoState() == GO_STATE_ACTIVE)
				go->SetGoState(GO_STATE_READY);
		}
	}
}

void ArenaDuel::Update(Map* map, uint32 diff)
{
	if (!map || map->GetId() != ARENA_DUEL_MAP)
		return;

	//报名阶段
	if (GetState() == ARENA_DUEL_STATE_SIGNUP)
		return;

	//活动开始
	if (GetState() == ARENA_DUEL_STATE_START)
	{
		sGameEventMgr->StopEvent(ARENA_DUEL_SIGNUP_EVENT_ID, false, true);

		if (!HasDueler())
		{
			SetState(ARENA_DUEL_STATE_END);
			return;
		}

		TelePortAll();
		SetState(ARENA_DUEL_STATE_WAIT);

		//柱子
		for (size_t i = 0; i < 4; i++)
		{
			if (GameObject* go = map->SummonGameObject(ARENA_DUEL_PILLAR_ENTRY, ARENA_DUEL_PILLAR_LOCATIONS[i].GetPositionX(), ARENA_DUEL_PILLAR_LOCATIONS[i].GetPositionY(), ARENA_DUEL_PILLAR_LOCATIONS[i].GetPositionZ(), ARENA_DUEL_PILLAR_LOCATIONS[i].GetOrientation(), 0, 0, 0, 0, 0))
			{
				go->SetGoState(GO_STATE_READY);
				_Pillar[i] = go->GetGUID();
			}
		}
	}

	//决斗等待
	if (GetState() == ARENA_DUEL_STATE_WAIT)
	{
		_WaitTimer += diff;
		if (_WaitTimer > IN_MILLISECONDS)
		{
			_WaitTimer = 0;
			_WaitSecs--;

			if (_WaitSecs > 0)
			{
				std::ostringstream oss;
				oss << "距离产生决斗者剩余时间：" << _WaitSecs << "秒";
				sWorld->SendScreenMessage(oss.str().c_str());

				if (_WaitSecs == 8 || _WaitSecs == 5)
				{
					if (!CheckDueler(_DUELER_1))
						_DUELER_1 = GenerateDueler();

					if (CheckDueler(_DUELER_1))
						_DUELER_1->GetSession()->SendNotification("你是下一位决斗者，请提前做好准备！");

					if (!CheckDueler(_DUELER_2))
						_DUELER_2 = GenerateDueler();

					if (CheckDueler(_DUELER_2))
						_DUELER_2->GetSession()->SendNotification("你是下一位决斗者，请提前做好准备！");
				}
			}
			else if (_WaitSecs == 0)
				SetState(ARENA_DUEL_STATE_SELECT);
		}
	}

	//产生决斗双方
	if (GetState() == ARENA_DUEL_STATE_SELECT)
	{
		if (!CheckDueler(_DUELER_1))
			_DUELER_1 = GenerateDueler();

		if (!CheckDueler(_DUELER_2))
			_DUELER_2 = GenerateDueler();

		if (CheckDueler(_DUELER_1) && CheckDueler(_DUELER_2))
		{
			if (ARENA_DUEL_GAMBLE_ENABLE)
				SetState(ARENA_DUEL_STATE_GAMBLE);
			else
				SetState(ARENA_DUEL_STATE_DUEL);

			TeleportDueler();
			StartDuel();
		}
		else
			SetState(ARENA_DUEL_STATE_END);


		for (size_t i = 0; i < 4; i++)
			if (GameObject *go = map->GetGameObject(_Pillar[i]))
				go->SetGoState(GO_STATE_READY);
	}

	//压注阶段
	if (GetState() == ARENA_DUEL_STATE_GAMBLE)
	{
		_GambleTimer += diff;

		if (_GambleTimer > ARENA_DUEL_GAMBLE_TIME)
		{
			//下注结束
			sWorld->SendScreenMessage("结束下注，决斗现在正式开始！");
			SetState(ARENA_DUEL_STATE_DUEL);
			CloseGamble();
		}
	}

	//开始决斗
	if (GetState() == ARENA_DUEL_STATE_DUEL)
	{
		_DuelTimer += diff;

		if (_DuelTimer > IN_MILLISECONDS)
		{
			if (CheckDueler(_DUELER_1))
				_DUELER_1->AddAura(ARENA_DUEL_SEC_DEBUFF, _DUELER_1);

			if (CheckDueler(_DUELER_2))
				_DUELER_2->AddAura(ARENA_DUEL_SEC_DEBUFF, _DUELER_2);

			_DuelTimer = 0;
			_DuelSedcs++;

			switch (_DuelSedcs)
			{
			case 0:
				sWorld->SendScreenMessage("距离眼睛刷新还有10秒！");
				break;
			case 5:
				TogglePillar(map);
				break;
			case 10:
			{
				TogglePillar(map);
				sWorld->SendScreenMessage("眼睛刷新了！");
				uint32 i = urand(0, 3);
				map->SummonGameObject(ARENA_DUEL_EYE_ENTRY, ARENA_DUEL_EYE_LOCATIONS[i].GetPositionX(), ARENA_DUEL_EYE_LOCATIONS[i].GetPositionY(), ARENA_DUEL_EYE_LOCATIONS[i].GetPositionZ(), ARENA_DUEL_EYE_LOCATIONS[i].GetOrientation(), 0, 0, 0, 0, 10);
			}
			break;
			case 20:
				TogglePillar(map);
				_DuelSedcs = 0;
				break;
			default:
				break;
			}
		}
	}

	//活动结束
	if (GetState() == ARENA_DUEL_STATE_END)
	{
		sWorld->SendScreenMessage("活动结束！");

		if (CheckDueler(_DUELER_1))
		{
			_DUELER_1->CastSpell(_DUELER_1, ARENA_DUEL_CHAMPION_SPELL);
			sRew->Rew(_DUELER_1, ARENA_DUEL_CHAMPION_REW_ID);
			std::string str = sCF->GetNameLink(_DUELER_1) + "获得了最终胜利！";
			sWorld->SendScreenMessage(str.c_str());
		}

		if (!map->GetPlayers().isEmpty())
			for (Map::PlayerList::const_iterator itr = map->GetPlayers().begin(); itr != map->GetPlayers().end(); ++itr)
				if (Player* pl = itr->GetSource())
					if (pl->GetAreaId() == ARENA_DUEL_AREA_OUT || pl->GetAreaId() == ARENA_DUEL_AREA_IN)
						sRew->Rew(pl, ARENA_DUEL_END_REW_ID);

		map->SummonGameObject(ARENA_DUEL_IN_GOB, ARENA_DUEL_MID_LOCATIONS[0].GetPositionX() + (ARENA_DUEL_MID_LOCATIONS[1].GetPositionX() - ARENA_DUEL_MID_LOCATIONS[0].GetPositionX()) / 2,
			ARENA_DUEL_MID_LOCATIONS[0].GetPositionY() + (ARENA_DUEL_MID_LOCATIONS[1].GetPositionY() - ARENA_DUEL_MID_LOCATIONS[0].GetPositionY()) / 2,
			ARENA_DUEL_MID_LOCATIONS[0].GetPositionZ(),
			ARENA_DUEL_MID_LOCATIONS[0].GetOrientation(), 0, 0, 0, 0, 30);

		SetState(ARENA_DUEL_STATE_SIGNUP);

		for (size_t i = 0; i < 4; i++)
		{
			if (GameObject *go = map->GetGameObject(_Pillar[i]))
				go->RemoveFromWorld();

			_Pillar[i] = 0;
		}

		sGameEventMgr->StopEvent(ARENA_DUEL_START_EVENT_ID, false, true);
	}

	//活动进行过程中
	if (GetState() == ARENA_DUEL_STATE_DUEL || GetState() == ARENA_DUEL_STATE_WAIT)
	{
		_RewTimer += diff;
		if (_RewTimer > ARENA_DUEL_REW_INTERVALS)
		{
			_RewTimer = 0;

			sWorld->SendScreenMessage("围观奖励发放！");

			if (!map->GetPlayers().isEmpty())
				for (Map::PlayerList::const_iterator itr = map->GetPlayers().begin(); itr != map->GetPlayers().end(); ++itr)
					if (Player* pl = itr->GetSource())
						if (pl->GetAreaId() == ARENA_DUEL_AREA_OUT || pl->GetAreaId() == ARENA_DUEL_AREA_IN)
							sRew->Rew(pl, ARENA_DUEL_PERMIN_REW_ID);
		}
	}

	//传送不在决斗中的玩家
	_AreaCheckTimer += diff;
	if (_AreaCheckTimer > 1000)
	{
		_AreaCheckTimer = 0;

		if (!map->GetPlayers().isEmpty())
			for (Map::PlayerList::const_iterator itr = map->GetPlayers().begin(); itr != map->GetPlayers().end(); ++itr)
				if (Player* pl = itr->GetSource())
					if (pl->GetAreaId() == ARENA_DUEL_AREA_IN && !IsDueler(pl))
					{
						TelePort(pl);
						pl->GetSession()->SendNotification("决斗过程中禁止无关玩家进入竞技场区域！");
					}

	}
}

void ArenaDuel::PopGamble()
{
	if (!CheckDueler(_DUELER_1) || !CheckDueler(_DUELER_2))
		return;

	_GambleTimer = 0;
	AreaDuelGambleMap.clear();

	SessionMap const& smap = sWorld->GetAllSessions();
	for (SessionMap::const_iterator iter = smap.begin(); iter != smap.end(); ++iter)
		if (WorldSession* ss = iter->second)
			if (Player* pl = ss->GetPlayer())
			{
				if (pl->GetGUID() == _DUELER_1->GetGUID() || pl->GetGUID() == _DUELER_2->GetGUID())
					continue;

				if (pl->GetAreaId() != ARENA_DUEL_AREA_IN && pl->GetAreaId() != ARENA_DUEL_AREA_OUT)
					continue;

				pl->PlayerTalkClass->ClearMenus();
				pl->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_MONEY_BAG, "|TInterface/Icons/INV_BannerPVP_01:30:30:0:0|t下注[红队]获胜 --> " + sCF->GetNameLink(_DUELER_1), GOSSIP_SENDER_MAIN, 1, "", 0, true);
				pl->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_MONEY_BAG, "|TInterface/Icons/INV_BannerPVP_02:30:30:0:0|t下注[蓝队]获胜 --> " + sCF->GetNameLink(_DUELER_2), GOSSIP_SENDER_MAIN, 2, "", 0, true);
				pl->PlayerTalkClass->GetGossipMenu().SetMenuId(ARENA_DUEL_GAMBLE_MENU_ID);
				pl->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pl->GetGUID());
			}
}

void ArenaDuel::CloseGamble()
{
	SessionMap const& smap = sWorld->GetAllSessions();
	for (SessionMap::const_iterator iter = smap.begin(); iter != smap.end(); ++iter)
		if (WorldSession* ss = iter->second)
			if (Player* pl = ss->GetPlayer())
			{
				if (pl->GetAreaId() != ARENA_DUEL_AREA_OUT && pl->GetAreaId() != ARENA_DUEL_AREA_IN)
					continue;

				pl->CLOSE_GOSSIP_MENU();
			}
}

void ArenaDuel::EndGamble(bool redwin)
{
	uint32 red = 0;
	uint32 blue = 0;
	uint32 total = 0;

	for (auto itr = AreaDuelGambleMap.begin(); itr != AreaDuelGambleMap.end(); itr++)
	{
		if (itr->second > 0)
			red += itr->second;
		else
			blue += abs(itr->second);
	}

	total = red + blue;

	if (total == 0)
		return;

	for (auto itr = AreaDuelGambleMap.begin(); itr != AreaDuelGambleMap.end(); itr++)
	{
		Player* pl = ObjectAccessor::FindPlayer(itr->first);

		if (!pl)
			continue;

		if (redwin)
		{
			if (itr->second > 0)
			{
				if (red > 0)
				{
					uint32 x = itr->second * total / red;
					sCF->UpdateTokenAmount(pl, x, true, "竞技场决斗赌博获胜");
					pl->GetSession()->SendAreaTriggerMessage("压注[红队]成功，获得%u[积分]", x);
				}
			}
			else
				pl->GetSession()->SendAreaTriggerMessage("压注[蓝队]失败");
		}
		else
		{
			if (itr->second < 0)
			{
				if (blue > 0)
				{
					uint32 x = abs(itr->second) * total / blue;
					sCF->UpdateTokenAmount(pl, x, true, "竞技场决斗赌博获胜");
					pl->GetSession()->SendAreaTriggerMessage("压注[蓝队]成功，获得%u[积分]", x);
				}
			}
			else
				pl->GetSession()->SendAreaTriggerMessage("压注[红队]失败");
		}
	}
}

class ArenaDuelPlayerScript : PlayerScript
{
public:
	ArenaDuelPlayerScript() : PlayerScript("ArenaDuelPlayerScript") {}

	void OnDuelEnd(Player* winner, Player* loser, DuelCompleteType /*type*/)
	{
		if (!sGameEventMgr->IsActiveEvent(ARENA_DUEL_START_EVENT_ID))
			return;

		if (winner->GetAreaId() != ARENA_DUEL_AREA_IN)
			return;

		sArenaDuel->EndDuel(winner, loser);
	}

	void OnGossipSelectCode(Player* pl, uint32 menu_id, uint32 sender, uint32 action, const char* code)
	{
		if (menu_id != ARENA_DUEL_GAMBLE_MENU_ID)
			return;

		pl->CLOSE_GOSSIP_MENU();

		int64 count = abs(atoi(code));

		if (sCF->GetTokenAmount(pl) < count)
		{
			pl->GetSession()->SendNotification("[积分]不足");
			return;
		}

		sCF->UpdateTokenAmount(pl, count, false, "竞技场决斗赌博压注");

		if (action == 1)
			pl->GetSession()->SendAreaTriggerMessage("你下注%u[积分][%s]获胜", count, "红队");
		else if (action == 2)
		{
			pl->GetSession()->SendAreaTriggerMessage("你下注%u[积分][%s]获胜", count, "蓝队");
			count = -count;
		}

		AreaDuelGambleMap.insert(std::make_pair(pl->GetGUID(), count));
	}
};

class ArenaDuelCreatureScript : public CreatureScript
{
public:
	ArenaDuelCreatureScript() : CreatureScript("ArenaDuelCreatureScript") { }


	bool OnGossipHello(Player* pl, Creature* c) override
	{
		pl->PlayerTalkClass->ClearMenus();

		if (sArenaDuel->GetState() == ARENA_DUEL_STATE_SIGNUP)
		{
			auto itr = ArenaDuelMap.find(pl->GetGUIDLow());
			if (itr != ArenaDuelMap.end())
			{
				pl->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "你已经报过名了！", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
			}
			else
			{
				if (sMF->Pop(ARENA_DUEL_SIGNUP_REQ_ID))
					pl->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "我要报名", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF, sReq->Notice(pl, ARENA_DUEL_SIGNUP_REQ_ID, "竞技场决斗报名", ""), sReq->Golds(ARENA_DUEL_SIGNUP_REQ_ID), false);
				else
					pl->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "我要报名", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
			}
		}
		else
			pl->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "竞技场报名已经结束，过去围观", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

		pl->SEND_GOSSIP_MENU(c->GetEntry(), c->GetGUID());

		return true;
	}

	bool OnGossipSelect(Player* pl, Creature* creature, uint32 sender, uint32 action) override
	{
		if (action == GOSSIP_ACTION_INFO_DEF)
		{
			sArenaDuel->Signup(pl);
			pl->GetSession()->SendAreaTriggerMessage("报名成功！");
		}
		else if (action == GOSSIP_ACTION_INFO_DEF + 1)
			sArenaDuel->TelePort(pl);

		pl->CLOSE_GOSSIP_MENU();

		return true;
	}
};

void AddSC_ArenaDuel()
{
	new ArenaDuelPlayerScript();
	new ArenaDuelCreatureScript();
}
