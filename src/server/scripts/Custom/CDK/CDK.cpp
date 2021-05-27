#pragma execution_character_set("utf-8")
#include "CDK.h"
#include "../Reward/Reward.h"
#include <fstream>
#include <iostream>

std::unordered_map<std::string, uint32> CDKMap;

void CDKC::Load()
{
	CDKMap.clear();

	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT �һ���, ����ģ��ID FROM __�һ���" :
		"SELECT cdk, rewId FROM _cdk");

	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			CDKMap.insert(std::make_pair(fields[0].GetString(), fields[1].GetUInt32()));

		} while (result->NextRow());
	}
}

std::string CDKC::Create()
{
	int name_len = 10;
	char buff[128];
	char metachar[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

	for (int i = 0; i < name_len - 1; i++)
		buff[i] = metachar[urand(0, 61)];

	buff[name_len - 1] = '\0';

	std::string s = buff;

	return s;
}

void CDKC::Create(uint32 count, uint32 rewId, std::string comment)
{
	for (size_t i = 0; i < count; i++)
		WorldDatabase.DirectPExecute(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"INSERT INTO __�һ���(��ע,�һ���,����ģ��ID) VALUES ('%s','%s','%u')" :
		"INSERT INTO _cdk(comment,cdk,rewId) VALUES ('%s','%s','%u')", comment, Create(), rewId);

	Load();
}

void CDKC::OutPut()
{
	std::ofstream outfile;
	outfile.open("�һ���.txt");

	if (outfile.is_open())
	{
		if (QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
			"SELECT ��ע,ID,�һ���,����ģ��ID FROM __�һ��� ORDER BY ��ע,ID" :
			"SELECT comment,id,cdk,rewId FROM _cdk ORDER BY comment,id"))
		{
			do
			{
				Field* fields = result->Fetch();
				outfile << "[" << fields[0].GetString() << "][���" << fields[1].GetUInt32() << "][����" << fields[3].GetUInt32() << "][" << fields[2].GetString() << "]" << std::endl;
			} while (result->NextRow());
		}

		outfile.close();
	}
}

void CDKC::AddGossip(Player* player, Object* obj)
{
	player->PlayerTalkClass->ClearMenus();
	player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_BATTLE, "", 9991, 9991, "", 0, true);

	if (obj->ToCreature())
		player->SEND_GOSSIP_MENU(obj->GetEntry(), obj->GetGUID());
	else
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, obj->GetGUID());

	ChatHandler(player->GetSession()).PSendSysMessage("[�һ���]����������ȷ�Ķһ�������ȡ������");
}

bool CDKC::Redeem(Player* player, uint32 sender, uint32 action, std::string cdk)
{
	if (sender == 9991 && action == 9991)
	{
		player->CLOSE_GOSSIP_MENU();

		auto itr = CDKMap.find(cdk);

		if (itr != CDKMap.end())
		{
			sRew->Rew(player, itr->second);
			WorldDatabase.PExecute(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? "DELETE FROM __�һ��� WHERE �һ��� = '%s'" : "DELETE FROM _CDK WHERE cdk = '%s'", cdk);
			ChatHandler(player->GetSession()).PSendSysMessage("[�һ���]���һ����");
			CDKMap.erase(itr++);
			return true;
		}

		ChatHandler(player->GetSession()).PSendSysMessage("[�һ���]���һ������");
		return true;
	}

	return false;
}
