enum GSTypes
{
	GS_TYPE_SPELL,
	GS_TYPE_ITEM_EQUIP,
	GS_TYPE_ITEM_HIDDEN,
};
struct GSTemplate
{
	uint32 id;
	GSTypes type;
	uint32 gs;
};

extern std::vector<GSTemplate> GSVec;

class GS
{
public:
	static GS* instance()
	{
		static GS instance;
		return &instance;
	}
	void Load();
	uint32 GetGS(uint32 id, GSTypes type);
	void UpdateGS(Player* pl);
	void UpdateGS(Player* pl, uint32 id, GSTypes type, bool add);
	void SendGSData(Player* player, Player* target);
	void SendVisableGSData(Player* player, bool sendtoself);
	uint32 GetItemGS(uint32 id);
	void SendSpellGSData(Player* pl);
private:

};
#define sGS GS::instance()