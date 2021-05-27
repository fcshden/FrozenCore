#define FountainSpellId 92000

class Fountain : public CreatureScript
{
public:
	Fountain() : CreatureScript("Fountain") { }
	struct FountainAI : public ScriptedAI
	{
		FountainAI(Creature* creature) : ScriptedAI(creature), Summons(me)
		{
			me->AddAura(FountainSpellId, me);
		}
		SummonList Summons;
		
		void Reset() override
		{
			me->AddAura(FountainSpellId, me);
		}
	};

	CreatureAI* GetAI(Creature* creature) const override
	{
		return new FountainAI(creature);
	}
};


class spell_fountain : public SpellScriptLoader
{
public:
	spell_fountain() : SpellScriptLoader("spell_fountain") { }

	class spell_fountain_AuraScript : public AuraScript
	{
		PrepareAuraScript(spell_fountain_AuraScript);

		bool CheckAreaTarget(Unit* target)
		{
			if (target->GetTypeId() == TYPEID_PLAYER)
			{
				if (target->ToPlayer()->duel || !GetCaster()->IsWithinDistInMap(target, 3.0f))
					return false;

				if (GetCaster()->GetEntry() == 50102 && target->ToPlayer()->GetTeamId() == TEAM_ALLIANCE || GetCaster()->GetEntry() == 50103 && target->ToPlayer()->GetTeamId() == TEAM_HORDE)
					return true;
			}

			return false;
		}
		void Register()
		{
			DoCheckAreaTarget += AuraCheckAreaTargetFn(spell_fountain_AuraScript::CheckAreaTarget);
		}
	};

	AuraScript* GetAuraScript() const
	{
		return new spell_fountain_AuraScript();
	}
};


class FountainPlayerScript : public PlayerScript
{
public:
	FountainPlayerScript() : PlayerScript("FountainPlayerScript") {}

	void OnUpdateZone(Player* player, uint32 /*newZone*/, uint32 /*newArea*/) override
	{
		if (player->HasAura(FountainSpellId))
			player->RemoveAura(FountainSpellId);
	}

};

void AddSC_Fountain()
{
	new spell_fountain();
	new Fountain();
	new FountainPlayerScript();
}