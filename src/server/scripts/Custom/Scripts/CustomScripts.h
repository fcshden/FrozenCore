
enum GobActionTypes
{
	gob_ACTION_TYPE_NONE,
	gob_ACTION_TYPE_AURA,							//�ͷż���				AuraID1,						AuraID2,			
	gob_ACTION_TYPE_TELE,							//�������				TelePosId						TeleAura				
	gob_ACTION_TYPE_ACTIVE_OR_DEACTIVE_OBJECT,		//�����ʹʧ��			CreatureEntry					GameObjectEntry		
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
	EVENT_TYPE_COMBAT,			//����ս��									�״δ�����ʱ			��С�ظ�ʱ��						����ظ�ʱ��	
	EVENT_TYPE_KILL,			//ɱ�����									�״δ�����ʱ			��С�ظ�ʱ��						����ظ�ʱ��	
	EVENT_TYPE_HP_90,			//Ѫ������90%(ֱ�Ӵ�����		-					-					-								-			
	EVENT_TYPE_HP_75,			//Ѫ������75%(ֱ�Ӵ���)		-					-					-								-			
	EVENT_TYPE_HP_50,			//Ѫ������50%(ֱ�Ӵ���)		-					-					-								-			
	EVENT_TYPE_HP_25,			//Ѫ������25%(ֱ�Ӵ���)		-					-					-								-			
	EVENT_TYPE_HP_10,			//Ѫ������10%(ֱ�Ӵ���)		-					-					-								-				
	EVENT_TYPE_DIED,			//����(ֱ�Ӵ���)				-					-					-								-	
	EVENT_TYPE_RESET,			//����(ֱ�Ӵ���)				-					-					-								-
};

enum CreatureActionTypes
{												//						actionParam1					actionParam2			actionParam3
	ACTION_TYPE_NONE,
	ACTION_TYPE_CAST_SPELL,						//�ͷż���				SPELL_ID_1,						SPELL_ID_2,				-
	ACTION_TYPE_TALK,							//˵��					˵������								-				
	ACTION_TYPE_ACTIVE_OR_DEACTIVE_OBJECT,		//����					CreatureEntry					GameObjectEntry			-
	ACTION_TYPE_SUMMON,							//�ٻ����������			CreatureEntry��-	GameObjectEntry	����ʱ��					����ID
	ACTION_TYPE_ALT_PHASE,						//�趨�׶�				eventPhase							-		
	ACTION_TYPE_ALT_GOB,						//�򿪻�ر�����			GameObjectEntry��-	GameObjectEntry	��Χ		
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
	SMT_DIRECT_DMG,					//ֱ���˺�����
	SMT_DIRECT_HEAL,				//ֱ����������
	SMT_MOVE_SPEED_INS,				//�ƶ��ٶ�����
	SMT_MOVE_SPEED_DES,				//�ƶ��ٶȼ���
	SMT_DMG_PCT,					//��ɵ��˺��ٷֱ�����
	SMT_HEAL_PCT,					//��ɵ����ưٷֱ�����
	SMT_DMG_PCT_TAKEN,				//�ܵ����˺��ٷֱ�����
	SMT_HEAL_PCT_TAKEN,				//�ܵ������ưٷֱ�����
	SMT_ATTACK_SPEED,				//������ʩ���ٶȰٷֱ�����
	SMT_STUN,						//����
	SMT_STUN_BREAKABLE,				//�ܵ��˺��ɴ�ϻ���
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
