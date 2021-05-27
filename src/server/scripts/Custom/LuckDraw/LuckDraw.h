struct LuckDrawTemplate
{
	float chance;
	int32 count;
};

extern std::unordered_map<uint32, LuckDrawTemplate> LuckDrawMap;

class LuckDraw
{
public:
	static LuckDraw* instance()
	{
		static LuckDraw instance;
		return &instance;
	}
	
	void Load();
	void Stop(Player* player);
	
	void SendCheckInfo(Player* player);
	void OpenPanel(Player* player);

	int32 GetCount(uint32 itemId);


	void SendData(Player* player);
	void SendUpdateData(Player* player);
	void Rew(Player* player, uint32 rewCount);
private:

};
#define sLuckDraw LuckDraw::instance()