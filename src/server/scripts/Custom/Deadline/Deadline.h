struct DeadlineTemplate
{
	uint32 mapId;
	uint32 countDown;
	uint32 rewId;
	uint32 killedEntry;
	uint32 challengeLv;
	uint32 worldStat;
	uint32 gobEntry;
	uint8 diff;
};
extern std::vector<DeadlineTemplate> DeadlineVec;

class Deadline
{
public:
	static Deadline* instance()
	{
		static Deadline instance;
		return &instance;
	}

	void Load();
	void SetActive(Map* map);
	void Reward(Player* killer);
	void Announce(Map* map, const char* text);
	void SetWorldState(Map* map,uint32 value);
private:

};
#define sDeadline Deadline::instance()