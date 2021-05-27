struct UkBGTemplate
{
	uint64 guid;
	uint32 killCount;
	uint32 killedSequence;
};

extern std::vector<UkBGTemplate> UkBGVec;

class UkBG
{
public:
	static UkBG* instance()
	{
		static UkBG instance;
		return &instance;
	}

	void Start();
	void Stop();
	bool Active;

	void SetKilledSequence(Unit* killer, Player* killed);
	void SetKillCount(Player* player);

	void AddPlayer(Player* player);
	void RemovePlayer(Player* player);
	bool InUkBG(Player* player);

	void CheckDone();

	void RandPos(float &x1, float& y1, float x, float y, float r);
private:
	uint32 _map = 1;
	float _x = -6116.08f;
	float _y = -3900.58f;
};
#define sUkBG UkBG::instance()