#pragma execution_character_set("utf-8")
#include "CustomScripts.h"
#include "../CommonFunc/CommonFunc.h"

void CustomScript::Cast(Unit* caster, Unit* target, SpellModTypes type, int value)
{
	uint32 spellId;
	SpellValueMod spellValueMod;
	GetSpellInfo(type, spellId, spellValueMod);

	caster->CastCustomSpell(spellId, spellValueMod, value, target);
}

void CustomScript::Cast(Unit* caster, Unit* target, SpellModTypes type)
{
	uint32 spellId = 0;

	switch (type)
	{
	case SMT_STUN:
		spellId = 80106;
		break;
	case SMT_STUN_BREAKABLE:
		spellId = 80107;
	default:
		break;
	}

	caster->CastSpell(target, spellId, TRIGGERED_FULL_MASK);
}

void CustomScript::GetSpellInfo(SpellModTypes type, uint32 &spellId, SpellValueMod &spellValueMod)
{
	switch (type)
	{
	case SMT_DIRECT_DMG:
		spellId = 80101;
		spellValueMod = SPELLVALUE_BASE_POINT0;
		break;
	case SMT_DIRECT_HEAL:
		spellId = 80101;
		spellValueMod = SPELLVALUE_BASE_POINT1;
		break;
	case SMT_MOVE_SPEED_INS:
		spellId = 80102;
		spellValueMod = SPELLVALUE_BASE_POINT0;
		break;
	case SMT_MOVE_SPEED_DES:
		spellId = 80102;
		spellValueMod = SPELLVALUE_BASE_POINT1;
		break;
	case SMT_DMG_PCT:
		spellId = 80103;
		spellValueMod = SPELLVALUE_BASE_POINT0;
		break;
	case SMT_HEAL_PCT:
		spellId = 80103;
		spellValueMod = SPELLVALUE_BASE_POINT1;
		break;
	case SMT_DMG_PCT_TAKEN:
		spellId = 80104;
		spellValueMod = SPELLVALUE_BASE_POINT0;
		break;
	case SMT_HEAL_PCT_TAKEN:
		spellId = 80104;
		spellValueMod = SPELLVALUE_BASE_POINT1;
		break;
	case SMT_ATTACK_SPEED:
		spellId = 80105;
		spellValueMod = SPELLVALUE_BASE_POINT0;
		break;
	default:
		break;
	}
}


class spell_dmg_target : public SpellScriptLoader
{
public:
	spell_dmg_target() : SpellScriptLoader("spell_dmg_target") { }

	class spell_dmg_target_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_dmg_target_SpellScript);

		void HandleScriptEffect(SpellEffIndex /*effIndex*/)
		{
			if (Unit* target = GetHitUnit())
			{
				sCustomScript->Cast(GetCaster(), target, SMT_DIRECT_DMG, 10000);
			}
		}

		void Register()
		{
			OnEffectHitTarget += SpellEffectFn(spell_dmg_target_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_dmg_target_SpellScript();
	}
};

//最大生命值百分比伤害
class spell_dmg_maxhealth_pct : public SpellScriptLoader
{
public:
	spell_dmg_maxhealth_pct() : SpellScriptLoader("spell_dmg_maxhealth_pct") { }

	class spell_dmg_maxhealth_pct_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_dmg_maxhealth_pct_SpellScript);

		void HandleScriptEffect(SpellEffIndex /*effIndex*/)
		{
			if (Unit* target = GetHitUnit())
			{
				float multi = 0.0f;

				switch (m_scriptSpellId)
				{
				
				default:
					break;
				}

				int32 damage = int32(target->GetMaxHealth() * multi);

				if (damage <= 0)
					return;

				sCustomScript->Cast(GetCaster(), target, SMT_DIRECT_DMG, damage);
			}
		}

		void Register()
		{
			OnEffectHitTarget += SpellEffectFn(spell_dmg_maxhealth_pct_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_dmg_maxhealth_pct_SpellScript();
	}
};

//当前生命值百分比伤害
class spell_dmg_health_pct : public SpellScriptLoader
{
public:
	spell_dmg_health_pct() : SpellScriptLoader("spell_dmg_health_pct") { }

	class spell_dmg_health_pct_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_dmg_health_pct_SpellScript);

		void HandleScriptEffect(SpellEffIndex /*effIndex*/)
		{
			if (Unit* target = GetHitUnit())
			{
				if (target->GetTypeId() == TYPEID_UNIT)
					if (target->ToCreature()->isWorldBoss() || target->ToCreature()->IsDungeonBoss())
						return;

				float multi = 0.0f;

				switch (m_scriptSpellId)
				{
				case 113011:
					multi = 0.05f;
					break;
				case 113012:
					multi = 0.1f;
					break;
				case 113013:
					multi = 0.15f;
					break;
				case 113014:
					multi = 0.2f;
					break;
				default:
					break;
				}

				const int32 damage = int32(target->GetHealth() * multi);

				const int32 selfHp = GetCaster()->GetMaxHealth();

				int32 value = int32(std::min(damage, selfHp) * multi);

				if (value <= 0)
					return;

				sCustomScript->Cast(GetCaster(), target, SMT_DIRECT_DMG, value);
			}
		}

		void Register()
		{
			OnEffectHitTarget += SpellEffectFn(spell_dmg_health_pct_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_dmg_health_pct_SpellScript();
	}
};

//损失生命值百分比伤害
class spell_dmg_los_health_pct : public SpellScriptLoader
{
public:
	spell_dmg_los_health_pct() : SpellScriptLoader("spell_dmg_los_health_pct") { }

	class spell_dmg_los_health_pct_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_dmg_los_health_pct_SpellScript);

		void HandleScriptEffect(SpellEffIndex /*effIndex*/)
		{
			if (Unit* target = GetHitUnit())
			{
				if (target->GetTypeId() == TYPEID_UNIT)
					if (target->ToCreature()->isWorldBoss() || target->ToCreature()->IsDungeonBoss())
						return;

				float multi = 0.0f;

				switch (m_scriptSpellId)
				{
				case 111011:
					multi = 0.2f;
					break;
				case 111012:
					multi = 0.3f;
					break;
				case 111013:
					multi = 0.4f;
					break;
				case 111014:
					multi = 0.5f;
					break;
				default:
					break;
				}

				const int32 losHp = target->GetMaxHealth() - target->GetHealth();
				const int32 selfHp = GetCaster()->GetMaxHealth();

				int32 damage = int32(std::min(losHp, selfHp) * multi);

				if (damage <= 0)
					return;

				sCustomScript->Cast(GetCaster(), target, SMT_DIRECT_DMG, damage);
			}
		}

		void Register()
		{
			OnEffectHitTarget += SpellEffectFn(spell_dmg_los_health_pct_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_dmg_los_health_pct_SpellScript();
	}
};


//最大生命值百分比治疗
class spell_heal_maxhealth_pct : public SpellScriptLoader
{
public:
	spell_heal_maxhealth_pct() : SpellScriptLoader("spell_heal_maxhealth_pct") { }

	class spell_heal_maxhealth_pct_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_heal_maxhealth_pct_SpellScript);

		void HandleScriptEffect(SpellEffIndex /*effIndex*/)
		{
			if (Unit* target = GetHitUnit())
			{
				float multi = 0.0f;

				switch (m_scriptSpellId)
				{
				
				default:
					break;
				}

				int32 heal = int32(target->GetMaxHealth() * multi);

				if (heal <= 0)
					return;

				sCustomScript->Cast(GetCaster(), target, SMT_DIRECT_HEAL, heal);
			}
		}

		void Register()
		{
			OnEffectHitTarget += SpellEffectFn(spell_heal_maxhealth_pct_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_heal_maxhealth_pct_SpellScript();
	}
};

//当前生命值百分比治疗
class spell_heal_health_pct : public SpellScriptLoader
{
public:
	spell_heal_health_pct() : SpellScriptLoader("spell_heal_health_pct") { }

	class spell_heal_health_pct_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_heal_health_pct_SpellScript);

		void HandleScriptEffect(SpellEffIndex /*effIndex*/)
		{
			if (Unit* target = GetHitUnit())
			{
				float multi = 0.0f;

				switch (m_scriptSpellId)
				{
				
				default:
					break;
				}

				int32 heal = int32(target->GetHealth() * multi);

				if (heal <= 0)
					return;

				sCustomScript->Cast(GetCaster(), target, SMT_DIRECT_HEAL, heal);
			}
		}

		void Register()
		{
			OnEffectHitTarget += SpellEffectFn(spell_heal_health_pct_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_heal_health_pct_SpellScript();
	}
};

//损失生命值百分比治疗
class spell_heal_los_health_pct : public SpellScriptLoader
{
public:
	spell_heal_los_health_pct() : SpellScriptLoader("spell_heal_los_health_pct") { }

	class spell_heal_los_health_pct_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_heal_los_health_pct_SpellScript);

		void HandleScriptEffect(SpellEffIndex /*effIndex*/)
		{
			if (Unit* target = GetHitUnit())
			{
				float multi = 0.0f;

				switch (m_scriptSpellId)
				{
				
				default:
					break;
				}

				int32 heal = int32((target->GetMaxHealth() - target->GetHealth()) * multi);

				if (heal <= 0)
					return;

				sCustomScript->Cast(GetCaster(), target, SMT_DIRECT_HEAL, heal);
			}
		}

		void Register()
		{
			OnEffectHitTarget += SpellEffectFn(spell_heal_los_health_pct_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_heal_los_health_pct_SpellScript();
	}
};


class spell_speed_des : public SpellScriptLoader
{
public:
	spell_speed_des() : SpellScriptLoader("spell_speed_des") { }

	class spell_speed_des_AuraScript : public AuraScript
	{
		PrepareAuraScript(spell_speed_des_AuraScript);

		void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*handle*/)
		{
			sCustomScript->Cast(GetCaster(), GetTarget(), SMT_MOVE_SPEED_INS, 100);
		}

		void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*handle*/)
		{
			GetTarget()->RemoveAura(80102);
		}

		void Register()
		{
			OnEffectApply += AuraEffectApplyFn(spell_speed_des_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
			OnEffectRemove += AuraEffectRemoveFn(spell_speed_des_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
		}
	};

	AuraScript* GetAuraScript() const
	{
		return new spell_speed_des_AuraScript();
	}
};


Creature* GetNearestSummon(Player* player, SummonList Summons, Creature* summoner)
{
	for (SummonList::const_iterator itr = Summons.begin(); itr != Summons.end(); ++itr)
	{
		if (Creature* summon = ObjectAccessor::GetCreature(*summoner, *itr))
		{
			bool flag = true;

			for (SummonList::const_iterator ii = Summons.begin(); ii != Summons.end(); ++ii)
			{
				if (Creature* summon_ii = ObjectAccessor::GetCreature(*summoner, *ii))
				{
					if (summon->GetDistance(player) > summon_ii->GetDistance(player))
						flag = false;
				}
			}

			if (flag)
				return summon;
		}
	}

	return NULL;
}

class spell_dest_dest : public SpellScriptLoader
{
public:
	spell_dest_dest() : SpellScriptLoader("spell_dest_dest") { }

	class spell_dest_dest_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_dest_dest_SpellScript);

		void HandleScriptEffect(SpellEffIndex /*effIndex*/)
		{
			WorldLocation const* dest = GetHitDest();

			if (!dest)
				return;

			Unit* caster = GetCaster();

			if (!caster)
				return;

			uint32 entry = 970000 + GetSpell()->GetSpellInfo()->GetRank();

			if (Creature* trigger = caster->SummonCreature(entry, *dest, TEMPSUMMON_TIMED_DESPAWN, 20000))
			{
				trigger->SetCreatorGUID(caster->GetGUID());
				trigger->setFaction(caster->getFaction());
			}
				
		}

		void Register()
		{
			OnEffectHit += SpellEffectFn(spell_dest_dest_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_dest_dest_SpellScript();
	}
};



class zhenfa_leiyun_trigger : public CreatureScript
{
public:
	zhenfa_leiyun_trigger() : CreatureScript("zhenfa_leiyun_trigger") { }
	struct zhenfa_leiyun_triggerAI : public ScriptedAI
	{
		zhenfa_leiyun_triggerAI(Creature* creature) : ScriptedAI(creature), Summons(me)
		{}

		SummonList Summons;
		uint32 Timer = 0;
		uint32 map = me->GetMapId();
		float x = me->GetPositionX();
		float y = me->GetPositionY();
		float z = me->GetPositionZ();
		uint32 respanTime = 20000;

		void Summon()
		{
			uint8 max_i = 15;

			for (uint8 i = 0; i < max_i; i++)
			{
				float x1 = x + 10 * cos(2 * M_PI * i / max_i);
				float y1 = y + 10 * sin(2 * M_PI * i / max_i);
				float z1 = z;
				if (Creature* summon = me->SummonCreature(960059, x1, y1, z1, 0, TEMPSUMMON_TIMED_DESPAWN, respanTime))
				{
					summon->SetCanFly(true);
					summon->SetDisableGravity(true);
					summon->SetHover(true);
					summon->SendMovementFlagUpdate();
				}
			}
		}

		void JustSummoned(Creature *summon) override
		{
			if (Player* creator = ObjectAccessor::FindPlayer(me->GetCreatorGUID()))
				summon->setFaction(creator->getFaction());

			Summons.Summon(summon);
		}


		void AttackPlayer()
		{
			Player* creator = ObjectAccessor::FindPlayer(me->GetCreatorGUID());

			if (!creator)
				return;

			std::list<Player*> playersNearby;
			me->GetPlayerListInGrid(playersNearby, 15.0f, true);

			if (!playersNearby.empty())
				for (std::list<Player*>::iterator iter = playersNearby.begin(); iter != playersNearby.end(); ++iter)
				{
					Player* player = *iter;

					if (!player)
						continue;

					if (!player->IsHostileTo(creator))
						continue;

					if (Creature* attacker = GetNearestSummon(player, Summons, me))
						attacker->CastSpell(player, 53072, TRIGGERED_FULL_MASK);
								
				}
		}

		void Reset() override
		{
			Summons.DespawnAll();
			Summon();
			Timer = 0;
		}

		void UpdateAI(uint32 diff) override
		{
			Timer += diff;

			if (Timer > 1000)
			{
				Timer = 0;
				AttackPlayer();
			}
		}		
	};

	CreatureAI* GetAI(Creature* creature) const override
	{
		return new zhenfa_leiyun_triggerAI(creature);
	}
};

class zhenfa_xuanguang_trigger : public CreatureScript
{
public:
	zhenfa_xuanguang_trigger() : CreatureScript("zhenfa_xuanguang_trigger") { }
	struct zhenfa_xuanguang_triggerAI : public ScriptedAI
	{
		zhenfa_xuanguang_triggerAI(Creature* creature) : ScriptedAI(creature), Summons(me)
		{}

		SummonList Summons;
		uint32 Timer = 0;
		uint32 map = me->GetMapId();
		float x = me->GetPositionX();
		float y = me->GetPositionY();
		float z = me->GetPositionZ();
		uint32 respanTime = 20000;

		void Summon()
		{
			uint8 max_i = 15;

			for (uint8 i = 0; i < max_i; i++)
			{
				float x1 = x + 8 * cos(2 * M_PI * i / max_i);
				float y1 = y + 8 * sin(2 * M_PI * i / max_i);
				float z1 = z;
				//if (Creature* summon = me->SummonCreature(1, x1, y1, z1, 0, TEMPSUMMON_TIMED_DESPAWN, respanTime))
				//{
				//	summon->SetCanFly(true);
				//	summon->SetDisableGravity(true);
				//	summon->SetHover(true);
				//	summon->SendMovementFlagUpdate();
					me->SummonGameObject(400001, x1, y1, z1 - 5, 0, 0, 0, 0, 0, respanTime / IN_MILLISECONDS);
				//}
			}
		}

		void JustSummoned(Creature *summon) override
		{
			if (Player* creator = ObjectAccessor::FindPlayer(me->GetCreatorGUID()))
				summon->setFaction(creator->getFaction());

			Summons.Summon(summon);
		}

		void AttackPlayer()
		{
			Player* creator = ObjectAccessor::FindPlayer(me->GetCreatorGUID());

			if (!creator)
				return;

			std::list<Player*> playersNearby;
			me->GetPlayerListInGrid(playersNearby, 8.0f, true);

			if (!playersNearby.empty())
				for (std::list<Player*>::iterator iter = playersNearby.begin(); iter != playersNearby.end(); ++iter)
				{
					Player* player = *iter;

					if (!player)
						continue;

					if (!player->IsHostileTo(creator))
						continue;

					me->CastSpell(player, 26108, TRIGGERED_FULL_MASK);
				}
		}

		void Reset() override
		{
			Summons.DespawnAll();
			Summon();
			Timer = 6000;
		}

		void UpdateAI(uint32 diff) override
		{
			Timer += diff;

			if (Timer > 6000)
			{
				Timer = 0;
				AttackPlayer();
			}
		}
	};

	CreatureAI* GetAI(Creature* creature) const override
	{
		return new zhenfa_xuanguang_triggerAI(creature);
	}
};

std::unordered_map<uint32, CircleTemplate> CircleDataMap;

void CustomScript::LoadCircleData()
{
	CircleDataMap.clear();
	//QueryResult result = WorldDatabase.PQuery("SELECT entry,respawnTime,searchRange,creatureId,creatureCount,creatureRadius,gameobjectId,gameobjectCount,gameobjectRadius,"
	//	"spellId1,spellInterval1,spellFriendly1,spellId2,spellInterval2,spellFriendly2,spellId3,spellInterval3,spellFriendly3,spellDelay1,spellDelay2,spellDelay3,"
	//	"creatureOffsetZ,gameobjectOffsetZ,creatureScale,gameobjectScale FROM _circle_script");
	//
	//if (!result) 
	//	return;
	//do
	//{
	//	Field* fields = result->Fetch();
	//	uint32 entry = fields[0].GetUInt32();
	//
	//	CircleTemplate Temp;
	//	Temp.respawnTime		= fields[1].GetUInt32();
	//	Temp.range				= fields[2].GetFloat();
	//	Temp.creatureId			= fields[3].GetUInt32();
	//	Temp.creatureCount		= fields[4].GetUInt32();
	//	Temp.creatureRadius		= fields[5].GetFloat();
	//	Temp.gameobjectId		= fields[6].GetUInt32();
	//	Temp.gameobjectCount	= fields[7].GetUInt32();
	//	Temp.gameobjectRadius	= fields[8].GetFloat();
	//	Temp.spellId1			= fields[9].GetUInt32();
	//	Temp.spellInterval1		= fields[10].GetUInt32();
	//	Temp.spellFriendly1		= fields[11].GetBool();
	//	Temp.spellId2			= fields[12].GetUInt32();
	//	Temp.spellInterval2		= fields[13].GetUInt32();
	//	Temp.spellFriendly2		= fields[14].GetBool();
	//	Temp.spellId3			= fields[15].GetUInt32();
	//	Temp.spellInterval3		= fields[16].GetUInt32();
	//	Temp.spellFriendly3		= fields[17].GetBool();
	//	Temp.spellDelay1		= fields[18].GetUInt32();
	//	Temp.spellDelay2		= fields[19].GetUInt32();
	//	Temp.spellDelay3		= fields[20].GetUInt32();
	//	Temp.creatureOffsetZ	= fields[21].GetFloat();
	//	Temp.gameobjectOffsetZ	= fields[22].GetFloat();
	//	Temp.creatureScale		= fields[23].GetFloat();
	//	Temp.gameobjectScale	= fields[24].GetFloat();
	//	CircleDataMap.insert(std::make_pair(entry, Temp));
	//} while (result->NextRow());
}

class CircleTrigger : public CreatureScript
{
public:
	CircleTrigger() : CreatureScript("CircleTrigger") { }
	struct CircleTriggerAI : public ScriptedAI
	{
		CircleTriggerAI(Creature* creature) : ScriptedAI(creature), Summons(me)
		{}

		SummonList Summons;
		uint32 map = me->GetMapId();
		float x = me->GetPositionX();
		float y = me->GetPositionY();
		float z = me->GetPositionZ();
		int32 spellTimer1 = 0;
		int32 spellTimer2 = 0;
		int32 spellTimer3 = 0;

		float range = 0;
		uint32 spellId1 = 0;
		bool spellFriendly1 = false;
		uint32 spellInterval1 = 0;
		uint32 spellDelay1 = 0;
		uint32 spellId2 = 0;
		bool spellFriendly2 = false;
		uint32 spellInterval2 = 0;
		uint32 spellDelay2 = 0;
		uint32 spellId3 = 0;
		bool spellFriendly3 = false;
		uint32 spellInterval3 = 0;
		uint32 spellDelay3 = 0;



		void Summon()
		{
			std::unordered_map<uint32, CircleTemplate>::iterator iter = CircleDataMap.find(me->GetEntry());

			if (iter != CircleDataMap.end())
			{
				for (uint8 i = 0; i < iter->second.creatureCount; i++)
				{
					float x1 = x + iter->second.creatureRadius * cos(2 * M_PI * i / iter->second.creatureCount);
					float y1 = y + iter->second.creatureRadius * sin(2 * M_PI * i / iter->second.creatureCount);
					float z1 = z + iter->second.creatureOffsetZ;

					if (Creature* summon = me->SummonCreature(iter->second.creatureId, x1, y1, z1, 0, TEMPSUMMON_TIMED_DESPAWN, iter->second.respawnTime))
					{
						summon->SetOrientation(summon->GetAngle(me));
						summon->SetObjectScale(iter->second.creatureScale);
						summon->SetCanFly(true);
						summon->SetDisableGravity(true);
						summon->SetHover(true);
						summon->SendMovementFlagUpdate();
					}
				}

				for (uint8 i = 0; i < iter->second.gameobjectCount; i++)
				{
					float x1 = x;
					float y1 = y;
					float z1 = z + iter->second.gameobjectOffsetZ;

					if (iter->second.creatureCount == 0)
					{
						x1 += iter->second.gameobjectRadius * cos(2 * M_PI * i / iter->second.gameobjectCount);
						y1 += iter->second.gameobjectRadius * sin(2 * M_PI * i / iter->second.gameobjectCount);

						if (GameObject* gob = me->SummonGameObject(iter->second.gameobjectId, x1, y1, z1, 2 * M_PI * i / iter->second.gameobjectCount, 0, 0, 0, 0, iter->second.respawnTime / IN_MILLISECONDS))
						{
							gob->SetOrientation(gob->GetAngle(me));
							gob->SetObjectScale(iter->second.gameobjectScale);
						}
					}
					else
					{
						x1 += iter->second.gameobjectRadius * cos(M_PI / iter->second.creatureCount + 2 * M_PI * i / iter->second.gameobjectCount);
						y1 += iter->second.gameobjectRadius * sin(M_PI / iter->second.creatureCount + 2 * M_PI * i / iter->second.gameobjectCount);

						if (GameObject* gob = me->SummonGameObject(iter->second.gameobjectId, x1, y1, z1, M_PI / iter->second.creatureCount + 2 * M_PI * i / iter->second.gameobjectCount, 0, 0, 0, 0, iter->second.respawnTime / IN_MILLISECONDS))
						{
							gob->SetOrientation(gob->GetAngle(me));
							gob->SetObjectScale(iter->second.gameobjectScale);
						}
					}
				}
			}
		}

		void JustSummoned(Creature *summon) override
		{
			if (Player* creator = ObjectAccessor::FindPlayer(me->GetCreatorGUID()))
				summon->setFaction(creator->getFaction());

			Summons.Summon(summon);
		}

		void CastSpell(uint32 index)
		{
			Player* creator = ObjectAccessor::FindPlayer(me->GetCreatorGUID());

			if (!creator)
				return;

			uint32 spellId = 0;
			bool spellFriendly = false;

			switch (index)
			{
			case 1:
				spellId = spellId1;
				spellFriendly = spellFriendly1;
				break;
			case 2:
				spellId = spellId2;
				spellFriendly = spellFriendly2;
				break;
			case 3:
				spellId = spellId3;
				spellFriendly = spellFriendly3;
				break;
			default:
				break;
			}

			std::list<Player*> playersNearby;
			me->GetPlayerListInGrid(playersNearby, range, true);

			if (!playersNearby.empty())
			{
				for (std::list<Player*>::iterator ii = playersNearby.begin(); ii != playersNearby.end(); ++ii)
				{
					Player* player = *ii;

					if (!player)
						continue;

					if (player->IsHostileTo(creator) && !spellFriendly)
						me->CastSpell(player, spellId, TRIGGERED_FULL_MASK);

					if (player->IsFriendlyTo(creator) && spellFriendly)
						me->CastSpell(player, spellId, TRIGGERED_FULL_MASK);
				}
			}		
		}

		void Reset() override
		{
			std::unordered_map<uint32, CircleTemplate>::iterator iter = CircleDataMap.find(me->GetEntry());

			if (iter != CircleDataMap.end())
			{
				range = iter->second.range;

				spellId1 = iter->second.spellId1;
				spellFriendly1 = iter->second.spellFriendly1;
				spellInterval1 = iter->second.spellInterval1;
				spellDelay1 = iter->second.spellDelay1;

				spellId2 = iter->second.spellId2;
				spellFriendly2 = iter->second.spellFriendly2;
				spellInterval2 = iter->second.spellInterval2;
				spellDelay2 = iter->second.spellDelay2;

				spellId3 = iter->second.spellId3;
				spellFriendly3 = iter->second.spellFriendly3;
				spellInterval3 = iter->second.spellInterval3;
				spellDelay3 = iter->second.spellDelay3;

				spellTimer1 = spellInterval1 - spellDelay1;
				spellTimer2 = spellInterval2 - spellDelay2;
				spellTimer3 = spellInterval3 - spellDelay3;
			}

			Summons.DespawnAll();
			Summon();
		}

		void UpdateAI(uint32 diff) override
		{
			if (spellInterval1 > 0)
			{
				spellTimer1 += diff;

				if (spellTimer1 > spellInterval1)
				{
					spellTimer1 = 0;
					CastSpell(1);
				}
			}
			
			if (spellInterval2 > 0)
			{
				spellTimer2 += diff;

				if (spellTimer2 > spellInterval2)
				{
					spellTimer2 = 0;
					CastSpell(2);
				}
			}

			if (spellInterval3 > 0)
			{
				spellTimer3 += diff;

				if (spellTimer3 > spellInterval3)
				{
					spellTimer3 = 0;
					CastSpell(3);
				}
			}
		}
	};

	CreatureAI* GetAI(Creature* creature) const override
	{
		return new CircleTriggerAI(creature);
	}
};


void CustomScript::CastSpellToNearbyPlayers(Unit* caster, uint32 e_spellId, uint32 f_spellId, float range)
{
	std::list<Player*> playersNearby;
	caster->GetPlayerListInGrid(playersNearby, range, true);
	if (!playersNearby.empty())
		for (std::list<Player*>::iterator itr = playersNearby.begin(); itr != playersNearby.end(); ++itr)
		{
			Player* pl = *itr;

			if (!pl->IsInWorld())
				continue;

			if (caster->IsFriendlyTo(pl))
			{
				if (pl->HasAura(f_spellId))
					pl->RemoveAura(f_spellId);
				caster->CastSpell(pl, f_spellId, true);
			}
			else
			{
				if (pl->HasAura(e_spellId))
					pl->RemoveAura(e_spellId);
				caster->CastSpell(pl, e_spellId, true);
			}
		}
}

void CustomScript::CastSpellToNearbyUnits(Unit* caster, uint32 e_spellId, uint32 f_spellId, float range)
{
	std::list<Creature*> creaturesNearby;
	caster->GetCreatureListInGrid(creaturesNearby, range);
	if (!creaturesNearby.empty())
		for (std::list<Creature*>::iterator itr = creaturesNearby.begin(); itr != creaturesNearby.end(); ++itr)
		{
			Creature* creature = *itr;

			if (!creature->IsInWorld())
				continue;

			if (!creature->IsAlive())
				continue;

			if (creature->isWorldBoss() || creature->IsDungeonBoss())
				continue;

			if (caster->IsFriendlyTo(creature))
			{
				if (creature->HasAura(f_spellId))
					creature->RemoveAura(f_spellId);
				caster->CastSpell(creature, f_spellId, true);
			}
			else
			{
				if (creature->HasAura(e_spellId))
					creature->RemoveAura(e_spellId);
				caster->CastSpell(creature, e_spellId, true);
			}
		}
}

class spell_liangqiping : public SpellScriptLoader
{
public:
	spell_liangqiping() : SpellScriptLoader("spell_liangqiping") { }

	class spell_liangqiping_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_liangqiping_SpellScript);

		void Action()
		{
			Unit* caster = GetCaster();
			if (!caster)
				return;
			uint32 e_spellId = 0;
			uint32 f_spellId = 0;

			switch (m_scriptSpellId)
			{
			case 117001:
				e_spellId = 117031;
				f_spellId = 117021;
				break;
			case 117002:
				e_spellId = 117032;
				f_spellId = 117022;
				break;
			case 117003:
				e_spellId = 117033;
				f_spellId = 117023;
				break;
			case 117004:
				e_spellId = 117034;
				f_spellId = 117024;
				break;
			default:
				break;
			}

			sCustomScript->CastSpellToNearbyPlayers(caster, e_spellId, f_spellId, 8);
			sCustomScript->CastSpellToNearbyUnits(caster, e_spellId, f_spellId, 8);
		}

		void Register()
		{
			OnCast += SpellCastFn(spell_liangqiping_SpellScript::Action);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_liangqiping_SpellScript();
	}
};

class spell_liangyihuan : public SpellScriptLoader
{
public:
	spell_liangyihuan() : SpellScriptLoader("spell_liangyihuan") { }

	class spell_liangyihuan_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_liangyihuan_SpellScript);

		void Action()
		{
			Unit* caster = GetCaster();
			if (!caster)
				return;

			sCustomScript->CastSpellToNearbyPlayers(caster, 81123, 81124, 8);
		}

		void Register()
		{
			OnCast += SpellCastFn(spell_liangyihuan_SpellScript::Action);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_liangyihuan_SpellScript();
	}
};


class spell_leihuogong : public SpellScriptLoader
{
public:
	spell_leihuogong() : SpellScriptLoader("spell_leihuogong") { }

	class spell_leihuogong_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_leihuogong_SpellScript);

		void HandleScriptEffect(SpellEffIndex /*effIndex*/)
		{
			if (Unit* target = GetHitUnit())
			{
				float mul = 0;

				switch (m_scriptSpellId)
				{
				case 125001:
					mul = 0.1;
					break;
				case 125002:
					mul = 0.2;
					break;
				case 125003:
					mul = 0.3;
					break;
				case 125004:
					mul = 0.4;
					break;
				default:
					break;
				}

				int32 dmg = int32(target->GetHealth() * mul);

				if (dmg <= 0)
					return;

				sCustomScript->Cast(GetCaster(), target, SMT_DIRECT_DMG, dmg);
			}
		}

		void Register()
		{
			OnEffectHitTarget += SpellEffectFn(spell_leihuogong_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_leihuogong_SpellScript();
	}
};


class spell_absorb_max_hp_pct_damage : public SpellScriptLoader
{
public:
	spell_absorb_max_hp_pct_damage() : SpellScriptLoader("spell_absorb_max_hp_pct_damage") { }

	class spell_absorb_max_hp_pct_damage_AuraScript : public AuraScript
	{
		PrepareAuraScript(spell_absorb_max_hp_pct_damage_AuraScript);

		void CalculateAmount(AuraEffect const* aurEff, int32& amount, bool& canBeRecalculated)
		{
			canBeRecalculated = false;
			if (Unit* caster = GetCaster())
			{
				float muil = 0;
				switch (m_scriptSpellId)
				{
				case 89580: muil = 0.05; break;
				case 89581: muil = 0.10; break;
				case 89582: muil = 0.15; break;
				case 89583: muil = 0.20; break;
				case 89584: muil = 0.25; break;
				case 89585: muil = 0.30; break;
				case 89586: muil = 0.35; break;
				case 89587: muil = 0.40; break;
				case 89588: muil = 0.45; break;
				case 89589: muil = 0.50; break;
				default: break;
				}

				amount = caster->GetMaxHealth() * muil;
			}
				
		}
		void Register()
		{
			DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_absorb_max_hp_pct_damage_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
		}
	};

	AuraScript* GetAuraScript() const
	{
		return new spell_absorb_max_hp_pct_damage_AuraScript();
	}

};


class spell_item_socket : public SpellScriptLoader
{
public:
	spell_item_socket() : SpellScriptLoader("spell_item_socket") { }

	class spell_item_socket_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_item_socket_SpellScript);

		SpellCastResult CheckCast()
		{
			Player* owner = GetCaster()->ToPlayer();
			Item* targetItem = GetExplTargetItem();
			Item* castItem = GetCastItem();

			if (!owner || !targetItem || !castItem || targetItem->GetGUID() == castItem->GetGUID())
				return SPELL_FAILED_BAD_TARGETS;

			uint32 count = 0;
			const ItemTemplate* proto = targetItem->GetTemplate();

			for (size_t i = 0; i < MAX_ITEM_PROTO_SOCKETS; i++)
				if (proto->Socket[i].Color != 0)
					count++;

			if (count == MAX_ITEM_PROTO_SOCKETS || targetItem->GetEnchantmentId(PRISMATIC_ENCHANTMENT_SLOT))
				return SPELL_FAILED_MAX_SOCKETS;

			owner->ApplyEnchantment(targetItem, PRISMATIC_ENCHANTMENT_SLOT, false);
			targetItem->SetEnchantment(PRISMATIC_ENCHANTMENT_SLOT, 3729, 0, 0, owner->GetGUID());
			owner->ApplyEnchantment(targetItem, PRISMATIC_ENCHANTMENT_SLOT, true);

			return SPELL_CAST_OK;
		}

		void Register() override
		{
			OnCheckCast += SpellCheckCastFn(spell_item_socket_SpellScript::CheckCast);
		}
	};

	SpellScript* GetSpellScript() const override
	{
		return new spell_item_socket_SpellScript();
	}
};
/*
#define PRISMATIC_ENCHANTMENT_1 4555
#define PRISMATIC_ENCHANTMENT_2 4556
#define PRISMATIC_ENCHANTMENT_3 4557

class spell_item_socket : public SpellScriptLoader
{
public:
	spell_item_socket() : SpellScriptLoader("spell_item_socket") { }

	class spell_item_socket_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_item_socket_SpellScript);

		SpellCastResult CheckCast()
		{
			Player* owner = GetCaster()->ToPlayer();
			Item* targetItem = GetExplTargetItem();
			Item* castItem = GetCastItem();

			if (!owner || !targetItem || !castItem || targetItem->GetGUID() == castItem->GetGUID())
				return SPELL_FAILED_BAD_TARGETS;

			uint32 count = 0;
			const ItemTemplate* proto = targetItem->GetTemplate();

			for (size_t i = 0; i < MAX_ITEM_PROTO_SOCKETS; i++)
				if (proto->Socket[i].Color != 0)
					count++;

			uint32 prismaticCount = 0;
			uint32 prismatic = targetItem->GetEnchantmentId(PRISMATIC_ENCHANTMENT_SLOT);

			if (SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(prismatic))
				for (uint8 s = 0; s < MAX_ITEM_ENCHANTMENT_EFFECTS; ++s)
					if (enchantEntry->type[s] == ITEM_ENCHANTMENT_TYPE_PRISMATIC_SOCKET)
						prismaticCount += enchantEntry->amount[s];

			if (count + prismaticCount >= MAX_ITEM_PROTO_SOCKETS)
				return SPELL_FAILED_MAX_SOCKETS;

			owner->ApplyEnchantment(targetItem, PRISMATIC_ENCHANTMENT_SLOT, false);

			switch (prismaticCount)
			{
			case 0:
				targetItem->SetEnchantment(PRISMATIC_ENCHANTMENT_SLOT, PRISMATIC_ENCHANTMENT_1, 0, 0, owner->GetGUID());
				break;
			case 1:
				targetItem->SetEnchantment(PRISMATIC_ENCHANTMENT_SLOT, PRISMATIC_ENCHANTMENT_2, 0, 0, owner->GetGUID());
				break;
			case 2:
				targetItem->SetEnchantment(PRISMATIC_ENCHANTMENT_SLOT, PRISMATIC_ENCHANTMENT_3, 0, 0, owner->GetGUID());
				break;
			}

			owner->ApplyEnchantment(targetItem, PRISMATIC_ENCHANTMENT_SLOT, true);

			owner->GetSession()->SendAreaTriggerMessage("%s获得额外一个插槽", sCF->GetItemLink(targetItem->GetEntry()).c_str());
			ChatHandler(owner->GetSession()).PSendSysMessage("%s获得额外一个插槽", sCF->GetItemLink(targetItem->GetEntry()).c_str());

			return SPELL_CAST_OK;
		}

		void Register() override
		{
			OnCheckCast += SpellCheckCastFn(spell_item_socket_SpellScript::CheckCast);
		}
	};

	SpellScript* GetSpellScript() const override
	{
		return new spell_item_socket_SpellScript();
	}
};
*/
void AddSC_SpellScripts()
{
	//new spell_dmg_maxhealth_pct();
	//new spell_dmg_health_pct();
	//new spell_dmg_los_health_pct();
	//new spell_heal_maxhealth_pct();
	//new spell_heal_health_pct();
	//new spell_heal_los_health_pct();
	//
	//new spell_speed_des();
	//
	//
	//new spell_dest_dest();
	//new zhenfa_leiyun_trigger();
	//new zhenfa_xuanguang_trigger();
	//new CircleTrigger();
	//
	//new spell_dmg_target();
	//
	//new spell_liangqiping();
	//new spell_liangyihuan();
	//new spell_leihuogong();
	//
	//new spell_absorb_max_hp_pct_damage();

	new spell_item_socket();
}




