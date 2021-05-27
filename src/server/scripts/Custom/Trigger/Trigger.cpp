#pragma execution_character_set("utf-8")
#include "Trigger.h"
#include "../CustomEvent/Event.h"

std::vector<TriggerTemplate> TriggerVec;

void Trigger::Load()
{
	TriggerVec.clear();
	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 生物GUID,类型,范围距离,联盟传送坐标ID,部落传送坐标ID,联盟光环ID,部落光环ID,提示内容,提示类型 FROM _自定义AI_触发器" :
		"SELECT guid,type,distance,A_PosId,H_PosId,A_AuraId,H_AuraId,notice,noticeType FROM _trigger");
	if (!result) return;
	do
	{
		Field* fields = result->Fetch();
		TriggerTemplate Temp;
		Temp.guid		= fields[0].GetUInt32();
		Temp.type		= fields[1].GetUInt32();
		Temp.distance	= fields[2].GetFloat();
		Temp.A_PosId	= fields[3].GetUInt32();
		Temp.H_PosId	= fields[4].GetUInt32();
		Temp.A_AuraId	= fields[5].GetUInt32();
		Temp.H_AuraId	= fields[6].GetUInt32();
		Temp.notice		= fields[7].GetString();
		Temp.noticeType = fields[8].GetUInt32();
		TriggerVec.push_back(Temp);
	} while (result->NextRow());
}


void Trigger::GetParam(Player* player, uint32 guid, uint32 &type, uint32 &distance, uint32 &posId, uint32 &auraId, std::string &notice, uint32 &noticeType)
{
	for (std::vector<TriggerTemplate>::iterator iter = TriggerVec.begin(); iter != TriggerVec.end(); ++iter)
	{
		if (guid == iter->guid)
		{
			type = iter->type;
			distance = iter->distance;
			player->GetTeamId() == TEAM_ALLIANCE ? posId = iter->A_PosId : posId = iter->H_PosId;
			player->GetTeamId() == TEAM_ALLIANCE ? auraId = iter->A_AuraId : auraId = iter->H_AuraId;

			//sEvent->GetTeam(player) == C_TEAM_DEFENSE ? posId = iter->A_PosId : posId = iter->H_PosId;
			//sEvent->GetTeam(player) == C_TEAM_OFFENSE ? auraId = iter->A_AuraId : auraId = iter->H_AuraId;

			notice = iter->notice;
			noticeType = iter->noticeType;
			break;
		}
	}
}

void Trigger::Tele(Player* player, uint32 posId)
{
	std::unordered_map<uint32, PosTemplate>::iterator it = PosMap.find(posId);

	if (it != PosMap.end())
		player->TeleportTo(it->second.map, it->second.x, it->second.y, it->second.z, it->second.o);

}

class TriggerScript : public CreatureScript
{
public:
	TriggerScript() : CreatureScript("TriggerScript") { }
	struct TriggerScriptAI : public ScriptedAI
	{
		TriggerScriptAI(Creature* creature) : ScriptedAI(creature), Summons(me){}

		SummonList Summons;

		uint32 type = 0;//1-传送 2-下马 3-aura
		uint32 distance = 0;
		uint32 posId = 0;
		uint32 auraId = 0;
		std::string notice = "";
		uint32 noticeType = 0;

		void MoveInLineOfSight(Unit* who)
		{
			Player* player = who->ToPlayer();

			if (!player)
				return;

			sTrigger->GetParam(player, me->GetGUIDLow(), type, distance, posId, auraId,notice,noticeType);

			if (!me->IsWithinDistInMap(player, distance))
				return;

			switch (type)
			{
			case 1:
				if (posId != 0)
				{
					sTrigger->Tele(player, posId);
					if (!notice.empty())
						noticeType == 0 ? player->GetSession()->SendAreaTriggerMessage(notice.c_str()) : player->GetSession()->SendNotification(notice.c_str());
				}			
				break;
			case 2:
				if (player->IsInWorld() && player->IsMounted())
				{
					player->Dismount();
					player->RemoveAurasByType(SPELL_AURA_MOUNTED);
					if (!notice.empty())
						noticeType == 0 ? player->GetSession()->SendAreaTriggerMessage(notice.c_str()) : player->GetSession()->SendNotification(notice.c_str());
				}
				break;
			case 3:
				if (auraId != 0 && !player->HasAura(auraId))
				{
					player->AddAura(auraId, player);
					if (!notice.empty())
						noticeType == 0 ? player->GetSession()->SendAreaTriggerMessage(notice.c_str()) : player->GetSession()->SendNotification(notice.c_str());
				}
				break;
			default:
				break;
			}

		}
	};

	CreatureAI* GetAI(Creature* creature) const override
	{
		return new TriggerScriptAI(creature);
	}
};

void AddSC_TriggerScript()
{
	new TriggerScript();
}
