class SellReward
{
public:
	static SellReward* instance()
	{
		static SellReward instance;
		return &instance;
	}
	void Load();
	void Reward(Player* player, Item* pItem);
	bool CanRefund(Player* player, Item* pItem);
	std::string GetSellDes(uint32 entry);
private:

};
#define sSellReward SellReward::instance()