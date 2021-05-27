struct SpellModTemplate
{
	uint32 reqId;
	float dmgMod;
	float healMod;
	bool accountBind;
	bool disable;
	uint32 castingtime;
	uint32 duration;
	uint32 cooldown;
	uint32 procChance;
	uint32 Periodic[MAX_SPELL_EFFECTS];
};

extern std::unordered_map<uint32, SpellModTemplate> SpellModMap;

struct SpellModBaseTemplate
{
	uint32    EffectImplicitTargetA[MAX_SPELL_EFFECTS];
	uint32    Effect[MAX_SPELL_EFFECTS];
	uint32    EffectApplyAuraName[MAX_SPELL_EFFECTS];
	int32     EffectMiscValue[MAX_SPELL_EFFECTS];
	int32     EffectBasePoints[MAX_SPELL_EFFECTS];
	uint32    EffectTriggerSpell[MAX_SPELL_EFFECTS];
	flag96    EffectSpellClassMask[MAX_SPELL_EFFECTS];
	uint32    SpellFamilyName;
	uint32    procChance;
	uint32    procCharges;
	uint32	  procFlags;
	int32     RecoveryTime;
};

extern std::unordered_map<uint32, SpellModBaseTemplate> SpellModBaseMap;

struct AuraModSpellTemplate
{
	uint32 AuraId;
	SpellFamilyNames SpellFamilyName;
	std::vector<uint32> SpellVec;
	SpellModType Type[MAX_SPELL_EFFECTS];
	SpellModOp Op[MAX_SPELL_EFFECTS];
	int32 Value[MAX_SPELL_EFFECTS];
};

extern std::vector<AuraModSpellTemplate> AuraModSpellVec;

struct AuraModClassSpellTemplate
{
	uint32 AuraId;
	SpellFamilyNames SpellFamilyName;
	SpellModType Type[MAX_SPELL_EFFECTS];
	SpellModOp Op[MAX_SPELL_EFFECTS];
	int32 Value[MAX_SPELL_EFFECTS];
};

extern std::unordered_map<uint32, AuraModClassSpellTemplate> AuraModClassSpellMap;

struct AuraModStatTemplate
{
	uint32 aura;
	uint32 auraType[MAX_SPELL_EFFECTS];
	int32 basePoints[MAX_SPELL_EFFECTS];
	int32 misc[MAX_SPELL_EFFECTS];
	int32 miscB[MAX_SPELL_EFFECTS];
};

extern std::unordered_map<uint32, AuraModStatTemplate> AuraModStatMap;

enum ModTypes
{
	MOD_TYPE_PCT,
	MOD_TYPE_AMOUNT,
};

struct AuraPctTemplate
{
	uint32 aura;
	uint32 auraType[MAX_SPELL_EFFECTS];
	int32 basePoints[MAX_SPELL_EFFECTS];
	uint32 misc[MAX_SPELL_EFFECTS];
	uint32 miscB[MAX_SPELL_EFFECTS];
};

extern std::unordered_map<uint32, AuraPctTemplate> AuraPctMap;

struct AccountSpellTemplate
{
	uint32 accountId;
	uint32 spellId;
};

extern std::vector<AccountSpellTemplate> AccountSpellVec;

struct MountSpellTemplate
{
	uint32 mount60;
	uint32 mount100;
	uint32 mount150;
	uint32 mount280;
	uint32 mount310;
};

extern std::unordered_map<uint32, MountSpellTemplate> MountSpellMap;

enum LeechTypes
{
	LEECH_TYPE_NONE,
	LEECH_TYPE_SELF_CUR_PCT,			//0 自身当前生命值百分比
	LEECH_TYPE_SELF_MAX_PCT,			//1 自身最大生命值百分比
	LEECH_TYPE_TARGET_CUR_PCT,			//2 目标当前生命值百分比
	LEECH_TYPE_TARGET_MAX_PCT,			//3 目标最大生命值百分比
	LEECH_TYPE_STATIC,					//4 固定数值
	LEECH_TYPE_DAMGE_PCT,				//5 技能伤害百分比
};

struct SpellLeechTemplate
{
	uint32 spellid;
	float chance;
	LeechTypes type;
	float basepoints;
	bool addDmg;
	uint32 meetAura;
};

extern std::vector<SpellLeechTemplate> SpellLeechVec;

struct AuraLeechTemplate
{
	float chance;
	LeechTypes type;
	float basepoints;
	bool addDmg;
};

extern std::unordered_map<uint32, AuraLeechTemplate> AuraLeechMap;

enum AuraTriggerIgnoreMask
{
	AURA_TRIGGER_IGNORE_BOSS = 1,
	AURA_TRIGGER_IGNORE_DUNGEON = 2,
	AURA_TRIGGER_IGNORE_NO_DUNGEON = 4,
	AURA_TRIGGER_IGNORE_PLAYER = 8,
	AURA_TRIGGER_IGNORE_UNIT = 16,
};

struct AuraTriggerSpellTemplate
{
	uint32 auraId;
	std::vector<uint32>triggerSpellData;
	float procChance;
	std::vector<uint32>linkSpellData;
	float bp0ApPct;
	float bp1ApPct;
	float bp2ApPct;
	float bp0SpPct;
	float bp1SpPct;
	float bp2SpPct;
	uint32 cooldown;
	uint32 ignoreMask;

	float TargetRange;
	uint32 Targets;

	uint32 procFlags;
};

extern std::unordered_map<uint32, AuraTriggerSpellTemplate> AuraTriggerSpellMap;

extern std::unordered_map<uint32, std::string> OnRemoveSpellMap;
extern std::unordered_map<uint32, std::string> OnLearnSpellMap;

extern std::unordered_map<uint32, uint32> SpellCusTargetMaskMap;

struct AuraStackTriggerTemplate
{
	uint32 Stacks;
	std::vector<uint32> TriggerSpellVec;
	uint32 RemoveStacks;
};

extern std::unordered_map<uint32, AuraStackTriggerTemplate> AuraStackTriggerMap;

struct AuraTriggerOnDeathTemplate
{
	uint32 CoolDown;
	uint32 AttackerTriggerSpell;
	float AttackerTriggerChance;
	uint32 SelfTriggerSpell;
	float SelfTriggerChance;
	bool PreventLastDamage;
};
extern std::unordered_map<uint32, AuraTriggerOnDeathTemplate> AuraTriggerOnDeathMap;

class SpellMod
{
public:
	static SpellMod* instance()
	{
		static SpellMod instance;
		return &instance;
	}

	void Load();
	bool Enable(Player* player, uint32 spellId);
	uint32 GetReqId(uint32 spellId);
	float GetDmgMod(uint32 spellId);
	float GetHealMod(uint32 spellId);
	bool AccontBind(uint32 spellId);

	//account spell
	void LearnAccountSpell(Player* player);
	void InsertAccountSpell(Player* player);


	bool Ignore(uint32 ignoreMask, Unit* target);
	bool AuraTrigger(Unit* caster, Unit* victim, SpellInfo const* auraSpellInfo, SpellInfo const* procSpell, uint32 procFlags, Unit*  &target, AuraEffect* triggeredByAura);
	void HealOnDamage(Unit* caster, Unit* target, SpellInfo const* spellInfo, uint32 &damage);

	void GetParams_PctOnStat(const char* strAuraType, const char* strStatType, AuraType &auraType, int32 &misc, int32 &miscB);
	void GetParams_Pct(const char* strAuraType, const char* strModType, AuraType &auraType, int32 &misc, int32 &miscB);
private:

};
#define sSpellMod SpellMod::instance()
