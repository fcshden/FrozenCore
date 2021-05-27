#include "../Switch/Switch.h"
#include "../VIP/VIP.h"

class TimeReward : PlayerScript
{
public:
	TimeReward() : PlayerScript("TimeReward") {}
	void OnLogin(Player* player) override
	{
		uint32 timeRewId  = sVIP->GetTimeRewId(player);
		player->timeRewId = (timeRewId == 0 ? atoi(sSwitch->GetFlagByIndex(ST_TIME_REW, 1).c_str()) : timeRewId);
	}
};

void AddSC_Time_Reward()
{
	new TimeReward();
}
