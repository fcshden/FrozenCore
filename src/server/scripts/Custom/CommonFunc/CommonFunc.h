extern bool CampAttackStartFlag;

class CommonFunc
{
public:
	static CommonFunc* instance()
	{
		static CommonFunc instance;
		return &instance;
	}

	void TeleToTarget(Player* player, Player* target);
	void TeleToTarget(Player* player, Player* target, float x, float y, float z);

	//掩码计算
	bool IsInclude(uint32 mask, uint32 totalMask);

	uint32 GetExtraTalentPoints(Player* player);

	uint32 GetTokenAmount(Player* player);
	uint32 GetTotalTokenAmount(Player* player);
	void UpdateTokenAmount(Player* player, uint32 amount, bool ins, std::string action = "");
	void SetOnlineRewardedCount(Player* player, uint32 count);

	//弹窗
	void SendAcceptOrCancel(Player* player, uint32 id, std::string text, bool quest = false);
	bool DoAciotnAfterAccept(Player* player, uint32 id, bool quest = false);

	//图标及物品链接
	std::string GetItemIcon(uint32 entry, uint32 width, uint32 height, int x, int y);
	std::string GetItemLink(uint32 entry);
	std::string GetItemLink(Item* item, WorldSession* session);


	//QQ消息
	void SendQQMsg(std::string gnum, std::string content);

	//播放声音
	void PlayCustomSound(Player* player, uint32 soundId);


	//获取成就点
	uint32 GetAchievementPoints(Player* player);


	//队伍奖励
	void GroupReward(Player* player, uint32 rewardTemplateId, uint32 chance);

	//additem 包包满时sendmail
	void AddOrMailItem(Player* player, uint32 itemId, uint32 count);


	//修改speed
	void SetSpeed(Player* player, float multi);

	//向所有在线玩家发送消息
	void SendMsGToAll(std::string /*str*/, uint32 /*0:notice 1:warning*/);

	//获取玩家信息
	std::string GetNameLink(Player* player);
	void GetNameLink(Player* player, std::string &nameLink, std::string &nameLinkWithColor, bool fakeplayer = false, uint8 fakeclass = 0, std::string fakename = "");
	void GetRace(Player* player, std::string &race, std::string &raceIcon, bool fakeplayer = false, uint8 fakerace = 0, uint8 gender = 0);
	void GetClass(Player* player, std::string &_class, std::string &classIcon, bool fakeplayer = false, uint8 fakeclass = 0);
	void GetHRTitle(Player* player, std::string &hrTitle, std::string &hrIcon, bool fakeplayer = false, uint8 fakehr = 0);
	std::string GetHRTitle(uint32 rank);
	//void updateAccountTitle(Player* player);
	void updateHRTitle(Player* player);
	uint32 getPlayerHrTitleLevel(Player* player);
	uint32 getAccountHrTitleLevel(Player* player);

	//更新玩家名字
	void UpdatePlayerNameWithHR(Player* player);
	uint32 GetHrTitleId(Player* player);

	void SetLootRate(Player* player);

	void StartCamAttack();

	uint32 GetOnlineIpCount(Player* player);
	uint32 GetBgIpCount(Player* player, Battleground* bg);

	bool onlineIsAllowed(Player* player);
	bool joinBgIsAllowed(Player* player, Battleground* bg);

	//获取宝石数量
	uint32 GetGemCountByEntry(Player* player, uint32 itemEntry);

	//完成任务
	void CompleteQuest(Player* player, uint32 questId);

	//reset instance
	void ResetInstance(Player* player, Difficulty diff, bool isRaid = false, uint32 mapId = 0);

	//分割字符串
	std::vector<std::string> SplitStr(std::string str, std::string pattern);


	//获取带各种前缀的玩家名字
	std::string GetPlayerTotalName(Player* player, std::string flag, bool fakeplayer = false, uint8 fake_class = 0, uint8 fake_race = 0, uint8 fake_hr = 0, std::string fake_name = "",uint32 vip = 0,uint8 gender = 0);


	uint8 getX(uint32 action) {
		return (uint8)(action >> 24);
	}

	uint32 getY(uint32 action) {
		return (uint32)(action & 0xFFFFFF);//屏蔽高8位
	}

	uint32 joinXY(uint8 x, uint32 y) {
		return (uint32)((x << 24) | (y & 0xFFFFFF));
	}

	uint32 GetCommercePoints(Player* player);
	void SetCommercePoints(Player* player);


private:
	uint32 m_delay = 0;
};

#define sCF CommonFunc::instance()

extern std::string GetDBName(std::string str);
extern void SplitStr(const char* delim, char* args, char** arg1, char** arg2);
extern void EncodeStr(std::string& c);
extern void DecodeStr(std::string& c);
extern char *url_encode(const char *s, int len, int *new_length);
extern void url_encode(std::string &s);
extern void OutString(std::string s);
extern void OutPos(uint32 posId);
extern void PrintStr(std::string s);
extern std::queue<std::string> QQMsgQueue;
extern std::string GetQQMsgTime();
extern void SendQQMsg(std::string gnum, std::string content);
extern std::string SecTimeString(uint64 timeInSecs, bool shortText);
extern uint32 GetIpCount();
extern bool IsGCValidString(std::string s, std::string description, WorldSession* session, WorldPacket& recvData);
