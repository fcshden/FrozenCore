#pragma execution_character_set("utf-8")
#include "CustomScripts.h"
#include "../CustomEvent/Event.h"

std::vector<GobScriptTemplate> GobScriptVec;

void CustomScript::LoadGobScripts()
{
	GobScriptVec.clear();
	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 物体ID,动作类型,动作参数1,动作参数2 FROM _自定义AI_物体" :
		"SELECT ID,actionType,param1,param2 FROM _ai_gameobject");
	if (!result) return;
	do
	{
		Field* fields = result->Fetch();
		GobScriptTemplate Temp;
		Temp.ID = fields[0].GetInt32();

		const char* str2 = fields[1].GetCString();

		if (strcmp("传送玩家", str2) == 0)
			Temp.gobActionType = gob_ACTION_TYPE_TELE;
		else if (strcmp("激活或使失活", str2) == 0)
			Temp.gobActionType = gob_ACTION_TYPE_ACTIVE_OR_DEACTIVE_OBJECT;
		else if (strcmp("释放技能", str2) == 0)
			Temp.gobActionType = gob_ACTION_TYPE_AURA;
		else if (strcmp("说话", str2) == 0)
			Temp.gobActionType = gob_ACTION_TALK;
		else if (strcmp("召唤", str2) == 0)
			Temp.gobActionType = gob_ACTION_SUMMON;
		else
			Temp.gobActionType = gob_ACTION_TYPE_NONE;

		Temp.actionParam1 = fields[2].GetString();
		Temp.actionParam2 = fields[3].GetInt32();

		GobScriptVec.push_back(Temp);
	} while (result->NextRow());
}


class CustomGameObject : PlayerScript
{
public:
	CustomGameObject() : PlayerScript("CustomGameObject") {}

	void OnUseGameObject(Player* player, GameObject* gob)
	{
		uint32 entry = gob->GetEntry();
		int32 guid = gob->GetGUIDLow();

		for (std::vector<GobScriptTemplate>::iterator itr = GobScriptVec.begin(); itr != GobScriptVec.end(); ++itr)
		{
			int32 ID = itr->ID;

			if (ID != entry && guid != abs(ID))
				continue;

			int32 i_actionParam1 = atoi(itr->actionParam1.c_str());
			std::string s_actionParam1 = itr->actionParam1;
			int32 i_actionParam2 = itr->actionParam2;

			GobActionTypes gobActionType = itr->gobActionType;
			switch (gobActionType)
			{
			case gob_ACTION_TYPE_AURA:
				player->AddAura(i_actionParam1, player);
				player->AddAura(i_actionParam2, player);
				break;
			case gob_ACTION_TYPE_TELE:
				if (!player->HasAura(i_actionParam2))
				{
					player->AddAura(i_actionParam2, player);
					Tele(player, i_actionParam1);
				}
				else
				{
					std::ostringstream oss;
					oss << "|cFFFF1717[" << gob->GetName() << "]|r正处于冷却中，传送失败...";
					player->GetSession()->SendAreaTriggerMessage(oss.str().c_str());
				}
				break;
			case gob_ACTION_TYPE_ACTIVE_OR_DEACTIVE_OBJECT:
			{
				uint32 guid = abs(i_actionParam1);
				bool isCreature = i_actionParam2 == 0 ? true : false;
				bool active = i_actionParam1 > 0 ? true : false;

				if (isCreature)
				{
					if (CreatureData const* data = sObjectMgr->GetCreatureData(guid))
						if (Creature* target = ObjectAccessor::GetObjectInWorld(MAKE_NEW_GUID(guid, data->id, HIGHGUID_UNIT), (Creature*)NULL))
							active ? target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE) : target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
				}
				else
				{
					if (GameObjectData const* data = sObjectMgr->GetGOData(guid))
						if (GameObject* target = ObjectAccessor::GetObjectInWorld(MAKE_NEW_GUID(guid, data->id, HIGHGUID_GAMEOBJECT), (GameObject*)NULL))
							active ? target->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE) : target->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
				}
			}
				break;
			case gob_ACTION_TALK:

				switch (i_actionParam2)
				{
					// 1zone text 2 中间刷屏 3 两者都有
				case 1:
					sWorld->SendZoneText(player->GetZoneId(),s_actionParam1.c_str());
					break;
				case 2:
				{
					SessionMap m_sessions = sWorld->GetAllSessions();
					for (SessionMap::iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
					{
						if (!itr->second || !itr->second->GetPlayer() || !itr->second->GetPlayer()->IsInWorld() || itr->second->GetPlayer()->GetZoneId() != gob->GetZoneId())
							continue;
						itr->second->SendAreaTriggerMessage(s_actionParam1.c_str());
					}
				}
					break;
				case 3:
				{
					SessionMap m_sessions = sWorld->GetAllSessions();
					for (SessionMap::iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
					{
						if (!itr->second || !itr->second->GetPlayer() || !itr->second->GetPlayer()->IsInWorld() || itr->second->GetPlayer()->GetZoneId() != gob->GetZoneId())
							continue;
						itr->second->SendAreaTriggerMessage(s_actionParam1.c_str());
					}

					sWorld->SendZoneText(player->GetZoneId(), s_actionParam1.c_str());
				}
					break;
				case 4:
					sWorld->SendScreenMessage(s_actionParam1.c_str());
					break;
				default:
					break;
				}
				break;
			case gob_ACTION_SUMMON:
			{
				bool isCreature = i_actionParam1 > 0 ? true : false;
				uint32 entry = abs(i_actionParam1);
				uint32 despawntime = i_actionParam2 * MINUTE * IN_MILLISECONDS;

				if (isCreature)
					gob->SummonCreature(entry, gob->GetPositionX(), gob->GetPositionY(), gob->GetPositionZ(), gob->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, despawntime);
				else
					gob->SummonGameObject(entry, gob->GetPositionX(), gob->GetPositionY(), gob->GetPositionZ(), gob->GetOrientation(), 0, 0, 0, 0, despawntime);

			}		
				break;
			default:
				break;
			}
		}
	}


	void Tele(Player* player, uint32 telePosId)
	{
		std::unordered_map<uint32, PosTemplate>::iterator it = PosMap.find(telePosId);

		if (it != PosMap.end())
			player->TeleportTo(it->second.map, it->second.x, it->second.y, it->second.z, it->second.o);
	}
};


class KeyStone :public GameObjectScript
{
public:
	KeyStone() : GameObjectScript("KeyStone") { }

	bool OnGossipHello(Player* player, GameObject* go) override
	{
		if (Creature* target = go->FindNearestCreature(1, 5.0f))
		{
			go->CastSpell(target, 64986);
			target->SetVisible(true);
		}
			
		return true;
	}

	bool OnGossipSelect(Player* player, GameObject* go, uint32 sender, uint32 action) override
	{
		
		return true;
	}

	
};



void AddSC_CustomGameObjectScript()
{
	new KeyStone();
	new CustomGameObject();
}
