class spell_dmg_to_heal : public SpellScriptLoader
{
public:
	spell_dmg_to_heal() : SpellScriptLoader("spell_dmg_to_heal") { }

	class spell_dmg_to_heal_AuraScript : public AuraScript
	{
		PrepareAuraScript(spell_dmg_to_heal_AuraScript);

		bool Validate(SpellInfo const* /*spellInfo*/)
		{
			if (!sSpellMgr->GetSpellInfo(404486))
				return false;
			return true;
		}

		void OnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
		{
			Unit* caster = GetCaster();
			if (!caster)
				return;

			PreventDefaultAction();
			int32 heal = CalculatePct(int32(eventInfo.GetDamageInfo()->GetDamage()), aurEff->GetAmount());

			float healthPct = caster->GetHealthPct();
			heal = (1 - healthPct) *heal;

			caster->CastCustomSpell(404486, SPELLVALUE_BASE_POINT0, heal, caster, TRIGGERED_FULL_MASK, NULL, aurEff);
		}

		void Register()
		{
			OnEffectProc += AuraEffectProcFn(spell_dmg_to_heal_AuraScript::OnProc, EFFECT_1, SPELL_AURA_DUMMY);
		}
	};

	AuraScript* GetAuraScript() const
	{
		return new spell_dmg_to_heal_AuraScript();
	}
};

void AddSC_custom_spell_scripts()
{
	new spell_dmg_to_heal();
}
