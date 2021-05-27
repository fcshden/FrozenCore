#define BGIC_HEALTH_MAX 100

enum BGICNpcs
{
	BGIC_NPC_POSION = 37690,
};

enum BGICGobs
{
	BGIC_GOB_CHEST = 1,
	BGIC_GOB_KILL_CHEST = 2849,
};

struct BGICPlayerScore
{
	uint32 kills;
	uint32 killeds;
};

class BGIC
{
public:
	static BGIC* instance()
	{
		static BGIC instance;
		return &instance;
	}

public:
	void Stop();
	void InitParams(uint32 eventId, Map* map);
	void Reset();
	void UpdateSpawn(uint32 diff);
	void OnPVPKill(Player* killer, Player* victim);
	bool InWar(Player* pl);
	void AddPlayer(Player* pl);
	void RemovePlayer(Player* pl);
	void InitPlayer(Player* pl);
	void ResetPlayer(Player* pl);
	void TeleToShip(Player* pl);
	void OnCreatureKill(Creature* creature, Player* victim);
	void SendScreenMessage(std::string text);
	void CheckStop();
	bool Valid(Unit* caster);
	bool IsAllowed(Unit* caster, uint32 spellId);
	void SetHealth(Player* attacker,Player* victim, int32 val);

private:
	std::vector<uint32> _PosionNpcPosVec;
	std::vector<uint64> _PoisonNpcGUIDVec;
	std::vector<uint32> _ChestGobPosVec;
	std::unordered_map<uint32,float> _DieLootItemMap;
	uint32 _SpawnUpdateTimer;
	uint32 _Timer;

	MotionTransport* gunshipAlliance;
	MotionTransport* gunshipHorde;
	Map* _Map;
	std::unordered_map<uint64, BGICPlayerScore> _PlayersMap;

	Creature* _TeleportTrigger;
};
#define sBGIC BGIC::instance()