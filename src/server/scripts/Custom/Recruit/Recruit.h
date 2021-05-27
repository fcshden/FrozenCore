#define SENDER_RECRUIT_NEW				6000
#define SENDER_RECRUIT_CHAR_INFO_LIST	6001
#define SENDER_RECRUIT_CHAR_DISS_LIST	6002
#define SENDER_RECRUIT_CHAR_TELE_LIST	6006
#define SENDER_RECRUIT_CHAR_DISMISS		6003
#define SENDER_RECRUIT_TELE				6004
#define SENDER_RECRUIT_CHAR_INFO		6005

//招募信息结构体
struct RecruitTemplate
{
	uint32 recruiterGUIDLow;
	uint32 friendGUIDLow;
	uint32 friendPlayedTime;
	bool timeRewarded1;
	bool timeRewarded2;
	bool timeRewarded3;
};
extern std::vector <RecruitTemplate> RecruitInfo;

//招募
struct RecruitOrDissTemplate
{
	uint32 recruitReqId;
	uint32 rewId1;
	uint32 rewId2;
	uint32 disReqId1;
	uint32 disReqId2;
	uint32 playersLimit;
	uint32 crossFaction;
	uint32 insLevel;
	uint32 timeForRew1;
	uint32 timeForRewId1;
	uint32 timeForRew2;
	uint32 timeForRewId2;
	uint32 timeForRew3;
	uint32 timeForRewId3;
};
extern std::vector<RecruitOrDissTemplate> RecruitOrDissVec;

//招募掉落共享
struct RucruitLootShareTemplate
{
	uint32 shareCountLimit;
	uint32 rewCountLimit;
	float shareChance;
	float rewChanceOnShare;
};
extern std::unordered_map<uint32, RucruitLootShareTemplate> RecruitLootMap;

class Recruit
{
public:
	static Recruit* instance()
	{
		static Recruit instance;
		return &instance;
	}
	void PopMsg(Player* player, uint32 menuId, std::string text);//弹出确认框

	bool IsRecruited(Player* player); //是否被招募过

	bool IsRecruitYourRecruiter(uint32 recruiterGUIDLow, uint32 friendGUIDLow);//是否在招募你的招募者

	uint32 GetFriendAmount(Player* recruiter);//获取招募的玩家数量

	GlobalPlayerData const* GetRecruiterData(Player* player);//获取招募者信息

	void GetFriendsDataList(std::vector<GlobalPlayerData const*> &friendsDataList, Player* player);//获取被招募的玩家列表

	bool RecruitAcceptOrCancel(Player*player, uint32 menuId);//确认招募

	bool UpdateRecruitDB(uint32 recruiterGUIDLow, uint32 friendGUIDLow);//更新数据库

	void Load();//加载数据库

	void DismissRecruit(uint32 recruiterGUIDLow, uint32 friendGUIDLow);//解除招募关系

	void RecruitItemReward(Player* player, Item* newItem, uint32 count);//招募奖励
	void RecruitMoneyReward(Player* player,uint32 count);//共享金币
	void RecruitXPReward(Player* player, uint32 xp,Unit* _victim = NULL, float _groupRate = 0 ,Group* _group = NULL);//共享经验
	void RecruitHonorReward(Player* player, uint32 honor);//共享荣誉
	void GetItemCount(uint32 entry, uint32 count, uint32 &recruiterItemCount, uint32 &followerItemCount);

	void RecruitTelePort(Player* player, uint32 targetGUIDLow);//传送

	void AddMainMenu(Player* player, Object* obj);
	bool AddSubMenuOrDoAction(Player* player, Object* obj, uint32 sender, uint32 action);
	bool RecruitFriend(Player* player, uint32 sender, const char* name);

	uint32 GetRecReqId();
	uint32 GetRewId1();
	uint32 GetRewId2();
	uint32 GetDissReqId1();
	uint32 GetDissReqId2();
	uint32 GetPlayersLimit();
	bool CrossFaction();
	uint32 GetInsLevel();
	uint32 GetTimeForRew1();
	uint32 GetTimeForRewId1();
	uint32 GetTimeForRew2();
	uint32 GetTimeForRewId2();
	uint32 GetTimeForRew3();
	uint32 GetTimeForRewId3();

	void UpdateHasRewad(Player* player, uint32 flag);
private:
	
};
#define sRecruit Recruit::instance()
