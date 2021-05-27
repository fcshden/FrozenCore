//掉率
extern std::unordered_map<uint32, float> PlayerDropRateMap;

////连杀////
struct KillerStreakTemplate
{
	uint32 rewId;
	uint32 rewFlag;
	uint32 endRewId;
	uint32 endRewFlag;
	uint32 announceFlag;
};
extern std::unordered_map<uint32, KillerStreakTemplate> KillerStreakMap;

//坐骑
#define BAN_MAP_MAX 10

struct MountAllowedTemplate
{
	bool indoor;
	bool instance;
	bool battleground;
	int BanMap[BAN_MAP_MAX];
};

extern std::unordered_map<uint32, MountAllowedTemplate> MountAllowedMap;

//幻化购买
extern std::unordered_map<uint32, uint32> BuyTransItemMap;

extern std::unordered_map<uint32, float> AreaVisibilityDistMap;

//过滤聊天字符
extern std::vector<std::string> DirtyWordVector;

/*----------------数据加载-------------*/
class DataLoader
{
public:
	static DataLoader* instance()
	{
		static DataLoader instance;
		return &instance;
	}

    void AddLootPl(Player* target, uint32 mod);
	void Load();
	void LoadAll();
	
private:
	ACE_Thread_Mutex _lock;
};
#define sDataLoader DataLoader::instance()
