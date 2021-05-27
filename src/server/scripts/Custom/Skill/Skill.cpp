#pragma execution_character_set("utf-8")
#include "Skill.h"
#include "../Reward/Reward.h"
#include "../Requirement/Requirement.h"
#include "../FunctionCollection/FunctionCollection.h"
#include "../CommonFunc/CommonFunc.h"

std::vector<CustomSkillTemplate> CustomSkillVec;
std::vector<CustomSkillCategoryTemplate> CustomSkillCategoryVec;

bool CategorySort(CustomSkillCategoryTemplate a, CustomSkillCategoryTemplate b) { return (a.categoryId < b.categoryId); }
bool SkillSort(CustomSkillTemplate a, CustomSkillTemplate b) { return (a.skillId < b.skillId); }

void CustomSkill::Load()
{
	CustomSkillCategoryVec.clear();
	//QueryResult result1 = WorldDatabase.PQuery("SELECT categoryId,categoryName,categoryIcon FROM _skill_category");
	//if (!result1)
	//	return;
	//do
	//{
	//	Field* fields = result1->Fetch();
	//	CustomSkillCategoryTemplate Temp;
	//	Temp.categoryId		= fields[0].GetUInt32();
	//	Temp.categoryName	= fields[1].GetString();
	//	Temp.icategoryIcon	= fields[2].GetString();
	//	CustomSkillCategoryVec.push_back(Temp);
	//} while (result1->NextRow());

	CustomSkillVec.clear();
	//QueryResult result = WorldDatabase.PQuery("SELECT skillId,skillName,categoryId,reqId,rewId,skillIcon FROM _skill");
	//if (!result)
	//	return;
	//do
	//{
	//	Field* fields = result->Fetch();
	//	CustomSkillTemplate Temp;
	//	Temp.skillId	= fields[0].GetUInt32();
	//	Temp.skillName	= fields[1].GetString();
	//	Temp.categoryId = fields[2].GetUInt32();
	//	Temp.reqId		= fields[3].GetUInt32();
	//	Temp.rewId		= fields[4].GetUInt32();
	//	Temp.skillIcon	= fields[5].GetString();
	//	CustomSkillVec.push_back(Temp);
	//} while (result->NextRow());
	//
	//sort(CustomSkillCategoryVec.begin(), CustomSkillCategoryVec.end(), CategorySort);//升序排列
	//sort(CustomSkillVec.begin(), CustomSkillVec.end(), SkillSort);//升序排列
}

std::string CustomSkill::GetSkillName(uint32 skillId)
{
	for (std::vector<CustomSkillTemplate>::iterator itr = CustomSkillVec.begin(); itr != CustomSkillVec.end(); itr++)
		if (skillId == itr->skillId)
			return itr->skillName;

	return "";
}

uint32 CustomSkill::GetReqId(uint32 skillId)
{
	for (std::vector<CustomSkillTemplate>::iterator itr = CustomSkillVec.begin(); itr != CustomSkillVec.end(); itr++)
		if (skillId == itr->skillId)
			return itr->reqId;

	return 0;
}

uint32 CustomSkill::GetRewId(uint32 skillId)
{
	for (std::vector<CustomSkillTemplate>::iterator itr = CustomSkillVec.begin(); itr != CustomSkillVec.end(); itr++)
		if (skillId == itr->skillId)
			return itr->rewId;

	return 0;
}

std::string CustomSkill::GetSkillIcon(uint32 skillId)
{
	for (std::vector<CustomSkillTemplate>::iterator itr = CustomSkillVec.begin(); itr != CustomSkillVec.end(); itr++)
		if (skillId == itr->skillId)
			return itr->skillIcon;

	return "";
}

std::string CustomSkill::GetCategoryName(uint32 skillId)
{
	for (std::vector<CustomSkillTemplate>::iterator itr = CustomSkillVec.begin(); itr != CustomSkillVec.end(); itr++)
		if (skillId == itr->skillId)
		{
			for (std::vector<CustomSkillCategoryTemplate>::iterator i = CustomSkillCategoryVec.begin(); i != CustomSkillCategoryVec.end(); i++)
				if (i->categoryId == itr->categoryId)
					return i->categoryName;
		}

	return "";
}

uint32 CustomSkill::GetCategoryId(uint32 skillId)
{
	for (std::vector<CustomSkillTemplate>::iterator itr = CustomSkillVec.begin(); itr != CustomSkillVec.end(); itr++)
		if (skillId == itr->skillId)
		{
			for (std::vector<CustomSkillCategoryTemplate>::iterator i = CustomSkillCategoryVec.begin(); i != CustomSkillCategoryVec.end(); i++)
				if (i->categoryId == itr->categoryId)
					return i->categoryId;
		}

	return 0;
}

void CustomSkill::LearnSkill(Player* player, uint32 skillId, bool learn)
{
	if (!learn)
	{
		std::vector<uint32>::iterator it;

		for (it = player->PCustomSkillVec.begin(); it != player->PCustomSkillVec.end();)
		{
			if (*it == skillId)
				it = player->PCustomSkillVec.erase(it);
		    else
		       ++it;
		}

		ChatHandler(player->GetSession()).PSendSysMessage("忘记%s", GetSkillName(skillId).c_str());
		CharacterDatabase.PQuery("DELETE FROM character_custom_skill WHERE guid = %u AND skill = %u", player->GetGUIDLow(), skillId);
	}
	else
	{
		if (std::find(player->PCustomSkillVec.begin(), player->PCustomSkillVec.end(), skillId) != player->PCustomSkillVec.end())
		{
			ChatHandler(player->GetSession()).PSendSysMessage("你已经学会%s", GetSkillName(skillId).c_str());
			return;
		}

		for (std::vector<CustomSkillTemplate>::iterator itr = CustomSkillVec.begin(); itr != CustomSkillVec.end(); itr++)
			if (skillId == itr->skillId)
			{
				PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CUSTOM_SKILL);
				stmt->setUInt32(0, player->GetGUIDLow());
				stmt->setUInt32(1, skillId);
				CharacterDatabase.Execute(stmt);

				player->PCustomSkillVec.push_back(skillId);
				ChatHandler(player->GetSession()).PSendSysMessage("学会%s", GetSkillName(skillId).c_str());
				return;
			}

		ChatHandler(player->GetSession()).PSendSysMessage("Id%u不存在", skillId);
	}
}

void CustomSkill::CharLoadSkill(Player* player)
{
	player->PCustomSkillVec.clear();

	QueryResult result = CharacterDatabase.PQuery("SELECT skill FROM character_custom_skill WHERE guid = '%u'", player->GetGUIDLow());
	if (!result)
		return;
	do
	{
		Field* fields = result->Fetch();
		uint32 skillId = fields[0].GetUInt32();
		
		for (std::vector<CustomSkillTemplate>::iterator itr = CustomSkillVec.begin(); itr != CustomSkillVec.end(); itr++)
			if (skillId == itr->skillId)
				player->PCustomSkillVec.push_back(skillId);

	} while (result->NextRow());
}

void CustomSkill::AddGossip(Player* player, Object* obj)
{
	for (std::vector<CustomSkillCategoryTemplate>::iterator i = CustomSkillCategoryVec.begin(); i != CustomSkillCategoryVec.end(); i++)
	{
		std::string categoryName = "|TInterface/ICONS/" + i->icategoryIcon + ":28:28:0:0|t" + i->categoryName;
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, categoryName.c_str(), SENDER_CUSTOM_SKILL, sCF->joinXY(250, i->categoryId));
	}
		

	player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, obj->GetGUID());
}

void CustomSkill::Action(Player* player, uint32 action, Object* obj)
{
	//添加类别下的skill菜单
	if (sCF->getX(action) == 250)
	{
		
		uint32 categoryId = sCF->getY(action);
		bool flag = false;
		for (std::vector<CustomSkillTemplate>::iterator itr = CustomSkillVec.begin(); itr != CustomSkillVec.end(); itr++)
		{
			uint32 skillId = itr->skillId;

			if (categoryId == itr->categoryId && std::find(player->PCustomSkillVec.begin(), player->PCustomSkillVec.end(), skillId) != player->PCustomSkillVec.end())
			{
				std::string gossipText = "|TInterface/ICONS/" + sCustomSkill->GetSkillIcon(skillId) + ":25:25:0:0|t" + sCustomSkill->GetSkillName(skillId);

				uint32 reqId = sCustomSkill->GetReqId(skillId);
				std::string generalText = "|cFFFFCC00" + sCustomSkill->GetCategoryName(skillId) + "|r\n\n";
				std::string text = "|TInterface/ICONS/" + sCustomSkill->GetSkillIcon(skillId) + ":20:20:0:-17|t|cFFFFCC00" + sCustomSkill->GetSkillName(skillId) + "|r\n";
				
				
				std::ostringstream oss;
				oss << sReq->Notice(player, reqId, generalText, text);

				player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, gossipText.c_str(), SENDER_CUSTOM_SKILL, sCF->joinXY(250 + 1, skillId), oss.str().c_str(), sReq->Golds(reqId), 0);
				flag = true;
			}
		}
		if (flag)
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "\n                 <<返回>>\n  ", SENDER_CUSTOM_SKILL, sCF->joinXY(250 + 2, 1));
		else
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "\n             <<未开启相关内容>>\n  ", SENDER_CUSTOM_SKILL, sCF->joinXY(250 + 2, 1));

		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, obj->GetGUID());
	}

	//点击菜单动作
	if (sCF->getX(action) == 250 + 1)
	{
		uint32 skillId = sCF->getY(action);
		uint32 reqId = sCustomSkill->GetReqId(skillId);
		uint32 rewId = sCustomSkill->GetRewId(skillId);

		if (sReq->Check(player, reqId))
		{
			player->CastSpell(player, 61456, true, NULL, NULL, player->GetGUID());
			sReq->Des(player, reqId);
			sRew->Rew(player, rewId);
		}

		//player->CLOSE_GOSSIP_MENU();

		AddGossip(player, obj);
	}

	//返回主菜单
	if (sCF->getX(action) == 250 + 2)
	{
		for (std::vector<CustomSkillCategoryTemplate>::iterator i = CustomSkillCategoryVec.begin(); i != CustomSkillCategoryVec.end(); i++)
		{
			std::string categoryName = "|TInterface/ICONS/" + i->icategoryIcon + ":28:28:0:0|t" + i->categoryName;
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, categoryName.c_str(), SENDER_CUSTOM_SKILL, sCF->joinXY(250, i->categoryId));
		}

		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, obj->GetGUID());
	}
}

class custom_skill_char : public PlayerScript
{
public:
	custom_skill_char() : PlayerScript("custom_skill_char") {}

	void OnLogin(Player* player) override
	{
		sCustomSkill->CharLoadSkill(player);
	}
};

void AddSC_CUSTOM_SKILL()
{
	new custom_skill_char();
}
