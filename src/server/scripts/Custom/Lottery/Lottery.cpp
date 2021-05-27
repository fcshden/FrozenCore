#pragma execution_character_set("utf-8")
#include "Lottery.h"
#include "../Switch/Switch.h"
#include "../CommonFunc/CommonFunc.h"
#include "../String/myString.h"

std::vector<LotteryTemplate> LotteryInfo;
std::vector<LotterySetTemplate> LotteryVec;

void Lottery::Load()
{
	LotteryVec.clear();

	if (!sSwitch->GetOnOff(ST_LOTTERY))
		return;

	QueryResult result = WorldDatabase.PQuery(
		sWorld->getBoolConfig(CONFIG_ZHCN_DB) ?
		//			0			1		2			3			4			5		6		7
		"SELECT �����ܻ�������,�������,�����ӷ�����,���󷭱���,��С������,�����Ӽ���,������,��С���� FROM __��Ʊ" :
		//			0			1		2			3		4		5		6			7
		"SELECT tokenLimit,intervals,aaa_muil,max_muil,min_muil,aaa_chance,max_chance,min_chance FROM _lottery");
	if (!result) return;

	Field* fields = result->Fetch();
	LotterySetTemplate LotteryTemp;
	LotteryTemp.tokenLimit	= fields[0].GetUInt32();
	LotteryTemp.interval	= fields[1].GetUInt32();
	LotteryTemp.aaa_muil	= fields[2].GetUInt32();
	LotteryTemp.max_muil	= fields[3].GetUInt32();
	LotteryTemp.min_muil	= fields[4].GetUInt32();
	LotteryTemp.aaa_chance	= fields[5].GetUInt32();
	LotteryTemp.max_chance	= fields[6].GetUInt32();
	LotteryTemp.min_chance	= fields[7].GetUInt32();
	LotteryVec.push_back(LotteryTemp);

	if (!LotteryVec.empty())
		SetLotteryAmountLimit(LotteryVec[0].tokenLimit);
}

void Lottery::LoadLotterData()
{
	LotteryInfo.clear();
	QueryResult result = CharacterDatabase.PQuery("SELECT guidLow,lotteryType,lotteryAmount FROM character_lottery");
	if (!result) return;
	do
	{
		Field* fields = result->Fetch();
		LotteryTemplate LotteryTemp;
		LotteryTemp.guidLow			= fields[0].GetUInt32();
		LotteryTemp.lotteryType		= LotteryType(fields[1].GetUInt32());
		LotteryTemp.lotteryAmount	= fields[2].GetUInt32();
		LotteryInfo.push_back(LotteryTemp);
	} while (result->NextRow());
}

uint32 Lottery::GetMuilByType(LotteryType type)
{
	if (LotteryVec.empty())
		return 0;

	switch (type)
	{
	case LOTTERY_TYPE_AAA:
		return LotteryVec[0].aaa_muil;
	case LOTTERY_TYPE_MAX:
		return LotteryVec[0].max_muil;
	case LOTTERY_TYPE_MIN:
		return LotteryVec[0].min_muil;
	default:
		return 0;
	}
}

uint32 Lottery::GetRewardTokenAmountByType(LotteryType type)
{
	uint32 AAA_Amount = 0;
	uint32 MAX_Amount = 0;
	uint32 MIN_Amount = 0;

	for (size_t i = 0; i < LotteryInfo.size(); i++)
	{
		if (type != LotteryInfo[i].lotteryType)
			continue;

		switch (type)
		{
		case LOTTERY_TYPE_AAA:
			AAA_Amount += LotteryInfo[i].lotteryAmount;
			break;
		case LOTTERY_TYPE_MAX:
			MAX_Amount += LotteryInfo[i].lotteryAmount;
			break;
		case LOTTERY_TYPE_MIN:
			MIN_Amount += LotteryInfo[i].lotteryAmount;
			break;
		default:
			break;
		}
	}

	switch (type)
	{
	case LOTTERY_TYPE_AAA:
		return AAA_Amount * GetMuilByType(LOTTERY_TYPE_AAA);
	case LOTTERY_TYPE_MAX:
		return MAX_Amount * GetMuilByType(LOTTERY_TYPE_MAX);
	case LOTTERY_TYPE_MIN:
		return MIN_Amount * GetMuilByType(LOTTERY_TYPE_MIN);
	default:
		return 0;
	}
}

void Lottery::GenerateLotteryType()
{
	LotteryType type;

	if (LotteryVec[0].aaa_chance == 0 && LotteryVec[0].max_chance == 0 && LotteryVec[0].min_chance == 0)
	{
		uint32 aaa = GetRewardTokenAmountByType(LOTTERY_TYPE_AAA);
		uint32 max = GetRewardTokenAmountByType(LOTTERY_TYPE_MAX);
		uint32 min = GetRewardTokenAmountByType(LOTTERY_TYPE_MIN);
		
		uint32 min_ammount = 0;
		
		if (aaa >= max)
		{
			min_ammount = max;
			type = LOTTERY_TYPE_MAX;
		}
		else
		{
			min_ammount = aaa;
			type = LOTTERY_TYPE_AAA;
		}

		if (min_ammount >= min)
			type = LOTTERY_TYPE_MIN;

		if (aaa == max && aaa < min)
			type = (urand(0, 1) == 0 ? LOTTERY_TYPE_MAX : LOTTERY_TYPE_AAA);

		if (aaa == min && aaa < max)
			type = (urand(0, 1) == 0 ? LOTTERY_TYPE_MIN : LOTTERY_TYPE_AAA);

		if (max == min && max < aaa)
			type = (urand(0, 1) == 0 ? LOTTERY_TYPE_MIN : LOTTERY_TYPE_MAX);

		if (max == min && max == aaa)
		{
			switch (urand(0,2))
			{
			case 0:
				type = LOTTERY_TYPE_AAA;
				break;
			case 1:
				type = LOTTERY_TYPE_MAX;
				break;
			case 2:
				type = LOTTERY_TYPE_MIN;
				break;
			}
		}

	}
	else
	{
		uint32 all = LotteryVec[0].aaa_chance + LotteryVec[0].max_chance + LotteryVec[0].min_chance;
		uint32 minmax = LotteryVec[0].max_chance + LotteryVec[0].min_chance;

		uint32 m = LotteryVec[0].max_chance < LotteryVec[0].min_chance ? LotteryVec[0].max_chance : LotteryVec[0].min_chance;

		if (urand(0, all) < LotteryVec[0].aaa_chance)
			type = LOTTERY_TYPE_AAA;
		else if (urand(0, minmax) < m)
			type = LotteryVec[0].max_chance < LotteryVec[0].min_chance ? LOTTERY_TYPE_MAX : LOTTERY_TYPE_MIN;
		else
			type = LotteryVec[0].max_chance < LotteryVec[0].min_chance ? LOTTERY_TYPE_MIN : LOTTERY_TYPE_MAX;
	}
	

	SetLotteryType(type);
}

void Lottery::LotteryRunNotice()
{
	std::ostringstream oss;

	switch (GetLotteryType())
	{
		case LOTTERY_TYPE_AAA:
			oss << "[��Ʊ]������[|cFFFF1717����|r]��|cFFFF1717" << GetMuilByType(LOTTERY_TYPE_AAA) << "|r������...";
			break;
		case LOTTERY_TYPE_MAX:
			oss << "[��Ʊ]������[|cFFFF1717��|r]��|cFFFF1717" << GetMuilByType(LOTTERY_TYPE_MAX) << "|r������...";
			break;
		case LOTTERY_TYPE_MIN:
			oss << "[��Ʊ]������[|cFFFF1717С|r]��|cFFFF1717" << GetMuilByType(LOTTERY_TYPE_MIN) << "|r������...";
			break;
		default:
			break;
	}
	sWorld->SendScreenMessage(oss.str().c_str());
}

void Lottery::LotteryEnd()
{
	//����
	GenerateLotteryType();
	//������Ϣ
	LotteryRunNotice();
	//�����н����
	RewardPlayer(GetLotteryType());
	//��ղ�Ʊ����
	LotteryInfo.clear();
	//������ݱ�
	CharacterDatabase.DirectPExecute("truncate table character_lottery");
}

bool Lottery::PlayerBuyLottery(Player* player, LotteryType type,uint32 lotteryAmount)
{
	bool flag = true;

	if (lotteryAmount == 0)
	{
		player->GetSession()->SendNotification("ѹעʧ�ܣ���������ȷ��ʽ�Ĺ����Ʊ��������");
		flag = false;
	}

	if (type != LOTTERY_TYPE_AAA && type != LOTTERY_TYPE_MIN && type != LOTTERY_TYPE_MAX)
		flag = false;

	uint32 count = sCF->GetTokenAmount(player);

	if (lotteryAmount > count)
	{
		std::ostringstream oss;
		oss << "ѹעʧ�ܣ�" << sString->GetText(CORE_STR_TYPES(STR_TOKEN)) << "���㣡";
		player->GetSession()->SendNotification(oss.str().c_str());
		flag = false;
	}

	if (!flag)
		return false;

	if (PlayerCanBuy(player, lotteryAmount))
	{
		CharacterDatabase.DirectPExecute("INSERT INTO character_lottery VALUES('%u', '%u', '%u')", player->GetGUIDLow(), uint32(type), lotteryAmount);
		uint32	accountId = sObjectMgr->GetPlayerAccountIdByGUID(MAKE_NEW_GUID(player->GetGUIDLow(), 0, HIGHGUID_PLAYER));
		sCF->UpdateTokenAmount(player, lotteryAmount, false, "�����Ʊ");
		LotteryTemplate LotteryTemp;
		LotteryTemp.guidLow			= player->GetGUIDLow();
		LotteryTemp.lotteryType		= type;
		LotteryTemp.lotteryAmount	= lotteryAmount;
		LotteryInfo.push_back(LotteryTemp);

		std::ostringstream oss;
		oss << "��ѹ";
		switch (type)
		{
		case LOTTERY_TYPE_AAA:
			oss << "[|cFFFF1717����|r]";
			break;
		case LOTTERY_TYPE_MAX:
			oss << "[|cFFFF1717��|r]";
			break;
		case LOTTERY_TYPE_MIN:
			oss << "[|cFFFF1717С|r]";
			break;
		default:
			break;
		}

		oss << " X |cFFFF1717";
		oss << lotteryAmount;
		oss << "|rע���ɹ���";
		player->GetSession()->SendAreaTriggerMessage(oss.str().c_str());
	}

	player->CLOSE_GOSSIP_MENU();
	return true;
}


bool Lottery::PlayerCanBuy(Player* player,uint32 lotteryAmount)
{
	if (GetPlayerLotteryAmount(player) + lotteryAmount > GetLotterAmountLimit())
	{
		std::ostringstream oss;
		oss << "���ƹ����Ʊ�ܼ�" << GetLotterAmountLimit();
		oss << ",�����ۼƹ����Ʊ" << GetPlayerLotteryAmount(player);
		oss << "\n���ι������������ó���" << GetLotterAmountLimit() - GetPlayerLotteryAmount(player);
		player->GetSession()->SendNotification(oss.str().c_str());

		return false;
	}
	return true;
}

uint32 Lottery::GetPlayerLotteryAmount(Player* player)
{
	uint32 amount = 0;

	for (size_t i = 0; i < LotteryInfo.size(); i++)
		if (player->GetGUIDLow() == LotteryInfo[i].guidLow)
			amount += LotteryInfo[i].lotteryAmount;

	return amount;
}

std::string Lottery::GetLotteryName(LotteryType type)
{
	switch (type)
	{
	case LOTTERY_TYPE_AAA:
		return "[|cFFFF1717����|r]";
	case LOTTERY_TYPE_MAX:
		return "[|cFFFF1717��|r]";
	case LOTTERY_TYPE_MIN:
		return "[|cFFFF1717С|r]";
	default:
		return "";
		break;
	}
}

void Lottery::RewardPlayer(LotteryType type)
{
	for (size_t i = 0; i < LotteryInfo.size(); i++)
		if (type == LotteryInfo[i].lotteryType)
		{
			uint32 lotteryTokenAmount = LotteryInfo[i].lotteryAmount * GetMuilByType(type);

			Player* player = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(LotteryInfo[i].guidLow, 0, HIGHGUID_PLAYER));

			if (player)
			{
				std::ostringstream oss;
				oss << "[��Ʊ]��������|cFFFF1717" << LotteryInfo[i].lotteryAmount << "|rע" << GetLotteryName(type) << "�����" << lotteryTokenAmount << sString->GetText(CORE_STR_TYPES(STR_TOKEN));
				player->GetSession()->SendAreaTriggerMessage(oss.str().c_str());
				ChatHandler(player->GetSession()).PSendSysMessage(oss.str().c_str());
				sCF->UpdateTokenAmount(player, lotteryTokenAmount, true, "��Ʊ�н�");
			}
			else
			{
				uint32	accountId = sObjectMgr->GetPlayerAccountIdByGUID(MAKE_NEW_GUID(LotteryInfo[i].guidLow, 0, HIGHGUID_PLAYER));
				LoginDatabase.DirectPExecute("UPDATE account SET tokenAmount = tokenAmount + '%u' WHERE id = '%u'", lotteryTokenAmount, accountId);
			}
		}
}

void Lottery::AddLotteryGossip(Player* player, Object* obj)
{
	player->PlayerTalkClass->ClearMenus();
	player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "ѹ����", LOTTERY_TYPE_AAA, GOSSIP_ACTION_INFO_DEF, "", 0, true);
	player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "ѹ��", LOTTERY_TYPE_MAX, GOSSIP_ACTION_INFO_DEF, "", 0, true);
	player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "ѹС", LOTTERY_TYPE_MIN, GOSSIP_ACTION_INFO_DEF, "", 0, true);
	if (obj->ToCreature())
		player->SEND_GOSSIP_MENU(obj->GetEntry(), obj->GetGUID());
	else
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, obj->GetGUID());

	ChatHandler(player->GetSession()).PSendSysMessage("[��Ʊ]��������Ҫѹע�Ĳ�Ʊ���ͣ���������Ҫѹע������");
	ChatHandler(player->GetSession()).PSendSysMessage("[��Ʊ]��1ע����1%s��ѹע��ȷ����������", sString->GetText(CORE_STR_TYPES(STR_TOKEN)));
}

class Lottery_WorldScript : public WorldScript
{
public:
	Lottery_WorldScript() : WorldScript("Lottery_WorldScript") {}

	uint32 Timer = 0;
	bool remind = false;

	void OnStartup() override
	{
		sLottery->LoadLotterData();
	}

	void OnUpdate(uint32 diff) override
	{
		if (LotteryVec.empty())
			return;

		Timer += diff;
		if (Timer > (LotteryVec[0].interval * MINUTE * IN_MILLISECONDS - 10 * IN_MILLISECONDS) && !remind)
		{
			remind = true;
			sWorld->SendScreenMessage("10���Ӻ󿪽�...");
		}
		else if (Timer > LotteryVec[0].interval * MINUTE * IN_MILLISECONDS)
		{
			Timer = 0;
			remind = false;
			sLottery->LotteryEnd();
		}
	}
};

void AddSC_LOTTERY()
{
	new Lottery_WorldScript();
}
