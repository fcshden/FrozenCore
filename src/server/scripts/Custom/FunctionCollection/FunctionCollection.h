#define SENDER_REINCARNATION			4001
#define SENDER_HR_UP					7991
#define SENDER_HR_MENU					7992
#define SENDER_RECRUIT					6000
#define SENDER_QUEST_TELE				7000
#define SENDER_CUSTOM_SKILL				7777
#define SENDER_RESET_INS				8888
#define SENDER_VIP_CURR					8000
#define SENDER_VIP_UP					8001
#define SENDER_CUSTOM_EVENT_ACTIVE		9008
#define SENDER_CUSTOM_EVENT_DEACTIVE	9009
#define SENDER_ALT_CLASS				8002
#define SENDER_RANK						9001
#define SENDER_FACTION					9002
#define SENDER_MORPH					9003

//enum FunctionTypes
//{
//	FT_START,
//	FT_MAIN_MENU,				//主菜单
//	FT_BACK_MENU,				//返回
//	FT_MOD_CHAR_NAME,			//姓名
//	FT_MOD_CHAR_RACE,			//种族
//	FT_MOD_CHAR_FACTION,		//阵营
//	FT_MOD_CHAR_CUSTOMIZE,		//外形
//	FT_RESET_TALENTS,			//重置天赋
//	FT_UPGRADE_WEAPON_SKILLS,	//武器技能全满
//	FT_QUERY_TOKEN,				//积分查询
//	FT_COMBATE_STOP,			//脱战
//	FT_ABTAIN_TIME_REWARD,		//泡点
//	FT_BUY_LOTTERY,				//彩票
//	FT_SHOW_ACTIVE_EVENTS,		//活动列表
//	FT_UPGRADE_HR,				//军衔菜单
//	FT_RECRUIT,					//招募
//	FT_QUEST_TELE,				//任务传送
//	FT_UPGRADE_VIP,				//VIP菜单
//	FT_REINCARNATION,			//转生
//	FT_RESET_INSTANCE,			//重置特定副本
//	FT_CUSTOM_SKILL,			//自定义商业技能
//	FT_RANK,					//自定义等级
//	FT_FACTION,					//自定义阵营
//	FT_RESET_DAILY_QUEST,		//重置日常任务
//	FT_LUCKDRAW,				//幸运抽奖
//	FT_SIGNIN,					//签到
//	FT_RECOVERY,				//物品回收
//	FT_STATPOINTS,				//斗气
//	FT_RANDOM_QUEST,			//随机任务
//	FT_RESET_INSTANCE_ALL,		//重置所有副本
//	FT_HOME_BIND,				//绑定炉石点
//	FT_HOME_TELE,				//传送炉石点
//	FT_REPAIR,					//修理
//	FT_BANK,					//银行
//	FT_MAIL,					//邮箱
//	FT_ALT_CLASS,				//转职
//};
//
//struct FunctionTemplate
//{
//	uint32 index;
//	uint32 reqId;
//	std::vector<uint32> telePosId_A;
//	std::vector<uint32> telePosId_H;
//	uint32 desReq;
//};
//
//extern std::vector<FunctionTemplate> FunctionVec;
//
//struct GossipTemplate
//{
//	uint32 entry;
//	uint32 type;
//	uint32 menuId;
//	int32 prevMenuId;
//	uint32 funcIndex;
//	uint8 smallIconType;
//	std::string bigIconName;
//	std::string menuText;
//	TeamId teamId;
//};
//
//extern std::vector<GossipTemplate> GossipVec;
//extern std::vector<GossipTemplate> AboveMenuVec;
//
//class FunctionCollection
//{
//public:
//	static FunctionCollection* instance()
//	{
//		static FunctionCollection instance;
//		return &instance;
//	}
//
//	void Load();
//	uint32 GetOjectType(Object* obj);
//	uint32 DoAction(Player* player, FunctionTypes type, Object* obj,bool destroy = false);
//
//	void ChangeFaction(Player* player);
//	void Customize(Player* player);
//	void ChangeName(Player* player);
//	void ChangeRace(Player* player);
//	void RestTalents(Player* player);
//	void UpgradeWeaponSkills(Player* player);
//
//	void QueryToken(Player* player);
//	void CombateStop(Player* player);
//	void AbtainTimeReward(Player* player);
//
//
//	void BuyLottery(Player* player, uint32 lotteryAmount, uint8 lotteryType);
//	void ShowActiveEvents(Player* player,Object* obj);
//	void Teleport(Player* player, uint32 map, float x, float y, float z, float o);
//	void AddTalentPoints(Player* player, Object* obj);
//
//	//gossip相关
//	void AddGossipOrDoAction(Player* player, Object* obj, uint32 sender, uint32 action);
//	void AddAboveMenu(Player* player, Object* obj, uint32 menuId);
//	
//	//limit reqId rewdId telePos 
//	bool GetDesReq(uint32 funcIndex);
//	uint32 GetReqId(uint32 funcIndex);
//	WorldLocation GetTelePos(Player*player,uint32 funcIndex);
//
//	//prevId 
//	uint32 GetPrevId(uint32 entry, uint32 funcIndex);
//
//	uint32 GetExtraTPs(Player* player);
//
//	void RestAllInstance(Player* player);
//	
//private:
//
//};
//#define sFC FunctionCollection::instance()
