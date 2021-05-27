struct CreautreModTemplate
{
	uint32 Entry;
	uint32 ChallengeLv;
	uint8 Level;										//等级
	uint32 Health;										//生命值
	int32 Armor;										//护甲-1 creature_template表确定
	uint32 MeleeDmg;									//物理伤害
	float SpellDmgMod;									//法术伤害倍率
	float HealMod;										//治疗倍率
	float ReduceDmgPct;									//减伤百分比
	int32 Resistance;									//抗性-1 creature_template表确定
	uint32	LootId[MAX_CUSTOM_LOOT_COUNT];				//掉落 creaute_loot_template
	bool  SrcLoot;
	uint32 KillRewId;									//击杀者获得奖励
	float KillRewChance;								//击杀者获得奖励几率
	uint32 KillGroupRewId;								//击杀者队伍获得奖励
	float KillGroupRewChance;							//击杀者队伍获得奖励几率
	bool KillAnnounce;									//击杀时广播内容
	uint32 AttackTime;									//物理攻击间隔
	float ResetDistance;
	int32 AddTalismanValue;
	int32 AddRankValue;
	uint32 KillRewGameObject;
	uint32 RandSpellGroupId;
	uint32 Diff;
};
extern std::vector<CreautreModTemplate> CreautreModVec;

class CreatureMod
{
public:
	static CreatureMod* instance()
	{
		static CreatureMod instance;
		return &instance;
	}

	void Load();
	void SetMod(Creature* creature);	
private:

};
#define sCreatureMod CreatureMod::instance()
