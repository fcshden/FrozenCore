class Market
{
public:
	static Market* instance()
	{
		static Market instance;
		return &instance;
	}
	void InitMarket(Player* player);
	void ClickItem(Player* player, Item* item);
	void UpdateMaket(Player* player);
	void UpdateMaket(Player* buyer, Player* seller);
	void SetPrice(Player* player, uint32 slot, uint32 token);
	void OffShelve(Player* player, uint32 slot);
	void OnSell(Player* seller,Player* buyer, uint32 slot);

	void SetSelection(Player* buyer, uint64 guid);
private:

};
#define sMarket Market::instance()