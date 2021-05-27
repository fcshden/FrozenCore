#pragma execution_character_set("utf-8")
#include "../PrecompiledHeaders/ScriptPCH.h"




/***********************************************,
* Creator: Zeau (Molten WoW - Deathwing)  *
* Creature: The Dark SAW - My First Boss!  *
* Made because: Eductional purposes.   *
* I learned a lot by creating this script and *
* I hope it will be usefull to you too. Enjoy! *
************************************************'

/* ScriptData
SDName: boss_darksaw
SD%Complete: 90
SDComment: I believe some (if not all) sounds the boss produces will be played amongst all the players @ current map. Im not 100 % sure of this,
anyway it is not a priority to me. (If this 'problem' is true and u fixed it, drop it in comments
SDCategory: Custom Boss
EndScriptData */

/* SQLData README:

For the dialogues to work (most of his talking) you will have to:
1 replace the '99xxx' with your own creatures GUID,
1b Only if your World database name differs from 'world', change 'world.creature_text' also.
2 execute the next query on your database:

REPLACE INTO `world.creature_text` (`entry`, `groupid`, `id`, `text`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES
(99xxx, 0, 0, 'So the Light''s vaunted justice has finally arrived? Shall I lay down Frostmourne and throw myself at your mercy, Fordring?', 14, 0, 0, 0, 0, 17349, 'The Lich King - SAY_LK_INTRO_1'),
(99xxx, 1, 0, 'You''ll learn of that first hand. When my work is complete, you will beg for mercy -- and I will deny you. Your anguished cries will be testament to my unbridled power...', 14, 0, 0, 22, 0, 17350, 'The Lich King - SAY_LK_INTRO_2'),
(99xxx, 2, 0, 'I''ll keep you alive to witness the end, Fordring. I would not want the Light''s greatest champion to miss seeing this wretched world remade in my image.', 14, 0, 0, 0, 0, 17351, 'The Lich King - SAY_LK_INTRO_3'),
(99xxx, 3, 0, 'Come then champions, feed me your rage!', 14, 0, 0, 0, 0, 0, 'The Lich King - SAY_LK_AGGRO'),
(99xxx, 4, 0, 'I will freeze you from within until all that remains is an icy husk!', 14, 0, 0, 0, 0, 17369, 'The Lich King - SAY_LK_REMORSELESS_WINTER'),
(99xxx, 5, 0, 'Watch as the world around you collapses!', 14, 0, 0, 0, 0, 17370, 'The Lich King - SAY_LK_QUAKE'),
(99xxx, 6, 0, 'Val''kyr, your master calls!', 14, 0, 0, 0, 0, 17373, 'The Lich King - SAY_LK_SUMMON_VALKYR'),
(99xxx, 7, 0, 'Frostmourne hungers...', 14, 0, 0, 0, 0, 17366, 'The Lich King - SAY_LK_HARVEST_SOUL'),
(99xxx, 8, 0, 'Argh... Frostmourne, obey me!', 14, 0, 0, 0, 0, 17367, 'The Lich King - SAY_LK_FROSTMOURNE_ESCAPE'),
(99xxx, 9, 0, 'Frostmourne feeds on the soul of your fallen ally!', 14, 0, 0, 0, 0, 17368, 'The Lich King - SAY_LK_FROSTMOURNE_KILL'),
(99xxx, 10, 0, 'Hope wanes!', 14, 0, 0, 0, 0, 17363, 'The Lich King - SAY_LK_KILL'),
(99xxx, 10, 1, 'The end has come!', 14, 0, 0, 0, 0, 17364, 'The Lich King - SAY_LK_KILL'),
(99xxx, 11, 0, 'Face now your tragic end!', 14, 0, 0, 0, 0, 17365, 'The Lich King - SAY_LK_BERSERK'),
(99xxx, 12, 0, '%s begins to cast Defile!', 41, 0, 0, 0, 0, 0, 'The Lich King - EMOTE_DEFILE_WARNING'),
(99xxx, 13, 0, '|TInterface\\Icons\\ability_creature_disease_02.blp:16|tYou have been infected by |cFFCF00FFNecrotic Plague!|r', 42, 0, 0, 0, 0, 0, 'The Lich King - EMOTE_NECROTIC_PLAGUE_WARNING'),
(99xxx, 14, 0, 'No questions remain unanswered. No doubts linger. You ARE Azeroth''s greatest champions. You overcame every challenge I laid before you. My mightiest servants have fallen before your relentless onslaught... your unbridled fury...', 14, 0, 0, 0, 0, 17353, 'The Lich King - SAY_LK_OUTRO_1'),
(99xxx, 15, 0, 'Is it truly the righteousness that drives you? I wonder...', 14, 0, 0, 0, 0, 17354, 'The Lich King - SAY_LK_OUTRO_2'),
(99xxx, 16, 0, 'You trained them well, Fordring. You delivered the greatest fighting force this world has ever known... right into my hands -- exactly as I intended. You shall be rewarded for your unwitting sacrifice.', 14, 0, 0, 0, 0, 17355, 'The Lich King - SAY_LK_OUTRO_3'),
(99xxx, 17, 0, 'Watch now as I raise them from the dead to become masters of the Scourge. They will shroud this world in chaos and destruction. Azeroth''s fall will come at their hands -- and you will be the first to die.', 14, 0, 0, 0, 0, 17356, 'The Lich King - SAY_LK_OUTRO_4'),
(99xxx, 18, 0, 'I delight in the irony...', 14, 0, 0, 0, 0, 17357, 'The Lich King - SAY_LK_OUTRO_5'),
(99xxx, 19, 0, 'Impossible...', 14, 0, 0, 0, 0, 17358, 'The Lich King - SAY_LK_OUTRO_6'),
(99xxx, 20, 0, 'Now I stand, the lion before the lambs... and they do not fear.', 14, 0, 0, 0, 0, 17361, 'The Lich King - SAY_LK_OUTRO_7'),
(99xxx, 21, 0, 'They cannot fear.', 14, 0, 0, 0, 0, 17362, 'The Lich King - SAY_LK_OUTRO_8');

And you are good to go
*/

//Spells
#define SPELL_ICE_ARMOR     36881
#define SPELL_LIGHTNING_BOLT        548
#define SPELL_BLISTERING_COLD       71049
#define SPELL_DARKWINTER   74275
#define SPELL_SHADOW_SPIKE   46589
#define SPELL_BERSERK               45078
#define SPELL_SINBEAM    40827
#define SPELL_SOULSTORM             68872
#define SPELL_SOULSTORM_CHANNEL     69008
#define SPELL_SOULSTORM_VISUAL      68870
#define SPELL_FEAR     68950
#define SPELL_FROST_BEACON   70126
#define SPELL_FRENZY                8269
#define SPELL_WHITEOUT    72034 // 30 % Debuff
#define SPELL_FROZEN_MALLET   71993
#define SPELL_CHAIN_LIGHTNING  33665
#define SPELL_HARVEST_SOUL   68980
#define SPELL_FURY_ANTICHEAT  72350
#define SPELL_RAISE_DEAD   71769
#define SPELL_DRAIN_MANA   5138
#define SPELL_BLIZZARD    26607
#define SPELL_JUMP_TO_TARGET  64430
#define DEATH_GRIP     64431
#define MIRROR_IMAGE    55342
#define MIRROR_IMAGE_PREEFFECT  8677
#define MIRROR_IMAGE_TIMER   180000
// Orb spell info
#define SPELL_ORB_FLARE_PASSIVE   30234

enum Events
{
	// Global Events:
	EVENT_WHITEOUT = 1,
	EVENT_BERSERK = 2,
	EVENT_BLISTER = 3,

	// Phase 1
	EVENT_SHADOW_SPIKE = 4,
	EVENT_SOULSTORM = 5,
	EVENT_FEAR = 6,
	EVENT_FRENZY = 7,
	EVENT_CHAIN_LIGHTNING = 8,

	// Phase 2
	EVENT_BLISTERING_COLD = 9,
	EVENT_DRAIN_MANA = 10,
	EVENT_FROZEN_ORB = 11,
	EVENT_SINBEAM = 12,
	EVENT_FROST_BEACON = 13,

	// Common Events:
	EVENT_RANDOM_CHAT = 14,
	EVENT_INTRO = 15,
	EVENT_COMBAT = 16,
	EVENT_INTROB = 17,
	EVENT_INTROC = 18,
	EVENT_PAYBACK = 19,
	EVENT_HARVEST_SOUL = 20,
	EVENT_REVIVE = 21,
	EVENT_PLAYSOUND_CAST1A = 22,
	EVENT_PLAYSOUND_CAST1B = 23,
	EVENT_SOULFRAGMENT = 24,
	EVENT_MIRROR_IMAGE = 25,
	EVENT_MIRROR_IMAGE_MAINSPELL = 26,
	EVENT_GHOSTLY = 27,
};

// FYI: You can have 1-8 phases max.
enum Phases
{
	PHASE_NONE = 1,
	PHASE_ONE = 2,
	PHASE_TWO = 3,
	PHASE_OWNED = 4,
	PHASE_OUTRO = 5,
};

//Chats
enum Texts
{
	// The Lich King
	SAY_LK_INTRO_1 = 0,
	SAY_LK_INTRO_2 = 1,
	SAY_LK_INTRO_3 = 2,
	SAY_LK_AGGRO = 3,
	SAY_LK_REMORSELESS_WINTER = 4,
	SAY_LK_QUAKE = 5,
	SAY_LK_SUMMON_VALKYR = 6,
	SAY_LK_HARVEST_SOUL = 7,
	SAY_LK_FROSTMOURNE_ESCAPE = 8,
	SAY_LK_FROSTMOURNE_KILL = 9,
	SAY_LK_KILL = 10,
	SAY_LK_BERSERK = 11,
	EMOTE_DEFILE_WARNING = 12,
	EMOTE_NECROTIC_PLAGUE_WARNING = 13,
	SAY_LK_OUTRO_1 = 14,
	SAY_LK_OUTRO_2 = 15,
	SAY_LK_OUTRO_3 = 16,
	SAY_LK_OUTRO_4 = 17,
	SAY_LK_OUTRO_5 = 18,
	SAY_LK_OUTRO_6 = 19,
	SAY_LK_OUTRO_7 = 20,
	SAY_LK_OUTRO_8 = 21,
};

enum MiscData
{
	MUSIC_FROZEN_THRONE = 17457,
	MUSIC_SPECIAL = 17458,
	MUSIC_FURY_OF_FROSTMOURNE = 17459,
	MUSIC_FINAL = 17460,
	EQUIP_ASHBRINGER_GLOWING = 50442,
	EQUIP_BROKEN_FROSTMOURNE = 50840,
};

enum AuraSpells
{
	AURA_TWISTER = 13913,
	AURA_SHADOW = 16592,
	AURA_REFLECTION = 10831,
	AURA_SOULFRAGMENT = 71905,
	AURA_GHOSTLY = 16713,
};

enum AuraSpells_Timers
{
	AURA_SOULFRAGMENT_timer = 6000,
};

// Adds Settings
#define ADD_ID_TOSPAWN   17096
#define ADD_MAX_ADDS   14
#define MIN_PAUSE_CHATTER  16000 // Timer of the random chatter (Do not set too low or you will get crazy).

// Declarations
bool _soulstormed;
bool _frenzied;
bool _berserking; // The scripting for this is not compltete yet so I recommend nog using the according phase yet or write it yourself it is not hard.
bool _reflectdmg; // Same --^
bool _has_orb_adds = false;
bool _playerscheating = false;

int _current_orbs;
int _ticks;
uint32 _inMeleeRange;
uint32 _lasthp;
uint32 _newhp, _hplost;
uint32 _totaldmgtaken;
uint32 _chatter_timer;
uint32 t;

//Modelsmorph
#define MODEL_DARKSAW  30721 // LichKing. In my case not needed, because my creature already has value 30721 as displayID in the database.
#define MODEL_CLONE_DIED 24641 // The spawned Arthas Mirages. We change their model @ death (mostly for visual enjoyment).

// The actual Boss Class:
class boss_darksaw : public CreatureScript
{
public:
	boss_darksaw() : CreatureScript("boss_darksaw"){}

	CreatureAI* GetAI(Creature* pCreature) const
	{
		return new boss_darksawAI(pCreature);
	}

	struct boss_darksawAI : public ScriptedAI
	{
		boss_darksawAI(Creature *c) : ScriptedAI(c) {}

		// Class Inits (Occurs only once @ script init:
		EventMap events;

		uint32 addztimer;
		uint32 _last_lk_special; // Also chatter.


		// What to reset @ first init, after boss died, players died or fled:
		void Reset()
		{
			// First, reset eventsmap
			events.Reset();
			// Walk Speed (Or set in in DB, but do NOT do that is you choose other Model ID than Lich King as I dids):
			me->SetSpeed(MOVE_WALK, 2.6f, false);
			// Act cool:
			me->SetWalk(false);

			// Remove all spells and auras from previous attempts
			me->RemoveAllAuras();

			// set some used variables
			_berserking = false;
			_frenzied = false;
			_soulstormed = false;
			_has_orb_adds = false;
			_playerscheating = false;
			_ticks = 0;

			events.SetPhase(PHASE_NONE);
			me->SetReactState(REACT_AGGRESSIVE);

			// Ghost Effect
			me->AddAura(AURA_GHOSTLY, me);
		}

		void EnterCombat(Unit* who)
		{
			// Stay relaxed:
			me->SetWalk(true);

			// Remove Ghost effect
			events.CancelEvent(EVENT_GHOSTLY);
			me->RemoveAura(AURA_GHOSTLY);

			// Some fool pulled,. (: start the war:
			events.SetPhase(PHASE_ONE);

			DoPlaySoundToSet(me, 17458); // Some intromusic, does not interrupt current music and does not loop.
			me->AddThreat(who, 30.0f);
			me->AI()->AttackStart(me);
			me->setActive(true);

			events.ScheduleEvent(EVENT_INTRO, 100);
		}
		void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType, SpellSchoolMask)
		{
			/* // Not Finished yet,.
			// Cannot lose 1 % HP in 1 tick.. I will cheat!
			if ((me->GetMaxHealth() / 100) <= damage) {
			events.ScheduleEvent(EVENT_PAYBACK, 2500);
			me->Yell("Your end is nigh!",LANG_UNIVERSAL,NULL);
			_playerscheating = true;
			}
			*/


			if (damage > 0) {

				me->AddThreat(me->GetVictim(), 1.0f);

				if (_reflectdmg && attacker->HasAura(AURA_REFLECTION))
				{ // When reflect avtive, cast 10% of damage done back to boss. 
					attacker->DealDamage(attacker,attacker->GetVictim(), damage * 0.1, 0, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_ALL, 0, true);
					// me->MonsterSay("Debug: Aura Powered!..",LANG_UNIVERSAL,NULL);
				}
				/*
				_totaldmgtaken = _totaldmgtaken + damage;
				if (_ticks == 10)
				{
				// After
				}
				*/
			}

			if (!_frenzied && !HealthAbovePct(75))
			{
				// add the shadowaura if not already present:
				if (!me->HasAura(AURA_REFLECTION)) { me->AddAura(AURA_SHADOW, me); }
			}
			if (!_frenzied && !HealthAbovePct(50))
			{
				// schedule the event that changes our phase
				_frenzied = true;
				events.ScheduleEvent(EVENT_FRENZY, 100);
			}
			if (!_berserking && !HealthAbovePct(24))
			{
				// schedule the event that also changes our phase:
				_berserking = true;
				// It works, but atm not recommended. It gives 900% damage buff 
				// Just leave the boolean True at all times.
				// events.ScheduleEvent(EVENT_BERSERK, 100);
			}
			// we're losing health bad, go insane!
			if (!_soulstormed && !HealthAbovePct(15))
			{
				_soulstormed = true;
				events.ScheduleEvent(EVENT_SOULSTORM, 100);
			}
		}

		void JustDied(Unit* /*killer*/)
		{
			events.Reset();
			if (RAND(1, 2) == 1) { Talk(SAY_LK_FROSTMOURNE_ESCAPE); _chatter_timer = 0; } // 50 % chance he will comment on death.
			me->RemoveAllAuras();
		}

		void KilledUnit(Unit* victim)
		{
			if (victim->GetGUID() == me->GetEntry()) // Wanneer een Clone.
			{
				victim->SetDisplayId(MODEL_CLONE_DIED);
			}
			if (victim && victim->GetCreatureType() == CREATURE_TYPE_HUMANOID) //GetTypeId() == TYPEID_PLAYER && me->IsValidAttackTarget(victim))
			{
				me->MonsterSay("哈...你完蛋了...", LANG_UNIVERSAL, NULL);
				if (RAND(1, 2) == 1) Talk(RAND(SAY_LK_FROSTMOURNE_KILL, SAY_LK_HARVEST_SOUL, SAY_LK_KILL)); // 50 % Kans dat er een opmerking komt.
				_chatter_timer = 0;
			}
		}

		//--

		void UpdateAI(uint32 diff)
		{
			// Updates Out of Combat
			if (me->GetAuraCount(AURA_GHOSTLY) < 1)
			{
				if (!UpdateVictim()) { me->AddAura(AURA_GHOSTLY, me); }
			}

			if (!UpdateVictim()) return;


			// Updates IN Combat
			events.Update(diff);

			//Timers Updates/Reset:
			_ticks++;
			_current_orbs = 0;
			_last_lk_special += diff;
			_chatter_timer += diff;


			// Adds and other EVENTless stuff:
			if (_has_orb_adds == true && addztimer <= diff)
			{
				std::list<Creature*> IcicleAddsList;
				GetCreatureListWithEntryInGrid(IcicleAddsList, me, ADD_ID_TOSPAWN, 100.0f);
				for (std::list<Creature*>::iterator itr = IcicleAddsList.begin(); itr != IcicleAddsList.end(); ++itr)
				{
					/*(*itr)->*/ ++_current_orbs; // Optionally, you also can do stuff with (*itr)-> here.
				}

				if (_current_orbs < ADD_MAX_ADDS)
				{
					if (RAND(1, 4) == 1) { events.ScheduleEvent(EVENT_PLAYSOUND_CAST1A, 10); _chatter_timer = 0; }

					//Summon Orb Add
					Creature* AstralOrb = DoSpawnCreature(17096, float(rand() % 47), float(rand() % 47), 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
					Unit* target = NULL; target = SelectTarget(SELECT_TARGET_TOPAGGRO, 0);

					if (AstralOrb && target)
					{
						AstralOrb->CastSpell(AstralOrb, SPELL_ORB_FLARE_PASSIVE, false);
						AstralOrb->AI()->AttackStart(target);
					}
				}
				addztimer = 10000;
			}
			else addztimer -= diff;


			// return back to main code if we're still casting
			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
					// CasterSounds:
				case EVENT_PLAYSOUND_CAST1A:
					t = RAND(14803, 14771, 14751);
					if (t == 14803) { me->MonsterYell("用身体去感受！", LANG_UNIVERSAL, NULL); }
					if (t == 14771) { me->MonsterYell("绝不留情！", LANG_UNIVERSAL, NULL); }
					if (t == 14751) { me->MonsterYell("你的队伍一无是处！", LANG_UNIVERSAL, NULL); }
					events.ScheduleEvent(EVENT_PLAYSOUND_CAST1B, 2500);
					DoPlaySoundToSet(me, t); // Touchy.. / No Mercy. / Your.. Army..
					_chatter_timer = 0;
					break;
				case EVENT_PLAYSOUND_CAST1B:
					_chatter_timer = 0;
					t = RAND(14795, 14786, 14787);
					if (t == 14795) { me->MonsterYell("那么, 你的勇气何在？", LANG_UNIVERSAL, NULL); }
					if (t == 14786) { me->MonsterYell("无论是生是死，你将永无宁日！", LANG_UNIVERSAL, NULL); }
					if (t == 14787) { me->MonsterYell("痛苦，折磨!", LANG_UNIVERSAL, NULL); }
					DoPlaySoundToSet(me, t); // Where is your light now crusaders? / 14786 u will serve me ..
					_chatter_timer = 0;
					break;

					// Berserk Handler:
				case EVENT_BERSERK:
					_reflectdmg = true; // Now reflecting big portion of damage back to LK.

					// Our positive BUFF:
					me->AddAura(AURA_REFLECTION, me->GetVictim());
					me->GetVictim()->BuildAuraStateUpdateForTarget(me->GetVictim());

					// Bosses Negative buff:
					me->RemoveAurasDueToSpell(SPELL_BERSERK);
					me->CastSpell(me, SPELL_BERSERK);

					DoPlaySoundToSet(me, 13878);// Buff Sound
					DoPlaySoundToSet(me, 14805); // LK Sound


					_chatter_timer = 0;
					break;

					// Cheaters payback! ^^ (Disabled)
				case EVENT_PAYBACK:
					/*
					_has_orb_adds = false; // No adds now.
					events.SetPhase(PHASE_OWNED);
					DoCastAOE(SPELL_FURY_ANTICHEAT,true); // Some more overkill
					me->Yell("So, Too can play that game, nib! ;)",LANG_UNIVERSAL,NULL);
					_chatter_timer = 0;
					me->SetWalk(true);
					Talk(SAY_LK_OUTRO_2); // Fake outro,.
					_chatter_timer = 0;
					*/
					break;

					// Random Chatter:
				case EVENT_RANDOM_CHAT:
					if (_chatter_timer > MIN_PAUSE_CHATTER)
					{
						_chatter_timer = 0;
						t = RAND(1, 4);
						if (t == 1){
							Talk(RAND(
								SAY_LK_OUTRO_5,
								SAY_LK_KILL,
								SAY_LK_FROSTMOURNE_KILL,
								SAY_LK_AGGRO));
						}
						else{
							t = RAND(
								14738, // "You have crossed into the world of the dead in search of answers. You wish to save you ally, and have risked life and limb to be here. Allow me to help."
								14745, // "But.. It is not yet your time to serve the Lich King. Yes, a greater destiny awaits you. Power.. You must become more powerful before you are to serve me."
								14754, // "Fail me and it shall be your undoing.."
								14755, // "Succeed and even greater power shall be yours!"

								14760, // Random,. 
								//14770, // "Very well.. warriors of the frozen wastes, rise up! I command you to fight, kill, and die for your master. Let none survive.."
								14773,  // "Mercy is for the weak!"
								14792,  // "Let the destruction of this place, serve as a lesson.. To all those who would dare oppose the scourge.."

								//14799,  // "Come to me, crusaders. I will remake you!"
								14802,  // "Lay down your arms, and surrender your souls."
								14766); // "Be warned.."

							if (t == 14738) { me->MonsterYell("你居然胆敢挑战我，让你死无葬身之地！", LANG_UNIVERSAL, NULL); }
							if (t == 14745) { me->MonsterYell("现在是时候付出代价了！", LANG_UNIVERSAL, NULL); }
							if (t == 14754) { me->MonsterYell("请打败我吧，无敌是多么寂寞...", LANG_UNIVERSAL, NULL); }
							if (t == 14755) { me->MonsterYell("你的力量将为我带来无尽喜悦...", LANG_UNIVERSAL, NULL); }

							if (t == 14760) { me->MonsterYell("LOL...", LANG_UNIVERSAL, NULL); }
							//if (t==14770) { me->Yell("Very well.. warriors of the frozen wastes, rise up! I command you to fight, kill, and die for your master. Let none survive..",LANG_UNIVERSAL,NULL); }
							if (t == 14773) { me->MonsterYell("弱肉强食！", LANG_UNIVERSAL, NULL); }
							if (t == 14792) { me->MonsterYell("我来此只是为了被挑战...", LANG_UNIVERSAL, NULL); }

							//if (t==14799) { me->Yell("Come to me, crusaders. I will remake you!",LANG_UNIVERSAL,NULL); }
							if (t == 14802) { me->MonsterYell("即使是灵魂，也要向我低头！", LANG_UNIVERSAL, NULL); }
							if (t == 14766) { me->MonsterYell("你死定了...", LANG_UNIVERSAL, NULL); }

							DoPlaySoundToSet(me, t); // Touchy.. / No Mercy. / Your.. Army..
							_chatter_timer = 0;
						}
					}
					events.ScheduleEvent(EVENT_RANDOM_CHAT, urand(14000, 16000));
					break;

					// PHASE_ONE:
				case EVENT_CHAIN_LIGHTNING:
					if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
						//me->CastCustomSpell(SPELL_CHAIN_LIGHTNING, SPELLVALUE_MAX_TARGETS, 1, target, false);
						DoCast(me->GetVictim(), SPELL_CHAIN_LIGHTNING);
					events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, urand(7000, 11000), 0, PHASE_ONE);
					break;

				case EVENT_WHITEOUT:
					Talk(RAND(SAY_LK_FROSTMOURNE_ESCAPE, SAY_LK_HARVEST_SOUL)); // Warning - Important.
					DoCast(me, SPELL_WHITEOUT);
					events.ScheduleEvent(EVENT_WHITEOUT, 120000, 0, PHASE_ONE);
					break;

				case EVENT_BLISTERING_COLD:
					_inMeleeRange = 0;
					for (uint8 i = 0; i < 10; ++i)
					{
						if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO, i))
							// check if target is within melee-distance
							if (me->IsWithinMeleeRange(target))
								++_inMeleeRange;
					}
					// trigger spellcast only if we have 1 or more targets to affect..
					if (_inMeleeRange >= 1)
					{ // ..and only if a frost add is near:
						if (Creature *SummonedAdd = GetClosestCreatureWithEntry(me, ADD_ID_TOSPAWN, 3.0f))
						{
							Talk(RAND(SAY_LK_OUTRO_5, SAY_LK_BERSERK));   // Warning - Important.
							DoCast(me->GetVictim(), SPELL_BLISTERING_COLD);
							_chatter_timer = 0;
						}
						else
						{
							//me->Yell("<Blistering Cold Evaded>",LANG_UNIVERSAL,NULL); // Debug.
						}

					}
					events.ScheduleEvent(EVENT_BLISTERING_COLD, urand(10000, 20000));
					break;

				case EVENT_SHADOW_SPIKE:
					if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
						DoCast(target, SPELL_SHADOW_SPIKE);
					events.ScheduleEvent(EVENT_SHADOW_SPIKE, urand(4500, 6000), 0, PHASE_ONE);
					break;

				case EVENT_FEAR:
					//me->Yell("Hahaha...",LANG_UNIVERSAL,NULL);
					DoPlaySoundToSet(me, 14820);
					_chatter_timer = 0;
					if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
						me->CastCustomSpell(SPELL_FEAR, SPELLVALUE_MAX_TARGETS, 1, target, false);
					events.ScheduleEvent(EVENT_FEAR, urand(12000, 48000), 0, PHASE_ONE);
					break;

				case EVENT_DRAIN_MANA:
					DoCastAOE(SPELL_DRAIN_MANA);
					events.ScheduleEvent(EVENT_DRAIN_MANA, urand(10000, 12000), 0, PHASE_TWO);
					break;

				case EVENT_SINBEAM:
					DoCast(me->GetVictim(), SPELL_SINBEAM);
					events.ScheduleEvent(EVENT_SINBEAM, urand(4000, 6000), 0, PHASE_TWO);
					break;

				case EVENT_GHOSTLY:
					me->AddAura(AURA_GHOSTLY, me);
					events.ScheduleEvent(EVENT_GHOSTLY, 55);
					break;

				case EVENT_SOULSTORM:
					_has_orb_adds = false; // From now on, no more ads that cause Blistering Cold explosions.
					// Notice: Resetting all current auras on boss, if you dont, the Soulstorm will most likely crash the client.
					me->RemoveAura(SPELL_BERSERK); // Extra, might not be neccesary.
					me->RemoveAura(AURA_SHADOW); // Extra, better safe than sorry.
					me->RemoveAllAuras();

					Talk(SAY_LK_QUAKE); _chatter_timer = 0;
					me->CastSpell(me, SPELL_SOULSTORM_VISUAL, true);
					me->CastSpell(me, SPELL_SOULSTORM, false);
					break;

				case EVENT_FROST_BEACON:
					if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
						DoCast(target, SPELL_FROST_BEACON);
					events.ScheduleEvent(EVENT_FROST_BEACON, urand(40000, 75000), 0, PHASE_TWO);
					break;

				case EVENT_HARVEST_SOUL:
					Talk(SAY_LK_HARVEST_SOUL); _chatter_timer = 0;
					if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
						// DoCast(target, SPELL_HARVEST_SOUL);
						DoCastAOE(SPELL_HARVEST_SOUL);
					events.ScheduleEvent(EVENT_HARVEST_SOUL, urand(40000, 75000), 0, PHASE_TWO);
					break;

				case EVENT_SOULFRAGMENT:
					if (me->GetAuraCount(AURA_SOULFRAGMENT) < 9)
					{
						events.CancelEvent(EVENT_SOULFRAGMENT);
						DoCast(me, AURA_SOULFRAGMENT);
					}
					events.ScheduleEvent(EVENT_SOULFRAGMENT, AURA_SOULFRAGMENT_timer); // Lasts untill cancelled 
					break;

				case EVENT_MIRROR_IMAGE:
					DoCast(me, MIRROR_IMAGE_PREEFFECT);
					events.ScheduleEvent(EVENT_MIRROR_IMAGE_MAINSPELL, 4800);
					break;
				case EVENT_MIRROR_IMAGE_MAINSPELL:
					for (uint8 i = 0; i < 2; ++i)
					{
						// DoCast(me, MIRROR_IMAGE);
						//Summon Clone, will dissappear after a short while when out of combat.
						Creature* MyDarkClone = DoSpawnCreature(me->GetEntry(), float(rand() % 47), float(rand() % 47), 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
						Unit* Clonetarget = NULL;
						Clonetarget = SelectTarget(SELECT_TARGET_TOPAGGRO, 0);

						if (MyDarkClone && Clonetarget)
						{
							// MyDarkClone->CastSpell(MyDarkClone, SPELL_ORB_FLARE_PASSIVE, false);
							MyDarkClone->AddAura(AURA_GHOSTLY, MyDarkClone);
							MyDarkClone->AI()->AttackStart(Clonetarget);
						}
						events.ScheduleEvent(EVENT_MIRROR_IMAGE, MIRROR_IMAGE_TIMER); // Lasts untill cancelled 
					}
					break;

					//
					// Defining Phases:
					//

					// Phase 2 Events:
				case EVENT_FRENZY:
					events.SetPhase(PHASE_TWO);

					events.ScheduleEvent(EVENT_DRAIN_MANA, 5000, 0, PHASE_TWO);
					events.ScheduleEvent(EVENT_SINBEAM, 2000, 0, PHASE_TWO);
					events.ScheduleEvent(EVENT_FROZEN_ORB, 1500, 0, PHASE_TWO);
					events.ScheduleEvent(EVENT_HARVEST_SOUL, urand(40000, 75000), 0, PHASE_TWO);
					events.ScheduleEvent(EVENT_BLISTERING_COLD, 120000, 0, PHASE_TWO);

					t = RAND(14808, 14800, 14787);
					if (t == 14808) { me->MonsterYell("我要你致歉！！！", LANG_UNIVERSAL, NULL); }
					if (t == 14800) { me->MonsterYell("颤抖吧，凡人！", LANG_UNIVERSAL, NULL); }
					if (t == 14787) { me->MonsterYell("痛苦，折磨！", LANG_UNIVERSAL, NULL); }
					DoPlaySoundToSet(me, t);
					_chatter_timer = 0;
					DoCast(me, SPELL_FRENZY, true);
					break;

					// Phase 1 Events:
				case EVENT_COMBAT:
					me->SetWalk(false);

					// Always running events
					_has_orb_adds = true;
					addztimer = 10000;

					// Starten random chats:
					events.ScheduleEvent(EVENT_RANDOM_CHAT, 21000);

					// If you uncomment next line, you will most certainly die (fast).
					// DoCast(me, SPELL_FROZEN_MALLET); // Will go insane with stacks...

					events.ScheduleEvent(EVENT_SOULFRAGMENT, 100);

					// Phase one events (regular form)
					events.ScheduleEvent(EVENT_MIRROR_IMAGE, 4000);
					events.ScheduleEvent(EVENT_SHADOW_SPIKE, 5000, 0, PHASE_ONE);
					events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, 9000, 0, PHASE_ONE);
					events.ScheduleEvent(EVENT_FEAR, 25000, 0, PHASE_ONE);
					events.ScheduleEvent(EVENT_BLISTERING_COLD, urand(10000, 20000), 0, PHASE_ONE);
					events.ScheduleEvent(EVENT_WHITEOUT, 120000, 0, PHASE_ONE);
					break;

					// Intro Events:
				case EVENT_INTRO:
					Talk(SAY_LK_INTRO_1);
					events.ScheduleEvent(EVENT_INTROB, 12500);
					break;

				case EVENT_INTROB:
					Talk(SAY_LK_INTRO_2);
					events.ScheduleEvent(EVENT_COMBAT, 12500);
					break;

				default:
					break;

				}
			}
			DoMeleeAttackIfReady();
		}
	};
};


void AddSC_boss_darksaw()
{
	new boss_darksaw();
}

