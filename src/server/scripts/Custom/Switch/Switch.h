enum SwithTypes
{
	ST_NONE,

	ST_TOKEN_ID							= 1,		//积分物品ID设置	 
	ST_CF_GROUP							= 2,		//跨阵营组队	
	ST_BG_QUEQUE_INTERVALS				= 3,		//战场队列提示间隔		
	ST_TALISMAN							= 4,		//法宝			
	ST_BG_FIXTIME_ENABLE				= 5,		//是否开启定时战场				
	ST_CF_AUCTION						= 6,		//跨阵营拍卖行			
	sT_BG_REW							= 7,		//随机/周常战场额外奖励				
	ST_CF_LANG							= 8,		//跨阵营聊天				
	ST_WHO								= 9,		//Whow列表开始加倍的人数|倍数		
	ST_CF_GUILD							= 10,		//跨阵营公会

	ST_SPS_LIMIT						= 11,		//斗气点上限设置					
	ST_EXTRA_TPS						= 12,		//额外天赋上限设置					
					
	ST_EXTRA_CPS_LIMIT					= 13,		//额外商业点上限设置					
	ST_LOOT_RATE_A						= 14,		//联盟掉率倍率设置					
	ST_LOOT_RATE_H						= 15,		//部落掉率倍率设置					
	ST_ONLINE_PLAYERS					= 16,		//同一IP在线玩家数量上限设置			
	ST_BG_PLAYERS						= 17,		//同一IP战场玩家数量上限设置			
	ST_EVENT_PLAYERS					= 18,		//同一IP事件玩家数量上限设置											  
						
	ST_STOP_COMBAT_CD					= 19,		//脱战冷却时间设置					
	ST_TOKEN_AUCTION_CUT				= 20,		//积分拍卖行手续费率设置				
	ST_XP_MAX_LV						= 21,		//获取经验的最高等级设置				
	ST_XP_RESET_STAT_MULTI				= 22,		//获取双倍经验的最高等级设置			
	ST_QUICK_RESPONSE					= 23,		//可获得奖励的抢答人数设置			
										  
	ST_FAKE_PLAYERS						= 24,		//假人系统配置									  																							  
	ST_LOG_IN							= 25,		//登录提示配置						
	ST_LOG_OUT							= 26,		//登出提示配置						
	ST_WORLD_CHAT						= 27,		//世界聊天配置						
	ST_FACTION_CHAT						= 28,		//阵营聊天配置						
	ST_TIME_REW							= 29,		//泡点奖励ID配置	
	ST_SEP_PREFIX_SUFFIX				= 30,		//玩家名字前缀后缀分割符配置	
	ST_STAGE							= 31,		//挑战模式配置						
	
	ST_HR_ACCOUNT_BIND					= 32,		//军衔是否账号绑定开关
	ST_TP_ACCOUNT_BIND					= 33,		//额外天赋是否账号绑定开关
	ST_DBBACKUP_INTERVALS				= 34,		//数据库自动备份时间间隔
	ST_AUTO_JOIN_GUILD					= 35,		//自动入会开关									
	ST_DODGE_FROM_AGILITY				= 36,		//敏捷加躲闪开关					
	ST_SPELLCRIT_FROM_INTELLECT			= 37,		//智力加爆击开关				
	ST_CRIT_FROM_AGILITY				= 38,		//敏捷加爆击开关						
	ST_TOKEN_AUCTION					= 39,		//积分拍卖行开关						
	ST_DEADLINE_TIME_NOTICE				= 40,		//限时模式时间提醒开关				
	ST_RESET_CHALLENGE					= 41,		//系统重置副本时是否重置挑战等级开关	
	ST_LOTTERY							= 42,		//彩票是否开启开关								
	ST_SAME_RELIC						= 43,		//QS DLY SM DK是否统一RELIC开关			
	ST_UNBIND_INSTANCE_ON_LEAVE_GROUP	= 44,		//离开队伍是否重置副本开关	
	ST_ACHIEVEMENT						= 45,		//成就开关	
	ST_FFAPVP							= 46,		//红名模式
	ST_RANK_ACCOUNT_BIND				= 47,		//Rank账号绑定

	ST_LUCKDRAW							= 48,		//幸运抽奖消耗ID
	ST_ANTFARM							= 49,		//人机验证	
	ST_FAKER_ONLINE						= 50,		//在线假人数量

	ST_DBC_PATH							= 51,		//DB路径
	ST_ENCHANT_SCROLL					= 52,		//提取附魔产生的卷轴ID

	ST_SPELL_DEBUG						= 53,		//技能调试输出

	ST_DAMAGE_PCT						= 54,		//伤害百分比乘法、加法

	ST_ANTICHEAT						= 55,		//反挂
	ST_SEVER_ANNOUNCE					= 56,		//世界公告刷屏配置(_server_announce)

	ST_ENCHANTSLOT_MAX					= 57,		//最大FM位置个数

	ST_ALT_CLASS                        = 58,		//转职
	ST_CF_TRADE							= 59,		//跨阵营交易
	TOP_93 = 60,
	TOP_94 = 61,
	TOP_95 = 62,
	TOP_96 = 63,
	TOP_97 = 64,
	GVG_109 = 65,
	GVG_110 = 66,
	GVG_111 = 67,
	GVG_112 = 68,
	GVG_113 = 69,
	GVG_114 = 70,
	GVG_115 = 71,
	GVG_116 = 72,
	GVG_117 = 73,
	GVG_118 = 74,
	GVG_119 = 75,
	GVG_120 = 76,
	GVG_178 = 77,
	GVG_179 = 78,
	GVG_136 = 79,
	GVG_137 = 80,
	GVG_138 = 81,

	ST_SS = 100,				//魂玉开关
	ST_SSEX = 101,				//魂玉扩展开关
	ST_SS_CXREQ = 110,			//魂玉开关
	ST_SSEX_CXREQ = 111,		//魂玉扩展开关

        MY_DQ_RENWUREW = 206,//完成一个任务给多少分配点
        MY_DQ_CHENGJIUREW = 207,//完成一个成就给多少分配点
        MY_DQ_CHENGZHIREQ = 208,//重置分配点需求的物品ID
        MY_DQ_REWLILIANG = 209, //1点力量需求多少分配点
        MY_DQ_REWMINGJIE = 210, //1点敏捷需求多少分配点
        MY_DQ_REWZHILI = 211, //1点智力需求多少分配点
        MY_DQ_REWJINGSHEN = 212, //1点精神需求多少分配点
        MY_DQ_REWNAILI = 213, //1点耐力需求多少分配点
        MY_DQ_REWFAQIANG = 214, //1点法强需求多少分配点
        MY_DQ_REWGONGQIANG = 215, //1点攻强需求多少分配点
        MY_DQ_REWBAOJI = 216, //1点暴击需求多少分配点
        MY_DQ_REWJISU = 217, //1点急速需求多少分配点
        MY_DQ_REWJISHA = 218, //击杀指定怪物给予分配点可多个 格式：怪物ID|分配点数|#怪物ID|分配点数|#
        MY_DQ_REWJISHADUIWU = 219,//击杀指定怪物给予分配点，队伍中其他成员是否也给分配点 0不给 1给 2给击杀者一半的点数
        MY_TIAOZHANXITNPC = 220,//0=挑战副本使用进副本时弹窗选着难度 1=用npc选择挑战难度

        MY_TIAOZHANXITNPC1 = 221,//随机boss组队给我的奖励
        MY_TIAOZHANXITNPC2 = 222,//随机boss个人给的奖励

        MY_DAMAGE_PCT_2 = 300,//最大上限假人数量
};

extern uint32 PlayerTimeRewType;
extern uint32 PlayerTimeRewIntervals;
extern uint32 AntiCheatType;

struct SwitchTemplate
{
	SwithTypes Type;
	std::string param;
};
extern std::vector<SwitchTemplate> SwitchVec;

class Switch
{
public:
	static Switch* instance()
	{
		static Switch instance;
		return &instance;
	}

	void Load();

	std::string GetParam(SwithTypes type);
	std::string GetFlag(SwithTypes type);
	std::string GetFlagByIndex(SwithTypes type, uint32 index);
	int32 GetValueByIndex(SwithTypes type, uint32 index);
	bool GetOnOff(SwithTypes type);
	int32 GetValue(SwithTypes type);
	void GetVec(SwithTypes type, std::vector<int32> &vec);
private:

};
#define sSwitch Switch::instance()
