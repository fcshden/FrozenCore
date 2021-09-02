#define AntiFarmCheckInterval	60000
#define AntiFarmTimeOut			30000

enum AntiFarmActions
{
	AF_CHECK_SUCCESS,
	AF_CHECK_REPEAT,
	AF_CHECK_FAIL,
	AF_CHECK_TIME_OUT,
    AF_CHECK_KILL,
};

class AntiFarm
{
public:
	static AntiFarm* instance()
	{
		static AntiFarm instance;
		return &instance;
	}

	void SetParams();
	uint32 GetInterval();
	uint32 GetTimeOut();
	bool GetOnOff();
	void SendCheck(Player* player);
	void DoCheck(Player* player, uint32 num);
	void Action(Player* player, AntiFarmActions action);
	void Ban(Player* player);
	void UnBan(Player* player);
	bool DisableCombat(Player* player);
private:
	uint32 _interval;
	uint32 _timeout;
	uint32 _rewId;
	bool _onoff;
};
#define sAntiFarm AntiFarm::instance()
