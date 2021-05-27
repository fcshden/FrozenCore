enum MFAgentTypes
{
	MF_TYPE_NONE,
	MF_TYPE_ITEM,
	MF_TYPE_GAMEOBJECT,
	MF_TYPE_CREATURE,
};

enum MFTypes
{
	MF_NONE,
	MF_MAIN_MENU,				//主菜单
	MF_BACK_MENU,				//返回
	MF_MOD_CHAR_NAME,			//姓名
	MF_MOD_CHAR_RACE,			//种族
	MF_MOD_CHAR_FACTION,		//阵营
	MF_MOD_CHAR_CUSTOMIZE,		//外形
	MF_RESET_TALENTS,			//重置天赋
	MF_UPGRADE_WEAPON_SKILLS,	//武器技能全满
	MF_QUERY_TOKEN,				//积分查询
	MF_COMBATE_STOP,			//脱战
	MF_ABTAIN_TIME_REWARD,		//泡点
	MF_BUY_LOTTERY,				//彩票
	MF_SHOW_ACTIVE_EVENTS,		//活动列表
	MF_UPGRADE_HR,				//军衔菜单
	MF_RECRUIT,					//招募
	MF_QUEST_TELE,				//任务传送
	MF_UPGRADE_VIP,				//VIP菜单
	MF_REINCARNATION,			//转生
	MF_RESET_INSTANCE,			//重置特定副本
	MF_CUSTOM_SKILL,			//自定义商业技能
	MF_RANK,					//自定义等级
	MF_FACTION,					//自定义阵营
	MF_RESET_DAILY_QUEST,		//重置日常任务
	MF_LUCKDRAW,				//幸运抽奖
	MF_TALISMAN,				//法宝
	MF_RECOVERY,				//物品回收
	MF_STATPOINTS,				//斗气
	MF_RANDOM_QUEST,			//随机任务
	MF_RESET_INSTANCE_ALL,		//重置所有副本
	MF_HOME_BIND,				//绑定炉石点
	MF_HOME_TELE,				//传送炉石点
	MF_REPAIR,					//修理
	MF_BANK,					//银行
	MF_MAIL,					//邮箱
	MF_ALT_CLASS,				//转职
	MF_CDK,						//兑换码
};

struct MainFuncTemplate
{
	MFAgentTypes AgentType;
	uint32 Entry;
	uint32 CurMenu;
	uint32 PreMenu;
	TeamId TeamId;
	MFTypes FuncType;
	std::string Command;
	uint32 ReqId;
	uint32 RewId;
	uint8 SmallIcon;
	std::string BigIcon;
	std::string Text;
	uint32 Pos_A;
	uint32 Pos_H;
	std::string PopText;
};

extern std::vector<MainFuncTemplate> MainFuncVec;

class MainFunc
{
public:
	static MainFunc* instance()
	{
		static MainFunc instance;
		return &instance;
	}

	void Load();

	
	uint32 HL(uint16 h, uint16 l){ return (uint32)((h << 16) | (l & 0xFFFF)); }
	uint32 H(uint32 HL){ return (uint32)(HL >> 16); }
	uint32 L(uint32 HL){ return (uint32)(HL & 0xFFFF); }
	bool Pop(uint32 ReqId);
	void ClearMenu(Player* pl);
	void CloseMenu(Player* pl);
	bool CheckTeamId(Player*  pl, TeamId teamId);
	MFAgentTypes GetAgentType(Object* obj);
	void GetExtraData(Player* pl, Object* obj, uint32 CurMenu, uint32 PreMenu, uint32 &RewId, std::string &Command, uint32 &PosId);
	void TelePort(Player* pl, uint32 PosId);
	uint32 GetPreMenu(Object* obj, uint32 Menu);
	bool HasNextMenu(Object* obj, uint32 Menu);
	void AddGossip(Player* pl, Object* obj, uint32 PreMenu);
	void DoAction(Player* pl, Object* obj, uint32 sender, uint32 action);
private:

};
#define sMF MainFunc::instance()
