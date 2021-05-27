extern std::unordered_map<uint32, uint32> SignInMap;


class SignIn
{
public:
	static SignIn* instance()
	{
		static SignIn instance;
		return &instance;
	}
	void Load();
	uint32 GetRewId(uint32 day);
	void GetInfo(Player* player, time_t &signinTime, uint32 &signinDays);
	bool CanSignIn(Player* player);
	void DoAction(Player* player);
	uint32 GetDiffDay(time_t  time1, time_t  time2);
	std::string GetTimeString(time_t time);
	void SendPacket(Player* player);
	void OpenPanel(Player* player);
private:

};
#define sSignIn SignIn::instance()