struct TriggerTemplate
{
	uint32 guid;
	uint32 type;//1-´«ËÍ 2-ÏÂÂí 3-aura
	uint32 distance;
	uint32 A_PosId;
	uint32 H_PosId;
	uint32 A_AuraId;
	uint32 H_AuraId;
	std::string notice;
	uint32 noticeType;
};
extern std::vector<TriggerTemplate> TriggerVec;

class Trigger
{
public:
	static Trigger* instance()
	{
		static Trigger instance;
		return &instance;
	}

	void Load();
	void GetParam(Player* player,uint32 guid, uint32 &type, uint32 &distance, uint32 &posId, uint32 &auraId, std::string &notice, uint32 &noticeType);
	void Tele(Player* player, uint32 posId);
};
#define sTrigger Trigger::instance()
