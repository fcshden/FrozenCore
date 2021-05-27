#pragma execution_character_set("utf-8")
#include "../AuthCheck/AuthCheck.h"
#include "../DataLoader/DataLoader.h"
#include "../CommonFunc/CommonFunc.h"
#include "../Switch/Switch.h"
#include "../Command/CustomCommand.h"
#include "../GCAddon/GCAddon.h"
#include "../LuckDraw/LuckDraw.h"

class WhenLogin : PlayerScript
{
public:
	WhenLogin() : PlayerScript("WhenLogin") {}

	void OnLogin(Player* player)
	{
		player->SetUnderACKmount();

		////更新军衔
		if (sSwitch->GetOnOff(ST_HR_ACCOUNT_BIND))
			sCF->updateHRTitle(player);
		
		//设置玩家掉率
		sCF->SetLootRate(player);

		//检测玩家在线数量，超过限制，踢掉
		if (!sCF->onlineIsAllowed(player))
			player->GetSession()->KickPlayer();

		//统一RELIC
		if (sSwitch->GetOnOff(ST_SAME_RELIC))
		{
			if (player->getClass() == CLASS_PALADIN || player->getClass() == CLASS_DRUID || player->getClass() == CLASS_SHAMAN || player->getClass() == CLASS_DEATH_KNIGHT)
			{
				//魔印
				if (!player->HasSpell(52665))
					player->learnSpell(52665);
				//圣物
				if (!player->HasSpell(27762))
					player->learnSpell(27762);
				//图腾
				if (!player->HasSpell(27763))
					player->learnSpell(27763);
				//神像
				if (!player->HasSpell(27764))
					player->learnSpell(27764);
			}	
		}
		else
		{
			if (player->getClass() == CLASS_PALADIN || player->getClass() == CLASS_DRUID || player->getClass() == CLASS_SHAMAN || player->getClass() == CLASS_DEATH_KNIGHT)
			{
				//魔印
				if (!player->HasSpell(52665))
					player->removeSpell(52665, SPEC_MASK_ALL, true);
				//圣物
				if (!player->HasSpell(27762))
					player->removeSpell(27762, SPEC_MASK_ALL, true);
				//图腾
				if (!player->HasSpell(27763))
					player->removeSpell(27763, SPEC_MASK_ALL, true);
				//神像
				if (!player->HasSpell(27764))
					player->removeSpell(27764, SPEC_MASK_ALL, true);
			}
		}

		//player->LootCheckLoad();
	}
};

class LootCheck : PlayerScript
{
public:
	LootCheck() : PlayerScript("LootCheck") {}

	void OnLogin(Player* player)
	{
		player->LootCheckLoad();
	}

	void OnLogout(Player* player)
	{
		player->LootCheckSave();
	}

	void OnGossipSelectCode(Player* player, uint32 menu_id, uint32 sender, uint32 /*action*/, const char* code) override
	{
		if (menu_id != 7878)
			return;

		if (sender == atoi(code))
		{
			player->LootCheckReset();
			player->GetSession()->SendAreaTriggerMessage("[拾取验证]恭喜你，验证通过！");
			ChatHandler(player->GetSession()).PSendSysMessage("[拾取验证]恭喜你，验证通过！");
			player->ModifyMoney(sWorld->getIntConfig(CONFIG_LOOTCHECK_MONEY));
		}
		else
			player->LootCheckPop(sWorld->getIntConfig(CONFIG_LOOTCHECK_SECONDS) - player->LootCheckTimer / IN_MILLISECONDS);
	}
};

void AddSC_WhenLogin()
{
	new WhenLogin();
	new LootCheck();
}
