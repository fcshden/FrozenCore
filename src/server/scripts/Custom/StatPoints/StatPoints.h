extern uint32 stat_muil[MAX_STAT_POINTS_TYPE];

class StatPoints
{
public:
	static StatPoints* instance()
	{
		static StatPoints instance;
		return &instance;
	}
	void Load();
	void LoadPlayerData(Player* player);
	void UpdateDB(Player* player);
	void UpdateDB(uint32 guid, uint32 statPoint);
	void Apply(Player* player, bool apply);
	void AddGosip(Player* player, Object* obj);
	void AddPoints(Player* player, uint32 sender);
	void ResetPoints(Player* player);

	void OpenPanel(Player* player);
	void Ins(Player* player, uint32 id);
	void Des(Player* player, uint32 id);
};
#define sStatPoints StatPoints::instance()