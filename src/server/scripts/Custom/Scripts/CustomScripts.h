
enum GobActionTypes
{
	gob_ACTION_TYPE_NONE,
	gob_ACTION_TYPE_AURA,							//释放技能				AuraID1,						AuraID2,			
	gob_ACTION_TYPE_TELE,							//传送玩家				TelePosId						TeleAura				
	gob_ACTION_TYPE_ACTIVE_OR_DEACTIVE_OBJECT,		//激活或使失活			CreatureEntry					GameObjectEntry		
	gob_ACTION_TALK,
	gob_ACTION_SUMMON,
};


struct GobScriptTemplate
{
	int32 ID;
	GobActionTypes gobActionType;
	std::string actionParam1;
	int32 actionParam2;
};

extern std::vector<GobScriptTemplate> GobScriptVec;

enum CreatureEventTypes
{								//						eventPhase			delayTime			repeatMinTime					repeatMaxTime	
	EVENT_TYPE_NONE,
	EVENT_TYPE_COMBAT,			//进入战斗									首次触发延时			最小重复时间						最大重复时间	
	EVENT_TYPE_KILL,			//杀死玩家									首次触发延时			最小重复时间						最大重复时间	
	EVENT_TYPE_HP_90,			//血量低于90%(直接触发）		-					-					-								-			
	EVENT_TYPE_HP_75,			//血量低于75%(直接触发)		-					-					-								-			
	EVENT_TYPE_HP_50,			//血量低于50%(直接触发)		-					-					-								-			
	EVENT_TYPE_HP_25,			//血量低于25%(直接触发)		-					-					-								-			
	EVENT_TYPE_HP_10,			//血量低于10%(直接触发)		-					-					-								-				
	EVENT_TYPE_DIED,			//死亡(直接触发)				-					-					-								-	
	EVENT_TYPE_RESET,			//重置(直接触发)				-					-					-								-
};

enum CreatureActionTypes
{												//						actionParam1					actionParam2			actionParam3
	ACTION_TYPE_NONE,
	ACTION_TYPE_CAST_SPELL,						//释放技能				SPELL_ID_1,						SPELL_ID_2,				-
	ACTION_TYPE_TALK,							//说话					说话内容								-				
	ACTION_TYPE_ACTIVE_OR_DEACTIVE_OBJECT,		//激活					CreatureEntry					GameObjectEntry			-
	ACTION_TYPE_SUMMON,							//召唤生物或物体			CreatureEntry或-	GameObjectEntry	持续时间					坐标ID
	ACTION_TYPE_ALT_PHASE,						//设定阶段				eventPhase							-		
	ACTION_TYPE_ALT_GOB,						//打开或关闭物体			GameObjectEntry或-	GameObjectEntry	范围		
	ACTION_TYPE_MOVE_RANDOM
};

struct CreatureScriptTemplate
{
	uint32 entry;
	CreatureEventTypes eventType;
	uint32 eventPhase;
	uint32 delayTime;
	uint32 repeatMinTime;
	uint32 repeatMaxTime;
	CreatureActionTypes actionType;
	std::string actionParam1;
	int32 actionParam2;
	uint32 actionFlags;
	int32 actionParam3;
};

extern std::vector<CreatureScriptTemplate> CreatureScriptVec;


struct ActionTemplate
{
	uint32 eventID;
	CreatureEventTypes eventType;
	uint32 eventPhase;
	uint32 delayTime;
	uint32 repeatMinTime;
	uint32 repeatMaxTime;
	CreatureActionTypes actionType;
	std::string actionParam1;
	int32 actionParam2;
	uint32 actionFlags;
	uint32 actionParam3;
};

struct CircleTemplate
{
	uint32 respawnTime;
	float range;
	uint32 creatureId;
	float creatureScale;
	float creatureOffsetZ;
	uint32 creatureCount;
	float creatureRadius;
	uint32 gameobjectId;
	float gameobjectScale;
	float gameobjectOffsetZ;
	uint32 gameobjectCount;
	float gameobjectRadius;
	uint32 spellId1;
	uint32 spellInterval1;
	uint32 spellDelay1;
	bool spellFriendly1;
	uint32 spellId2;
	uint32 spellInterval2;
	uint32 spellDelay2;
	bool spellFriendly2;
	uint32 spellId3;
	uint32 spellInterval3;
	uint32 spellDelay3;
	bool spellFriendly3;
};

extern std::unordered_map<uint32,CircleTemplate> CircleDataMap;

enum SpellModTypes
{
	SMT_DIRECT_DMG,					//直接伤害增加
	SMT_DIRECT_HEAL,				//直接治疗增加
	SMT_MOVE_SPEED_INS,				//移动速度增加
	SMT_MOVE_SPEED_DES,				//移动速度减少
	SMT_DMG_PCT,					//造成的伤害百分比增减
	SMT_HEAL_PCT,					//造成的治疗百分比增减
	SMT_DMG_PCT_TAKEN,				//受到的伤害百分比增减
	SMT_HEAL_PCT_TAKEN,				//受到的治疗百分比增减
	SMT_ATTACK_SPEED,				//攻击、施法速度百分比增减
	SMT_STUN,						//昏迷
	SMT_STUN_BREAKABLE,				//受到伤害可打断昏迷
};

struct AIRandSpellTemplate
{
	uint32 GroupId;
	uint32 SpellId;
	uint32 MinDelayTime;
	uint32 MaxDelayTime;
};

extern std::vector<AIRandSpellTemplate> AIRandSpellVec;

class CustomScript
{
public:
	static CustomScript* instance()
	{
		static CustomScript instance;
		return &instance;
	}

	void LoadGobScripts();
	void LoadCreautreScripts();
	void LoadCircleData();

	std::string GetCreatureScript(uint32 entry);

	void GetSpellInfo(SpellModTypes type, uint32 &spellId, SpellValueMod &spellValueMod);
	void Cast(Unit* caster, Unit* target, SpellModTypes type, int value);
	void Cast(Unit* caster, Unit* target, SpellModTypes type);
	void CastSpellToNearbyPlayers(Unit* caster, uint32 e_spellId, uint32 f_spellId, float range);
	void CastSpellToNearbyUnits(Unit* caster, uint32 e_spellId, uint32 f_spellId, float range);


	void CastRandSpell(Creature* creautre, uint32 diff);
private:

};
#define sCustomScript CustomScript::instance()
