enum FakerStateType
{
	COMBAT,
	STAY,
	MOVE,
};

enum FakerZoneType
{
	CITY,
	WS,
	AB,
	OUTDOOR,
	DUNGEON,
};


struct FakerLocationTemplate
{
	uint32 Map;
	float X;
	float Y;
	float Z;
	float O;
};

extern std::vector<FakerLocationTemplate> FakerLocation;
extern std::vector<WorldSession *> FakerSessions;
extern std::unordered_map<uint32, bool>FakerMap;

void AddFaker(int i);

class Faker
{
public:
	static Faker* instance()
	{
		static Faker instance;
		return &instance;
	}

	void Load();
	void Add(bool ingnoreLimit);

	void UpdateAllSessions(uint32 diff);
	void Update(Player* faker, uint32 diff);

	void Remove(uint64 guid);

	

	void SetStateType(FakerStateType state){ _state = state; }
	FakerStateType GetStateType(){ return _state; }
	void SetZoneType(FakerZoneType zone){ _zone = zone; }
	FakerZoneType GetZoneType(){ return _zone; }




	void OnLogin(Player* faker);
	void OnLogout(Player* faker);
	void OnMove(Player* faker);
	void OnMoveToObject(Player* faker, Object* obj);
	void OnDetectObject(Player* faker, Object* obj);

	void OnEnterCombat(Player* faker, Unit* victim);

	void OnUpdateZone(Player* faker);

	void OnKilled(Player* faker, Unit* killer);
	void OnKill(Player* faker, Unit* victim);

	void OnRecvWhisper(Player* faker, Player* sender);

	void OnEventStart(Player* faker, uint32 eventId);

	void OnAcceptQuest(Player* faker, uint32 questId);
	void OnCompeleteQuest(Player* faker, uint32 questId);

private:
	FakerZoneType _zone;
	FakerStateType _state;

};
#define sFaker Faker::instance()