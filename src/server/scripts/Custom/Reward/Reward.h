////所需条件及奖励模板////

#define REW_ITEM_MAX 10

struct RewCommandTemplate
{
	std::string command;
	std::string des;
	std::string icon;
};

struct RewItemTemplate
{
	uint32 itemId;
	uint32 itemCount;
};


struct RewTemplate
{
	uint32		xp			;
	uint32		goldCount	;
	uint32		tokenCount	;
	uint32		hrPoints	;
	uint32		arenaPoints	;
	uint32		statPoints	;
	std::vector<RewItemTemplate> ItemDataVec;
	std::vector<int32> SpellDataVec;
	std::vector<RewCommandTemplate> CommandDataVec;
};
extern std::unordered_map<uint32, RewTemplate> RewMap;

struct CreatureDamageSend
{
	uint32 creatureid;
	uint32 mindamage;
	std::map<uint32, uint32> itemsends;
	uint32 sendgossipcount;
	uint32 maxsend;
};

class Reward
{
public:
	static Reward* instance()
	{
		static Reward instance;
		return &instance;
	}
	void Load();
	void Rew(Player* player, uint32 rewId, uint32 muilt = 1);
	void RewItem(Player* player, uint32 item, uint32 count);
	void MailRew(Player* senderPlayer, uint32 receiverGuidLow, uint32 rewId, std::string titleText, std::string content);
	std::string GetDescription(uint32 rewId, bool quest = false);
	bool IsExist(uint32 rewId);
	std::string GetAnounceText(uint32 rewId);

	typedef std::map<uint32, CreatureDamageSend> CreatureDamageSend_t;
	CreatureDamageSend_t VCreatureDamageSend;

	CreatureDamageSend *  FindCreatureDamageSend(uint32 entry)
	{
		CreatureDamageSend_t::iterator It = VCreatureDamageSend.find(entry);
		if (It != VCreatureDamageSend.end())
			return &It->second;

		return NULL;
	}
	void LoadDamCreToSend();

private:

};
#define sRew Reward::instance()
