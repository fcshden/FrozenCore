class SpiritPower
{
public:
	static SpiritPower* instance()
	{
		static SpiritPower instance;
		return &instance;
	}

	void Init(Player* player);
	void UpdateMax(Player* player, uint32 maxValue);
	void Update(Player* player, uint32 value, bool ins = true, bool notice = true);
	void GetDBValue(Player* player);
	void SaveToDB(Player* player);
	void SendPacket(Player* player);
private:

};
#define sSpiritPower SpiritPower::instance()