enum StatLimitTypes
{
	LIMIT_ARMOR,
	LIMIT_DODGE,
	LIMIT_PARRY,
	LIMIT_BLOCK,
	LIMIT_CRIT,

	LIMIT_HASTE,
	LIMIT_HP,
	LIMIT_MANA,
	LIMIT_MELEEDMG,
	LIMIT_SPELLDMG,
	LIMIT_HEAL,
};

enum StatModTypes
{
	CHAR_MOD_PVP_MELEE_DMG,
	CHAR_MOD_PVP_SPELL_DMG,
	CHAR_MOD_PVP_HEAL,
	CHAR_MOD_PVE_MELEE_DMG,
	CHAR_MOD_PVE_SPELL_DMG,
	CHAR_MOD_PVE_HEAL,
	CHAR_MOD_REDUCE_DMG,
};

enum SwitchStatTypes
{
	//精神加法伤
	SWITCH_SPIRIT2SP,
	//精神加治疗
	SWITCH_SPIRIT2HEAL,
	//智力加法伤
	SWITCH_INTELLECT2SP,
	//智力加治疗
	SWITCH_INTELLECT2HEAL,
	//力量加攻强
	SWITCH_STRENGTH2AP,
	//敏捷加攻强
	SWITCH_AGILITY2AP,
	
	SWITCH_SPIRIT2AP,
	SWITCH_INTELLECT2AP,

	SWITCH_STRENGTH2SP,
	SWITCH_AGILITY2SP,

	SWITCH_STRENGTH2HEAL,
	SWITCH_AGILITY2HEAL,
};

struct CharStatTemplate
{
	uint8 classIndex;
	float meleePVPDmgMod;
	float spellPVPDmgMod;
	float healPVPMod;
	float meleePVEDmgMod;
	float spellPVEDmgMod;
	float healPVEMod;
	float reduceDmgMod;
	uint32 hasteLimit;
	uint32 hpLimit;
	uint32 manaLimit;
	uint32 meleeDmgLimit;
	uint32 spellDmgLimit;
	uint32 healLimit;
	float armorLimit;
	float dodgeLimit;
	float parryLimit;
	float blockLimit;
	float critLimit;
	float spirit2SP;
	float spirit2Heal;
	float intellect2SP;
	float intellect2Heal;
	float strength2AP;
	float agility2AP;

	float spirit2AP;
	float intellect2AP;

	float strength2SP;
	float agility2SP;

	float strength2Heal;
	float agility2Heal;

};
extern std::vector<CharStatTemplate> CharStatVec;
extern std::vector<uint32> AltClassSpellVec;

struct StaticStatTemplate
{
	uint8 _class;
	uint32 vip;
	uint32 map;
	std::unordered_map<Stats, uint32> StaticStatsMap;
	std::unordered_map<CombatRating, uint32> StaticCombatRatingMap;
	uint32 StaticHealth;
	uint32 StaticSpellPower;
	uint32 StaticHealPower;
	uint32 StaticAttackPower;
	uint32 StaticRangeAttackPower;
};

extern std::vector<StaticStatTemplate> StaticStatVec;

class CharMod
{
public:
	static CharMod* instance()
	{
		static CharMod instance;
		return &instance;
	}

	void Load();

	float GetLimit(StatLimitTypes type, uint8 classIndex);
	float GetMod(StatModTypes type, uint8 classIndex);
	float GetValue(SwitchStatTypes type, uint8 classIndex);
	
	int32 GetExtraSP(Player* player);
	int32 GetExtraAP(Player* player);
	int32 GetExtraHeal(Player* player);

	void ModLimit(Player* player);
	void ModStats(Player* player);

	std::string GetClassName1(uint32 _class);
	bool CheckFamily(Player* player, uint32 SpellFamily);
	bool CheckSkill(SpellInfo const* spellInfo);
	void ModClass(Player* player, uint8 targetClass);
	void AddGossip(Player* player, Object* obj);
	
private:

};
#define sCharMod CharMod::instance()
