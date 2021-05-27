struct ResetInsTemplate
{
	uint32 mapid;
	uint8 diff;
	uint32 reqId;
};


extern std::vector<ResetInsTemplate> ResetInsVec;

class ResetIns
{
public:
	static ResetIns* instance()
	{
		static ResetIns instance;
		return &instance;
	}

	uint8 getDiff(uint32 action);
	uint32 getMapId(uint32 action);
	uint32 actionValue(uint8 diff, uint32 mapid);

	void Load();
	uint32 GetReqId(uint32 mapid, uint8 diff);
	std::string GetGossipText(uint32 maipid, uint8 diff);

	void AddGossip(Player* player, Object* obj);
	void Action(Player* player, uint32 action);
	void ResetInstance(Player* player, Difficulty diff, uint32 mapId);
private:

};
#define sResetIns ResetIns::instance()