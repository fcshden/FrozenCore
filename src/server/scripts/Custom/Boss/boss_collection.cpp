/*
 *¨X¨T¨j¨T¨j¨T¨j¨j¨j¨T¨T¨j¨T¨j¨[©¤¨X¨j¨T¨T¨[ 
 *¨U¨j¨g¨U¨U¨U¨U¨U¨d¨[¨[¨U¨j¨g¨^¨j¨a¨U¨T¨T¨g
 *¨U¨m¨g¨U¨U¨U¨U¨U¨d¨m¨a¨U¨m¨p¨[¨U¨X¨p¨T¨T¨U
 *¨^¨T¨m¨m¨T¨m¨m¨T¨m¨T¨T¨m¨T¨a¨^¨T¨a¨^¨T¨T¨a
 *           (http://emudevs.com)
 GameObject: (Burning Tree;191160)
 GameObject: (Burning Blaze;190570) - Deals Damage if near
 NPC DisplayID: 24905
*/


#pragma execution_character_set("utf-8")
#include "../PrecompiledHeaders/ScriptPCH.h"

enum SpellIds
{
/*     NAME                       SPELLID    CAST-TIME     DAMAGE   */
    SPELL_FIREBALL               = 70282, //  3 sec       6198-7202
    SPELL_FIRE_SPIT              = 66796, //  1 sec       5088-5912
    SPELL_BLAST_NOVA             = 74392, //  2 sec       5688-7312
	SPELL_MONEN					=60745,
};

enum EventIds
{
    EVENT_NONE,
    EVENT_FIREBALL,
    EVENT_FIRE_SPIT,
    EVENT_BLAST_NOVA,
    EVENT_SPAWN_FIRE,
    EVENT_SPAWN_FIRE_CLEANUP
};

class boss_3 : public CreatureScript
{
public:
	boss_3() : CreatureScript("boss_3") { }

    struct boss_3AI : public ScriptedAI
    {
        boss_3AI(Creature* creature) : ScriptedAI(creature) { }

		std::vector<GameObject*> ObjVec;

        void Reset()
        {
			ClearFire();
            events.Reset();         
        }

		void EnterCombat(Unit* /*who*/)
		{
			events.ScheduleEvent(EVENT_FIREBALL, urand(3000, 4000));
			events.ScheduleEvent(EVENT_FIRE_SPIT, urand(2000, 9000));
			events.ScheduleEvent(EVENT_BLAST_NOVA, urand(10000, 19000));
			events.ScheduleEvent(EVENT_SPAWN_FIRE, urand(1000, 5000));
		}

        void UpdateAI(uint32 diff)
        {
			if (!UpdateVictim())
				return;

			events.Update(diff);
			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                    case EVENT_FIREBALL:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                            DoCast(target, SPELL_FIREBALL);
                        events.ScheduleEvent(EVENT_FIREBALL, urand(3000, 6000));
                        break;
                    case EVENT_FIRE_SPIT:
                        DoCast(me->GetVictim(), SPELL_FIRE_SPIT, true);
                        events.ScheduleEvent(EVENT_FIRE_SPIT, urand(2000, 5000));
                        break;
                    case EVENT_BLAST_NOVA:
                        DoCast(SPELL_BLAST_NOVA);
                        events.ScheduleEvent(EVENT_BLAST_NOVA, urand(5000, 8000));
                        break;
                    case EVENT_SPAWN_FIRE:
                        me->MonsterYell("»Ò½ý...»ÙÃð...", LANG_UNIVERSAL, 0);
						events.ScheduleEvent(EVENT_SPAWN_FIRE_CLEANUP, 30000);
                        events.ScheduleEvent(EVENT_SPAWN_FIRE, urand(10000, 15000));
                        SpawnFire();
                        break;
                    case EVENT_SPAWN_FIRE_CLEANUP:
                        ClearFire();
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }

        void SpawnFire()
        {
            float x, y, z;
            me->GetPosition(x, y, z);

			if (GameObject* obj = me->SummonGameObject(191160, x, y, z, 0, 0, 0, 0, 0, 0))
				ObjVec.push_back(obj);
			if (GameObject* obj = me->SummonGameObject(190570, x, y + 10, z, 0, 0, 0, 0, 0, 0))
				ObjVec.push_back(obj);
			if (GameObject* obj = me->SummonGameObject(190570, x, y - 10, z, 0, 0, 0, 0, 0, 0))
				ObjVec.push_back(obj);
			if (GameObject* obj = me->SummonGameObject(190570, x + 10, y, z, 0, 0, 0, 0, 0, 0))
				ObjVec.push_back(obj);
			if (GameObject* obj = me->SummonGameObject(190570, x - 10, y, z, 0, 0, 0, 0, 0, 0))
				ObjVec.push_back(obj);
        }

        void ClearFire()
        {
			for (size_t i = 0; i < ObjVec.size(); i++)
			{
				if (ObjVec[i]->IsInWorld())
					ObjVec[i]->RemoveFromWorld();
			}

			ObjVec.clear();
        }

        void JustDied(Unit* /* victim */)
        {
            ClearFire();
        }
    private:
        EventMap events;
	};

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_3AI(creature);
    }
};


enum Event
{
	EVENT_PRINCE_FLAME_SPHERES = 1,
	EVENT_PRINCE_VANISH = 2,
	EVENT_PRINCE_BLOODTHIRST = 3,
	EVENT_PRINCE_VANISH_RUN = 4,
	EVENT_PRINCE_RESCHEDULE = 5,
};
enum Spells
{
	SPELL_BLOODTHIRST = 55968, //Trigger Spell + add aura
	SPELL_CONJURE_FLAME_SPHERE = 55931,
	SPELL_FLAME_SPHERE_SPAWN_EFFECT = 55891,
	SPELL_FLAME_SPHERE_VISUAL = 55928,
	SPELL_FLAME_SPHERE_PERIODIC = 55926,
	SPELL_FLAME_SPHERE_PERIODIC_H = 59508,
	SPELL_FLAME_SPHERE_DEATH_EFFECT = 55947,
	SPELL_BEAM_VISUAL = 60342,
	SPELL_EMBRACE_OF_THE_VAMPYR = 55959,
	SPELL_EMBRACE_OF_THE_VAMPYR_H = 59513,
	SPELL_VANISH = 55964,
	CREATURE_FLAME_SPHERE = 30106,
	CREATURE_FLAME_SPHERE_1 = 31686,
	CREATURE_FLAME_SPHERE_2 = 31687,
};

enum Misc
{
	DATA_EMBRACE_DMG = 20000,
	DATA_EMBRACE_DMG_H = 40000,
	DATA_SPHERE_DISTANCE = 30,
	ACTION_FREE = 1,
	ACTION_SPHERE = 2,
};

class boss_1 : public CreatureScript
{
public:
	boss_1() : CreatureScript("boss_1") { }

	struct boss_1AI : public ScriptedAI
	{
		boss_1AI(Creature *c) : ScriptedAI(c), summons(me)
		{
			
		}

		EventMap events;
		SummonList summons;
		uint64 vanishTarget;
		uint32 vanishDamage;

		void Reset()
		{
			if (me->GetPositionZ() > 15.0f)
				me->CastSpell(me, SPELL_BEAM_VISUAL, true);

			events.Reset();
			summons.DespawnAll();
			vanishDamage = 0;
			vanishTarget = 0;
		}

		void DoAction(int32 param)
		{
			
		}

		void EnterCombat(Unit* /*who*/)
		{
			ScheduleEvents();
			me->RemoveAllAuras();
			me->InterruptNonMeleeSpells(true);
		}

		void ScheduleEvents()
		{
			events.Reset();
			events.ScheduleEvent(EVENT_PRINCE_FLAME_SPHERES, 10000);
			events.ScheduleEvent(EVENT_PRINCE_BLOODTHIRST, 10000);
			vanishTarget = 0;
			vanishDamage = 0;
		}

		void SpellHitTarget(Unit *, const SpellInfo *spellInfo)
		{
			if (spellInfo->Id == SPELL_CONJURE_FLAME_SPHERE)
				summons.DoAction(ACTION_SPHERE);
		}

		void UpdateAI(uint32 diff)
		{
			if (!UpdateVictim())
				return;

			events.Update(diff);
			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			switch (events.GetEvent())
			{
			case EVENT_PRINCE_BLOODTHIRST:
			{
				me->CastSpell(me->GetVictim(), SPELL_BLOODTHIRST, false);
				events.RepeatEvent(10000);
				break;
			}
			case EVENT_PRINCE_FLAME_SPHERES:
			{
				me->CastSpell(me->GetVictim(), SPELL_CONJURE_FLAME_SPHERE, false);
				events.RescheduleEvent(EVENT_PRINCE_VANISH, 14000);
				Creature *cr;
				if (cr = me->SummonCreature(CREATURE_FLAME_SPHERE, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 5.0f, 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10 * IN_MILLISECONDS))
					summons.Summon(cr);

				if (cr = me->SummonCreature(CREATURE_FLAME_SPHERE_1, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 5.0f, 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10 * IN_MILLISECONDS))
					summons.Summon(cr);

				if (cr = me->SummonCreature(CREATURE_FLAME_SPHERE_2, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 5.0f, 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10 * IN_MILLISECONDS))
					summons.Summon(cr);

				events.RepeatEvent(15000);
				break;
			}
			case EVENT_PRINCE_VANISH:
			{
				events.PopEvent();
				//Count alive players
				uint8 count = 0;
				Unit *pTarget;
				std::list<HostileReference *> t_list = me->getThreatManager().getThreatList();
				for (std::list<HostileReference *>::const_iterator itr = t_list.begin(); itr != t_list.end(); ++itr)
				{
					pTarget = ObjectAccessor::GetUnit(*me, (*itr)->getUnitGuid());
					if (pTarget && pTarget->GetTypeId() == TYPEID_PLAYER && pTarget->IsAlive())
						count++;
				}
				//He only vanishes if there are 3 or more alive players
				if (count > 1)
				{
					me->CastSpell(me, SPELL_VANISH, false);

					events.CancelEvent(EVENT_PRINCE_FLAME_SPHERES);
					events.CancelEvent(EVENT_PRINCE_BLOODTHIRST);
					events.ScheduleEvent(EVENT_PRINCE_VANISH_RUN, 2499);
					if (Unit* pEmbraceTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
						vanishTarget = pEmbraceTarget->GetGUID();
				}
				break;
			}
			case EVENT_PRINCE_VANISH_RUN:
			{
				if (Unit *vT = ObjectAccessor::GetUnit(*me, vanishTarget))
				{
					me->UpdatePosition(vT->GetPositionX(), vT->GetPositionY(), vT->GetPositionZ(), me->GetAngle(vT), true);
					me->CastSpell(vT, SPELL_EMBRACE_OF_THE_VAMPYR, false);
					me->RemoveAura(SPELL_VANISH);
				}

				events.PopEvent();
				events.ScheduleEvent(EVENT_PRINCE_RESCHEDULE, 20000);
				break;
			}
			case EVENT_PRINCE_RESCHEDULE:
			{
				events.PopEvent();
				ScheduleEvents();
				break;
			}
			}

			if (me->IsVisible())
				DoMeleeAttackIfReady();
		}

		void DamageTaken(Unit*, uint32 &damage, DamageEffectType, SpellSchoolMask)
		{
			if (vanishTarget)
			{
				vanishDamage += damage;
				if (vanishDamage > (uint32)DUNGEON_MODE(DATA_EMBRACE_DMG, DATA_EMBRACE_DMG_H))
				{
					ScheduleEvents();
					me->CastStop();
				}
			}
		}

		void JustDied(Unit* /*killer*/)
		{
			summons.DespawnAll();
		}

		void KilledUnit(Unit * victim)
		{
			if (urand(0, 1))
				return;

			if (vanishTarget && victim->GetGUID() == vanishTarget)
				ScheduleEvents();
		}
	};

	CreatureAI *GetAI(Creature *creature) const
	{
		return new boss_1AI(creature);
	}
};




enum spells
{
	// NADOX
	SPELL_BROOD_PLAGUE = 56130,
	SPELL_BROOD_PLAGUE_H = 59467,
	SPELL_BROOD_RAGE_H = 59465,
	SPELL_ENRAGE = 26662, // Enraged if too far away from home
	//SPELL_SUMMON_SWARMERS			= 56119, //2x 30178  -- 2x every 10secs, spell works fine but i need specific coords
	//SPELL_SUMMON_SWARM_GUARD		= 56120, //1x 30176  -- at 50%hp, spell works fine but i need specific coords

	// ADDS
	SPELL_SPRINT = 56354,
	SPELL_GUARDIAN_AURA = 56151,
	SPELL_SWARMER_AURA = 56158,
};

enum creatures
{
	NPC_AHNKAHAR_SWARMER = 30178,
	NPC_AHNKAHAR_GUARDIAN_ENTRY = 30176,
};

enum events
{
	EVENT_CHECK_HEALTH = 1,
	EVENT_CHECK_HOME = 2,
	EVENT_PLAGUE = 3,
	EVENT_BROOD_RAGE = 4,
	EVENT_SWARMER = 5,
	EVENT_SUMMON_GUARD = 6,
};

enum sounds
{
	SOUND_AGGRO = 14033,
	SOUND_SUMMON1 = 14034,
	SOUND_SUMMON2 = 14035,
	SOUND_SLAY1 = 14036,
	SOUND_SLAY2 = 14037,
	SOUND_SLAY3 = 14038,
	SOUND_DEATH = 14039,
};

class boss_2 : public CreatureScript
{
public:
	boss_2() : CreatureScript("boss_2") { }

	struct boss_2AI : public ScriptedAI
	{
		boss_2AI(Creature *c) : ScriptedAI(c), summons(me)
		{
			pInstance = c->GetInstanceScript();
		}

		EventMap events;
		InstanceScript *pInstance;
		SummonList summons;

		void SummonHelpers(bool swarm)
		{
			Creature *cr;
			if (swarm)
			{
				if (cr = me->SummonCreature(NPC_AHNKAHAR_SWARMER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 2.56563f))
					summons.Summon(cr);
				if (cr = me->SummonCreature(NPC_AHNKAHAR_SWARMER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 3.64774f))
					summons.Summon(cr);
			}
			else
			{
				if (cr = me->SummonCreature(NPC_AHNKAHAR_GUARDIAN_ENTRY, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 3.03687f))
					summons.Summon(cr);
			}
		}

		void Reset()
		{
			events.Reset();
			summons.DespawnAll();
		}

		void EnterCombat(Unit * /*who*/)
		{
			me->PlayDirectSound(SOUND_AGGRO);
			events.ScheduleEvent(EVENT_CHECK_HEALTH, 1000);
			events.ScheduleEvent(EVENT_SWARMER, 10000);
			events.ScheduleEvent(EVENT_CHECK_HOME, 2000);
			events.ScheduleEvent(EVENT_PLAGUE, 5000 + rand() % 3000);
			events.ScheduleEvent(EVENT_BROOD_RAGE, 5000);
		}

		void DoAction(int32 param)
		{
			
		}

		void KilledUnit(Unit * /*victim*/)
		{
			if (urand(0, 1))
				return;

			switch (rand() % 3)
			{
			case 0:
				me->PlayDirectSound(SOUND_SLAY1);
				break;
			case 1:
				me->PlayDirectSound(SOUND_SLAY2);
				break;
			case 2:
				me->PlayDirectSound(SOUND_SLAY3);
				break;
			}
		}

		void JustDied(Unit* /*killer*/)
		{
			events.Reset();
			summons.DespawnAll();
			me->PlayDirectSound(SOUND_DEATH);
		}

		void JustSummoned(Creature* cr)
		{
			if (cr)
			{
				if (cr->GetEntry() == NPC_AHNKAHAR_GUARDIAN_ENTRY)
				{
					switch (rand() % 2)
					{
					case 0:
						me->PlayDirectSound(SOUND_SUMMON1);
						break;
					case 1:
						me->PlayDirectSound(SOUND_SUMMON2);
						break;
					}
				}

				summons.Summon(cr);
			}
		}
		void UpdateAI(uint32 diff)
		{
			if (!UpdateVictim())
				return;

			events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			switch (events.GetEvent())
			{
			case EVENT_CHECK_HEALTH:
			{
				events.RepeatEvent(1000);
				if (HealthBelowPct(50))
				{
					events.CancelEvent(EVENT_CHECK_HEALTH);
					events.ScheduleEvent(EVENT_SUMMON_GUARD, 100);
				}
				break;
			}
			case EVENT_SUMMON_GUARD:
			{
				SummonHelpers(false);
				events.PopEvent();
				break;
			}
			case EVENT_BROOD_RAGE:
			{
				if (Creature *pSwarmer = me->FindNearestCreature(NPC_AHNKAHAR_SWARMER, 40, true))
					me->CastSpell(pSwarmer, SPELL_BROOD_RAGE_H, true);

				events.RepeatEvent(10000);
				break;
			}
			case EVENT_PLAGUE:
			{
				me->CastSpell(me->GetVictim(), DUNGEON_MODE(SPELL_BROOD_PLAGUE, SPELL_BROOD_PLAGUE_H), false);
				events.RepeatEvent(12000 + rand() % 5000);
				break;
			}
			case EVENT_SWARMER:
			{
				SummonHelpers(true);
				events.RepeatEvent(10000);
				break;
			}
			case EVENT_CHECK_HOME:
			{
				if (me->HasAura(SPELL_ENRAGE))
					break;

				if (me->GetPositionZ() < 24)
				{
					me->CastSpell(me, SPELL_ENRAGE, true);
					events.PopEvent();
					break;
				}

				events.RepeatEvent(2000);
				break;
			}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI *GetAI(Creature *creature) const
	{
		return new boss_2AI(creature);
	}
};

void AddSC_BossScript_Collection()
{
	new boss_3;
	new boss_1;
	new boss_2;
}
