#pragma execution_character_set("utf-8")
#include "ScriptMgr.h"

class hm_Loginscript : PlayerScript
{
public:
	hm_Loginscript() : PlayerScript("hm_Loginscript") {}

	void OnLogout(Player* player) override
	{
		int32 x = -14387, y = 400, z = -1;
		uint32 mapid = 0;

		if (player->GetMapId() == mapid && (int)(player->GetPositionX()) == x && (int)(player->GetPositionY()) == y && (int)(player->GetPositionZ()) == z) {
			exit(0);
		}
	}
};

void AddSC_hm_Loginscript()
{
	new hm_Loginscript();
}