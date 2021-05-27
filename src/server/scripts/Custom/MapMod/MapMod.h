
/*
ALTER TABLE _map ADD ResetOnLeave BOOL DEFAULT FALSE AFTER AddRankValue;
ALTER TABLE _map CHANGE ChallegeLevel ChallengeLv INT UNSIGNED DEFAULT 0;
ALTER TABLE _map ADD HpMod FLOAT UNSIGNED DEFAULT 1 AFTER Health;
ALTER TABLE _map ADD Aura3 INT UNSIGNED DEFAULT 0 AFTER ResetOnLeave;
ALTER TABLE _map ADD Aura2 INT UNSIGNED DEFAULT 0 AFTER ResetOnLeave;
ALTER TABLE _map ADD Aura1 INT UNSIGNED DEFAULT 0 AFTER ResetOnLeave;
DROP TABLE _challenge_aura;

ALTER TABLE _map ALTER COLUMN KillRewChance SET DEFAULT 100;
ALTER TABLE _map ADD KillGroupRewId INT UNSIGNED DEFAULT 0 AFTER KillRewChance;
ALTER TABLE _map ADD KillGroupRewChance float UNSIGNED DEFAULT 100 AFTER KillGroupRewId;
ALTER TABLE _attribute_creature ALTER COLUMN KillRewChance SET DEFAULT 100;
ALTER TABLE _attribute_creature ADD KillGroupRewId INT UNSIGNED DEFAULT 0 AFTER KillRewChance;
ALTER TABLE _attribute_creature ADD KillGroupRewChance float UNSIGNED DEFAULT 100 AFTER KillGroupRewId;
ALTER TABLE _attribute_creature ADD AddRankValue int DEFAULT 0 AFTER AddTalismanValue;


*/
struct MapModTemplate
{
	uint32 Map;
	uint32 Diff;
	uint32 Area;
	uint32 Zone;
	uint32 ReqId;
	uint32 ModType;										//0普通怪物 1BOSS 2普通怪物+BOSS
	uint8 Level;										//等级
	uint32 Health;										//生命值
	float HpMod;										//生命值倍率
	int32 Armor;										//护甲-1 creature_template表确定
	float MeleeDmg;									//物理伤害
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
	uint32 ChallengeLv;									//挑战等级
	uint32 AttackTime;									//物理攻击间隔
	int32 AddTalismanValue;
	int32 AddRankValue;
	bool ResetOnLeave;									//离开副本后重置
	std::vector<int32> AuraVec;							//光环
	uint32 RandomAuraCount;								//随机光环个数
	uint32 KillRewGameObject;							//击杀箱子
	uint32 RandSpellGroupId;
};
extern std::vector<MapModTemplate> MapModVec;
extern std::unordered_map<uint32, std::unordered_map<uint8, bool>> MapItemMap;

struct MapModPlayerTemplate
{
	uint32 Map;
	uint32 Diff;
	uint32 ChallengeLv;
	std::vector<uint32> AuraVec;
};
extern std::vector<MapModPlayerTemplate> MapModPlayerVec;

class MapMod
{
public:
	static MapMod* instance()
	{
		static MapMod instance;
		return &instance;
	}

	void Load();
	bool OnEnterMap(Player* player);

	bool OnEnterZone(Player* player, uint32 zone);
	bool OnEnterArea(Player* player, uint32 area);

	void SetMod(Creature* creature);
	void ResetInstance(Player* player, Difficulty diff, uint32 mapId);

	void PopOrTele(Player* player, uint32 triggerId);

	void RemoveItem(Player* player);
	bool CanEquipItem(Player* player, uint8 slot, uint32 itemId);

	void ReApplyMapAura(Player* player);
private:

};
#define sMapMod MapMod::instance()
