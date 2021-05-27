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

		////���¾���
		if (sSwitch->GetOnOff(ST_HR_ACCOUNT_BIND))
			sCF->updateHRTitle(player);
		
		//������ҵ���
		sCF->SetLootRate(player);

		//�����������������������ƣ��ߵ�
		if (!sCF->onlineIsAllowed(player))
			player->GetSession()->KickPlayer();

		//ͳһRELIC
		if (sSwitch->GetOnOff(ST_SAME_RELIC))
		{
			if (player->getClass() == CLASS_PALADIN || player->getClass() == CLASS_DRUID || player->getClass() == CLASS_SHAMAN || player->getClass() == CLASS_DEATH_KNIGHT)
			{
				//ħӡ
				if (!player->HasSpell(52665))
					player->learnSpell(52665);
				//ʥ��
				if (!player->HasSpell(27762))
					player->learnSpell(27762);
				//ͼ��
				if (!player->HasSpell(27763))
					player->learnSpell(27763);
				//����
				if (!player->HasSpell(27764))
					player->learnSpell(27764);
			}	
		}
		else
		{
			if (player->getClass() == CLASS_PALADIN || player->getClass() == CLASS_DRUID || player->getClass() == CLASS_SHAMAN || player->getClass() == CLASS_DEATH_KNIGHT)
			{
				//ħӡ
				if (!player->HasSpell(52665))
					player->removeSpell(52665, SPEC_MASK_ALL, true);
				//ʥ��
				if (!player->HasSpell(27762))
					player->removeSpell(27762, SPEC_MASK_ALL, true);
				//ͼ��
				if (!player->HasSpell(27763))
					player->removeSpell(27763, SPEC_MASK_ALL, true);
				//����
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
			player->GetSession()->SendAreaTriggerMessage("[ʰȡ��֤]��ϲ�㣬��֤ͨ����");
			ChatHandler(player->GetSession()).PSendSysMessage("[ʰȡ��֤]��ϲ�㣬��֤ͨ����");
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
