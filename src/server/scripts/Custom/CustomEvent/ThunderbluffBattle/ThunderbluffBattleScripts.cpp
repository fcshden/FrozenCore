//#pragma execution_character_set("utf-8")
//#include "ThunderbluffBattle.h"
//#include "../CustomEvent/Event.h"
//#include "Vehicle.h"
//
//enum SpellIds
//{
//	SPELL_TELE_DEBUFF = 54643,
//
//	//牛头守卫
//	SPELL_THROW_FIRE = 250000,
//	SPELL_THROW_WIND = 250001,
//
//	SPELL_dianran = 250002,
//	SPELL_CHARGE = 40497,
//	SPELL_huimiedaji = 250003,
//	SPELL_HEAL = 250004,
//};
//
//
//
//class ThunderbluffController : public CreatureScript
//{
//public:
//	ThunderbluffController() : CreatureScript("ThunderbluffController") { }
//	struct ThunderbluffControllerAI : public ScriptedAI
//	{
//		ThunderbluffControllerAI(Creature* creature) : ScriptedAI(creature), Summons(me){}
//		SummonList Summons;
//		uint32 TimerMinuteCount;
//
//		void InitializeAI() override
//		{ 
//			events.Reset();
//			TimerMinuteCount = 0;
//			events.ScheduleEvent(EVENT_TIMER, IN_MILLISECONDS);
//			events.ScheduleEvent(EVENT_PHASE_ONE, TIME_PHASE_ONE_DELAY);
//			events.ScheduleEvent(EVENT_PHASE_TWO, TIME_PHASE_TWO_DELAY);
//			events.ScheduleEvent(EVENT_PHASE_THREE, TIME_PHASE_THREE_DELAY);
//			events.ScheduleEvent(EVENT_PHASE_FINAL, TIME_PHASE_FINAL_DELAY);
//			sThunderbluffBattle->Init(me);
//		}
//
//		void JustSummoned(Creature* summoned) override
//		{
//			ThunderbluffCreatureList.push_back(summoned);
//		}
//
//		void SummonedCreatureDies(Creature* summoned, Unit* /*killer*/) override
//		{
//			sThunderbluffBattle->RewardByCreature(summoned->GetEntry(), true);
//
//			switch (summoned->GetEntry())
//			{
//			case NPC_GENERAL_1:
//				isGeneral_1_Alive = false;
//				sThunderbluffBattle->SetWorldState(WORLD_STATE_GENERAL_1, 1);
//				break;
//			case NPC_GENERAL_2:
//				isGeneral_2_Alive = false;
//				sThunderbluffBattle->SetWorldState(WORLD_STATE_GENERAL_2, 1);
//				break;
//			case NPC_GENERAL_3:
//				isGeneral_3_Alive = false;
//				sThunderbluffBattle->SetWorldState(WORLD_STATE_GENERAL_3, 1);
//				break;
//			case NPC_BAINE:
//				isBaine_Alive = false;
//				sGameEventMgr->StopEvent(EVENT_THUDERBLUFF_BATTLE_ID,false,false);
//				break;
//			default:
//				break;
//			}
//		}
//
//		void UpdateAI(uint32 diff)
//		{
//			events.Update(diff);
//			while (uint32 eventId = events.ExecuteEvent())
//			{
//				switch (eventId)
//				{
//				case EVENT_TIMER:
//				{
//					//TimerMinuteCount++;
//					//std::ostringstream oss;
//					//oss << "时间：" << TimerMinuteCount << "秒";
//					//sWorld->SendServerMessage(SERVER_MSG_STRING, oss.str().c_str());
//				}	
//				events.ScheduleEvent(EVENT_TIMER, IN_MILLISECONDS);
//					break;
//				case EVENT_PHASE_ONE:
//					Announce(PHASE_ONE);
//					sThunderbluffBattle->SetPhase(PHASE_ONE);
//					sThunderbluffBattle->SetWorldState(WORLD_STATE_GENERAL_1, 0);
//					sThunderbluffBattle->SetWorldState(WORLD_STATE_GENERAL_2, 0);
//					sThunderbluffBattle->SetWorldState(WORLD_STATE_GENERAL_3, 0);
//					break;
//				case EVENT_PHASE_TWO:
//					Announce(PHASE_TWO);
//					sThunderbluffBattle->SetPhase(PHASE_TWO);
//					sThunderbluffBattle->TeleAllPlayers();
//					if (isGeneral_1_Alive)
//						sThunderbluffBattle->RewardByCreature(NPC_GENERAL_1, false);
//					break;
//				case EVENT_PHASE_THREE:
//					Announce(PHASE_THREE);
//					sThunderbluffBattle->SetPhase(PHASE_THREE);
//					sThunderbluffBattle->TeleAllPlayers();
//					if (isGeneral_2_Alive)
//						sThunderbluffBattle->RewardByCreature(NPC_GENERAL_2, false);
//					break;
//				case EVENT_PHASE_FINAL:
//					Announce(PHASE_FINAL);
//					sThunderbluffBattle->SetPhase(PHASE_FINAL);
//					sThunderbluffBattle->TeleAllPlayers();
//					if (isGeneral_3_Alive)
//						sThunderbluffBattle->RewardByCreature(NPC_GENERAL_3, false);
//					break;
//				default:
//					break;
//				}
//			}
//		}
//		
//
//		void Announce(PhaseIds phase)
//		{
//			std::ostringstream oss;
//			oss << event_name << "第" << phase << "阶段激活！";
//
//			SessionMap const& smap = sWorld->GetAllSessions();
//			for (SessionMap::const_iterator iter = smap.begin(); iter != smap.end(); ++iter)
//				if (Player* pl = iter->second->GetPlayer())
//					if (pl->event_team != EVENT_TEAM_NONE)
//					{
//						pl->GetSession()->SendAreaTriggerMessage(oss.str().c_str());
//						ChatHandler(pl->GetSession()).PSendSysMessage(oss.str().c_str());
//					}				
//		}
//
//
//	private:
//		EventMap events;
//	};
//
//
//
//	CreatureAI* GetAI(Creature* creature) const override
//	{
//		return new ThunderbluffControllerAI(creature);
//	}
//};
//
//
//class ThunderbluffTeleTrigger : public CreatureScript
//{
//public:
//	ThunderbluffTeleTrigger() : CreatureScript("ThunderbluffTeleTrigger") { }
//	struct ThunderbluffTeleTriggerAI : public ScriptedAI
//	{
//		ThunderbluffTeleTriggerAI(Creature* creature) : ScriptedAI(creature), Summons(me){}
//		SummonList Summons;
//
//		void MoveInLineOfSight(Unit* who) override
//		{
//			if (!gunship)
//				return;
//
//			if (who->GetTypeId() != TYPEID_PLAYER)
//				return;
//
//			if (!me->IsWithinDistInMap(who, 10.0f))
//				return;
//
//			Player* player = who->ToPlayer();
//
//			if (player->event_team == EVENT_TEAM_OFFENSE)
//				sThunderbluffBattle->TelePlayerToShip(player, gunship);
//			else
//				sThunderbluffBattle->TelePlayerToHighLand(player);
//		}	
//	};
//
//	CreatureAI* GetAI(Creature* creature) const override
//	{
//		return new ThunderbluffTeleTriggerAI(creature);
//	}
//};
//
//
//class ThunderbluffBattlePortal : public GameObjectScript
//{
//public:
//	ThunderbluffBattlePortal() : GameObjectScript("ThunderbluffBattlePortal") { }
//
//	bool OnGossipHello(Player* player, GameObject* go)
//	{
//		if (!gunship)
//			return false;
//
//		if (sThunderbluffBattle->GetPhase() == PHASE_ZERO)
//		{
//			player->GetSession()->SendNotification("等待传送器激活!");
//			return false;
//		}
//
//		if (!player->HasAura(SPELL_TELE_DEBUFF))
//		{
//			player->AddAura(SPELL_TELE_DEBUFF, player);
//			switch (go->GetEntry())
//			{
//			case GO_PORTAL_HIGHLAND:
//				sThunderbluffBattle->TelePlayerToShip(player, gunship);
//				break;
//			case GO_PORTAL_SHIP:
//				sThunderbluffBattle->TelePlayerToHighLand(player);
//				break;
//			default:
//				break;
//			}
//		}
//		return true;
//	}
//};
//
//
//class ThunderbluffBattleSpiritHealer : public CreatureScript
//{
//public:
//	ThunderbluffBattleSpiritHealer() : CreatureScript("ThunderbluffBattleSpiritHealer") { }
//	struct ThunderbluffBattleSpiritHealerAI : public CreatureAI
//	{
//		SummonList Summons;
//		EventMap events;
//		uint32 Timer;
//		bool announce_30;
//		bool announce_20;
//		bool announce_10;
//		bool revive;
//
//		ThunderbluffBattleSpiritHealerAI(Creature* creature) : CreatureAI(creature), Summons(me){}
//
//		void Reset() override
//		{
//			Timer = 0;
//			announce_30 = false;
//			announce_20 = false;
//			announce_10 = false;
//			revive = false;
//		}
//
//		void UpdateAI(uint32 diff) override
//		{
//			Timer += diff;
//
//			if (!announce_30 && Timer > 10000)
//			{
//				announce_30 = true;
//				Announce(30);
//			}
//
//			if (!announce_20 && Timer > 20000)
//			{
//				announce_20 = true;
//				Announce(20);
//			}
//
//			if (!announce_10 && Timer > 30000)
//			{
//				announce_10 = true;
//				Announce(10);
//			}
//
//			if (!revive && Timer > 40000)
//			{
//				Revive();
//				Reset();
//			}
//		}
//
//		void Announce(uint32 seconds)
//		{
//			std::ostringstream oss;
//			oss << event_name << seconds << "秒后集体复活，请靠近灵魂医者...";
//
//			std::list<Player*> playersNearby;
//			me->GetPlayerListInGrid(playersNearby, 30, false);
//			if (!playersNearby.empty())
//				for (std::list<Player*>::iterator itr = playersNearby.begin(); itr != playersNearby.end(); ++itr)
//				{
//					Player* pl = *itr;
//
//					if (!pl->IsInWorld())
//						continue;
//
//					if (pl->event_team == EVENT_TEAM_NONE || pl->getFaction() != me->getFaction())
//						continue;
//
//					if (pl->IsAlive())
//						continue;
//
//					pl->GetSession()->SendAreaTriggerMessage(oss.str().c_str());
//				}
//		}
//
//		void Revive()
//		{
//			std::ostringstream oss;
//
//			oss << event_name << "拿出你的勇气，继续战斗！";
//
//			std::list<Player*> playersNearby;
//			me->GetPlayerListInGrid(playersNearby, 30, false);
//
//			if (!playersNearby.empty())
//			{
//				for (std::list<Player*>::iterator itr = playersNearby.begin(); itr != playersNearby.end(); ++itr)
//				{
//					Player* pl = *itr;
//
//					if (!pl->IsInWorld())
//						continue;
//
//					if (pl->event_team == EVENT_TEAM_NONE)
//						continue;
//
//			
//					if (pl->IsAlive())
//						continue;
//
//					pl->GetSession()->SendNotification(oss.str().c_str());
//					me->CastSpell(me, 22012, true);
//					pl->CastSpell(*itr, 24171, true);
//					pl->ResurrectPlayer(1.0f);
//					pl->CastSpell(*itr, 6962, true);
//					pl->CastSpell(*itr, 44535, true);
//					pl->SpawnCorpseBones();
//				}
//			}
//		}
//	};
//
//	CreatureAI* GetAI(Creature* creature) const override
//	{
//		return new ThunderbluffBattleSpiritHealerAI(creature);
//	}
//};
//
//
//class ThunderbluffAirVehicle : public CreatureScript
//{
//public:
//	ThunderbluffAirVehicle() : CreatureScript("ThunderbluffAirVehicle") { }
//
//	struct ThunderbluffAirVehicleAI : public VehicleAI
//	{
//		ThunderbluffAirVehicleAI(Creature *pCreature) : VehicleAI(pCreature){}
//
//		Unit* thrower;
//		uint32 Timer = 0;
//		bool countdown = false;
//		
//		float x = 0;
//		float y = 0;
//		float z = 0;
//
//		void SpellHit(Unit* caster, SpellInfo const* spell) override
//		{
//			if (spell->Id == 49756)
//			{
//				thrower = caster;
//
//				me->CastSpell(caster, 43789);
//
//				Unit* ps = me->GetVehicleKit()->GetPassenger(0);
//				me->MonsterWhisper("该死，被那个牛头人击中了，快干掉它...否则我们就坠毁了！", ps->ToPlayer());
//				countdown = true;
//			}
//		}
//
//
//		void UpdateAI(uint32 diff)
//		{
//			if (countdown)
//			{
//				Timer += diff;
//
//				if (Timer > 5 * IN_MILLISECONDS)
//				{
//					countdown = false;
//					Timer = 0;			
//					
//					me->RemoveAura(43789);
//					
//					if (thrower->IsAlive())
//					{
//						thrower->RemoveAura(43789);
//						me->SetReactState(REACT_PASSIVE);
//						me->setDeathState(JUST_DIED);
//						me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
//					}	
//				}
//			}
//
//			VehicleAI::UpdateAI(diff);
//		}
//		void PassengerBoarded(Unit* who, int8 seatid, bool add)
//		{
//			me->SetDisableGravity(false);
//			me->SendMovementFlagUpdate();
//			me->SetCanFly(true);
//			for (uint32 i = 0; i < 9;i++)
//				me->SetSpeed(UnitMoveType(i), 2.0f, true);
//		}
//
//		void JustDied(Unit* killer)
//		{
//			
//		}
//
//		void SpellHitTarget(Unit* target, SpellInfo const* spell)
//		{
//			if (target->GetEntry() == NPC_BAINE)
//				target->ModifyHealth(-10000);
//		}
//
//	};
//
//	CreatureAI* GetAI(Creature* pCreature) const
//	{
//		return new ThunderbluffAirVehicleAI(pCreature);
//	}
//};
//
//enum THRWOER_EVENTS
//{
//	EVENT_THROW = 1,
//	EVENT_CAST_SPELL_dianran,
//	EVENT_CAST_SPELL_CHARGE,
//	EVENT_CAST_SPELL_huimiedaji,
//	
//	EVENT_CHECK_HP,
//	EVENT_CAST_SPELL_HEAL,
//};
//
//class ThunderbluffThower : public CreatureScript
//{
//public:
//	ThunderbluffThower() : CreatureScript("ThunderbluffThower") { }
//
//	struct ThunderbluffThowerAI : public ScriptedAI
//	{
//		ThunderbluffThowerAI(Creature* creature) : ScriptedAI(creature), Summons(me){}
//
//		SummonList Summons;
//
//		uint32 throwTimer;
//		void InitializeAI() override
//		{
//			throwTimer = urand(1000, 3000);
//			events.ScheduleEvent(EVENT_THROW, throwTimer);
//		}
//
//		void Reset() override
//		{
//			events.Reset();
//			throwTimer = urand(1000, 3000);
//			events.ScheduleEvent(EVENT_THROW, throwTimer);
//		}
//
//		void EnterCombat(Unit* victim) override
//		{
//			events.ScheduleEvent(EVENT_CHECK_HP, 0);
//			events.ScheduleEvent(EVENT_CAST_SPELL_CHARGE, 1000);
//			events.ScheduleEvent(EVENT_CAST_SPELL_dianran, 3000);
//			events.ScheduleEvent(EVENT_CAST_SPELL_huimiedaji, 6000);
//		}
//
//		void UpdateAI(uint32 diff) override
//		{
//			events.Update(diff);
//
//			if (me->HasUnitState(UNIT_STATE_CASTING))
//				return;
//
//			while (uint32 eventId = events.ExecuteEvent())
//			{
//				switch (eventId)
//				{
//				case EVENT_THROW:
//					if (urand(0, 1) == 0)
//					{
//						if (gunshipHP > 0)
//						{
//							if (Creature* target = me->FindNearestCreature(NPC_GUNSHIP_SHELL, 500.0f))
//							{
//								me->SetFacingToObject(target);
//								me->CastSpell(target, SPELL_THROW_FIRE);
//								gunshipHP -= DmgPerThrow;
//								if (gunshipHP < 0)
//									gunshipHP = 0;
//							}
//						}
//					}			
//					events.ScheduleEvent(EVENT_THROW, throwTimer);
//					break;
//				case EVENT_CHECK_HP:
//					if (me->HealthBelowPct(50))
//					{
//						events.ScheduleEvent(EVENT_CAST_SPELL_HEAL, 0);
//						break;
//					}		
//					events.ScheduleEvent(EVENT_CHECK_HP, 1000);
//					break;
//				case EVENT_CAST_SPELL_HEAL:
//					DoCast(SPELL_HEAL);
//					events.ScheduleEvent(EVENT_CAST_SPELL_HEAL, urand(3000, 5000));
//					break;
//				case EVENT_CAST_SPELL_dianran:
//					DoCast(SPELL_dianran);
//					events.ScheduleEvent(EVENT_CAST_SPELL_dianran, urand(10000, 15000));
//					break;
//				case EVENT_CAST_SPELL_CHARGE:
//					DoCast(SPELL_CHARGE);
//					events.ScheduleEvent(EVENT_CAST_SPELL_CHARGE, urand(6000, 10000));
//					break;
//				case EVENT_CAST_SPELL_huimiedaji:
//					DoCast(SPELL_huimiedaji);
//					events.ScheduleEvent(EVENT_CAST_SPELL_huimiedaji, urand(5000, 8000));
//					break;
//				default:
//					break;
//				}
//			}
//
//			DoMeleeAttackIfReady();
//		}
//
//	private:
//		EventMap events;
//	};
//
//	CreatureAI* GetAI(Creature* creature) const
//	{
//		return new ThunderbluffThowerAI(creature);
//	}
//};
//
//
//class ThunderbluffHarpoonTrigger : public CreatureScript
//{
//public:
//	ThunderbluffHarpoonTrigger() : CreatureScript("ThunderbluffHarpoonTrigger") { }
//
//	struct ThunderbluffHarpoonTriggerAI : public ScriptedAI
//	{
//		ThunderbluffHarpoonTriggerAI(Creature* creature) : ScriptedAI(creature), Summons(me){}
//
//		SummonList Summons;
//
//		void InitializeAI() override
//		{
//			events.ScheduleEvent(EVENT_THROW, urand(1000, 15000));
//		}
//
//		void Reset() override
//		{
//			events.Reset();
//			events.ScheduleEvent(EVENT_THROW, urand(1000, 15000));
//		}
//
//		void UpdateAI(uint32 diff) override
//		{
//			events.Update(diff);
//
//			while (uint32 eventId = events.ExecuteEvent())
//			{
//				if (eventId == EVENT_THROW)
//				{
//					Map::PlayerList const &players = me->GetMap()->GetPlayers();
//					for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
//					{
//						if (Player* player = itr->GetSource())
//						{
//							if (me->GetDistance2d(player) > 250.0f)
//								continue;
//
//							if (Vehicle* v = player->GetVehicle())
//								if (Unit* mount = v->GetBase())
//								{
//									me->SetFacingToObject(mount);
//									me->CastSpell(mount, 49727);
//									break;
//								}
//						}
//
//						
//					}
//					events.ScheduleEvent(EVENT_THROW, urand(1000, 2000));
//				}				
//			}
//		}
//
//	private:
//		EventMap events;
//	};
//
//	CreatureAI* GetAI(Creature* creature) const
//	{
//		return new ThunderbluffHarpoonTriggerAI(creature);
//	}
//};
//
//
//
//void AddSC_Thunderbluff_Scripts()
//{
//	new ThunderbluffController();
//	new ThunderbluffTeleTrigger();
//	new ThunderbluffBattlePortal();
//	new ThunderbluffBattleSpiritHealer();
//	new ThunderbluffAirVehicle();
//	new ThunderbluffThower();
//	//new ThunderbluffHarpoonTrigger();
//}
