#pragma execution_character_set("utf-8")
#include "BattleIC.h"
#include "../Event.h"
#include "Transport.h"
#include "BattlegroundIC.h"
#include "../Custom/CommonFunc/CommonFunc.h"
#include "../Custom/Reward/Reward.h"

int BGICSort(const std::pair<uint64, BGICPlayerScore>& x, const std::pair<uint64, BGICPlayerScore>& y)
{
	return x.second.kills > y.second.kills;
}

void BGICSortMapByValue(std::unordered_map<uint64, BGICPlayerScore>& tMap, std::vector<std::pair<uint64, BGICPlayerScore> >& tVector)
{
	for (std::unordered_map<uint64, BGICPlayerScore>::iterator curr = tMap.begin(); curr != tMap.end(); curr++)
		tVector.push_back(std::make_pair(curr->first, curr->second));

	sort(tVector.begin(), tVector.end(), BGICSort);
}

void BGIC::Stop()
{
	std::vector<std::pair<uint64, BGICPlayerScore>> tVector;
	BGICSortMapByValue(_PlayersMap, tVector);

	std::ostringstream oss;
	std::ostringstream oss1;
	uint32 count = 0;

	oss << "吃鸡榜\n\n";

	for (auto itr = _PlayersMap.begin(); itr != _PlayersMap.end(); itr++)
	{
		if (count > BGIC_DISPLAY_MAX)
			break;

		if (Player* pl = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(itr->first, 0, HIGHGUID_PLAYER)))
		{
			count++;
			oss << sCF->GetNameLink(pl) << " - " << itr->second.kills << "\n";
		}	
	}

	for (auto itr = _PlayersMap.begin(); itr != _PlayersMap.end(); itr++)
	{
		if (Player* pl = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(itr->first, 0, HIGHGUID_PLAYER)))
		{
			if (itr == _PlayersMap.begin())
			{
				oss1 << "\n大吉大利，今晚吃鸡！\n";
				oss1 << "\n你的个人战绩" << " - " << itr->second.kills << "\n";
				sRew->Rew(pl, BGIC_SSS_REWARD_ID);
				sCF->SendAcceptOrCancel(pl, 0, oss.str() + oss1.str());
			}
			else
			{
				oss1 << "你的个人战绩" << " - " << itr->second.kills;
				sRew->Rew(pl, BGIC_REWARD_ID, itr->second.kills);
				sCF->SendAcceptOrCancel(pl, 0, oss.str() + oss1.str());
			}
		}
	}

	Reset();
}

void BGIC::InitParams(uint32 eventId, Map* map)
{
	if (eventId != BGIC_EVENT_ID)
		return;

	if (!map || map->GetId() != BGIC_EVENT_MAP)
		return;

	_Map = map;

	for (size_t i = 110; i <= 475; i++)
		_PosionNpcPosVec.push_back(i);


	_DieLootItemMap.insert(std::make_pair(25, 100));

	if (gunshipHorde = sTransportMgr->CreateTransport(GO_HORDE_GUNSHIP))
		gunshipHorde->EnableMovement(true);

	if (gunshipAlliance = sTransportMgr->CreateTransport(GO_ALLIANCE_GUNSHIP))
		gunshipAlliance->EnableMovement(true);
}

void BGIC::Reset()
{
	_Timer = 0;
	_SpawnUpdateTimer = 0;
	_PosionNpcPosVec.clear();
	_ChestGobPosVec.clear();
	_DieLootItemMap.clear();
	_PlayersMap.clear();

	if (gunshipHorde)
	{
		gunshipHorde->CleanupsBeforeDelete();
		gunshipHorde->AddObjectToRemoveList();
		gunshipHorde = NULL;
	}

	if (gunshipAlliance)
	{
		gunshipAlliance->CleanupsBeforeDelete();
		gunshipAlliance->AddObjectToRemoveList();
		gunshipAlliance = NULL;
	}

	if (_Map)
		for (auto itr = _PoisonNpcGUIDVec.begin(); itr != _PoisonNpcGUIDVec.end(); itr++)
			if (Creature* c = _Map->GetCreature(*itr))
				c->RemoveFromWorld();

	_Map = NULL;
}

void BGIC::AddPlayer(Player* pl)
{
	BGICPlayerScore Temp;
	Temp.kills = 0;
	Temp.killeds = 0;
	_PlayersMap.insert(std::make_pair(pl->GetGUID(), Temp));
	InitPlayer(pl);
	TeleToShip(pl);
	SetHealth(NULL, pl, BGIC_HEALTH_MAX);
}

void BGIC::RemovePlayer(Player* pl)
{
	auto itr = _PlayersMap.find(pl->GetGUID());
	if (itr != _PlayersMap.end())
	{
		_PlayersMap.erase(itr);
		ChatHandler(pl->GetSession()).PSendSysMessage("你已经离开吃鸡战场！");
		ResetPlayer(pl);
	}	
}

void BGIC::InitPlayer(Player* pl)
{
	//if (!pl->IsAlive())
	//{
	//	pl->ResurrectPlayer(1.0f);
	//	pl->SpawnCorpseBones();
	//}
	//
	//pl->SetHealth(1000);
	//pl->SetMaxHealth(1000);
}

void BGIC::ResetPlayer(Player* pl)
{
	if (!pl->IsAlive())
	{
		pl->ResurrectPlayer(1.0f);
		pl->SpawnCorpseBones();
	}

	pl->UpdateMaxHealth();
}

void BGIC::TeleToShip(Player* pl)
{
	uint32 random = urand(TEAM_ALLIANCE, TEAM_HORDE);

	MotionTransport* transport = random == TEAM_ALLIANCE ? gunshipAlliance : gunshipHorde;
	float x = BG_IC_HangarTrigger[random].GetPositionX();
	float y = BG_IC_HangarTrigger[random].GetPositionY();
	float z = BG_IC_HangarTrigger[random].GetPositionZ();

	if (transport)
	{
		transport->CalculatePassengerPosition(x, y, z);
		pl->TeleportTo(pl->GetMapId(), x, y, z + 1.0f, pl->GetOrientation(), TELE_TO_NOT_LEAVE_TRANSPORT);
		transport->AddPassenger(pl, true);
	}
	
	

	//if (!Valid(pl))
	//	return;
	//
	//pl->SetUnderACKmount();
	//
	//MotionTransport* gunship;
	//float offset_z = 0;
	//
	//if (urand(0, 1) == 1)
	//{
	//	offset_z = 25.0f;
	//	gunship = gunshipAlliance;
	//}	
	//else
	//{
	//	offset_z = 35.0f;
	//	gunship = gunshipHorde;
	//}
	//	
	//if (gunship)
	//{
	//	pl->TeleportTo(BGIC_EVENT_MAP, gunship->RealPosition.GetPositionX(), gunship->RealPosition.GetPositionY(), gunship->RealPosition.GetPositionZ() + offset_z, gunship->RealPosition.GetOrientation() + 3.13f, TELE_TO_NOT_LEAVE_TRANSPORT);
	//	gunship->AddPassenger(pl, true);
	//}
}

void BGIC::UpdateSpawn(uint32 diff)
{
	if (!_Map)
		return;

	_Timer += diff;
	
	//for (auto itr = _PlayersMap.begin(); itr != _PlayersMap.end(); itr++)
	//	if (Player* pl = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(itr->first, 0, HIGHGUID_PLAYER)))
	//	{
	//
	//		float x = gunshipAlliance->RealPosition.GetPositionX();
	//		float y = gunshipAlliance->RealPosition.GetPositionY();
	//		float z = gunshipAlliance->RealPosition.GetPositionZ() + 21.0f;
	//		pl->SetTelePortDest(BGIC_EVENT_MAP, x, y, z,pl->GetOrientation());
	//	}

	if (_Timer < IN_MILLISECONDS)
		return;

	//for(auto itr = _PlayersMap.begin(); itr != _PlayersMap.end(); itr++)
	//if (Player* pl = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(itr->first, 0, HIGHGUID_PLAYER)))
	//{
	//	float x = gunshipAlliance->RealPosition.GetPositionX();
	//	float y = gunshipAlliance->RealPosition.GetPositionY();
	//	float z = gunshipAlliance->RealPosition.GetPositionZ() + 21.0f;
	//
	//	if (pl->GetDistance(x, y, z) > 1.0f)
	//	{
	//		pl->TeleportTo(BGIC_EVENT_MAP, x, y, z, gunshipAlliance->RealPosition.GetOrientation() + 3.13f, TELE_TO_NOT_LEAVE_TRANSPORT);
	//		pl->GetSession()->SendNotification("tele");
	//	}
	//
	//}
	//
			;//;pl->SetTelePortDest(BGIC_EVENT_MAP, gunshipAlliance->RealPosition.GetPositionX(), gunshipAlliance->RealPosition.GetPositionY(), gunshipAlliance->RealPosition.GetPositionZ() + 20.66, gunshipAlliance->RealPosition.GetOrientation() + 3.13f);

	_Timer = 0;

	_SpawnUpdateTimer += 1;

	if (!_PosionNpcPosVec.empty())
	{
		uint32 i = urand(0, _PosionNpcPosVec.size() - 1);
		uint32 posId = _PosionNpcPosVec[i];
		auto itr = _PosionNpcPosVec.begin() + i;
		_PosionNpcPosVec.erase(itr);

		uint32 _map = 0;
		float x = 0;
		float y = 0;
		float z = 0;
		float o = 0;
		sEvent->GetTelePos(posId, _map, x, y, z, o);

		if (_map != 0)
		{
			Position pos;
			pos.Relocate(x, y, z, o);
			if (Creature* c = _Map->SummonCreature(BGIC_NPC_POSION, pos))//, 0, 2 * 60 * IN_MILLISECONDS))
				_PoisonNpcGUIDVec.push_back(c->GetGUID());
		}
	}
}

bool BGIC::InWar(Player* pl)
{
	if (pl->GetMapId() != BGIC_EVENT_MAP)
		return false;

	if (pl->GetBattleground())
		return false;

	return true;
}

void BGIC::OnPVPKill(Player* killer, Player* victim)
{
	if (!InWar(killer) || !InWar(victim))
		return;

	std::string text = "吃鸡战场:" + sCF->GetNameLink(killer) + "击杀了" + sCF->GetNameLink(victim) + "!";

	if (GameObject* chest = victim->SummonGameObject(BGIC_GOB_KILL_CHEST, victim->GetPositionX(), victim->GetPositionY(), victim->GetPositionZ(), 0, 0, 0, 0, 0, 0))
	{
		for (size_t slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; slot++)
		{
			Item * item = victim->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);

			if (!item)
				continue;

			uint32 entry = item->GetEntry();

			auto itr = _DieLootItemMap.find(entry);

			if (itr == _DieLootItemMap.end())
				continue;

			if (!roll_chance_f(itr->second))
				continue;

			victim->DestroyItem(item->GetBagSlot(), item->GetSlot(), true);

			chest->LootExtraItems.insert(std::make_pair(entry, 1));
		}

		if (!chest->LootExtraItems.empty())
		{
			for (auto itr = chest->LootExtraItems.begin(); itr != chest->LootExtraItems.end(); itr++)
				text += sCF->GetItemLink(itr->first);

			text += "已掉落！";
		}
			
	}

	SendScreenMessage(text);

	InitPlayer(victim);
	TeleToShip(victim);

	for (auto itr = _PlayersMap.begin(); itr != _PlayersMap.end(); itr++)
	{
		if (itr->first == victim->GetGUID())
			itr->second.killeds++;
		else if (itr->first == killer->GetGUID())
			itr->second.kills++;
	}	
}

void BGIC::OnCreatureKill(Creature* creature, Player* victim)
{
	if (!InWar(victim))
		return;

	std::string text = "吃鸡战场:" + sCF->GetNameLink(victim) + "被剧毒吞噬!";

	if (GameObject* chest = victim->SummonGameObject(BGIC_GOB_KILL_CHEST, victim->GetPositionX(), victim->GetPositionY(), victim->GetPositionZ(), 0, 0, 0, 0, 0, 0))
	{
		for (size_t slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; slot++)
		{
			Item * item = victim->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);

			if (!item)
				continue;

			uint32 entry = item->GetEntry();

			auto itr = _DieLootItemMap.find(entry);

			if (itr == _DieLootItemMap.end())
				continue;

			if (!roll_chance_f(itr->second))
				continue;

			victim->DestroyItem(item->GetBagSlot(), item->GetSlot(), true);

			chest->LootExtraItems.insert(std::make_pair(entry, 1));
		}

		if (!chest->LootExtraItems.empty())
		{
			for (auto itr = chest->LootExtraItems.begin(); itr != chest->LootExtraItems.end(); itr++)
				text += sCF->GetItemLink(itr->first);

			text += "已掉落！";
		}
	}

	SendScreenMessage(text);

	InitPlayer(victim);
	TeleToShip(victim);
}

void BGIC::SendScreenMessage(std::string text)
{
	if (text.empty())
		return;

	WorldPacket data;
	ChatHandler::BuildChatPacket(data, CHAT_MSG_RAID_BOSS_EMOTE, LANG_UNIVERSAL,
		NULL, NULL, text);

	for (auto itr = _PlayersMap.begin(); itr != _PlayersMap.end(); itr++)
		if (Player* pl = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(itr->first, 0, HIGHGUID_PLAYER)))
			if (WorldSession* ss = pl->GetSession())
				pl->GetSession()->SendPacket(&data);
}

void BGIC::CheckStop()
{
	uint32 max = 0;
	for (auto itr = _PlayersMap.begin(); itr != _PlayersMap.end(); itr++)
		if (Player* pl = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(itr->first, 0, HIGHGUID_PLAYER)))
			if (itr->second.kills >= max)
				max = itr->second.kills;

	if (max >= BGIC_STOP_KILLS)
		Stop();
}

bool BGIC::Valid(Unit* caster)
{
	if (!caster || !_Map || _Map->GetId() != BGIC_EVENT_MAP || caster->GetTypeId() != TYPEID_PLAYER || caster->ToPlayer()->InBattleground() || caster->GetMapId() != BGIC_EVENT_MAP)
		return false;

	return true;
}

bool BGIC::IsAllowed(Unit* caster, uint32 spellId)
{
	if (!Valid(caster))
		return false;

	//以下技能允许平静状态下使用
	return false;
}

void BGIC::SetHealth(Player* attacker, Player* victim, int32 val)
{
	if (attacker)
	{
		if (val <= 0)
		{
			OnPVPKill(attacker, victim);
			CheckStop();
			val = BGIC_HEALTH_MAX;
		}
	}
	
	if (val > BGIC_HEALTH_MAX)
		val = BGIC_HEALTH_MAX;

	victim->SetUInt32Value(UNIT_FIELD_HEALTH, val);
	victim->SetUInt32Value(UNIT_FIELD_MAXHEALTH, BGIC_HEALTH_MAX);
}

class BGICPlayerScript : PlayerScript
{
public:
	BGICPlayerScript() : PlayerScript("BGICPlayerScript") {}

	void OnCreatureKill(Player* player, Creature* creature)
	{
		
	}

	void OnUseGameObject(Player* player, GameObject* gob)
	{
		
	}

	void OnPVPKill(Player* killer, Player* killed)
	{
		sBGIC->OnPVPKill(killer, killed);
		sBGIC->CheckStop();
	}

	void OnPlayerKilledByCreature(Creature* killer, Player* killed)
	{
		sBGIC->OnCreatureKill(killer, killed);
	}

	void OnUpdateZone(Player* player, uint32 newZone, uint32 /*newArea*/)
	{
		
	}

	void OnMapChanged(Player* player)
	{
		sBGIC->RemovePlayer(player);
	}
};


class spell_bgic_shoot : public SpellScriptLoader
{
public:
	spell_bgic_shoot() : SpellScriptLoader("spell_bgic_shoot") { }

	class spell_bgic_shoot_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_bgic_shoot_SpellScript);

		bool Load()
		{
			_x = _y = _z = 0.0f;
			return true;
		}

		SpellCastResult Cast()
		{
			Unit* caster = GetCaster();
			//float o = Position::NormalizeOrientation(caster->GetOrientation() + frand(0.0f, 2 * M_PI));
			//float dist = frand(5.0f, 30.0f);
			//_x = caster->GetPositionX() + dist*cos(o);
			//_y = caster->GetPositionY() + dist*sin(o);
			//_z = caster->GetPositionZ() + frand(-10.0f, 15.0f);
			//GetSpell()->m_targets.SetDst(_x, _y, _z, 0.0f, caster->GetMapId());


			float x = GetSpell()->m_targets.GetDstPos()->GetPositionX();
			float y = GetSpell()->m_targets.GetDstPos()->GetPositionY();
			float z = GetSpell()->m_targets.GetDstPos()->GetPositionZ();

			Creature* victim = caster->FindNearestCreature(3, 100);

			Position srcPos;
			srcPos.m_positionX = caster->GetPositionX();
			srcPos.m_positionY = caster->GetPositionY();
			srcPos.m_positionZ = caster->GetPositionZ();

			Position victimPos;
			victimPos.m_positionX = victim->GetPositionX();
			victimPos.m_positionY = victim->GetPositionY();
			victimPos.m_positionZ = victim->GetPositionZ();

			if ((&srcPos)->HasInArc(1.0f,(&victimPos)))


			//if (caster->isInFrontInMap(victim, caster->GetDistance(x,y,z), 1))
				ChatHandler(caster->ToPlayer()->GetSession()).PSendSysMessage("2");


			return SPELL_CAST_OK;
		}

		void ChangeDest(SpellEffIndex effIndex)
		{
			PreventHitDefaultEffect(effIndex);
		}

	
		void Register()
		{
			OnCheckCast += SpellCheckCastFn(spell_bgic_shoot_SpellScript::Cast);
			OnEffectHit += SpellEffectFn(spell_bgic_shoot_SpellScript::ChangeDest, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);		
		}

	private:
		float _x, _y, _z;
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_bgic_shoot_SpellScript();
	}
};


void AddSC_BGICScript()
{
	new BGICPlayerScript();
	new spell_bgic_shoot();
}
