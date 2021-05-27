#pragma execution_character_set("utf-8")
#include "Event.h"
#include "GameEventMgr.h"
#include "Battleground.h"
#include "BattlegroundMgr.h"
#include "../CustomEvent/FixedTimeBG/FixedTimeBG.h"
#include "../Reward/Reward.h"
#include "GroupMgr.h"
#include "Group.h"
#include "../FakePlayers/FakePlayers.h"
#include "ThunderbluffBattle/ThunderbluffBattle.h"
#include "../AuthCheck/AuthCheck.h"
#include "../CommonFunc/CommonFunc.h"
#include "SACity/SA.h"
#include "../QuickResponse/QuickResponse.h"
#include "../Talisman/Talisman.h"
#include "../FunctionCollection/FunctionCollection.h"
#include "UnknownBG/UnknownBG.h"
#include "../Command/CustomCommand.h"
#include "WaypointManager.h"
#include "MapManager.h"
#include "Guild.h"
#include "../Switch/Switch.h"
#include "BattleIC/BattleIC.h"
#include "ArenaDuel/ArenaDuel.h"
// load db

void Player::UpdateEventDamage(Unit* target, float damage)
{ 
	if (!target || target->GetGUID() == GetGUID() || (target->GetTypeId() == TYPEID_UNIT && target->ToCreature()->IsCivilian()))
		return;

	_EventData.Damage += damage; 
}

void Player::UpdateEventHeal(Unit* target, float heal)
{ 
	if (!target || target->GetTypeId() != TYPEID_PLAYER || target->GetGUID() == GetGUID()) 
		return;

	_EventData.Heal += heal; 
}

void Player::UpdateEventKills(Unit* target)
{ 
	if (!target || target->GetGUID() == GetGUID()) 
		return;

	_EventData.Kills++; 
	sEvent->UpdateKillInfo(this, true);
}

void Player::UpdateEventKilleds(Unit* target)
{ 
	if (!target || target->GetGUID() == GetGUID()) 
		return;

	_EventData.Killeds++;
	sEvent->UpdateKillInfo(this, false);
}

void Player::InitEventData(uint32 EventId, EventFactionId FactionId, std::string FactionName)
{
	_EventData.EventId = EventId;
	_EventData.FactionId = FactionId;
	_EventData.FactionName = FactionName;
	_EventData.Damage = 0;
	_EventData.Heal = 0;
	_EventData.Kills = 0;
	_EventData.Killeds = 0;

	if (FactionId != 0)
		setFaction(FactionId);
}

void Player::EventRest()
{
	if (!InEvent())
		return;

	_EventData = { 0, "", 0, 0, 0, 0, 0 };
	setFactionForRace(getRace());
	UpdateMaxHealth();
	UninviteFromGroup();
	if (GetGroup())
		RemoveFromGroup();
}

class EventPlayerScript : PlayerScript
{
public:
	EventPlayerScript() : PlayerScript("EventPlayerScript") {}

	void OnCreatureKill(Player* player, Creature* creature)
	{
		if (!sEvent->Valid())
			return;

		sEvent->UpdateActive(creature);
		sEvent->UpdatePhase(creature);
		sEvent->UpdateWorldState(creature);
		sEvent->Process(creature);
	}

	void OnUseGameObject(Player* player, GameObject* gob)
	{	
		if (!sEvent->Valid())
			return;

		sEvent->UpdateActive(gob);
		sEvent->UpdatePhase(gob);
		sEvent->UpdateWorldState(gob);
		sEvent->Process(gob);
	}

	void OnPVPKill(Player* killer, Player* killed)
	{
		if (!sEvent->Valid())
			return;

		sEvent->UpdateActive(killer, true);
		sEvent->UpdateActive(killed, false);
		sEvent->UpdatePhase(killer, true);
		sEvent->UpdatePhase(killed, false);
		sEvent->UpdateWorldState(killer, true);
		sEvent->UpdateWorldState(killed, false);
		sEvent->Process(killer);
	}

	void OnPlayerKilledByCreature(Creature* killer, Player* killed) 
	{ 
		
	}

	void OnUpdateZone(Player* player, uint32 newZone, uint32 /*newArea*/) 
	{
		if (!player->InEvent())
			return;

		int32 zone = sEvent->GetZoneId();

		if (zone <= 0)
			return;

		if (zone == newZone)
			return;
		
		player->EventRest();
	}

	void OnMapChanged(Player* player)
	{
		if (!player->InEvent())
			return;

		int32 zone = sEvent->GetZoneId();
		
		if (zone >= 0)
			return;

		if (-zone == player->GetMapId())
			return;

		player->EventRest();
	}
};


//Ұ����ս�ű�
class StageTrigger : public CreatureScript
{
public:
	StageTrigger() : CreatureScript("StageTrigger") { }

	struct StageTriggerAI : public ScriptedAI
	{
		StageTriggerAI(Creature* creature) : ScriptedAI(creature), Summons(me)
		{
			stage = 1;
			active = true;
		}

		SummonList Summons;
		uint32 stage;
		bool active;

		void JustSummoned(Creature* summon) override
		{
			summon->GetMotionMaster()->MoveRandom(5.0f);
			Summons.Summon(summon);
		}

		void Reset() override
		{
			stage = 1;
			active = true;
			Summons.DespawnAll();
		}

		void MoveInLineOfSight(Unit* who)
		{
			if (Summons.empty() && active)
			{
				std::ostringstream oss;
				oss << "[ʤ��Ϊ��]����" << stage << "�ؿ�ʼ!";
				sWorld->SendScreenMessage(oss.str().c_str());
				me->SummonCreatureGroup(stage);
			}
		}

		void SummonedCreatureDies(Creature* summon, Unit* killer)  override
		{	
			std::ostringstream oss;
			oss << "[ʤ��Ϊ��]��[" << killer->GetName() << "]��ɱ[" << summon->GetCreatureTemplate()->Name << "]����ս�ɹ���";
			sWorld->SendScreenMessage(oss.str().c_str());
			Summons.Despawn(summon);

			me->SummonGameObject(350005, -13216.9, 232.457, 190.98, 1.29123, 0, 0, 0, 0, 0);

			stage++;

			if (stage > 10)
				active = false;
		}
	};

	CreatureAI* GetAI(Creature* creature) const override
	{
		return new StageTriggerAI(creature);
	}
};


#define MAX_MOUNT_SPELLS 8

uint32 MountSpells[MAX_MOUNT_SPELLS] = { 95001, 95002, 95003, 95004, 95005, 95006, 95007, 95008 };

//HitSpellClone
class HitSpellClone : public CreatureScript
{
public:
	HitSpellClone() : CreatureScript("HitSpellClone") { }

	struct HitSpellCloneAI : public ScriptedAI
	{
		HitSpellCloneAI(Creature* creature) : ScriptedAI(creature), Summons(me)
		{
			MoveTimer = 0;
			SpellTimer = 0;
			MoveInterval = 20 * IN_MILLISECONDS;
		}

		SummonList Summons;
		uint32 MoveTimer;
		uint32 SpellTimer;
		uint32 MoveInterval;
		std::unordered_map<uint32, int32>SpellMap;
		std::vector<const SpellInfo*> SpellVec;
		
		void Reset() override
		{
			if (MAX_MOUNT_SPELLS > 1)
			{
				for (size_t i = 0; i < MAX_MOUNT_SPELLS; i++)
					me->RemoveAura(MountSpells[i]);
			
				me->AddAura(MountSpells[urand(0, MAX_MOUNT_SPELLS - 1)], me);
			}

			SpellVec.clear();
			MoveSpline(MoveInterval);
			Summons.DespawnAll();
		}

		void SpellHit(Unit* caster, const SpellInfo* spell) override
		{
			if (!caster)
				return;

			std::unordered_map<uint32, int32>::iterator iter = SpellMap.find(spell->Id);
			if (iter != SpellMap.end())
				return;

			SpellMap.insert(std::make_pair(spell->Id, 0));

			if (std::find(SpellVec.begin(), SpellVec.end(), spell) != SpellVec.end())
				return;

			SpellVec.push_back(spell);
		}

		void SpellCooldown()
		{
			for (std::unordered_map<uint32, int32>::iterator iter = SpellMap.begin(); iter != SpellMap.end(); iter++)
				if (iter->second > 0)
					iter->second--;
		}

		void CastSpell(uint32 diff)
		{
			SpellTimer += diff;

			if (SpellTimer > 2 * IN_MILLISECONDS)
			{
				SpellTimer = 0;

				std::vector<uint32/*spellId*/> SpellVec;

				for (std::unordered_map<uint32, int32>::iterator iter = SpellMap.begin(); iter != SpellMap.end(); iter++)
					if (iter->second <= 0)
						SpellVec.push_back(iter->first);

				if (SpellVec.empty())
					return;

				uint32 spellId = SpellVec[urand(0, SpellVec.size() - 1)];

				DoCast(spellId);

				const SpellInfo* spellInfo = sSpellMgr->GetSpellInfo(spellId);

				std::unordered_map<uint32, int32>::iterator iter = SpellMap.find(spellId);

				if (iter != SpellMap.end())
					iter->second = spellInfo->GetRecoveryTime();
			}	
		}

		void MoveSpline(uint32 diff)
		{
			if (me->IsInCombat())
				return;

			MoveTimer += diff;

			if (MoveTimer >= MoveInterval)
			{
				MoveTimer = 0;

				if (WaypointPath const* i_path = sWaypointMgr->GetPath(me->GetWaypointPath()))
				{
					me->StopMoving();

					float x = me->GetPositionX();
					float y = me->GetPositionY();
					float z = me->GetPositionZ();

					Movement::PointsArray pathPoints;
					pathPoints.push_back(G3D::Vector3(x, y, z));

					WaypointData const* node;

					uint8 _i = 0;
					float _dist = 100000.0f;

					for (uint8 i = 0; i < i_path->size(); ++i)
					{
						node = i_path->at(i);

						float dist = sqrt((node->x - x) * (node->x - x) + (node->y - y) * (node->y - y) + (node->z - z) * (node->z - z));

						if (dist < _dist)
						{
							_dist = dist;
							_i = i;
						}
					}

					for (uint8 i = _i; i < i_path->size(); ++i)
					{
						node = i_path->at(i);
						pathPoints.push_back(G3D::Vector3(node->x, node->y, node->z));
					}

					for (uint8 i = 0; i < i_path->size(); ++i)
					{
						node = i_path->at(i);
						pathPoints.push_back(G3D::Vector3(node->x, node->y, node->z));
					}

					me->GetMotionMaster()->MoveSplinePath(&pathPoints);
				}
			}
		}

		bool StopOnCasting()
		{
			if (me->HasUnitState(UNIT_STATE_CASTING))
			{
				me->StopMoving();
				return true;
			}
			return false;
		}

		void UpdateAI(uint32 diff)
		{
			SpellCooldown();
			MoveSpline(diff);

			if (!UpdateVictim() || StopOnCasting())
				return;

			CastSpell(diff);
			DoMeleeAttackIfReady();
		}

		void AttackStart(Unit* who)
		{
			ScriptedAI::AttackStartCaster(who, 10.0f);
			ScriptedAI::AttackStart(who);
		}

		void EnterCombat(Unit* victim) 
		{
			
		}

		void MoveInLineOfSight(Unit* who)
		{
			if (who->IsHostileTo(me) && who->GetDistance(me) < 10.0f && !me->IsInCombat())
				AttackStart(who);
		}

		void JustSummoned(Creature* summon) override
		{
			Summons.Summon(summon);
		}

		void SummonedCreatureDies(Creature* summon, Unit* killer)  override
		{
			
		}
	};

	CreatureAI* GetAI(Creature* creature) const override
	{
		return new HitSpellCloneAI(creature);
	}
};

void AddSC_CustomEventScript()
{
	new EventPlayerScript();
	new StageTrigger();
	new HitSpellClone();
}

bool Event::CanRew(Player* pl)
{
	EventDataTemplate EventData = pl->GetEventData();

	if (EventData.Damage >= _DamageForRew ||
		EventData.Heal >= _HealForRew ||
		EventData.Kills >= _KillsForRew ||
		EventData.Killeds >= _KilledForRew)
		return true;

	return false;
}

std::unordered_map<uint32, PosTemplate> PosMap;

void Event::LoadPos()
{
	PosMap.clear();
	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		//			0	1	2		3	4		5
		"SELECT ��ͼID,X����,Y����,Z����,O����,����ID FROM _ģ��_����" :
		//		0	1 2 3 4	 5
		"SELECT map,x,y,z,o,posId FROM _position");
	if (!result) return;
	do
	{
		Field* fields = result->Fetch();
		PosTemplate Temp;
		Temp.map = fields[0].GetUInt32();
		Temp.x = fields[1].GetFloat();
		Temp.y = fields[2].GetFloat();
		Temp.z = fields[3].GetFloat();
		Temp.o = fields[4].GetFloat();
		uint32 ID = fields[5].GetUInt32();
		PosMap.insert(std::make_pair(ID, Temp));
	} while (result->NextRow());
}

void Event::Reset()
{
	for (auto itr = _HealerMap.begin(); itr != _HealerMap.end(); itr++)
		if (Creature* healer = _Map->GetCreature(itr->second))
			healer->RemoveFromWorld();
	_HealerMap.clear();

	_EventId = 0;
	_EventName = "";
	_EventType = C_ET_NONE;
	_GroupType = C_GT_RANDOM_PLAYER;
	_RejoinType = C_RT_REDIS_GROUP;
	_NoticeText = "";
	_GossipText = "";
	_ZoneId = 0;
	_Map = NULL;
	_Phase = 1;
	_RandomAH = urand(0, 100) > 50 ? false : true;
	_WinRewId = 0;
	_LosRewId = 0;
	_DamageForRew = 0;
	_HealForRew = 0;
	_KillsForRew = 0;
	_KilledForRew = 0;
	_GroupMap.clear();
	_FactionVec.clear();
	_PlayersMap.clear();
	_KillInfoMap.clear();
	_FactionNameMap.clear();
	_StartPosMap.clear();
	_StopPosMap.clear();
	_StopVec.clear();
	_AltVec.clear();
	_AltFactionVec.clear();
	_AltedVec.clear();
	_PhaseFactionVec.clear();
	_PhaseVec.clear();
	_UpdateSafeTimer = 0;
	_UpdateReviveTimer = 0;
	_SoulVisual = false;
	_DelayStartCall = false;
	_DelayStopCall = false;
	_UpdateDelayNotice = 0;
	_WinFaction = 0;
	_WorldStateVec.clear();
	_WorldStateFactionVec.clear();
	_SpawnVec.clear();
	_UpdateSpawnTimer = 0;
}

void Event::InitParams(uint32 eventId)
{
	if (eventId < 100)
		return;

	QueryResult result;

	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		//		0			1			2			3	4		5			6
		"SELECT �¼�����,������ʾ�ı�,�˵���ʾ�ı�,����ID,�¼�����,�������,���¼�������,"
		//		7			8			9					10				11				12
		"ʤ������ģ��ID,ʧ�ܽ���ģ��ID,��ý�����С�˺�ֵ,��ý�����С����ֵ,��ý�����С��ɱ��,��ý�����С��ɱ�� From _�¼� WHERE �¼�ID = '%u'" :
		//		0			1			2			3		4		5		6
		"SELECT EventName,NoticeText,GossipText,ZoneId,EventType,GroupType,RejoinType,"
		//7			8		9				10		11			12
		"WinRewId,LosRewId,DamageForRew,HealForRew,KillsForRew,KilledForRew From _event WHERE EventId = '%u'", eventId))
	{
		Reset();

		Field* fields = result->Fetch();
		_EventId = eventId;
		_EventName = fields[0].GetString();
		_NoticeText = fields[1].GetString();
		_GossipText = fields[2].GetString();
		
		const char* type = fields[4].GetCString();

		if (type && *type != '\0')
		{
			if (strcmp(type, "ս�� - ս��") == 0)
				_EventType = C_ET_WS;
			else if (strcmp(type, "ս�� - ����ϣ") == 0)
				_EventType = C_ET_AB;
			else if (strcmp(type, "ս�� - �籩֮��") == 0)
				_EventType = C_ET_EY;
			else if (strcmp(type, "ս�� - Զ�ź�̲") == 0)
				_EventType = C_ET_SA;
			else if (strcmp(type, "ս�� - ��ɽ") == 0)
				_EventType = C_ET_AV;
			else if (strcmp(type, "ս�� - ����֮��") == 0)
				_EventType = C_ET_IC;
			else if (strcmp(type, "������") == 0)
				_EventType = C_ET_TELE;
			else if (strcmp(type, "����") == 0)
				_EventType = C_ET_LINE;
			else if (strcmp(type, "�Զ�����Ӫ") == 0)
				_EventType = C_ET_FACTION;
			else if (strcmp(type, "����") == 0)
				_EventType = C_ET_GUILD;
			else if (strcmp(type, "����") == 0)
				_EventType = C_ET_SINGLE;
		}

		if (_EventType < C_ET_TELE)
			return;

		_ZoneId = fields[3].GetInt32();

		if (_ZoneId < 0)
			_Map = sMapMgr->FindMap(abs(_ZoneId), 0);
		else
		{
			int32 map = GetMapIdByZone(_ZoneId);
			if (map < 0)
				;
			else
				_Map = sMapMgr->FindMap(map, 0);
		}
			
		type = fields[5].GetCString();

		if (type && *type !='\0')
		{
			if (strcmp(type, "���� - ȫ����Ӫ1") == 0)
				_GroupType = C_GT_ONE_ALL;
			else if (strcmp(type, "���� - ������Ӫ1BL��Ӫ2") == 0)
				_GroupType = C_GT_ONE_A;
			else if (strcmp(type, "���� - ���˲������") == 0)
				_GroupType = C_GT_RANDOM_A_H;
			else if (strcmp(type, "���� - ������") == 0)
				_GroupType = C_GT_RANDOM_PLAYER;
		}
		
		type = fields[6].GetCString();

		if (type && *type != '\0')
		{
			if (strcmp(type, "���¼��� - ��ֹ") == 0)
				_RejoinType = C_RT_BANED;
			else if (strcmp(type, "���¼���- ֮ǰ����") == 0)
				_RejoinType = C_RT_KEEP_GROUP;
			else if (strcmp(type, "���¼��� - ϵͳ����") == 0)
				_RejoinType = C_RT_REDIS_GROUP;
		}
		
		_WinRewId		= fields[7].GetUInt32();
		_LosRewId		= fields[8].GetUInt32();
		_DamageForRew	= fields[9].GetUInt32();
		_HealForRew		= fields[10].GetUInt32();
		_KillsForRew	= fields[11].GetUInt32();
		_KilledForRew	= fields[12].GetUInt32();
	}
	else
		return;

	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		//		0		1		2					3
		"SELECT ����ID,��������,�¼���ʼ��������ID,�¼�������������ID From _�¼�_�������� WHERE �¼�ID = '%u'" :
		//		0		1		2					3
		"SELECT TeamId,TeamName,StartTelePosId,StopTelePosId From _event_team WHERE EventId = '%u'", eventId))
	{
		do
		{
			Field* fields = result->Fetch();
			EventFactionId faction		= fields[0].GetUInt32();
			_FactionVec.push_back(faction);
			_FactionNameMap[faction]	= fields[1].GetString();
			_StartPosMap[faction]		= fields[2].GetUInt32();
			_StopPosMap[faction]		= fields[3].GetUInt32();
		} while (result->NextRow());
	}

	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		//		0			1	2		3
		"SELECT ��������,����ID, ����,Ȩ��ֵ From _�¼�_������������ WHERE �¼�ID = '%u'" :
		//		0			1	 2		3
		"SELECT StopType,TeamId, Param,Weight From _event_stop WHERE EventId = '%u'", eventId))
	{
		do
		{
			Field* fields = result->Fetch();
			EventStopTemplate Temp;

			const char* type = fields[0].GetCString();
			if (strcmp(type, "��ɱ���� - guid") == 0)
				Temp.StopType = TYPEID_UNIT;
			else if (strcmp(type, "�ݻ����� - guid") == 0)
				Temp.StopType = TYPEID_GAMEOBJECT;
			else if (strcmp(type, "��ɱ��� - ����") == 0)
				Temp.StopType = TYPEID_PLAYER;

			Temp.Faction	= fields[1].GetUInt32();
			Temp.Param		= fields[2].GetUInt32();
			Temp.Weight		= fields[3].GetUInt32();
			_StopVec.push_back(Temp);
		} while (result->NextRow());
	}

	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		//			0				1				2
		"SELECT ���������GUID,��������弤���FLAG,����ʱ��ʾ�ı�,"
		//3						4		5					6		7						8	9						10	11						12
		"���ɱ�����ݻ�����ID1,����1,���ɱ�����ݻ�����ID2,����2,���ɱ�����ݻ�����ID3,����3,���ɱ�����ݻ�����ID4,����4,���ɱ�����ݻ�����ID5,����5 "
		"From _�¼�_��������弤���������� WHERE �¼�ID = '%u'" :		
		//		0		1		2
		"SELECT GUID,ActiveFlag,NoticeText,"
		//3				4			5			6				7			8			9				10			11			12
		"TargetEntry1,TargetCount1,TargetEntry2,TargetCount2,TargetEntry3,TargetCount3,TargetEntry4,TargetCount4,TargetEntry5,TargetCount5 "
		"From _event_active WHERE EventId = '%u'", eventId))
	{
		do
		{
			Field* fields = result->Fetch();
			EventAltTemplate Temp;
			Temp.GUID		= fields[0].GetInt64();
			Temp.AltFlag	= fields[1].GetUInt32();
			Temp.NoticeText = fields[2].GetString();
			for (size_t i = 0; i < ALT_GUID_MAX; i++)
			{
				Temp.TargetEntry[i] = fields[3 + i * 2].GetInt64();
				Temp.TargetCount[i] = fields[4 + i * 2].GetUInt32();
				Temp.TargetKillCount[i] = 0;
			}
				
			_AltVec.push_back(Temp);
		} while (result->NextRow());
	}

	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		//		0				1		2		3	
		"SELECT ���������GUID,����ID,��ɱ�����,��ұ�ɱ�� From _�¼�_��������弤�������������� WHERE �¼�ID = '%u'" :
		//		0		1	2		3
		"SELECT GUID,TeamId,Kills,Killeds From _event_active_team WHERE EventId = '%u'", eventId))
	{
		do
		{
			Field* fields = result->Fetch();
			EventAltFactionTemplate Temp;
			Temp.GUID			= fields[0].GetInt64();
			Temp.Faction		= fields[1].GetUInt32();
			Temp.Kills			= fields[2].GetUInt32();
			Temp.Killeds		= fields[3].GetUInt32();
			_AltFactionVec.push_back(Temp);
		} while (result->NextRow());
	}

	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		//		0		1									
		"SELECT �׶�ID,�׶ν������ɱ�����ݻ�����GUID From _�¼�_�׶�������������� WHERE �¼�ID = '%u'" :
		//		0		1
		"SELECT Phase,StopGUID From _event_phase WHERE EventId = '%u'", eventId))
	{
		do
		{
			Field* fields = result->Fetch();
			EventPhaseTemplate Temp;
			Temp.Phase		= fields[0].GetUInt8();
			Temp.StopGUID	= fields[1].GetInt64();
			_PhaseVec.push_back(Temp);
		} while (result->NextRow());
	}

	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		//			0	1		2			3			4				5					6						7					8					9
		"SELECT �׶�ID,����ID,Ĺ������ID, Ĺ�ر�������Χ, Ĺ�����ҽ��ID,�׶ν������ɱ�����,�׶ν�������ұ�ɱ��,�׶ν������崫������ID,����Ĺ�ر�������ʾ�ı�,�׶ν�����ʾ�ı� "
		"From _�¼�_�׶ζ������� WHERE �¼�ID = '%u'":
		//			0	1		2			3				4				5					6			7					8					9
		"SELECT Phase,TeamId,GraveyardPos, GraveyardSafe, GraveyardHealer,StopPlayerKills,StopPlayerKilleds,StopTelePos,GraveyardSafeNotice,StopNoticeText "
		"From _event_phase_team WHERE EventId = '%u'", eventId))
	{
		do
		{
			Field* fields = result->Fetch();
			EventPhaseFactionTemplate Temp;
			Temp.Phase					= fields[0].GetUInt32();
			Temp.Faction				= fields[1].GetUInt32();
			Temp.GraveyardPos			= fields[2].GetUInt32();
			Temp.GraveyardSafe			= fields[3].GetUInt32();
			Temp.GraveyardHealer		= fields[4].GetUInt32();
			Temp.StopPlayerKills		= fields[5].GetUInt32();
			Temp.StopPlayerKilleds		= fields[6].GetUInt32();
			Temp.StopTelePos			= fields[7].GetUInt32();
			Temp.GraveyardSafeNotice	= fields[8].GetString();
			Temp.StopNotice				= fields[9].GetString();
			_PhaseFactionVec.push_back(Temp);
		} while (result->NextRow());

		SummonSoulHealer();
	}

	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		//		0				1			2									3					4									5					6									7					8				9
		"SELECT ����״̬ID,����״̬��ʼֵ,��������״̬��Ҫ��ɱ�����ݻ�����ID1,��������״̬����ֵ1,��������״̬��Ҫ��ɱ�����ݻ�����ID2,��������״̬����ֵ2,��������״̬��Ҫ��ɱ�����ݻ�����ID3,��������״̬����ֵ3,����״̬�ﵽ��ֵʱ��ʾ,��ʾ�ı� From _�¼�_����״̬���� WHERE �¼�ID = '%u'" :
		//		0		1		2		3	4		5		6	7		8			9
		"SELECT ID,StartValue,Entry1,Step1,Entry2,Step2,Entry3,Step3,NoticeValue,NoticeText From _event_worldstate WHERE EventId = '%u'", eventId))
	{
		do
		{
			Field* fields = result->Fetch();
			EventWorldStateTemplate Temp;
			Temp.ID				= fields[0].GetUInt32();
			Temp.InitValue		= fields[1].GetInt32();
			for (size_t i = 0; i < WORLDSTATE_ENTRY_MAX; i++)
			{
				Temp.Entry[i]	= fields[2 + i * 2].GetInt32();
				Temp.Step[i]	= fields[3 + i * 2].GetInt32();
			}
			Temp.NoticeValue	= fields[8].GetInt32();
			Temp.Notice			= fields[9].GetString();
			_WorldStateVec.push_back(Temp);
		} while (result->NextRow());
	}

	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		//		0			1		2			3					4					5
		"SELECT ����״̬ID,����ID,��ɱ��ɱ,��������״̬����ֵ,����״̬�ﵽ��ֵʱ��ʾ,��ʾ�ı� From _�¼�_����״̬�������� WHERE �¼�ID  = '%u'" :
		//		0	1		2			3		4			5
		"SELECT ID,TeamId,KillOrKilled,Step,NoticeValue,NoticeText From _event_worldstate_team WHERE EventId = '%u'", eventId))
	{
		do
		{
			Field* fields = result->Fetch();
			EventWorldStateFactionTemplate Temp;
			Temp.ID				= fields[0].GetUInt32();
			Temp.Faction		= fields[1].GetUInt32();
			Temp.Kill			= fields[2].GetBool();
			Temp.Step			= fields[3].GetInt32();
			Temp.NoticeValue	= fields[4].GetInt32();
			Temp.Notice			= fields[5].GetString();
			_WorldStateFactionVec.push_back(Temp);
		} while (result->NextRow());
	}

	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		//					0		1		2		3			4
		"SELECT ˢ�����������ID,�׶�ID,ˢ��ʱ��,ˢ��λ������ID,����ʱ��,"
		//5				6		7		8		9		10
		"��ʾʱ��1,��ʾ�ı�1,��ʾʱ��2,��ʾ�ı�2,��ʾʱ��3,��ʾ�ı�3 From _�¼�_���������ˢ������ WHERE �¼�ID = '%u'":
		//		0		1		2		3		4
		"SELECT Entry,Phase,SpawnTime,PosId,Duration,"
		//5				6			7			8				9		10
		"NoticeTime1,SpawnNotice1,NoticeTime2,SpawnNotice2,NoticeTime3,SpawnNotice3 From _event_spawn WHERE EventId = '%u'", eventId))
	{
		do
		{
			Field* fields = result->Fetch();
			EventSpawnTemplate Temp;
			Temp.Entry		= fields[0].GetInt64();
			Temp.Phase		= fields[1].GetUInt32();
			Temp.SpawnTime	= fields[2].GetUInt32();
			Temp.PosId		= fields[3].GetUInt32();
			Temp.Duration	= fields[4].GetUInt32();

			for (size_t i = 0; i < MAX_SPAWNNOTICE_COUNT; i++)
			{
				Temp.NoticeTime[i] = fields[5 + i * 2].GetInt32();
				Temp.SpawnNotice[i] = fields[6 + i * 2].GetString();
			}
			
			_SpawnVec.push_back(Temp);
		} while (result->NextRow());
	}
}

bool Event::AcceptInvitation(Player* player, uint32 menuId)
{
	if (menuId < 1000000)
		return false;

	uint32 eventId = menuId - 1000000;

	if (!sGameEventMgr->IsActiveEvent(eventId))
	{
		player->GetSession()->SendNotification("�û�Ѿ�������");
		return true;
	}

	if (eventId == ARENA_DUEL_SIGNUP_EVENT_ID)
	{
		if (sArenaDuel->GetState() == ARENA_DUEL_STATE_SIGNUP)
		{
			auto itr = ArenaDuelMap.find(player->GetGUIDLow());
			if (itr == ArenaDuelMap.end())
				sArenaDuel->Signup(player);
			player->GetSession()->SendAreaTriggerMessage("�����ɹ�����̨��������ʼ���뱣�����ߣ�");
		}else
			player->GetSession()->SendNotification("�����Ѿ���ֹ��");
		return true;
	}
		
	if (eventId == UKBG_EVENT_ID)
	{
		sUkBG->AddPlayer(player);
		return true;
	}

	if (eventId == BGIC_EVENT_ID)
	{
		sBGIC->AddPlayer(player);
		return true;
	}

	switch (_EventType)
	{
	case C_ET_NONE:
		break;
	case C_ET_WS:
		player->GetSession()->FixedBGJoin(BATTLEGROUND_WS);
		break;
	case C_ET_AB:
		player->GetSession()->FixedBGJoin(BATTLEGROUND_AB);
		break;
	case C_ET_EY:
		player->GetSession()->FixedBGJoin(BATTLEGROUND_EY);
		break;
	case C_ET_SA:
		player->GetSession()->FixedBGJoin(BATTLEGROUND_SA);
		break;
	case C_ET_AV:
		player->GetSession()->FixedBGJoin(BATTLEGROUND_AV);
		break;
	case C_ET_IC:
		player->GetSession()->FixedBGJoin(BATTLEGROUND_IC);
		break;
	case C_ET_TELE:
		Tele(player, _StartPosMap[player->GetTeamId()]);
	break;
	case C_ET_LINE:
	case C_ET_FACTION:
	case C_ET_GUILD:
		Join(player);
		break;
	}

	return true;
}

void Event::AddEventList(Player* player, Object* obj, bool isBG)
{
	player->PlayerTalkClass->ClearMenus();

	GameEventMgr::GameEventDataMap const& events = sGameEventMgr->GetEventMap();
	GameEventMgr::ActiveEvents const& activeEventsList = sGameEventMgr->GetActiveEventList();

	for (auto itr = events.begin(); itr != events.end(); itr++)
	{
		if (itr->eventId <= 70)
			continue;

		if (sGameEventMgr->IsActiveEvent(itr->eventId))
		{
			std::ostringstream oss;
			oss << itr->description;
			oss << "\n|cFF33CC00[�Ѽ��������� - ";
			oss << SecTimeString(sGameEventMgr->NextCheck(itr->eventId), true);
			oss << "]|r";
			player->ADD_GOSSIP_ITEM(0, oss.str(), SENDER_CUSTOM_EVENT_ACTIVE, itr->eventId);
		}
	}

	for (auto itr = events.begin(); itr != events.end(); itr++)
	{
		if (itr->eventId <= 70)
			continue;

		if (!sGameEventMgr->IsActiveEvent(itr->eventId))
		{
			std::ostringstream oss;
			oss << itr->description;
			oss << "\n|cFF666666[δ������뿪ʼ - ";
			oss << SecTimeString(sGameEventMgr->NextCheck(itr->eventId), true);
			oss << "]|r";
			player->ADD_GOSSIP_ITEM(0, oss.str(), SENDER_CUSTOM_EVENT_DEACTIVE, itr->eventId);
		}
	}

	player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, obj->GetGUID());
}
void Event::GetTelePos(uint32 posId, uint32 &map, float &x, float &y, float &z, float &o)
{
	std::unordered_map<uint32, PosTemplate>::iterator it = PosMap.find(posId);

	if (it != PosMap.end())
	{
		map = it->second.map;
		x = it->second.x;
		y = it->second.y;
		z = it->second.z;
		o = it->second.o;
	}
}

//Unk1 = 1ʱ �����Ӫ�Ƿ�һ�� һ����ͨ�� �����ͷż���
bool Event::GobUnk1Check(Unit* target, GameObject *const gob)
{
	if (!target)
		return true;

    
	GameObjectTemplate const* temp = sObjectMgr->GetGameObjectTemplate(gob->GetEntry());

    GameObjectTemplateAddon const* addon = sObjectMgr->GetGameObjectTemplateAddon(gob->GetEntry());
	if (std::strcmp(temp->unk1.c_str(), "1") != 0)
		return true;

	if (addon->faction != target->getFaction())
		return false;

	return true;
}

bool Event::IpCheck(Player* player)
{
	uint32 count = 0;

	for (auto itr = _PlayersMap.begin(); itr != _PlayersMap.end(); itr++)
		if (Player* pl = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(itr->first, 0, HIGHGUID_PLAYER)))
			if (strcmp(pl->GetSession()->GetRemoteAddress().c_str(), player->GetSession()->GetRemoteAddress().c_str()) == 0)
				count++;


	uint32 max = sSwitch->GetValue(ST_EVENT_PLAYERS);

	if (count > max)
	{
		std::ostringstream oss;
		oss << "��������������Ѵﵽ���ޣ�����ʧ�ܣ�" << count << "/" << max;
		player->GetSession()->SendNotification(oss.str().c_str());
		return false;
	}
	else
		return true;
}

//ս��
bool Event::IsFixedTimeBgActive(BattlegroundTypeId bgTypeId)
{
	EventTypes eventType = C_ET_NONE;

	switch (bgTypeId)
	{
	case BATTLEGROUND_AV:
		eventType = C_ET_AV;
		break;
	case BATTLEGROUND_WS:
		eventType = C_ET_WS;
		break;
	case BATTLEGROUND_AB:
		eventType = C_ET_AB;
		break;
	case BATTLEGROUND_EY:
		eventType = C_ET_EY;
		break;
	case BATTLEGROUND_SA:
		eventType = C_ET_SA;
		break;
	case BATTLEGROUND_IC:
		eventType = C_ET_IC;
		break;
	}

	if (eventType == _EventType)
		return true;

	return false;
}


void Event::Start(uint32 eventId)
{
	if (eventId == EVENT_ID_QUICK_RESPONSE)
	{
		sQuickResponse->Anounce();
		return;
	}
	
	//�ڴ������������� �л�������¼�����
	
	if (eventId == EVENT_ID_FAKE_PLAYER_LOGIN_OR_LOGOUT)
	{
		sFakePlayers->LoginOrLogout();
		return;
	}
	
	if (eventId == EVENT_ID_FAKE_PLAYER_UPDATE_ZONE_ID)
	{
		sFakePlayers->UpdateZone();
		return;
	}
	
	if (eventId == EVENT_ID_FAKE_PLAYER_LEVEL_UP)
	{
		sFakePlayers->LevelUp();
		return;
	}

	if (eventId < 100)
		return;

	InitParams(eventId);

	//�Լ���ʼ��
	sBGIC->InitParams(eventId, _Map);

	if (_EventType == C_ET_NONE)
		return;

	std::ostringstream oss;
	oss << _EventName << "��ʼ��";
	sWorld->SendServerMessage(SERVER_MSG_STRING, oss.str().c_str());

	if (eventId == ARENA_DUEL_START_EVENT_ID)
	{
		sArenaDuel->SetState(ARENA_DUEL_STATE_START);
		return;
	}

	ReviveAll(true);

	if (_EventType < C_ET_LINE)
	{
		StartCall();
		return;
	}
		
	_DelayStartCall = true;

	if (_EventId == BGIC_EVENT_ID)
		return;

	InitWorldState();

	oss.str("");
	oss << _EventName;

	switch (_RejoinType)
	{
	case C_RT_BANED:
		oss << "���¼���������ֹ��";
		break;
	case C_RT_KEEP_GROUP:
		oss << "���¼�������ı���飡";
		break;
	case C_RT_REDIS_GROUP:
		oss << "���¼���ʱ��ϵͳ�����·��飡";
		break;
	}

	sWorld->SendServerMessage(SERVER_MSG_STRING, oss.str().c_str());
}

int32 Event::GetWeightLeft(EventFactionId faction)
{
	bool exist = false;

	uint32 Weight = 0;

	for (auto itr = _StopVec.begin(); itr != _StopVec.end(); itr++)
	{
		if (faction != itr->Faction)
			continue;
		
		exist = true;

		if (itr->StopType == TYPEID_PLAYER)
			Weight += itr->Param * itr->Weight;
		else if (itr->Param != 0)
			Weight += itr->Weight;
	}

	if (!exist)
		return 0;

	if (Weight == 0)
		return -1;

	return Weight;
}

void Event::Stop(uint32 eventId)
{
	//����
	if (eventId == EVENT_ID_QUICK_RESPONSE)
	{
		sQuickResponse->Reward();
		return;
	}

	if (eventId != 0 && _EventId != eventId)
		return;

	if (_EventType >= C_ET_WS && _EventType <= C_ET_IC)
	{
		Reset();
		return;
	}

	if (!Valid())
		return;

	if (_EventId == BGIC_EVENT_ID)
	{
		sBGIC->Stop();
		return;
	}

	int32 w = 100000;

	for (auto itr = _FactionVec.begin(); itr != _FactionVec.end(); itr++)
	{
		int32 weight = GetWeightLeft(*itr);

		if (weight < w)
		{
			w = weight;
			_WinFaction = *itr;
		}
	}

	auto itr = _FactionNameMap.find(_WinFaction);
	if (itr != _FactionNameMap.end())
		SendScreenMessage(_EventName + itr->second + "��ʤ��");

	ReviveAll(false);
	_DelayStopCall = true;
}

void Event::UpdateActive(Object* obj,bool kill)
{
	if (_AltVec.empty())
		return;
	
	uint32 targetEntry = obj->GetEntry();
	TypeID typeId = obj->GetTypeId();

	switch (typeId)
	{
	case TYPEID_UNIT:
		for (auto itr = _AltVec.begin(); itr != _AltVec.end(); itr++)
			for (size_t i = 0; i < ALT_GUID_MAX; i++)
				if (targetEntry == itr->TargetEntry[i] && itr->TargetCount[i] != itr->TargetKillCount[i])
				{
					itr->TargetKillCount[i]++;
					SendScreenMessage("��ɱ[" + sObjectMgr->GetCreatureTemplate(obj->GetEntry())->Name + "] " + std::to_string(itr->TargetKillCount[i]) + " / " + std::to_string(itr->TargetCount[i]));
				}
					
		break;
	case TYPEID_GAMEOBJECT:
		for (auto itr = _AltVec.begin(); itr != _AltVec.end(); itr++)
			for (size_t i = 0; i < ALT_GUID_MAX; i++)
				if (targetEntry == -itr->TargetEntry[i] && itr->TargetCount[i] != itr->TargetKillCount[i])
				{
					itr->TargetKillCount[i]++;
					SendScreenMessage("�ݻ�[" + sObjectMgr->GetGameObjectTemplate(obj->GetEntry())->name + "] " + std::to_string(itr->TargetKillCount[i]) + "/" + std::to_string(itr->TargetCount[i]));
				}
		break;
	case TYPEID_PLAYER:
		for (auto itr = _AltFactionVec.begin(); itr != _AltFactionVec.end(); itr++)
		{
			if (itr->Faction == obj->ToPlayer()->GetEventFaction())
			{
				if (itr->Kills > 0 && kill)
					itr->Kills--;
				if (itr->Killeds > 0 && !kill)
					itr->Killeds--;
			}
		}
		break;
	}

	for (auto itr = _AltVec.begin(); itr != _AltVec.end(); itr++)
	{
		int64 GUID = itr->GUID;

		if (find(_AltedVec.begin(), _AltedVec.end(), GUID) != _AltedVec.end())
			continue;

		bool check1 = true;

		for (size_t i = 0; i < ALT_GUID_MAX; i++)
			if (itr->TargetCount[i] != itr->TargetKillCount[i])
				check1 = false;

		bool check2 = true;

		if (check1)
			for (auto i = _AltFactionVec.begin(); i != _AltFactionVec.end(); i++)
				if (i->GUID == GUID)
					if (i->Kills != 0 || i->Killeds != 0)
						check2 = false;

		if (check1 && check2)
		{
			_AltedVec.push_back(GUID);

			if (GUID > 0)
			{
				if (CreatureData const* data = sObjectMgr->GetCreatureData(GUID))
					if (Creature* c = ObjectAccessor::GetObjectInWorld(MAKE_NEW_GUID(GUID, data->id, HIGHGUID_UNIT), (Creature*)NULL))
					{
						c->SetUInt32Value(UNIT_FIELD_FLAGS, itr->AltFlag);
						SendScreenMessage(itr->NoticeText);
					}
			}
			else
			{
				if (GameObjectData const* data = sObjectMgr->GetGOData(-GUID))
					if (GameObject* g = ObjectAccessor::GetObjectInWorld(MAKE_NEW_GUID(-GUID, data->id, HIGHGUID_GAMEOBJECT), (GameObject*)NULL))
					{
						g->SetUInt32Value(GAMEOBJECT_FLAGS, itr->AltFlag);
						SendScreenMessage(itr->NoticeText);
					}			
			}
		}
	}
}

float Event::GetDistance(Player* player, uint32 pos)
{
	std::unordered_map<uint32, PosTemplate>::iterator it = PosMap.find(pos);
	
	if (it != PosMap.end())
		if (it->second.map == player->GetMapId())
			return player->GetDistance(it->second.x, it->second.y, it->second.z);
	
	return 1000.0f;
}

void Event::Tele(Player* player, uint32 pos)
{
	std::unordered_map<uint32, PosTemplate>::iterator it = PosMap.find(pos);
	
	if (it != PosMap.end())
		player->TeleportTo(it->second.map, it->second.x, it->second.y, it->second.z, it->second.o);
}

void Event::TeleToGraveyard(Player* player)
{
	Tele(player, GetPhaseGraveyardPos(player->GetEventFaction()));
}

typedef std::pair<std::string, uint32> PAIR;

int DoSort(const std::pair<std::string, uint32>& x, const std::pair<std::string, uint32>& y)
{
	return x.second > y.second;
}

void SortMapByValue(std::unordered_map<std::string, uint32>& tMap, std::vector<std::pair<std::string, uint32> >& tVector)
{
	for (std::unordered_map<std::string, uint32>::iterator curr = tMap.begin(); curr != tMap.end(); curr++)
		tVector.push_back(std::make_pair(curr->first, curr->second));

	sort(tVector.begin(), tVector.end(), DoSort);
}

void Event::RewAndRest()
{
	std::unordered_map<std::string, uint32 >KillsMap;
	std::unordered_map<std::string, uint32 >KilledsMap;
	std::unordered_map<std::string, uint32 >DamageMap;
	std::unordered_map<std::string, uint32 >HealMap;

	for (auto itr = _PlayersMap.begin(); itr != _PlayersMap.end(); itr++)
	{
		if (Player* pl = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(itr->first, 0, HIGHGUID_PLAYER)))
		{
			if (!pl->InEvent())
				continue;

			std::string name = sCF->GetNameLink(pl);
			KillsMap[name] = pl->GetEventData().Kills;
			KilledsMap[name] = pl->GetEventData().Killeds;
			DamageMap[name] = pl->GetEventData().Damage;
			HealMap[name] = pl->GetEventData().Heal;
		}
	}

	std::ostringstream oss;

	std::vector<std::pair<std::string, uint32>> tVector;
	uint32 count;

	oss << "|cffFF0000��ɱ����|r\n";
	count = 0;
	tVector.clear();
	SortMapByValue(KillsMap, tVector);

	for (uint32 i = 0; i < tVector.size(); i++)
	{
		if (count > 3)
			continue;
		count++;
		oss << "|cFFF49B00" << tVector[i].first << "|r - " << tVector[i].second << "\n";
	}

	//oss << "\n\n";
	//oss << "��ɱ����\n";
	//count = 0;
	//tVector.clear();
	//SortMapByValue(KilledsMap, tVector);
	//
	//for (uint32 i = 0; i < tVector.size(); i++)
	//{
	//	if (count > 3)
	//		continue;
	//	count++;
	//	oss << "|cFFF49B00" << tVector[i].first << "|r - " << tVector[i].second << "\n";
	//}

	oss << "\n\n";
	oss << "|cffFF0000�˺�����|r\n";
	count = 0;
	tVector.clear();
	SortMapByValue(DamageMap, tVector);

	for (uint32 i = 0; i < tVector.size(); i++)
	{
		if (count > 3)
			continue;
		count++;
		oss << "|cFFF49B00" << tVector[i].first << "|r - " << tVector[i].second << "\n";
	}

	oss << "\n\n";
	oss << "|cffFF0000��������|r\n";
	count = 0;
	tVector.clear();
	SortMapByValue(HealMap, tVector);

	for (uint32 i = 0; i < tVector.size(); i++)
	{
		if (count > 3)
			continue;
		count++;
		oss << "|cFFF49B00" << tVector[i].first << "|r - " << tVector[i].second << "\n";
	}

	oss <<"\n\n";
	for (auto itr = _PlayersMap.begin(); itr != _PlayersMap.end(); itr++)
	{
		if (Player* pl = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(itr->first, 0, HIGHGUID_PLAYER)))
		{
			if (!pl->InEvent())
				continue;

			std::ostringstream oss1;
			oss1 << "|cffFF0000��ĸ���ս��|r" << "\n";
			oss1 << "|cFFF49B00��ɱ|r - " << pl->GetEventData().Kills << "\n";
			oss1 << "|cFFF49B00��ɱ|r - " << pl->GetEventData().Killeds << "\n";
			oss1 << "|cFFF49B00�˺�|r - " << pl->GetEventData().Damage << "\n";
			oss1 << "|cFFF49B00����|r - " << pl->GetEventData().Heal << "\n";

			sCF->SendAcceptOrCancel(pl, 0, oss.str() + oss1.str());

			if (CanRew(pl))
			{
				if (pl->GetEventFaction() == _WinFaction)
					sRew->Rew(pl, _WinRewId);
				else
					sRew->Rew(pl, _LosRewId);
			}

			auto i = _StopPosMap.find(pl->GetEventFaction());

			if (i != _StopPosMap.end())
				Tele(pl, i->second);

			pl->EventRest();
		}
	}

	Reset();
}

void Event::Update(Map* map, uint32 diff)
{
	if (_EventType < C_ET_LINE && !map || !_Map || map->GetId() != _Map->GetId())
		return;

	if (_EventId == BGIC_EVENT_ID)
		sBGIC->UpdateSpawn(diff);

	//�ӳ��ٻ�
	if (_DelayStartCall)
	{
		_UpdateDelayNotice += diff;

		if (_UpdateDelayNotice > 500)
		{
			_UpdateDelayNotice = 0;
			_DelayStartCall = false;
			StartCall();
		}
	}

	if (_EventId == BGIC_EVENT_ID)
		return;

	//�ӳٽ���������
	if (_DelayStopCall)
	{
		_UpdateDelayNotice += diff;

		if (_UpdateDelayNotice > 500)
		{
			_UpdateDelayNotice = 0;
			_DelayStopCall = false;
			RewAndRest();
		}
	}

	UpdateRevive(diff);
	UpdateSafe(diff);
	UpdateSpawn(diff);
}

void Event::UpdateSpawn(uint32 diff)
{
	_UpdateSpawnTimer += diff;

	if (_UpdateSpawnTimer < IN_MILLISECONDS)
		return;

	_UpdateSpawnTimer = 0;

	for (auto itr = _SpawnVec.begin(); itr != _SpawnVec.end(); itr++)
	{
		if (itr->SpawnTime == 0 || (itr->Phase != 0 && itr->Phase != _Phase))
			continue;

		itr->SpawnTime -= 1;

		for (size_t i = 0; i < MAX_SPAWNNOTICE_COUNT; i++)
			if (itr->SpawnTime == itr->NoticeTime[i])
				SendScreenMessage(itr->SpawnNotice[i]);

		if (itr->SpawnTime == 0)
		{
			uint32 map = 0;
			float x = 0;
			float y = 0;
			float z = 0;
			float o = 0;

			GetTelePos(itr->PosId, map, x, y, z, o);

			Position pos;
			pos.Relocate(x, y, z, o);

			if (itr->Entry > 0)
				_Map->SummonCreature(itr->Entry, pos, 0, itr->Duration * IN_MILLISECONDS);
			else
				_Map->SummonGameObject(-itr->Entry, x, y, z, o, 0, 0, 0, 0, itr->Duration);
		}
	}
}

void Event::UpdateRevive(uint32 diff)
{
	_UpdateReviveTimer += diff;

	/*if (_UpdateReviveTimer > 29500 && !_SoulVisual)
	{
		for (auto itr = _PlayersMap.begin(); itr != _PlayersMap.end(); itr++)
		{
			if (Player* pl = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(itr->first, 0, HIGHGUID_PLAYER)))
			{
				if (!pl->InEvent())
					continue;

				if (pl->IsAlive())
					continue;

				//if (GetDistance(pl, GetPhaseGraveyardPos(pl->GetEventFaction())) < 20.0f)
					//pl->CastSpell(pl, 24171, true);
				auto itr = _HealerMap.find(pl->GetEventFaction());

				if (itr != _HealerMap.end())
				{
					if (Creature* healer = _Map->GetCreature(itr->second))
					{
						if (healer->GetDistance(pl) < 20.0f)
							healer->CastSpell(pl, 24171, true);
					}
				}
			}
		}

		_SoulVisual = true;

	}else */if (_UpdateReviveTimer > 30000)
	{
		_UpdateReviveTimer = 0;
		_SoulVisual = false;

		for (auto itr = _PlayersMap.begin(); itr != _PlayersMap.end(); itr++)
		{
			if (Player* pl = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(itr->first, 0, HIGHGUID_PLAYER)))
			{
				if (!pl->InEvent())
					continue;

				if (pl->IsAlive())
					continue;

				if (GetDistance(pl, GetPhaseGraveyardPos(pl->GetEventFaction())) < 20.0f)
				{
					pl->GetSession()->SendAreaTriggerMessage("%s�ó��������������ս����", _EventName.c_str());
					pl->CastSpell(pl, 24171, true);
					pl->ResurrectPlayer(1.0f);
					pl->CastSpell(pl, 6962, true);
					pl->CastSpell(pl, SPELL_SPIRIT_HEAL_MANA, true);
					sObjectAccessor->ConvertCorpseForPlayer(pl->GetGUID());
				}
			}
		}
	}
}

void Event::UpdateSafe(uint32 diff)
{
	_UpdateSafeTimer += diff;

	if (_UpdateSafeTimer > 1000)
	{
		_UpdateSafeTimer = 0;

		for (auto itr = _PlayersMap.begin(); itr != _PlayersMap.end(); itr++)
		{
			if (Player* pl = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(itr->first, 0, HIGHGUID_PLAYER)))
			{
				if (pl->InEvent() && pl->IsAlive())
				{
					for (auto i = _PhaseFactionVec.begin(); i != _PhaseFactionVec.end(); i++)
					{
						if (i->Phase == _Phase)
						{
							if (i->Faction != pl->GetEventFaction() && GetDistance(pl, i->GraveyardPos) < i->GraveyardSafe)
							{
								TeleToGraveyard(pl);
								pl->GetSession()->SendNotification(i->GraveyardSafeNotice.c_str());
								break;
							}
						}
					}
				}	
			}
		}
	}
}

uint32 Event::GetFactionMembers(EventFactionId faction)
{
	uint32 count = 0;

	for (auto iter = _PlayersMap.begin(); iter != _PlayersMap.end(); iter++)
		if (faction == iter->second)
			count++;

	return count;
}

void Event::ReviveAll(bool start)
{
	SessionMap const& smap = sWorld->GetAllSessions();
	for (SessionMap::const_iterator iter = smap.begin(); iter != smap.end(); ++iter)
		if (Player* player = iter->second->GetPlayer())
		{
			if (player->IsAlive())
				continue;

			if (start || !start && player->InEvent())
			{
				player->ResurrectPlayer(1.0f);
				player->SpawnCorpseBones();
			}		
		}
}

void Event::StartCall()
{
	if (_NoticeText.empty())
		return;

	uint32 menuId = _EventId + 1000000;

	SessionMap const& smap = sWorld->GetAllSessions();
	for (SessionMap::const_iterator iter = smap.begin(); iter != smap.end(); ++iter)
		if (Player* player = iter->second->GetPlayer())
		{
			WorldPacket data(SMSG_GOSSIP_MESSAGE, 100);
			data << uint64(player->GetGUID());
			data << uint32(menuId);
			data << uint32(1);
			data << uint32(1);
			data << uint32(1);
			data << uint8(1);
			data << uint8(0);
			data << uint32(0);
			data << "";
			data << _NoticeText;
			player->GetSession()->SendPacket(&data);
		}
}

void Event::Join(Player* pl)
{
	if (!Valid())
	{
		ChatHandler(pl->GetSession()).PSendSysMessage("����ʧ�ܣ���Ѿ�������");
		return;
	}

	//����IP����
	if (!IpCheck(pl))
		return;

	//�Ѿ����¼���

	if (pl->InEvent())
	{
		ChatHandler(pl->GetSession()).PSendSysMessage("%s����ʧ�ܣ����Ѿ�������", _EventName.c_str());
		return;
	}
#pragma region ����

	EventFactionId faction = Subgroup(pl);

	if (faction == 0)
	{
		ChatHandler(pl->GetSession()).PSendSysMessage("%s����ʧ�ܣ��޷����¼��룡", _EventName.c_str());
		return;
	}

	if (pl->isDead())
	{
		pl->ResurrectPlayer(1.0f);
		pl->SpawnCorpseBones();
	}

	pl->UpdateMaxHealth();
	pl->UninviteFromGroup();
	if (pl->GetGroup())
		pl->RemoveFromGroup();

	pl->InitEventData(_EventId, faction, _FactionNameMap[faction]);
	_PlayersMap[pl->GetGUIDLow()] = faction;

	bool groupExist = false;

	for (auto iter = _GroupMap.begin(); iter != _GroupMap.end(); iter++)
	{
		if (Group* group = sGroupMgr->GetGroupByGUID(iter->first))
		{
			if (!group->IsFull() && iter->second == faction)
			{
				group->AddMember(pl);
				groupExist = true;
				break;
			}
		}
	}

	if (!groupExist)
	{
		Group* group = new Group;
		group->Create(pl);
		group->ConvertToRaid();
		sGroupMgr->AddGroup(group);
		_GroupMap[group->GetLowGUID()] = faction;
	}

#pragma endregion

	auto itr = _StartPosMap.find(faction);

	if (itr != _StartPosMap.end())
		Tele(pl, itr->second);
}

EventFactionId Event::Subgroup(Player* pl)
{
	auto iter = _PlayersMap.find(pl->GetGUIDLow());

	if (iter != _PlayersMap.end())
	{
		if (_RejoinType == C_RT_BANED)
			return 0;
		else if (_RejoinType == C_RT_KEEP_GROUP)
			return iter->second;
	}

	if (_EventType == C_ET_LINE)
	{
		switch (_GroupType)
		{
		case C_GT_ONE_ALL:
			return 1;
		case C_GT_ONE_A:
			if (pl->GetTeamId() == TEAM_ALLIANCE)
				return 1;
			else
				return 2;
		case C_GT_ONE_H:
			if (pl->GetTeamId() == TEAM_ALLIANCE)
				return 2;
			else
				return 1;
		case C_GT_RANDOM_A_H:
			if (_RandomAH)
			{
				if (pl->GetTeamId() == TEAM_ALLIANCE)
					return 1;
				else
					return 2;
			}
			else
			{
				if (pl->GetTeamId() == TEAM_ALLIANCE)
					return 2;
				else
					return 1;
			}
			break;
		case C_GT_RANDOM_PLAYER:
		{
			if (GetFactionMembers(1) > GetFactionMembers(2))
				return 2;
			else
				return 1;
		}
			break;
		default:
			break;
		}
	}
	else if (_EventType == C_ET_FACTION)
		return pl->faction;
	else if (_EventType == C_ET_GUILD)
		return pl->GetGuildId();

	return 0;
}

void Event::UpdatePhase(Object* obj, bool kill)
{
	bool nextphaseexsit = false;

	for (auto itr = _PhaseFactionVec.begin(); itr != _PhaseFactionVec.end(); itr++)
	{
		if (itr->Phase == (_Phase + 1))
		{
			nextphaseexsit = true;
			break;
		}	
	}
	
	if (!nextphaseexsit)
		return;


	uint32 guid = obj->GetGUIDLow();
	TypeID typeId = obj->GetTypeId();


	switch (typeId)
	{
	case TYPEID_UNIT:
	{
		for (auto itr = _PhaseVec.begin(); itr != _PhaseVec.end(); itr++)
		{
			if (itr->Phase == _Phase)
			{
				if (itr->StopGUID == guid && itr->StopGUID > 0)
					itr->StopGUID = 0;
			}
		}
	}
		break;
	case TYPEID_PLAYER:
	{
		for (auto itr = _PhaseFactionVec.begin(); itr != _PhaseFactionVec.end(); itr++)
		{
			if (itr->Phase == _Phase && obj->ToPlayer()->GetEventFaction() == itr->Faction)
			{
				if (itr->StopPlayerKills > 0 && kill)
					itr->StopPlayerKills--;

				if (itr->StopPlayerKilleds > 0 && !kill)
					itr->StopPlayerKilleds--;
			}
		}
	}
		break;
	case TYPEID_GAMEOBJECT:
	{
		for (auto itr = _PhaseVec.begin(); itr != _PhaseVec.end(); itr++)
		{
			if (itr->Phase == _Phase)
			{
				if (-itr->StopGUID == guid && itr->StopGUID < 0)
					itr->StopGUID = 0;
			}
		}
	}
		break;
	}

	for (auto itr = _PhaseVec.begin(); itr != _PhaseVec.end(); itr++)
		if (itr->Phase == _Phase)
			if (itr->StopGUID != 0)
				return;

	for (auto itr = _PhaseFactionVec.begin(); itr != _PhaseFactionVec.end(); itr++)
		if (itr->Phase == _Phase && itr->StopPlayerKilleds == 0 && itr->StopPlayerKills == 0)
		{
			for (auto itr = _PlayersMap.begin(); itr != _PlayersMap.end(); itr++)
			{
				if (Player* pl = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(itr->first, 0, HIGHGUID_PLAYER)))
				{
					if (!pl->InEvent())
						continue;

					for (auto i = _PhaseFactionVec.begin(); i != _PhaseFactionVec.end(); i++)
						if (_Phase == i->Phase && pl->GetEventFaction() == i->Faction)
						{
							if (!i->StopNotice.empty())
							{
								WorldPacket data;
								ChatHandler::BuildChatPacket(data, CHAT_MSG_RAID_BOSS_EMOTE, LANG_UNIVERSAL,
									NULL, NULL, i->StopNotice);

								if (WorldSession* ss = pl->GetSession())
									ss->SendPacket(&data);
							}
							Tele(pl, i->StopTelePos);
						}
				}		
			}

			_Phase++;
			SummonSoulHealer();
			return;
		}


	//if (typeId == TYPEID_PLAYER)
	//	for (auto itr = _PhaseFactionVec.begin(); itr != _PhaseFactionVec.end(); itr++)
	//		if (itr->Phase == _Phase && obj->ToPlayer()->GetEventFaction() == itr->Faction)
	//			if (itr->StopPlayerKilleds != 0 || itr->StopPlayerKills != 0)
	//				return;
	//
	//for (auto itr = _PlayersMap.begin(); itr != _PlayersMap.end(); itr++)
	//{
	//	if (Player* pl = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(itr->first, 0, HIGHGUID_PLAYER)))
	//	{
	//		if (!pl->InEvent())
	//			continue;
	//
	//		for (auto i = _PhaseFactionVec.begin(); i != _PhaseFactionVec.end(); i++)
	//			if (_Phase == i->Phase && pl->GetEventFaction() == i->Faction && !i->StopNotice.empty())
	//				pl->GetSession()->SendNotification(i->StopNotice.c_str()); 
	//	}
	//}
	//
	//_Phase++;
	//
	//SummonSoulHealer();
}

void Event::UpdateWorldState(uint32 ID, uint32 value)
{
	for (auto itr = _PlayersMap.begin(); itr != _PlayersMap.end(); itr++)
		if (Player* pl = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(itr->first, 0, HIGHGUID_PLAYER)))
			pl->SendUpdateWorldState(ID, value);
}


void Event::InitWorldState()
{
	for (auto itr = _WorldStateVec.begin(); itr != _WorldStateVec.end(); itr++)
	{
		sWorld->setWorldState(itr->ID, itr->InitValue);

		SessionMap const& smap = sWorld->GetAllSessions();
		for (SessionMap::const_iterator i = smap.begin(); i != smap.end(); ++i)
			if (Player* pl = i->second->GetPlayer())
				pl->SendUpdateWorldState(itr->ID, itr->InitValue);
	}
}

void Event::UpdateWorldState(Object* obj, bool kill)
{
	int64 entry = obj->GetEntry();
	TypeID typeId = obj->GetTypeId();

	switch (typeId)
	{
	case TYPEID_UNIT:
	{
		for (auto itr = _WorldStateVec.begin(); itr != _WorldStateVec.end(); itr++)
		{
			for (size_t i = 0; i < WORLDSTATE_ENTRY_MAX; i++)
			{
				if (entry == itr->Entry[i])
				{
					if (itr->Step == 0)
						continue;

					uint64 value = sWorld->getWorldState(itr->ID);
					value += itr->Step[i];
					sWorld->setWorldState(itr->ID, value);
					UpdateWorldState(itr->ID, value);
					if (value == itr->NoticeValue || itr->NoticeValue == 0)
						SendScreenMessage(itr->Notice);
				}
			}
		}
	}
		break;
	case TYPEID_GAMEOBJECT:
	{
		entry = -entry;

		for (auto itr = _WorldStateVec.begin(); itr != _WorldStateVec.end(); itr++)
		{
			for (size_t i = 0; i < WORLDSTATE_ENTRY_MAX; i++)
			{
				if (entry == itr->Entry[i])
				{
					if (itr->Step == 0)
						continue;

					uint64 value = sWorld->getWorldState(itr->ID);
					value += itr->Step[i];
					sWorld->setWorldState(itr->ID, value);
					UpdateWorldState(itr->ID, value);
					if (value == itr->NoticeValue || itr->NoticeValue == 0)
						SendScreenMessage(itr->Notice);
				}
			}
		}
	}
		break;
	case TYPEID_PLAYER:
	{
		for (auto itr = _WorldStateFactionVec.begin(); itr != _WorldStateFactionVec.end(); itr++)
		{
			if (itr->Faction == obj->ToPlayer()->GetEventFaction() && kill == itr->Kill)
			{
				if (itr->Step == 0)
					continue;

				uint64 value = sWorld->getWorldState(itr->ID);
				value += itr->Step;
				sWorld->setWorldState(itr->ID, value);
				UpdateWorldState(itr->ID, value);
				if (value == itr->NoticeValue || itr->NoticeValue == 0)
					SendScreenMessage(itr->Notice);
			}
		}
	}
		break;
	}
}

void Event::Process(Object* obj)
{
	uint32 guid = obj->GetGUIDLow();

	switch (obj->GetTypeId())
	{
	case TYPEID_UNIT:
		for (auto itr = _StopVec.begin(); itr != _StopVec.end(); itr++)
			if (guid == itr->Param && itr->StopType == TYPEID_UNIT)
				itr->Param = 0;
		break;
	case TYPEID_GAMEOBJECT:
		for (auto itr = _StopVec.begin(); itr != _StopVec.end(); itr++)
			if (guid == itr->Param && itr->StopType == TYPEID_GAMEOBJECT)
				itr->Param = 0;
	break;
	case TYPEID_PLAYER:
		for (auto itr = _StopVec.begin(); itr != _StopVec.end(); itr++)
			if (itr->StopType == TYPEID_PLAYER && itr->Faction == obj->ToPlayer()->GetEventFaction())
				if (itr->Param > 0)
					itr->Param--;
	break;
	}

	

	for (auto itr = _StopVec.begin(); itr != _StopVec.end(); itr++)
	{
		EventFactionId faction = itr->Faction;

		bool stop = true;

		for (auto i = _StopVec.begin(); i != _StopVec.end(); i++)
			if (faction == i->Faction)
				if (itr->Param != 0)
					stop = false;

		if (stop)
		{
			sGameEventMgr->StopEvent(_EventId, false, true);
			return;
		}
	}
}

bool Event::DiedTele(Player* player)
{
	if (player->InEvent())
	{
		Tele(player, GetPhaseGraveyardPos(player->GetEventFaction()));
		return true;
	}

	return false;
}

bool Event::SendAreaSpiritHealerQueryOpcode(Player* player)
{
	if (!player || !player->InEvent() || player->IsAlive())
		return false;

	auto itr = _HealerMap.find(player->GetEventFaction());

	if (itr != _HealerMap.end())
	{
		WorldPacket data(SMSG_AREA_SPIRIT_HEALER_TIME, 12);

		data << itr->second;
		data << 30000 - _UpdateReviveTimer;
		player->GetSession()->SendPacket(&data);
		return true;
	}
	
	return false;
}

void Event::SummonSoulHealer()
{
	if (!_Map)
		return;

	for (auto itr = _HealerMap.begin(); itr != _HealerMap.end(); itr++)
		if (Creature* healer = _Map->GetCreature(itr->second))
			healer->RemoveFromWorld();

	_HealerMap.clear();

	for (auto itr = _PhaseFactionVec.begin(); itr != _PhaseFactionVec.end(); itr++)
	{
		if (itr->Phase != _Phase)
			continue;

		uint32 map = 0;
		float x = 0;
		float y = 0;
		float z = 0;
		float o = 0;

		GetTelePos(itr->GraveyardPos, map, x, y, z, o);

		Position pos;
		pos.Relocate(x, y, z, o);

		if (Creature* healer = _Map->SummonCreature(itr->GraveyardHealer, pos, 0, 60 * 60 * IN_MILLISECONDS))
		{
			_HealerMap[itr->Faction] = healer->GetGUID();
			healer->setFaction(itr->Faction);
			healer->setDeathState(DEAD);
			healer->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, healer->GetGUID());
			healer->SetUInt32Value(UNIT_CHANNEL_SPELL, SPELL_SPIRIT_HEAL_CHANNEL);
			healer->SetFloatValue(UNIT_MOD_CAST_SPEED, 1.0f);
		}
	}
}

void Event::SendScreenMessage(std::string text, EventFactionId faction, bool onlydead)
{
	if (text.empty())
		return;

	WorldPacket data;
	ChatHandler::BuildChatPacket(data, CHAT_MSG_RAID_BOSS_EMOTE, LANG_UNIVERSAL,
		NULL, NULL, text);

	for (auto itr = _PlayersMap.begin(); itr != _PlayersMap.end(); itr++)
	{
		if (Player* pl = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(itr->first, 0, HIGHGUID_PLAYER)))
		{
			if (!pl->InEvent())
				continue;

			if (faction == 0 || faction == pl->GetEventFaction())
			{
				if (pl->IsAlive() && onlydead)
					continue;

				if (WorldSession* ss = pl->GetSession())
					pl->GetSession()->SendPacket(&data);
			}
		}
	}
}
