struct InstanceDieTeleTemplate
{
	uint32 DieTelePos_A;
	uint32 DieTelePos_H;
	bool Unique;
};
extern std::unordered_map<uint32, InstanceDieTeleTemplate> InstanceDieTeleMap;

class InstanceDieTele
{
public:
	static InstanceDieTele* instance()
	{
		static InstanceDieTele instance;
		return &instance;
	}

	void Load();
	bool Tele(Player* player);
	bool Unique(uint32 MapId);
};
#define sInstanceDieTele InstanceDieTele::instance()