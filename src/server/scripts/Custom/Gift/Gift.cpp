#pragma execution_character_set("utf-8")
#include "Gift.h"
#include "../Reward/Reward.h"
#include "../CommonFunc/CommonFunc.h"

std::vector<GiftTemplate> GiftVec;

void Gift::Load()
{
	GiftVec.clear();
	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ?
		//			0		1			2
		"SELECT 上线时长,奖励模板ID,弹窗提示文本 from __上线奖励与提示" :
		//		0		1	2
		"SELECT time,rewId,notice from _gift");
	if (!result)
		return;
	
	do
	{
		Field* fields = result->Fetch();
		GiftTemplate Temp;
		Temp.time	= fields[0].GetUInt32();
		Temp.rewId	= fields[1].GetUInt32();
		Temp.notice = fields[2].GetString();
		GiftVec.push_back(Temp);
	} while (result->NextRow());
}

void Gift::Check(Player* player)
{
	std::vector<GiftTemplate>::iterator itr;
	for (itr = GiftVec.begin(); itr != GiftVec.end(); ++itr)
		if (player->GetGiftTime() / IN_MILLISECONDS > itr->time && (std::find(player->GiftTimeVec.begin(), player->GiftTimeVec.end(), itr->time) == player->GiftTimeVec.end()))
		{
			player->GiftTimeVec.push_back(itr->time);

			sRew->Rew(player, itr->rewId);

			if (!itr->notice.empty())
				sCF->SendAcceptOrCancel(player, 1022, itr->notice);

			break;
		}
}
