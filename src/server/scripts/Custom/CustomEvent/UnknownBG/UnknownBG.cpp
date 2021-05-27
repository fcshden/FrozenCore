#pragma execution_character_set("utf-8")
#include "UnknownBG.h"
#include "../Event.h"
#include "../../CommonFunc/CommonFunc.h"
#include "../../Reward/Reward.h"

std::vector<UkBGTemplate> UkBGVec;
uint32 UKBG_KILLEDSEQUENCE;
float UKBG_R;

void UkBG::Start()
{
	UKBG_KILLEDSEQUENCE = 0;
	UkBGVec.clear();
	Active = true;
}

void UkBG::Stop()
{
	UKBG_KILLEDSEQUENCE = 0;
	UkBGVec.clear();
}

void UkBG::RandPos(float &x1, float& y1, float x, float y, float r)
{
	uint32 i = urand(0, 100);
	float R = frand(0, r);
	x1 = x + R * cos(2 * M_PI * i / 100);
	y1 = y + R * sin(2 * M_PI * i / 100);
}

void UkBG::AddPlayer(Player* player)
{
	if (!sUkBG->Active)
	{
		player->GetSession()->SendNotification("[����Ѱ��]�Ѿ�������");
		return;
	}

	if (InUkBG(player))
	{
		player->GetSession()->SendNotification("�����ظ�����[����Ѱ��]��");
		return;
	}

	UkBGTemplate Temp;
	Temp.guid = player->GetGUID();
	Temp.killCount = 0;
	Temp.killedSequence = 0;

	UkBGVec.push_back(Temp);

	float x1;float y1;
	RandPos(x1, y1, _x, _y, 300);
	player->TeleportTo(_map, x1, y1, 50, 0);
	player->CastSpell(player, 79404);
}

void UkBG::RemovePlayer(Player* player)
{
	uint32 totalCount = UkBGVec.size();

	for (std::vector<UkBGTemplate>::iterator itr = UkBGVec.begin(); itr != UkBGVec.end(); itr++)
		if (player->GetGUID() == itr->guid)
		{
			if (itr->killedSequence > 0)
				break;

			UKBG_KILLEDSEQUENCE++;
			itr->killedSequence = UKBG_KILLEDSEQUENCE;

			std::ostringstream oss;
			oss << "|cFFFF1717[����Ѱ��]|r" << sCF->GetNameLink(player) << "��|cFFFF1717[����֮Ϣ]|r" << "���ɣ�" << UKBG_KILLEDSEQUENCE << " / " << totalCount;

			sWorld->SendScreenMessage(oss.str().c_str());

			CheckDone();

			break;
		}
}

bool UkBG::InUkBG(Player* player)
{
	for (std::vector<UkBGTemplate>::iterator itr = UkBGVec.begin(); itr != UkBGVec.end(); itr++)
		if (player->GetGUID() == itr->guid)
			return true;

	return false;
}

void UkBG::SetKilledSequence(Unit* killer, Player* killed)
{
	uint32 totalCount = UkBGVec.size();

	for (std::vector<UkBGTemplate>::iterator itr = UkBGVec.begin(); itr != UkBGVec.end(); itr++)
		if (killed->GetGUID() == itr->guid)
		{
			if (itr->killedSequence > 0)
			{
				killed->GetSession()->SendNotification("������|cFFFF1717[����Ѱ��]|r�б�ɱ����");
				killed->TeleportTo(killed->m_homebindMapId, killed->m_homebindX, killed->m_homebindY, killed->m_homebindZ, 0);
				break;
			}
				
			UKBG_KILLEDSEQUENCE++;
			itr->killedSequence = UKBG_KILLEDSEQUENCE;

			std::ostringstream oss;

			if (killer->GetTypeId() == TYPEID_PLAYER)
				oss << "|cFFFF1717[����Ѱ��]|r" << sCF->GetNameLink(killed) << "��" << sCF->GetNameLink(killer->ToPlayer()) << "��ɱ��" << UKBG_KILLEDSEQUENCE << "/" << totalCount;
			else if (killer->GetTypeId() == TYPEID_UNIT)
				oss << "|cFFFF1717[����Ѱ��]|r" << sCF->GetNameLink(killed) << "��|cFFFF1717[" + killer->GetName() + "]|r" << "���ɣ�" << UKBG_KILLEDSEQUENCE << " / " << totalCount;

			sWorld->SendScreenMessage(oss.str().c_str());

			Unit::Kill(killer, killed->ToUnit());

			CheckDone();

			break;
		}
}

void UkBG::SetKillCount(Player* player)
{
	for (std::vector<UkBGTemplate>::iterator itr = UkBGVec.begin(); itr != UkBGVec.end(); itr++)
		if (player->GetGUID() == itr->guid)
		{
			itr->killCount++;
			break;
		}
}

void UkBG::CheckDone()
{
	//�������Ѱ���Ƿ���� ֻʣ��һ��������������XX
	uint32 totalCount = UkBGVec.size();

	if (/*deathcount > 3 && */totalCount - UKBG_KILLEDSEQUENCE == 1 && totalCount > 1)
	{
		sWorld->SendScreenMessage("����Ѱ���Ѿ�������");
		Active = false;

		//����
		for (std::vector<UkBGTemplate>::iterator itr = UkBGVec.begin(); itr != UkBGVec.end(); itr++)
		{
			if (Player* player = ObjectAccessor::FindPlayerInOrOutOfWorld(itr->guid))
			{
				uint32 rewId = 0;
				std::ostringstream oss;

				if (itr->killedSequence == 0)
				{
					if (itr->killCount > 0)
					{
						rewId = UKBG_H_REWID;
						oss << "|cFFFF1717[����Ѱ��]|r" << sCF->GetNameLink(player) << "��ǿ����������������˻�ɱĿ�꣬���|cFFFF1717[����]|r������";
						sWorld->SendScreenMessage(oss.str().c_str());
					}	
					else
					{
						rewId = UKBG_L_REWID;
						oss << "|cFFFF1717[����Ѱ��]|r" << sCF->GetNameLink(player) << "���ǵ�����������δ��ɻ�ɱĿ�꣬���|cFFFF1717[����]|r������";
						sWorld->SendScreenMessage(oss.str().c_str());
					}				
				}
				//else if (itr->killedSequence <= 5)
				//{
				//	if (itr->killCount > 0)
				//	{
				//		rewId = UKBG_M_REWID;
				//		oss << "|cFFFF1717[����Ѱ��]|r" << sCF->GetNameLink(player) << "��ǿƴ�����������٣����|cFFFF1717[�е�]|r������";
				//		sWorld->SendServerMessage(SERVER_MSG_STRING, oss.str().c_str());
				//	}	
				//	else
				//	{
				//		rewId = UKBG_L_REWID;
				//		oss << "|cFFFF1717[����Ѱ��]|r" << sCF->GetNameLink(player) << "����͵����������ϧ�����|cFFFF1717[����]|r������";
				//		sWorld->SendServerMessage(SERVER_MSG_STRING, oss.str().c_str());
				//	}			
				//}
				else
				{
					if (itr->killCount > 0)
					{
						rewId = UKBG_L_REWID;
						ChatHandler(player->GetSession()).PSendSysMessage("|cFFFF1717[����Ѱ��]|r��������Ѱ���б���ƽƽ�����|cFFFF1717[����]|r������");
					}	
				}

				sRew->Rew(player, rewId);
			}
		}
	}
}

class UkBGPlayer : PlayerScript
{
public:
	UkBGPlayer() : PlayerScript("UkBGPlayer") {}
	void OnPVPKill(Player* killer, Player* killed) 
	{ 
		if (!sUkBG->InUkBG(killer) || !sUkBG->InUkBG(killed))
			return;

		sUkBG->SetKillCount(killer);
		sUkBG->SetKilledSequence(killer, killed);	
	}

	void OnPlayerKilledByCreature(Creature* killer, Player* killed)
	{
		if (!sUkBG->InUkBG(killed))
			return;

		sUkBG->SetKilledSequence(killer, killed);
	}

	void OnLogout(Player* player) 
	{ 
		if (!sUkBG->InUkBG(player))
			return;

		sUkBG->RemovePlayer(player);
	}

	void OnUpdateZone(Player* player, uint32 newZone, uint32 /*newArea*/) 
	{ 
		if (newZone !=400 && sUkBG->InUkBG(player))
			sUkBG->RemovePlayer(player);
	}
};

class UkBGTrigger : public CreatureScript
{
public:
	UkBGTrigger() : CreatureScript("UkBGTrigger") { }
	struct UkBGTriggerAI : public ScriptedAI
	{
		SummonList Summons;
		uint8 i_max = 250;
		uint32 interval = UKBG_LENGTH * 1000 / i_max;
		uint32 PlayerTimer = 0;
		uint32 SummonTimer = 0;
		uint32 SpawnTimer = 0;

		uint32 map = me->GetMapId();
		float x = me->GetPositionX();
		float y = me->GetPositionY();
		float z = me->GetPositionZ() + 2.0f;

		UkBGTriggerAI(Creature* creature) : ScriptedAI(creature), Summons(me)
		{
			me->m_SightDistance = 1000.0f;
			me->GetMap()->SetVisibilityRange(420.0f);
			UKBG_R = 1.5 * i_max;
		}

		void Summon()
		{
			UKBG_R = 1.5 * i_max;

			for (uint8 i = 0; i < i_max; i++)
			{
				float x1 = x + UKBG_R * cos(2 * M_PI * i / i_max);
				float y1 = y + UKBG_R * sin(2 * M_PI * i / i_max);
				float z1 = -59.93f;
				//me->SummonCreature(28253, x1, y1, z1 - 7, 0, TEMPSUMMON_TIMED_DESPAWN, interval);

				//����֮��ħ������ 179506

				//����
				//me->SummonGameObject(19587, x1, y1, z1, 0, 0, 0, 0, 0, ceil(interval / 1000));

				//��ʥ����(��ֱ)
				//me->SummonGameObject(191302, x1, y1, z1, 0, 0, 0, 0, 0, 2 * ceil(interval / 1000));
				//
				////������֮�����б�����
				me->SummonGameObject(192862, x1, y1, z1, 0, 0, 0, 0, 0, ceil(interval / 1000));
				//
				////����
				//me->SummonGameObject(189330, x1, y1, z1, 0, 0, 0, 0, 0, ceil(interval / 1000));
				//
				////��ľ��Ũ��
				//me->SummonGameObject(177671, x1, y1, z1, 0, 0, 0, 0, 0, ceil(interval / 1000));
				//
				////WotLK�����⻷
				me->SummonGameObject(190745, x1, y1, z1 - 5, 0, 0, 0, 0, 0, ceil(interval / 1000));
			}

			i_max = i_max - 1;


			sWorld->SendZoneText(me->GetZoneId(), "����֮Ϣ�ڱƽ�..");
		}

		void PlayerPosCheck()
		{
			std::list<Player*> playersNearby;
			me->GetPlayerListInGrid(playersNearby, 1000, true);

			if (!playersNearby.empty())
				for (std::list<Player*>::iterator itr = playersNearby.begin(); itr != playersNearby.end(); ++itr)
				{
					if (sUkBG->InUkBG(*itr))
					{
						if (UKBG_R < (*itr)->GetExactDist2d(me))
							sUkBG->SetKilledSequence(me, (*itr));
					}else
					{
						(*itr)->GetSession()->SendNotification("��δ����|cFFFF1717[����Ѱ��]|r��");
						(*itr)->TeleportTo((*itr)->m_homebindMapId, (*itr)->m_homebindX, (*itr)->m_homebindY, (*itr)->m_homebindZ, 0);
					}
				}
		}

		void Spawn()
		{
			uint32 count = i_max / 10;

			for (size_t i = 0; i < count; i++)
			{
				float x1; float y1;
				sUkBG->RandPos(x1, y1, x, y, UKBG_R);


				uint32 rand = urand(0, 4);

				if (rand == 0)
				{
					me->SummonGameObject(350000, x1, y1, z, frand(0,2), 0, 0, 0, 0, 180);
					me->SummonCreature(60010, x1, y1, z + 3.0f, frand(0, 2), TEMPSUMMON_TIMED_DESPAWN, 180000);
				}
				else if (rand == 1)
					me->SummonGameObject(350002, x1, y1, z, 0, 0, 0, 0, 0, 180);
				else
					me->SummonGameObject(350001, x1, y1, z, 0, 0, 0, 0, 0, 180);
			}
		}

		void Reset()
		{
			i_max = 250;
			UKBG_R = 1.5 * i_max;
			SummonTimer = 0;
			PlayerTimer = 0;
			SpawnTimer = 0;
			Summon();
			Spawn();
		}

		void UpdateAI(uint32 diff) override
		{
			if (!sUkBG->Active)
				return;

			//���λ�ü��
			PlayerTimer += diff;

			if (PlayerTimer > 500)
			{
				PlayerTimer = 0;
				PlayerPosCheck();
			}

			//�ٻ�����
			SummonTimer += diff;

			if (SummonTimer > interval)
			{
				SummonTimer = 0;

				if (i_max > 5)
					Summon();
			}

			//ˢ�����塢����
			SpawnTimer += diff;

			if (SpawnTimer > 180000)
			{
				SpawnTimer = 0;
				Spawn();
			}
		}
	};

	CreatureAI* GetAI(Creature* creature) const override
	{
		return new UkBGTriggerAI(creature);
	}
};

void AddSC_UkBG()
{
	new UkBGTrigger();
	new UkBGPlayer();
}
