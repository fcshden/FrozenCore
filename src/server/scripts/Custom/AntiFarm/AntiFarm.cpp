#pragma execution_character_set("utf-8")
#include "AntiFarm.h"
#include "../GCAddon/GCAddon.h"
#include "../Reward/Reward.h"
#include "../Switch/Switch.h"

void AntiFarm::SetParams()
{
	_interval = atoi(sSwitch->GetFlagByIndex(ST_ANTFARM, 1).c_str())  * IN_MILLISECONDS;

	if (_interval <= 0)
		_interval = 1800 * IN_MILLISECONDS;

	_timeout = atoi(sSwitch->GetFlagByIndex(ST_ANTFARM, 2).c_str()) * IN_MILLISECONDS;

	if (_timeout <= 0)
		_timeout = 30 * IN_MILLISECONDS;

	_rewId = uint32(atoi(sSwitch->GetFlagByIndex(ST_ANTFARM, 3).c_str()));

	_onoff = sSwitch->GetOnOff(ST_ANTFARM);
}

uint32 AntiFarm::GetInterval()
{
	return _interval;
}

uint32 AntiFarm::GetTimeOut()
{
	return _timeout;
}

bool AntiFarm::GetOnOff()
{
	return _onoff;
}

void AntiFarm::SendCheck(Player* player)
{
	player->AntiFarmCount++;
	player->AntiFarmNum = urand(1, 9);

	std::ostringstream oss;
	
	oss << player->AntiFarmNum << "#";

	std::vector<uint32> NumVec;

	while (NumVec.empty() || NumVec.size() < 9)
	{
		uint32 i = urand(1, 9);

		if (std::find(NumVec.begin(), NumVec.end(), i) == NumVec.end())
			NumVec.push_back(i);
	}

	for (std::vector<uint32>::iterator it = NumVec.begin(); it != NumVec.end(); it++)
		oss << *it << ":";

	sGCAddon->SendPacketTo(player, "GC_S_ANTIFARM", oss.str());
}

void AntiFarm::DoCheck(Player* player, uint32 num)
{
	if (player->AntiFarmNum == num)
		Action(player, AF_CHECK_SUCCESS);
	else if (player->AntiFarmCount < 3)
		Action(player, AF_CHECK_REPEAT);
	else
		Action(player, AF_CHECK_FAIL);
}

void AntiFarm::Action(Player* player, AntiFarmActions action)
{
	switch (action)
	{
	case AF_CHECK_SUCCESS:
		player->GetSession()->SendAreaTriggerMessage("人机验证成功，感谢您的配合，祝游戏愉快！");
		UnBan(player);
		sRew->Rew(player, _rewId);
		break;
	case AF_CHECK_REPEAT:
		player->GetSession()->SendNotification("第%u次人机验证失败，请重新验证！", player->AntiFarmCount);
		player->AntiFarmTimer = 0;
		SendCheck(player);
		break;
	case AF_CHECK_FAIL:
		player->GetSession()->SendNotification("第%u次人机验证失败，无法继续游戏！请重新上线！", player->AntiFarmCount);
		Ban(player);
		break;
	case AF_CHECK_TIME_OUT:
		player->GetSession()->SendNotification("人机验证超时，你的游戏已被暂停！");
		Ban(player);
		SendCheck(player);
		break;
    case AF_CHECK_KILL:
        player->GetSession()->SendNotification("非法验证，无法继续游戏！请重新上线！");
        Ban(player);
        break;
	default:
		break;
	}
}

void AntiFarm::Ban(Player* player)
{
	player->AntiFarmNum = 0;
	player->AntiFarmCount = 0;
	player->AntiFarmTimer = 0;
	player->AntiFarmBaned = true;

	player->SetMovement(MOVE_ROOT);
	player->SetStandState(UNIT_STAND_STATE_SIT);
	player->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_UNK24);
}

void AntiFarm::UnBan(Player* player)
{
	player->AntiFarmNum = 0;
	player->AntiFarmCount = 0;
	player->AntiFarmTimer = 0;
	player->AntiFarmBaned = false;

	player->SetMovement(MOVE_UNROOT);
	player->RemoveFlag(PLAYER_FLAGS, PLAYER_FLAGS_UNK24);
}

bool AntiFarm::DisableCombat(Player* player)
{
	if (!player->AntiFarmBaned)
		return false;

	player->GetSession()->SendNotification("人机验证未通过，无法继续游戏！请重新登录游戏！");
	player->SetMovement(MOVE_ROOT);
	player->SetStandState(UNIT_STAND_STATE_SIT);
	player->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_UNK24);

	return true;
}

class AntiFarmPlayerScript : PlayerScript
{
public:
	AntiFarmPlayerScript() : PlayerScript("AntiFarmPlayerScript") {}

	void OnLogin(Player* player)
	{
		sAntiFarm->UnBan(player);
	}
};

void AddSC_AntiFarm()
{
	new AntiFarmPlayerScript();
}
