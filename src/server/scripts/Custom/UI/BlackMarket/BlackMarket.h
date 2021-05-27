struct BlackMarketTemplate
{
	uint32 id;
	uint32 itemId;
	uint32 itemCount;
	uint32 reqId;
	bool enable;
};

extern std::vector<BlackMarketTemplate> BlackMarketVec;

class BlackMarket
{
public:
	static BlackMarket* instance()
	{
		static BlackMarket instance;
		return &instance;
	}
	void Load();
	void SendData(Player* player);
	
	void Start();
	void Stop();

	void Update(Player* player, uint32 id);
	void SendData(Player* player, uint32 id, uint32 itemId, uint32 itemCount, uint32 reqId, bool enable);
};
#define sBlackMarket BlackMarket::instance()