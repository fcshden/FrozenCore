#pragma execution_character_set("utf-8")
#include "../PrecompiledHeaders/ScriptPCH.h"
enum Spells
{
	SPELL_STEALTH = 30991,
	SPELL_INSANITY = 57496, //Dummy
	INSANITY_VISUAL = 57561,
	SPELL_INSANITY_TARGET = 57508,
	SPELL_SLICEANDDICE = 6434,
	SPELL_SLASH = 71623,
	SPELL_FRENZY = 58841,
	SPELL_CLONE_PLAYER = 57507, //casted on player during insanity
	SPELL_INSANITY_PHASING_1 = 57508,
	SPELL_INSANITY_PHASING_2 = 57509,
	SPELL_INSANITY_PHASING_3 = 57510,
	SPELL_INSANITY_PHASING_4 = 57511,
	SPELL_INSANITY_PHASING_5 = 57512
};

enum Creatures
{
	NPC_TWISTED_VISAGE = 30625
};




class boss_bartimus : public CreatureScript
{
public:
	boss_bartimus() : CreatureScript("boss_bartimus") { }

	struct boss_bartimusAI : public ScriptedAI
	{
		boss_bartimusAI(Creature* creature) : ScriptedAI(creature), Summons(me)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;

		uint32 Slash_Timer;
		uint32 Sliceanddice_Timer;
		uint32 Frenzy_Timer;
		uint32 insanityHandled;
		SummonList Summons;

		// returns the percentage of health after taking the given damage.
		uint32 GetHealthPct(uint32 damage)
		{
			if (damage > me->GetHealth())
				return 0;
			return 100 * (me->GetHealth() - damage) / me->GetMaxHealth();
		}

		void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType, SpellSchoolMask)
		{
			if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
				damage = 0;

			if ((GetHealthPct(0) >= 33 && GetHealthPct(damage) < 33))
			{
				me->MonsterYell("理智..理智？混乱！疯狂！", LANG_UNIVERSAL, NULL);
				me->InterruptNonMeleeSpells(false);
				me->MonsterYell("啊..啊.. 我的头..", LANG_UNIVERSAL, NULL);
				DoCast(me, SPELL_INSANITY, false);
			}
		}

		void SpellHitTarget(Unit* target, const SpellInfo* spell)
		{
			if (spell->Id == SPELL_INSANITY)
			{
				// Not good target or too many players
				if (target->GetTypeId() != TYPEID_PLAYER || insanityHandled > 4)
					return;
				// First target - start channel visual and set self as unnattackable
				if (!insanityHandled)
				{
					// Channel visual
					DoCast(me, INSANITY_VISUAL, true);
					// Unattackable
					me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
					me->SetControlled(true, UNIT_STATE_STUNNED);
				}
				// phase mask
				target->CastSpell(target, SPELL_INSANITY_TARGET + insanityHandled, true);
				// Summon essence of insanity in target's phase
				Map::PlayerList const &players = me->GetMap()->GetPlayers();
				for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
				{
					Player* player = i->GetSource();
					if (!player || !player->IsAlive())
						continue;
					// Summon insanity
					if (Unit* summon = me->SummonCreature(NPC_TWISTED_VISAGE, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_DESPAWN, 0))
					{
						player->CastSpell(summon, SPELL_CLONE_PLAYER, true);
						// set phase
						summon->SetPhaseMask((1 << (4 + insanityHandled)), true);
					}
				}
				++insanityHandled;
			}
		}

		void ResetPlayersPhaseMask()
		{
			Map::PlayerList const &players = me->GetMap()->GetPlayers();
			for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
			{
				Player* player = i->GetSource();
				player->RemoveAurasDueToSpell(GetSpellForPhaseMask(player->GetPhaseMask()));
			}
		}

		void Reset()
		{
			Slash_Timer = 4000;
			Sliceanddice_Timer = 8000;
			Frenzy_Timer = 20000;


			// Visible for all players in insanity
			me->SetPhaseMask((1 | 16 | 32 | 64 | 128 | 256), true);
			// Used for Insanity handling
			insanityHandled = 0;

			ResetPlayersPhaseMask();

			// Cleanup
			Summons.DespawnAll();
			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			me->SetControlled(false, UNIT_STATE_STUNNED);
		}

		void EnterCombat(Unit* /*who*/)
		{
			me->RemoveAurasDueToSpell(SPELL_STEALTH);
			me->MonsterYell("你好，这位朋友，请允许我给你一个惊喜！", LANG_UNIVERSAL, NULL);
		}

		void JustSummoned(Creature* summon)
		{
			Summons.Summon(summon);
		}

		uint32 GetSpellForPhaseMask(uint32 phase)
		{
			uint32 spell = 0;
			switch (phase)
			{
			case 16:
				spell = SPELL_INSANITY_PHASING_1;
				break;
			case 32:
				spell = SPELL_INSANITY_PHASING_2;
				break;
			case 64:
				spell = SPELL_INSANITY_PHASING_3;
				break;
			case 128:
				spell = SPELL_INSANITY_PHASING_4;
				break;
			case 256:
				spell = SPELL_INSANITY_PHASING_5;
				break;
			}
			return spell;
		}

		void SummonedCreatureDespawn(Creature* summon)
		{
			uint32 phase = summon->GetPhaseMask();
			uint32 nextPhase = 0;
			Summons.Despawn(summon);

			// Check if all summons in this phase killed
			for (SummonList::const_iterator iter = Summons.begin(); iter != Summons.end(); ++iter)
			{
				if (Creature* visage = ObjectAccessor::GetCreature(*me, *iter))
				{
					// Not all are dead
					if (phase == visage->GetPhaseMask())
						return;
					else
						nextPhase = visage->GetPhaseMask();
				}
			}

			// Roll Insanity
			uint32 spell = GetSpellForPhaseMask(phase);
			uint32 spell2 = GetSpellForPhaseMask(nextPhase);
			Map* map = me->GetMap();
			if (!map)
				return;

			Map::PlayerList const &PlayerList = map->GetPlayers();
			if (!PlayerList.isEmpty())
			{
				for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
				{
					if (Player* player = i->GetSource())
					{
						if (player->HasAura(spell))
						{
							player->RemoveAurasDueToSpell(spell);
							if (spell2) // if there is still some different mask cast spell for it
								player->CastSpell(player, spell2, true);
						}
					}
				}
			}
		}

		void UpdateAI(uint32 diff)

		{
			if (!me->GetVictim())
			{
				DoCast(me, SPELL_STEALTH);
			}

			//Return since we have no target
			if (!UpdateVictim())
				return;

			if (insanityHandled)
			{
				if (!Summons.empty())
					return;

				insanityHandled = 0;
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				me->SetControlled(false, UNIT_STATE_STUNNED);
				me->RemoveAurasDueToSpell(INSANITY_VISUAL);
			}

			if (Slash_Timer <= diff)
			{
				DoCast(me->GetVictim(), SPELL_SLASH);
				Slash_Timer = 5000;
			}
			else Slash_Timer -= diff;

			if (Sliceanddice_Timer <= diff)
			{
				DoCast(me, SPELL_SLICEANDDICE);
				Sliceanddice_Timer = 20 * IN_MILLISECONDS;
			}
			else Sliceanddice_Timer -= diff;

			if (Frenzy_Timer <= diff)
			{
				DoCast(me, SPELL_FRENZY);
				Frenzy_Timer = 50 * IN_MILLISECONDS;
			}
			else Frenzy_Timer -= diff;

			DoMeleeAttackIfReady();
		}

		void JustDied(Unit* /*killer*/)
		{
			me->MonsterYell("安息...", LANG_UNIVERSAL, NULL);
			Summons.DespawnAll();
			ResetPlayersPhaseMask();

			Map::PlayerList const &players = me->GetMap()->GetPlayers();
			for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
			{
				Player* player = i->GetSource();
				player->AddItem(44990, 7);
			}
		}

		void KilledUnit(Unit* /*victim*/)
		{

		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new boss_bartimusAI(creature);
	}
};

void AddSC_boss_bartimus()
{
	new boss_bartimus();
}
