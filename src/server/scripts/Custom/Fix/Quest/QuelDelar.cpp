#pragma execution_character_set("utf-8")
#include "../Custom/CommonFunc/CommonFunc.h"
/*
DELETE FROM creature WHERE id = 37552;
INSERT INTO creature (guid, id, map, spawnMask, phaseMask, modelid, equipment_id, position_x, position_y, position_z, orientation, spawntimesecs, spawndist, currentwaypoint, curhealth, curmana, MovementType, npcflag, unit_flags, dynamicflags) VALUES('1976588','37552','530','1','2048','0','0','11781.6','-7068.71','24.9312','3.10074','300','0','0','123','180','0','1','768','32');

DELETE FROM creature WHERE id = 37205;
DELETE FROM creature_template WHERE entry = 37205;
insert into creature_template (entry, difficulty_entry_1, difficulty_entry_2, difficulty_entry_3, KillCredit1, KillCredit2, modelid1, modelid2, modelid3, modelid4, name, subname, IconName, gossip_menu_id, minlevel, maxlevel, exp, faction, npcflag, speed_walk, speed_run, scale, rank, mindmg, maxdmg, dmgschool, attackpower, dmg_multiplier, baseattacktime, rangeattacktime, unit_class, unit_flags, unit_flags2, dynamicflags, family, trainer_type, trainer_spell, trainer_class, trainer_race, minrangedmg, maxrangedmg, rangedattackpower, type, type_flags, lootid, pickpocketloot, skinloot, resistance1, resistance2, resistance3, resistance4, resistance5, resistance6, spell1, spell2, spell3, spell4, spell5, spell6, spell7, spell8, PetSpellDataId, VehicleId, mingold, maxgold, AIName, MovementType, InhabitType, HoverHeight, Health_mod, Mana_mod, Armor_mod, RacialLeader, questItem1, questItem2, questItem3, questItem4, questItem5, questItem6, movementId, RegenHealth, mechanic_immune_mask, flags_extra, ScriptName, VerifiedBuild) values('37205','0','0','0','0','0','30570','0','0','0','萨洛瑞安·寻晨者','','','0','80','80','2','250','0','1','1.14286','1','1','500','700','0','300','5','2000','2000','2','0','2048','0','0','0','0','0','0','0','0','0','7','4096','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','SmartAI','0','3','1','3.5','3','1','0','0','0','0','0','0','0','0','1','0','0','','12340');


-- Thalorien Dawnseeker's Remains
SET @ENTRY := 37552;

DELETE FROM creature_template_addon WHERE(entry=@ENTRY);
INSERT INTO creature_template_addon (entry,path_id,mount,bytes1,bytes2,emote,auras) VALUES
(@ENTRY, 0, 0, 0, 0, 0, '25824 31261');

UPDATE creature_template SET ScriptName="npc_thalorien" WHERE entry=@ENTRY;
UPDATE creature_template SET faction=35 WHERE entry=@ENTRY;

-- Thalorien Dawnseeker SAI
SET @ENTRY := 37205;
Delete FROM creature WHERE id = 37205;
UPDATE creature_template SET faction=250 WHERE entry=@ENTRY;
UPDATE creature_template SET AIName="SmartAI" WHERE entry=@ENTRY;
DELETE FROM smart_scripts WHERE entryorguid=@ENTRY AND source_type=0;
INSERT INTO smart_scripts (entryorguid,source_type,id,link,event_type,event_phase_mask,event_chance,event_flags,event_param1,event_param2,event_param3,event_param4,action_type,action_param1,action_param2,action_param3,action_param4,action_param5,action_param6,target_type,target_param1,target_param2,target_param3,target_x,target_y,target_z,target_o,comment) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 5000, 7000, 15000, 15000, 11, 67541, 3, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, "Thalorien Dawnseeker - In Combat - Cast 'Bladestorm' (Dungeon & Raid)"),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 2000, 2000, 13000, 13000, 11, 67542, 2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, "Thalorien Dawnseeker - In Combat - Cast 'Mortal Strike'"),
(@ENTRY, 0, 2, 0, 0, 0, 100, 0, 19000, 19000, 19000, 19000, 11, 67716, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Thalorien Dawnseeker - In Combat - Cast 'Whirlwind'"),
(@ENTRY, 0, 3, 0, 0, 0, 100, 0, 1000, 1000, 8000, 8000, 11, 57846, 2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, "Thalorien Dawnseeker - In Combat - Cast 'Heroic Strike'");

-- Morlen Coldgrip SAI
SET @ENTRY := 37542;
UPDATE creature_template SET faction=16 WHERE entry=@ENTRY;
UPDATE creature_template SET AIName="SmartAI" WHERE entry=@ENTRY;
DELETE FROM smart_scripts WHERE entryorguid=@ENTRY AND source_type=0;
INSERT INTO smart_scripts (entryorguid,source_type,id,link,event_type,event_phase_mask,event_chance,event_flags,event_param1,event_param2,event_param3,event_param4,action_type,action_param1,action_param2,action_param3,action_param4,action_param5,action_param6,target_type,target_param1,target_param2,target_param3,target_x,target_y,target_z,target_o,comment) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 2000, 3000, 9000, 10000, 11, 50688, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, "Morlen Coldgrip - In Combat - Cast 'Plague Strike' (Dungeon & Raid)");


-- Crypt Raider SAI
SET @ENTRY := 37541;
UPDATE creature_template SET faction=16 WHERE entry=@ENTRY;
UPDATE creature_template SET AIName="SmartAI" WHERE entry=@ENTRY;
DELETE FROM smart_scripts WHERE entryorguid=@ENTRY AND source_type=0;
INSERT INTO smart_scripts (entryorguid,source_type,id,link,event_type,event_phase_mask,event_chance,event_flags,event_param1,event_param2,event_param3,event_param4,action_type,action_param1,action_param2,action_param3,action_param4,action_param5,action_param6,target_type,target_param1,target_param2,target_param3,target_x,target_y,target_z,target_o,comment) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 1000, 2000, 4000, 5000, 11, 31600, 2, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, "Crypt Raider - In Combat - Cast 'Crypt Scarabs' (Dungeon & Raid)");

-- Ghoul Invader SAI
SET @ENTRY := 37539;
UPDATE creature_template SET faction=16 WHERE entry=@ENTRY;
UPDATE creature_template SET AIName="SmartAI" WHERE entry=@ENTRY;
DELETE FROM smart_scripts WHERE entryorguid=@ENTRY AND source_type=0;
INSERT INTO smart_scripts (entryorguid,source_type,id,link,event_type,event_phase_mask,event_chance,event_flags,event_param1,event_param2,event_param3,event_param4,action_type,action_param1,action_param2,action_param3,action_param4,action_param5,action_param6,target_type,target_param1,target_param2,target_param3,target_x,target_y,target_z,target_o,comment) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 1000, 2000, 9000, 10000, 11, 38056, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, "Ghoul Invader - In Combat - Cast 'Flesh Rip' (Dungeon & Raid)");

-- Scourge Zombie SAI
SET @ENTRY := 37538;
UPDATE creature_template SET faction=16 WHERE entry=@ENTRY;
UPDATE creature_template SET AIName="SmartAI" WHERE entry=@ENTRY;
DELETE FROM smart_scripts WHERE entryorguid=@ENTRY AND source_type=0;
INSERT INTO smart_scripts (entryorguid,source_type,id,link,event_type,event_phase_mask,event_chance,event_flags,event_param1,event_param2,event_param3,event_param4,action_type,action_param1,action_param2,action_param3,action_param4,action_param5,action_param6,target_type,target_param1,target_param2,target_param3,target_x,target_y,target_z,target_o,comment) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 1000, 2000, 9000, 10000, 11, 49861, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, "Scourge Zombie - In Combat - Cast 'Infected Bite' (Dungeon & Raid)");

*/


enum ThalorienNPCs
{
	THALORIEN_NPC_THALORIEN = 37205,
	THALORIEN_NPC_DEFENDER	= 37211,
	THALORIEN_NPC_BOSS		= 37542,
	THALORIEN_NPC_ENEMY_1	= 37538,
	THALORIEN_NPC_ENEMY_2	= 37539,
	THALORIEN_NPC_ENEMY_3	= 37541,
};

Position const ThalorienNPCPos[16] =
{
	//[0]			寻晨者1
	{ 11787.2,	-7070.56,	25.8603,	6.0444},
	//[1 - 10]		卫兵
	{ 11791.2,	-7070.76,	26.129,		2.31049 },
	{ 11789.4,	-7072.64,	26.0378,	2.63958 },
	{ 11787,	-7074.43,	25.8398,	2.51784 },
	{ 11785,	-7075.82,	25.7344,	2.2186 },
	{ 11782.8,	-7078,		25.7069,	2.28615 },
	{ 11784.8,	-7078.31,	25.7336,	2.24374 },
	{ 11787.3,	-7076.61,	25.7956,	2.18483 },
	{ 11789.3,	-7074.77,	26.0251,	2.17541 },
	{ 11791.6,	-7073.19,	26.192,		2.16755 },
	{ 11793.8,	-7071.21,	26.2234,	2.17619 },
	//[11]			BOSS
	{ 11759.3,	-7052.25,	25.9366,	5.70586 },
	//[12 - 15]
	{ 11760.6,	-7067.11,	25.1363,	0.286524 },
	{ 11760.5,	-7059.13,	25.2377,	0.039123 },
	{ 11760.6,	-7062.04,	25.2717,	0.0116341 },
	{ 11760,	-7064.6,	25.3605,	0.137299 },
};

class npc_thalorien : public CreatureScript
{
public:
	npc_thalorien() : CreatureScript("npc_thalorien") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		player->PlayerTalkClass->ClearMenus();

		if (npc_thalorien::npc_thalorienAI* creatureAI = CAST_AI(npc_thalorien::npc_thalorienAI, creature->AI()))
			if (creatureAI->HasBeginScence())
			{
				player->SEND_GOSSIP_MENU(creature->GetEntry(), creature->GetGUID());
				return true;
			}

		if (player->GetQuestStatus(24563) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(24535) == QUEST_STATUS_INCOMPLETE)
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "检查遗骸", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

		player->SEND_GOSSIP_MENU(creature->GetEntry(), creature->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 /*action*/) 
	{ 
		player->CLOSE_GOSSIP_MENU();
	
		if (npc_thalorien::npc_thalorienAI* creatureAI = CAST_AI(npc_thalorien::npc_thalorienAI, creature->AI()))
			creatureAI->BeginScene();

		return true;
	}

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_thalorienAI(creature);
	}

	struct npc_thalorienAI : public ScriptedAI
	{
		npc_thalorienAI(Creature* creature) : ScriptedAI(creature) 
		{
			Reset();
		}

		void Reset()
		{
			_Phase = 0;
			_Timer = 0;

			for (auto itr = _gurads.begin(); itr != _gurads.end(); itr++)
				if (Creature* guard = *itr)
					guard->DespawnOrUnsummon(0);
			_gurads.clear();

			for (auto itr = _enemies.begin(); itr != _enemies.end(); itr++)
				if (Creature* guard = *itr)
					guard->DespawnOrUnsummon(0);
			_enemies.clear();

			if (Creature* thalorien = me->GetMap()->GetCreature(_thalorien))
				thalorien->DespawnOrUnsummon(0);
			_thalorien = 0;

			if (Creature* boss = me->GetMap()->GetCreature(_boss))
				boss->DespawnOrUnsummon(0);
			_boss = 0;

		}

		void BeginScene()
		{
			_Timer = 100;
			_Phase = 1;
		}

		void EndScene(bool win)
		{
			if (win)
			{
				std::list<Player*> playersNearby;
				me->GetPlayerListInGrid(playersNearby, 100);
				if (!playersNearby.empty())
					for (std::list<Player*>::iterator itr = playersNearby.begin(); itr != playersNearby.end(); ++itr)
						if ((*itr) && (*itr)->IsInWorld())
							(*itr)->KilledMonsterCredit(37601, 0);
			}

			Reset();
		}

		bool HasBeginScence()
		{
			return _Phase != 0;
		}

		void AddThalorienThreat(Creature* c)
		{
			if (Creature* thalorien = me->GetMap()->GetCreature(_thalorien))
			{
				thalorien->AddThreat(c, 100.0f);	
				c->AddThreat(thalorien, 100.0f);
				thalorien->AI()->AttackStart(c);
			}
		}

		void JustSummoned(Creature* summon) override
		{
			switch (summon->GetEntry())
			{
			case THALORIEN_NPC_DEFENDER:
				_gurads.push_back(summon);
				break;
			case THALORIEN_NPC_THALORIEN:
				_thalorien = summon->GetGUID();
				break;
			case THALORIEN_NPC_BOSS:
				_boss = summon->GetGUID();
				summon->Mount(25678);
				summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
				break;
			default:
				summon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
				_enemies.push_back(summon);
				break;
			}
		}

		void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
		{
			switch (summon->GetEntry())
			{
			case THALORIEN_NPC_THALORIEN:
				EndScene(false);
				break;
			case THALORIEN_NPC_BOSS:
				_Timer = 2000;
				break;
			default:
				for (auto itr = _enemies.begin(); itr != _enemies.end();)
					if ((*itr)->GetGUID() == summon->GetGUID())
						_enemies.erase(itr++);
					else
						itr++;
				if (_enemies.empty())
					_Timer = 2000;
				break;
			}
		}

		void UpdateAI(uint32 diff)
		{
			if (_Timer)
			{
				if (_Timer <= diff)
				{
					switch (_Phase)
					{
					//召唤 寻晨者 卫兵 BOSS
					case 1:
						me->SummonCreature(THALORIEN_NPC_THALORIEN, ThalorienNPCPos[0]);
						for (size_t i = 1; i < 11; i++)
							me->SummonCreature(THALORIEN_NPC_DEFENDER, ThalorienNPCPos[i]);
						me->SummonCreature(THALORIEN_NPC_BOSS, ThalorienNPCPos[11]);
						_Timer = 1000;
						_Phase++;
						break;
					//寻晨者对卫兵说话1
					case 2:
						if (Creature* thalorien = me->GetMap()->GetCreature(_thalorien))
						{
							thalorien->MonsterSay("我们必须不惜一切代价保卫太阳之井。", LANG_UNIVERSAL, NULL);
							thalorien->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						}
							
						_Timer = 3000;
						_Phase++;
						break;
					//寻晨者对卫兵说话2
					case 3:
						if (Creature* thalorien = me->GetMap()->GetCreature(_thalorien))
						{
							thalorien->MonsterSay("我命令你们立即回防太阳之井，荣耀与我们同在。", LANG_UNIVERSAL, NULL);
							thalorien->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
						}
							
						_Timer = 3000;
						_Phase++;
						break;
					//寻晨者对卫兵说话3
					case 4:
						if (Creature* thalorien = me->GetMap()->GetCreature(_thalorien))
						{
							thalorien->MonsterSay("我会留在这里为你们争取尽可能多的时间。", LANG_UNIVERSAL, NULL);
							thalorien->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
						}
							
						_Timer = 3000;
						_Phase++;
						break;
					//寻晨者对卫兵说话4
					case 5:
						if (Creature* thalorien = me->GetMap()->GetCreature(_thalorien))
						{
							thalorien->MonsterSay("与你们并肩战斗，守卫我们的领土和人民，是我最大的荣耀。", LANG_UNIVERSAL, NULL);
							thalorien->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
						}
							
						_Timer = 3000;
						_Phase++;
						break;
					//卫兵说话
					case 6:
						for (auto itr = _gurads.begin(); itr != _gurads.end(); itr++)
						{
							if (Creature* guard = *itr)
							{
								guard->MonsterSay("遵命，长官！", LANG_UNIVERSAL, NULL);
								guard->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
							}
						}				
						_Timer = 1000;
						_Phase++;
						break;
					//卫兵离开
					case 7:
						for (auto itr = _gurads.begin(); itr != _gurads.end();)
						{
							if (Creature* guard = *itr)
							{
								_gurads.erase(itr++);																	
								guard->SetWalk(false);
								guard->GetMotionMaster()->MovePoint(0, 11836.7, -7070.49, 26.9794);
								guard->DespawnOrUnsummon(3000);
							}
							else
								itr++;
						}
						_Timer = 1000;
						_Phase++;
						break;
					//寻晨者移动到BOSS前，移动过程中要讲话
					case 8:
						if (Creature* thalorien = me->GetMap()->GetCreature(_thalorien))
						{
							thalorien->MonsterSay("陌生人...你不是我的士兵。你将会和我一起面对强大的敌人吗？", LANG_UNIVERSAL, NULL);
							thalorien->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
							thalorien->SetWalk(true);
							thalorien->GetMotionMaster()->MovePoint(0, 11777.6, -7063.79, 24.7945);
							if (Creature* boss = me->GetMap()->GetCreature(_boss))
								thalorien->SetFacingToObject(boss);
						}						
						_Timer = 5000;
						_Phase++;
						break;
					//寻晨者对BOSS大喊1
					case 9:
						if (Creature* thalorien = me->GetMap()->GetCreature(_thalorien))
						{
							thalorien->SetWalk(false);
							thalorien->MonsterYell("听好了，天灾军团的渣子们。只要我还活着，你们休想进入太阳之井！", LANG_UNIVERSAL, NULL);
							thalorien->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
							thalorien->SetHomePosition(thalorien->GetPositionX(), thalorien->GetPositionY(), thalorien->GetPositionZ(), thalorien->GetOrientation());
						}					
						_Timer = 1000;
						_Phase++;
						break;
					case 10:
						if (Creature* boss = me->GetMap()->GetCreature(_boss))
							boss->MonsterYell("你将和你的城市一起毁灭。巫妖王将会获得伟大的胜利，然而却没有人会记得你！", LANG_UNIVERSAL, NULL);
						_Timer = 1000;
						_Phase++;
						break;
					//BOSS大喊1，召唤第一波小怪
					case 11:
						if (Creature* boss = me->GetMap()->GetCreature(_boss))
						{
							boss->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
							boss->MonsterYell("进攻！", LANG_UNIVERSAL, NULL);
						}

						for (size_t i = 12; i < 16; i++)
						{
							if (Creature* c = me->SummonCreature(THALORIEN_NPC_ENEMY_1, ThalorienNPCPos[i]))
								AddThalorienThreat(c);
						}
							
						_Timer = 0;
						_Phase++;
						break;
					//召唤第二波小怪
					case 12:
						if (Creature* boss = me->GetMap()->GetCreature(_boss))
						{
							boss->MonsterYell("食尸鬼，干掉那个精灵！", LANG_UNIVERSAL, NULL);
							boss->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
						}
							
						for (size_t i = 12; i < 16; i++)
						{
							if (Creature* c = me->SummonCreature(THALORIEN_NPC_ENEMY_2, ThalorienNPCPos[i]))
								AddThalorienThreat(c);
						}
						_Timer = 0;
						_Phase++;
						break;
					//召唤第三波小怪
					case 13:
						if (Creature* boss = me->GetMap()->GetCreature(_boss))
						{
							boss->MonsterYell("天灾军团战无不胜！蜘蛛们，现在进攻！", LANG_UNIVERSAL, NULL);
							boss->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
						}

						for (size_t i = 12; i < 15; i++)
						{
							if (Creature* c = me->SummonCreature(THALORIEN_NPC_ENEMY_3, ThalorienNPCPos[i]))
								AddThalorienThreat(c);
						}
						_Timer = 0;
						_Phase++;
						break;
					//BOSS激活
					case 14:
						if (Creature* boss = me->GetMap()->GetCreature(_boss))
						{
							boss->MonsterYell("我要亲自杀了你！", LANG_UNIVERSAL, NULL);
							boss->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
						}			

						if (Creature* boss = me->GetMap()->GetCreature(_boss))
						{
							boss->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
							AddThalorienThreat(boss);
						}				
						_Timer = 0;
						_Phase++;
						break;
					//与玩家对话
					case 15:
						if (Creature* thalorien = me->GetMap()->GetCreature(_thalorien))
						{
							thalorien->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
							thalorien->MonsterSay("我...从未想过能从这场战斗中生存下来。", LANG_UNIVERSAL, NULL);
						}					
						_Timer = 5000;
						_Phase++;
						break;
					case 16:
						if (Creature* thalorien = me->GetMap()->GetCreature(_thalorien))
						{
							thalorien->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
							thalorien->MonsterSay("发生了什么？我怎么在这里？", LANG_UNIVERSAL, NULL);
						}
							
						_Timer = 5000;
						_Phase++;
						break;
					case 17:
						if (Creature* thalorien = me->GetMap()->GetCreature(_thalorien))
						{
							thalorien->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
							thalorien->MonsterSay("这是...奎尔德拉！你得到了这把剑！", LANG_UNIVERSAL, NULL);
						}
							
						_Timer = 5000;
						_Phase++;
						break;
					case 18:
						if (Creature* thalorien = me->GetMap()->GetCreature(_thalorien))
							thalorien->MonsterSay("这把剑选择了你成为新的主人，这是我的祝福，请像我一样与天灾军团战斗！", LANG_UNIVERSAL, NULL);
							
						_Timer = 5000;
						_Phase++;
						break;
					case 19:
						EndScene(true);
						break;
					}
				}
				else
					_Timer -= diff;
			}	
		}

	private:
		uint32 _Timer;
		uint32 _Phase;
		std::list<Creature*> _gurads;
		std::list<Creature*> _enemies;
		uint64 _thalorien;
		uint64 _boss;
	};
};


//净化奎尔德拉
/*
-- creature
DELETE FROM creature_template WHERE entry IN (37781,36991,38047,37746,37763,37764,37765,38056);

INSERT INTO creature_template (entry, difficulty_entry_1, difficulty_entry_2, difficulty_entry_3, KillCredit1, KillCredit2, modelid1, modelid2, modelid3, modelid4, name, subname, IconName, gossip_menu_id, minlevel, maxlevel, exp, faction, npcflag, speed_walk, speed_run, scale, rank, mindmg, maxdmg, dmgschool, attackpower, dmg_multiplier, baseattacktime, rangeattacktime, unit_class, unit_flags, unit_flags2, dynamicflags, family, trainer_type, trainer_spell, trainer_class, trainer_race, minrangedmg, maxrangedmg, rangedattackpower, type, type_flags, lootid, pickpocketloot, skinloot, resistance1, resistance2, resistance3, resistance4, resistance5, resistance6, spell1, spell2, spell3, spell4, spell5, spell6, spell7, spell8, PetSpellDataId, VehicleId, mingold, maxgold, AIName, MovementType, InhabitType, HoverHeight, Health_mod, Mana_mod, Armor_mod, RacialLeader, questItem1, questItem2, questItem3, questItem4, questItem5, questItem6, movementId, RegenHealth, mechanic_immune_mask, flags_extra, ScriptName, VerifiedBuild) VALUES('36991','0','0','0','0','0','21460','0','0','0','太阳之井守护者','','','0','80','80','1','1960','0','1','1.14286','1','0','0','0','0','0','1','2000','2000','1','0','2048','0','0','0','0','0','0','0','0','0','9','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','NullCreatureAI','0','3','1','2','1','1','0','0','0','0','0','0','0','144','1','0','0','','12340');
INSERT INTO creature_template (entry, difficulty_entry_1, difficulty_entry_2, difficulty_entry_3, KillCredit1, KillCredit2, modelid1, modelid2, modelid3, modelid4, name, subname, IconName, gossip_menu_id, minlevel, maxlevel, exp, faction, npcflag, speed_walk, speed_run, scale, rank, mindmg, maxdmg, dmgschool, attackpower, dmg_multiplier, baseattacktime, rangeattacktime, unit_class, unit_flags, unit_flags2, dynamicflags, family, trainer_type, trainer_spell, trainer_class, trainer_race, minrangedmg, maxrangedmg, rangedattackpower, type, type_flags, lootid, pickpocketloot, skinloot, resistance1, resistance2, resistance3, resistance4, resistance5, resistance6, spell1, spell2, spell3, spell4, spell5, spell6, spell7, spell8, PetSpellDataId, VehicleId, mingold, maxgold, AIName, MovementType, InhabitType, HoverHeight, Health_mod, Mana_mod, Armor_mod, RacialLeader, questItem1, questItem2, questItem3, questItem4, questItem5, questItem6, movementId, RegenHealth, mechanic_immune_mask, flags_extra, ScriptName, VerifiedBuild) VALUES('37746','0','0','0','0','0','11686','0','0','0','太阳之井施法假人','','','0','80','80','2','114','0','1','1.14286','1','0','0','0','0','0','1','2000','2000','1','33554432','2048','0','0','0','0','0','0','0','0','0','10','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','NullCreatureAI','0','4','1','1','1','1','0','0','0','0','0','0','0','0','1','0','0','','12340');
INSERT INTO creature_template (entry, difficulty_entry_1, difficulty_entry_2, difficulty_entry_3, KillCredit1, KillCredit2, modelid1, modelid2, modelid3, modelid4, name, subname, IconName, gossip_menu_id, minlevel, maxlevel, exp, faction, npcflag, speed_walk, speed_run, scale, rank, mindmg, maxdmg, dmgschool, attackpower, dmg_multiplier, baseattacktime, rangeattacktime, unit_class, unit_flags, unit_flags2, dynamicflags, family, trainer_type, trainer_spell, trainer_class, trainer_race, minrangedmg, maxrangedmg, rangedattackpower, type, type_flags, lootid, pickpocketloot, skinloot, resistance1, resistance2, resistance3, resistance4, resistance5, resistance6, spell1, spell2, spell3, spell4, spell5, spell6, spell7, spell8, PetSpellDataId, VehicleId, mingold, maxgold, AIName, MovementType, InhabitType, HoverHeight, Health_mod, Mana_mod, Armor_mod, RacialLeader, questItem1, questItem2, questItem3, questItem4, questItem5, questItem6, movementId, RegenHealth, mechanic_immune_mask, flags_extra, ScriptName, VerifiedBuild) VALUES('37763','0','0','0','0','0','19706','0','0','0','大博学者洛摩斯','','','0','80','80','1','35','0','1','1.14286','1','3','0','0','0','0','1','2000','2000','2','0','2048','0','0','0','0','0','0','0','0','0','7','4','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','','0','3','1','200','20','1','0','0','0','0','0','0','0','0','1','0','0','','12340');
INSERT INTO creature_template (entry, difficulty_entry_1, difficulty_entry_2, difficulty_entry_3, KillCredit1, KillCredit2, modelid1, modelid2, modelid3, modelid4, name, subname, IconName, gossip_menu_id, minlevel, maxlevel, exp, faction, npcflag, speed_walk, speed_run, scale, rank, mindmg, maxdmg, dmgschool, attackpower, dmg_multiplier, baseattacktime, rangeattacktime, unit_class, unit_flags, unit_flags2, dynamicflags, family, trainer_type, trainer_spell, trainer_class, trainer_race, minrangedmg, maxrangedmg, rangedattackpower, type, type_flags, lootid, pickpocketloot, skinloot, resistance1, resistance2, resistance3, resistance4, resistance5, resistance6, spell1, spell2, spell3, spell4, spell5, spell6, spell7, spell8, PetSpellDataId, VehicleId, mingold, maxgold, AIName, MovementType, InhabitType, HoverHeight, Health_mod, Mana_mod, Armor_mod, RacialLeader, questItem1, questItem2, questItem3, questItem4, questItem5, questItem6, movementId, RegenHealth, mechanic_immune_mask, flags_extra, ScriptName, VerifiedBuild) VALUES('37764','0','0','0','0','0','17122','0','0','0','洛索玛·塞隆','奎尔萨拉斯摄政王','','0','80','80','2','35','0','1','1.14286','1','3','0','0','0','0','1','2000','2000','2','0','2048','0','0','0','0','0','0','0','0','0','7','76','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','NullCreatureAI','0','3','1','400','30','1','1','0','0','0','0','0','0','0','1','0','0','','12340');
INSERT INTO creature_template (entry, difficulty_entry_1, difficulty_entry_2, difficulty_entry_3, KillCredit1, KillCredit2, modelid1, modelid2, modelid3, modelid4, name, subname, IconName, gossip_menu_id, minlevel, maxlevel, exp, faction, npcflag, speed_walk, speed_run, scale, rank, mindmg, maxdmg, dmgschool, attackpower, dmg_multiplier, baseattacktime, rangeattacktime, unit_class, unit_flags, unit_flags2, dynamicflags, family, trainer_type, trainer_spell, trainer_class, trainer_race, minrangedmg, maxrangedmg, rangedattackpower, type, type_flags, lootid, pickpocketloot, skinloot, resistance1, resistance2, resistance3, resistance4, resistance5, resistance6, spell1, spell2, spell3, spell4, spell5, spell6, spell7, spell8, PetSpellDataId, VehicleId, mingold, maxgold, AIName, MovementType, InhabitType, HoverHeight, Health_mod, Mana_mod, Armor_mod, RacialLeader, questItem1, questItem2, questItem3, questItem4, questItem5, questItem6, movementId, RegenHealth, mechanic_immune_mask, flags_extra, ScriptName, VerifiedBuild) VALUES('37765','0','0','0','0','0','18146','0','0','0','阿瑞克·追日者上尉','高等精灵代表','','0','80','80','1','534','0','1','1.14286','1','0','0','0','0','0','1','2000','2000','1','0','2048','0','0','0','0','0','0','0','0','0','7','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','NullCreatureAI','0','3','1','1','1','1','0','0','0','0','0','0','0','0','1','0','0','','12340');
INSERT INTO creature_template (entry, difficulty_entry_1, difficulty_entry_2, difficulty_entry_3, KillCredit1, KillCredit2, modelid1, modelid2, modelid3, modelid4, name, subname, IconName, gossip_menu_id, minlevel, maxlevel, exp, faction, npcflag, speed_walk, speed_run, scale, rank, mindmg, maxdmg, dmgschool, attackpower, dmg_multiplier, baseattacktime, rangeattacktime, unit_class, unit_flags, unit_flags2, dynamicflags, family, trainer_type, trainer_spell, trainer_class, trainer_race, minrangedmg, maxrangedmg, rangedattackpower, type, type_flags, lootid, pickpocketloot, skinloot, resistance1, resistance2, resistance3, resistance4, resistance5, resistance6, spell1, spell2, spell3, spell4, spell5, spell6, spell7, spell8, PetSpellDataId, VehicleId, mingold, maxgold, AIName, MovementType, InhabitType, HoverHeight, Health_mod, Mana_mod, Armor_mod, RacialLeader, questItem1, questItem2, questItem3, questItem4, questItem5, questItem6, movementId, RegenHealth, mechanic_immune_mask, flags_extra, ScriptName, VerifiedBuild) VALUES('37781','0','0','0','0','0','15511','0','0','0','太阳之井荣誉守卫','','','0','80','80','2','35','0','1','1.14286','1','1','0','0','0','0','1','2000','2000','1','0','2048','0','0','0','0','0','0','0','0','0','7','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','NullCreatureAI','0','3','1','3','1','1','0','0','0','0','0','0','0','144','1','0','0','','12340');
INSERT INTO creature_template (entry, difficulty_entry_1, difficulty_entry_2, difficulty_entry_3, KillCredit1, KillCredit2, modelid1, modelid2, modelid3, modelid4, name, subname, IconName, gossip_menu_id, minlevel, maxlevel, exp, faction, npcflag, speed_walk, speed_run, scale, rank, mindmg, maxdmg, dmgschool, attackpower, dmg_multiplier, baseattacktime, rangeattacktime, unit_class, unit_flags, unit_flags2, dynamicflags, family, trainer_type, trainer_spell, trainer_class, trainer_race, minrangedmg, maxrangedmg, rangedattackpower, type, type_flags, lootid, pickpocketloot, skinloot, resistance1, resistance2, resistance3, resistance4, resistance5, resistance6, spell1, spell2, spell3, spell4, spell5, spell6, spell7, spell8, PetSpellDataId, VehicleId, mingold, maxgold, AIName, MovementType, InhabitType, HoverHeight, Health_mod, Mana_mod, Armor_mod, RacialLeader, questItem1, questItem2, questItem3, questItem4, questItem5, questItem6, movementId, RegenHealth, mechanic_immune_mask, flags_extra, ScriptName, VerifiedBuild) VALUES('38047','0','0','0','0','0','30894','30896','30895','30897','血精灵朝圣者','','','0','80','80','2','35','0','1','1.14286','1','0','0','0','0','0','1','2000','2000','2','0','2048','0','0','0','0','0','0','0','0','0','7','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','NullCreatureAI','0','3','1','1','1','1','0','0','0','0','0','0','0','0','1','0','0','','12340');
INSERT INTO creature_template (entry, difficulty_entry_1, difficulty_entry_2, difficulty_entry_3, KillCredit1, KillCredit2, modelid1, modelid2, modelid3, modelid4, name, subname, IconName, gossip_menu_id, minlevel, maxlevel, exp, faction, npcflag, speed_walk, speed_run, scale, rank, mindmg, maxdmg, dmgschool, attackpower, dmg_multiplier, baseattacktime, rangeattacktime, unit_class, unit_flags, unit_flags2, dynamicflags, family, trainer_type, trainer_spell, trainer_class, trainer_race, minrangedmg, maxrangedmg, rangedattackpower, type, type_flags, lootid, pickpocketloot, skinloot, resistance1, resistance2, resistance3, resistance4, resistance5, resistance6, spell1, spell2, spell3, spell4, spell5, spell6, spell7, spell8, PetSpellDataId, VehicleId, mingold, maxgold, AIName, MovementType, InhabitType, HoverHeight, Health_mod, Mana_mod, Armor_mod, RacialLeader, questItem1, questItem2, questItem3, questItem4, questItem5, questItem6, movementId, RegenHealth, mechanic_immune_mask, flags_extra, ScriptName, VerifiedBuild) VALUES('38056','0','0','0','0','0','21008','0','0','0','司库嘉立洛斯','','','0','80','80','2','35','0','1','1.14286','1','0','0','0','0','0','1','2000','2000','2','0','2048','0','0','0','0','0','0','0','0','0','7','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','NullCreatureAI','0','3','1','1','1','1','0','0','0','0','0','0','0','0','1','0','0','npc_queldelar_sunwell_plateau','12340');
DELETE FROM creature WHERE map = 580 AND id IN (37781,36991,38047,37746,37763,37764,37765,38056);
DELETE FROM creature WHERE guid = 1976653;
INSERT INTO creature (guid, id, map, spawnMask, phaseMask, modelid, equipment_id, position_x, position_y, position_z, orientation, spawntimesecs, spawndist, currentwaypoint, curhealth, curmana, MovementType, npcflag, unit_flags, dynamicflags) VALUES('1976653','38056','580','1','2048','0','0','1671.49','606.427','28.0503','3.79407','300','0','0','41','60','0','0','0','0');

-- spell
DELETE FROM spell_script_names WHERE spell_id IN(70477,70548);
INSERT INTO spell_script_names (spell_id, ScriptName) VALUES('70477','spell_bh_cleanse_quel_delar');
INSERT INTO spell_script_names (spell_id, ScriptName) VALUES('70548','spell_bh_cleanse_quel_delar');

-- gob
delete from gameobject_template where entry =195682;
insert into gameobject_template (entry, type, displayId, name, IconName, castBarCaption, unk1, faction, flags, size, questItem1, questItem2, questItem3, questItem4, questItem5, questItem6, data0, data1, data2, data3, data4, data5, data6, data7, data8, data9, data10, data11, data12, data13, data14, data15, data16, data17, data18, data19, data20, data21, data22, data23, AIName, ScriptName, VerifiedBuild) values('195682','22','8111','达拉然传送门','','','','94','0','1','0','0','0','0','0','0','68328','0','0','1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','','','12340');
DELETE FROM gameobject WHERE map = 580 AND id = 195682;
*/


enum QuelDelarCreatures
{
	NPC_ROMMATH = 37763,
	NPC_THERON = 37764,
	NPC_AURIC = 37765,
	NPC_QUEL_GUARD = 37781,
	NPC_TRIGGER = 38056
};

enum QuelDelarGameobjects
{
	GO_QUEL_DANAR = 201794
};
enum QuelDelarMisc
{
	ITEM_TAINTED_QUELDANAR_1 = 49879,
	ITEM_TAINTED_QUELDANAR_2 = 49889,
	SPELL_WRATH_QUEL_DANAR = 70493,
	SPELL_ICY_PRISON = 70540
};

class npc_sunwell_warder : public CreatureScript
{
public:
	npc_sunwell_warder() : CreatureScript("npc_sunwell_warder") { }
	bool OnGossipHello(Player* player, Creature* creature)
	{
		player->PlayerTalkClass->ClearMenus();

		if (player->HasItemCount(ITEM_TAINTED_QUELDANAR_1, 1) || player->HasItemCount(ITEM_TAINTED_QUELDANAR_2, 1))
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "带我去太阳之井", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

		player->SEND_GOSSIP_MENU(creature->GetEntry(), creature->GetGUID());
		return true;
	}
	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
	{
		player->PlayerTalkClass->ClearMenus();

		player->SetGameMaster(true);
		player->TeleportTo(580, 1728.5f, 709.219f, 71.1905f, 2.78676f);
		player->SetGameMaster(false);
		player->SetPhaseMask(2048, true);
		player->CLOSE_GOSSIP_MENU();

		return true;
	}
};


class npc_queldelar_sunwell_plateau : public CreatureScript
{
public:
	npc_queldelar_sunwell_plateau() : CreatureScript("npc_queldelar_sunwell_plateau") { }

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_queldelar_sunwell_plateauAI(creature);
	}

	struct npc_queldelar_sunwell_plateauAI : public ScriptedAI
	{
		npc_queldelar_sunwell_plateauAI(Creature* creature) : ScriptedAI(creature)
		{
			Reset();
		}

		void Reset()
		{
			_Phase = 0;
			_Timer = 0;
		}

		void BeginScene()
		{
			_Timer = 100;
			_Phase = 1;
		}

		void MoveInLineOfSight(Unit* who) override
		{
			if (_Phase != 0)
				return;

			if (who->GetDistance2d(me) > 20.0f)
				return;

			std::ostringstream oss;
			oss << "先生们，女士们，有请奎尔德拉的新主人，" << who->GetName() << "。";
			me->MonsterYell(oss.str().c_str(), LANG_UNIVERSAL, NULL);

			if (who->GetTypeId() == TYPEID_PLAYER)
				_player = who->GetGUID();

			_Phase++;

			me->SummonCreature(37781, 1614.89, 598.706, 33.3211, 0.367885);
			me->SummonCreature(37781, 1677.1, 570.175, 33.3212, 4.29488);
			me->SummonCreature(37781, 1669.38, 573.631, 33.3212, 4.2242);
			me->SummonCreature(37781, 1654.74, 583.131, 33.3212, 4.03963);
			me->SummonCreature(37781, 1649.73, 588.163, 33.3212, 3.86292);
			me->SummonCreature(37781, 1638.94, 602.943, 33.3212, 3.5252);
			me->SummonCreature(37781, 1635.69, 610.968, 33.3212, 3.45451);
			me->SummonCreature(37781, 1632.77, 627.612, 32.9608, 3.26994);
			me->SummonCreature(37781, 1735.87, 599.171, 28.0505, 2.41);
			me->SummonCreature(37781, 1727.98, 591.396, 28.0505, 2.23);
			me->SummonCreature(37781, 1670.23, 665.44, 28.0503, 5.41);
			me->SummonCreature(37781, 1662.86, 658.458, 28.0503, 5.59);
			me->SummonCreature(37781, 1663.75, 633.058, 28.0503, 4.49);
			me->SummonCreature(37781, 1665.48, 640.896, 28.0503, 4.49);
			me->SummonCreature(37781, 1674.14, 638.984, 28.0503, 4.49);
			me->SummonCreature(37781, 1672.48, 631.053, 28.0503, 4.55);
			me->SummonCreature(37781, 1688.26, 616.497, 27.3452, 3.95);
			me->SummonCreature(37781, 1683.62, 624.152, 27.3538, 3.42);
			me->SummonCreature(37781, 1781.84, 680.202, 71.1904, 3.67);
			me->SummonCreature(37781, 1767.5, 697.132, 71.1904, 3.91);
			me->SummonCreature(37781, 1748.77, 709.204, 71.1904, 4.24);
			me->SummonCreature(37781, 1727.06, 716.86, 71.1904, 4.48);
			me->SummonCreature(37781, 1704.8, 720.264, 71.058, 4.78);
			me->SummonCreature(37781, 1718.79, 698.847, 71.1902, 1.34);
			me->SummonCreature(37781, 1727.47, 696.601, 71.1902, 1.10);
			me->SummonCreature(37781, 1742.9, 688.146, 71.1902, 1.05);
			me->SummonCreature(37781, 1748.59, 683.721, 71.1902, 0.62);
			me->SummonCreature(37781, 1760.28, 670.492, 71.1902, 0.67);
			me->SummonCreature(37781, 1764.82, 663.761, 71.1902, 0.63);
			me->SummonCreature(37781, 1769.97, 650.558, 71.1914, 1.95);
			me->SummonCreature(37781, 1776.09, 652.853, 71.1943, 2.17);
			me->SummonCreature(37781, 1793.8, 661.515, 71.1943, 2.19);
			me->SummonCreature(36991, 1619.75, 634.382, 33.486, 4.90);
			me->SummonCreature(36991, 1625.67, 577.498, 33.3213, 0.62);
			me->SummonCreature(36991, 1640.45, 558.392, 33.3213, 0.85);
			me->SummonCreature(36991, 1658.87, 545.124, 33.3213, 1.16);
			me->SummonCreature(36991, 1704.09, 583.276, 28.182, 1.75);
			me->SummonCreature(36991, 1653.75, 634.033, 28.085, 6.10);
			me->SummonCreature(36991, 1694.93, 674.09, 28.0503, 4.76);
			me->SummonCreature(36991, 1744.84, 621.776, 28.0503, 3.00);
			me->SummonCreature(38047, 1695.1, 583.061, 28.2018, 3.56);
			me->SummonCreature(38047, 1692.36, 582.342, 29.06, 0.23);
			me->SummonCreature(38047, 1688.99, 604.902, 28.0608, 0.48);
			me->SummonCreature(38047, 1690.91, 605.88, 28.3581, 3.61);
			me->SummonCreature(38047, 1669.28, 624.177, 28.0504, 2.57);
			me->SummonCreature(38047, 1667.75, 625.38, 28.0504, 5.73);
			me->SummonCreature(38047, 1716.75, 632.675, 27.7991, 3.43);
			me->SummonCreature(38047, 1705.7, 612.287, 27.6113, 1.99);
			me->SummonCreature(38047, 1702.86, 611.087, 27.6827, 1.85);
			me->SummonCreature(38047, 1700.1, 610.616, 27.6777, 1.69);
			me->SummonCreature(38047, 1685.62, 638.985, 27.6363, 5.59);
			me->SummonCreature(38047, 1683.64, 636.337, 27.6529, 5.82);
			me->SummonCreature(38047, 1682.23, 633.38, 27.638, 6.01);
			me->SummonCreature(38047, 1699.66, 659.52, 28.0503, 4.68);
			me->SummonCreature(38047, 1702.88, 659.398, 28.0503, 4.55);
			me->SummonCreature(38047, 1706.31, 658.903, 28.0503, 4.53);
			me->SummonCreature(38047, 1707.11, 656.197, 28.0503, 4.48);
			me->SummonCreature(38047, 1701.32, 657.371, 28.0503, 4.61);
			me->SummonCreature(38047, 1704.05, 656.947, 28.0503, 4.53);
			me->SummonCreature(37746, 1698.87, 628.417, 28.1989, 3.69);
			me->SummonCreature(37763, 1662.56, 622.758, 28.0502, 4.88);
			me->SummonCreature(37764, 1666.48, 620.999, 28.0504, 5.00);
			me->SummonCreature(37765, 1686.1, 607.186, 28.1281, 1.99);

			if (GameObject* go = me->SummonGameObject(195682, 1666.13, 662.584, 28.0504, 5.44551, 0, 0, 0, 0, 0))
			{
				go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
				_portal1 = go->GetGUID();
			}
				
			if (GameObject* go = me->SummonGameObject(195682, 1732.62, 594.658, 28.0503, 2.27643, 0, 0, 0, 0, 0))
			{
				go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
				_portal2 = go->GetGUID();
			}
				
		}

		void JustSummoned(Creature* summon) override
		{
			_summonmap.insert(std::make_pair(summon->GetGUID(), summon->GetOrientation()));

			switch (summon->GetEntry())
			{
			case NPC_THERON:
				summon->SetWalk(true);
				_theron = summon->GetGUID();
				break;
			case NPC_ROMMATH:
				summon->SetWalk(true);
				_rommath = summon->GetGUID();
				break;
			case NPC_AURIC:
				summon->SetWalk(true);
				_auric = summon->GetGUID();
				break;
			case NPC_QUEL_GUARD:
				_guards.push_back(summon);
			default:
				break;
			}
		}

		void GuardAction(uint8 aciton)
		{
			if (aciton == 0)
			{
				if (Player* pl = ObjectAccessor::GetPlayer(*me, _player))
					if (Creature* guard = pl->FindNearestCreature(NPC_QUEL_GUARD, 500.0f))
					{
						guard->GetMotionMaster()->MoveChase(pl, 1.0f);
						guard->MonsterSay("放下武器投降，你这个叛徒！", LANG_UNIVERSAL, NULL);
						guard->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_READY1H);
						guard->CastSpell(pl, SPELL_ICY_PRISON);
						guard->SetTarget(_player);
					}

				if (Creature* auric = ObjectAccessor::GetCreature(*me, _auric))
					if (Creature* guard = auric->FindNearestCreature(NPC_QUEL_GUARD, 500.0f))
					{
						_summonmap.insert(std::make_pair(guard->GetGUID(), guard->GetOrientation()));
						guard->GetMotionMaster()->MoveChase(auric, 1.0f);
						guard->MonsterSay("放下武器投降，你这个叛徒！", LANG_UNIVERSAL, NULL);
						guard->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_READY1H);
						guard->SetTarget(_auric);
					}
			}
			else
			{
				for (auto itr = _guards.begin(); itr != _guards.end(); itr++)
				{
					if (Creature* guard = *itr)
					{
						guard->SetTarget(0);
						guard->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_STAND);
						guard->GetMotionMaster()->MovePoint(0, guard->GetHomePosition());
					}
				}
			}
		}

		void SummonedMovementInform(Creature* summon, uint32 type, uint32 point)
		{
			if (type != CHASE_MOTION_TYPE && type != POINT_MOTION_TYPE)
				return;

			switch (summon->GetEntry())
			{
			case NPC_THERON:
			case NPC_ROMMATH:
			case NPC_AURIC:
				if (point == 0)
				{
					if (GameObject* quelDelar = ObjectAccessor::GetGameObject(*me, _QuelDelar))
						summon->SetFacingToObject(quelDelar);
				}
				else if (point == 1)
				{
					auto itr = _summonmap.find(summon->GetGUID());
					if (itr != _summonmap.end())
						summon->SetOrientation(itr->second);
				}
				
				break;
			case NPC_QUEL_GUARD:
				if (type == CHASE_MOTION_TYPE)
				{
					Unit* target = ObjectAccessor::GetUnit(*me, summon->GetTarget());
						summon->SetFacingToObject(target);
				}
				else if (type == POINT_MOTION_TYPE)
				{
					auto itr = _summonmap.find(summon->GetGUID());
					if (itr != _summonmap.end())
						summon->SetOrientation(itr->second);
				}
				break;
			}
		}

		void UpdateAI(uint32 diff)
		{
			if (_Timer)
			{
				if (_Timer <= diff)
				{
					switch (_Phase)
					{
					case 1:
						if (GameObject* quelDelar = me->SummonGameObject(GO_QUEL_DANAR, 1683.99f, 620.231f, 29.3599f, 0.410932f, 0, 0, 0, 0, 0))
						{
							_QuelDelar = quelDelar->GetGUID();
							quelDelar->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
						}
						_Timer = 1000;
						_Phase++;
						break;
					case 2:
						if (Creature* guard = me->FindNearestCreature(NPC_QUEL_GUARD, 100.0f, true))
							guard->MonsterSay("真的是奎尔德拉吗？", LANG_UNIVERSAL, NULL);
						_Timer = 4000;
						_Phase++;
						break;
					
					case 3:
						if (Creature* theron = ObjectAccessor::GetCreature(*me, _theron))
							theron->MonsterSay("让我们看一看...", LANG_UNIVERSAL, NULL);
						
						if (Creature* rommath = ObjectAccessor::GetCreature(*me, _rommath))
							rommath->GetMotionMaster()->MovePoint(0, 1675.8f, 617.19f, 28.0504f);

						if (Creature*auric = ObjectAccessor::GetCreature(*me, _auric))
							auric->GetMotionMaster()->MovePoint(0, 1681.77f, 612.084f, 28.4409f);

						if (Creature* theron = ObjectAccessor::GetCreature(*me, _theron))
							theron->GetMotionMaster()->MovePoint(0, 1677.07f, 613.122f, 28.0504f);

						_Timer = 10000;
						_Phase++;
						break;
					case 4:
						if (Creature* rommath = ObjectAccessor::GetCreature(*me, _rommath))
							rommath->MonsterSay("洛塞玛，看，毫无疑问，这是奎尔德拉。", LANG_UNIVERSAL, NULL);

						_Timer = 10000;
						_Phase++;
						break;
					case 5:
						if (Creature* theron = ObjectAccessor::GetCreature(*me, _theron))
						{
							std::ostringstream oss;
							if (Player* pl = ObjectAccessor::GetPlayer(*me, _player))
								oss << "好极了，非常感谢你将奎尔德拉带给它真正的主人，" << pl->GetName() << "。";
							theron->MonsterSay(oss.str().c_str(), LANG_UNIVERSAL, NULL);
							theron->GetMotionMaster()->MovePoint(0, 1681.9f, 619.13f, 27.95f);
						}
							
						_Timer = 10000;
						_Phase++;
						break;
						
					case 6:
						if (Creature* theron = ObjectAccessor::GetCreature(*me, _theron))
							theron->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
						_Timer = 1000;
						_Phase++;
						break;
						
					case 7:
						if (Creature* theron = ObjectAccessor::GetCreature(*me, _theron))
						{
							theron->CastSpell(theron, SPELL_WRATH_QUEL_DANAR, true);
							theron->KnockbackFrom(theron->GetPositionX(), theron->GetPositionY(), 20.0f, 10.0f);
						}
							
						_Timer = 1000;
						_Phase++;
						break;
						
					case 8:
						if (Creature* rommath = ObjectAccessor::GetCreature(*me, _rommath))
						{
							if (Player* player = me->SelectNearestPlayer(200.0f))
								rommath->AddAura(SPELL_ICY_PRISON, player);
	
							rommath->MonsterSay("背叛？", LANG_UNIVERSAL, NULL);
						}

						GuardAction(0);
						_Timer = 5000;
						_Phase++;
						break;
					case 9:
						_Timer = 1000;
						_Phase++;
						break;
					case 10:
						if (Creature* auric = ObjectAccessor::GetCreature(*me, _auric))
							auric->MonsterSay("这不是我的错，洛塞玛。这不是一个叛徒。", LANG_UNIVERSAL, NULL);
						_Timer = 5000;
						_Phase++;
						break;
					case 11:
						if (Creature* auric = ObjectAccessor::GetCreature(*me, _auric))
							auric->MonsterSay("让你的人退下，是洛塞玛的愚蠢导致他受了伤。人们不可以选择奎尔德拉，奎尔德拉自己选择主人。", LANG_UNIVERSAL, NULL);

						_Timer = 5000;
						_Phase++;
						break;
					case 12:
						if (Creature* rommath = ObjectAccessor::GetCreature(*me, _rommath))
							rommath->MonsterSay("卫兵，退下。", LANG_UNIVERSAL, NULL);
						_Timer = 5000;
						_Phase++;
						break;
					case 13:
						GuardAction(1);
						if (Creature* rommath = ObjectAccessor::GetCreature(*me, _rommath))
						{
							rommath->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
							std::ostringstream oss;
							if (Player* pl = ObjectAccessor::GetPlayer(*me, _player))
								oss << "你得到了一直寻找的力量，" << pl->GetName() << "。拿上这把剑，离开吧。至于你，阿瑞克，在这神圣之地，请记住你自己说过的话。";

							rommath->MonsterSay(oss.str().c_str(), LANG_UNIVERSAL, NULL);
						}
						_Timer = 1000;
						_Phase++;
						break;
					case 14:
						if (Creature* auric = ObjectAccessor::GetCreature(*me, _auric))
						{
							auric->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
							auric->MonsterSay("拿着这把剑，穿过去达拉然的传送门。你完成了很多人梦寐以求多年的事，你重铸了奎尔德拉。", LANG_UNIVERSAL, NULL);
						}

						if (GameObject* quelDelar = ObjectAccessor::GetGameObject(*me, _QuelDelar))
						{
							quelDelar->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
							if (Player* pl = ObjectAccessor::GetPlayer(*me, _player))
								pl->GetSession()->SendAreaTriggerMessage("奎尔德拉现在可以拾取了");
						}
							

						if (GameObject* go = ObjectAccessor::GetGameObject(*me, _portal1))
							go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);

						if (GameObject* go = ObjectAccessor::GetGameObject(*me, _portal2))
							go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
					
						_Timer = 1000;
						_Phase++;
						break;
					case 15:
						if (Creature* auric = ObjectAccessor::GetCreature(*me, _auric))
							auric->GetMotionMaster()->MovePoint(1, auric->GetHomePosition());
						if (Creature* rommath = ObjectAccessor::GetCreature(*me, _rommath))
							rommath->GetMotionMaster()->MovePoint(1, rommath->GetHomePosition());
						if (Creature* theron = ObjectAccessor::GetCreature(*me, _theron))	
							theron->GetMotionMaster()->MovePoint(1, theron->GetHomePosition());
						_Timer = 5000;
						_Phase++;
						break;
					}
				}
				else
					_Timer -= diff;
			}
		}

	private:
		uint32 _Timer;
		uint32 _Phase;
		
		uint64 _rommath;
		uint64 _auric;
		uint64 _theron;
		uint64 _player;
		uint64 _QuelDelar;
		uint64 _portal1;
		uint64 _portal2;

		std::list<Creature*> _guards;
		std::unordered_map<uint64, float> _summonmap;
	};
};

class spell_bh_cleanse_quel_delar : public SpellScriptLoader
{
public:
	spell_bh_cleanse_quel_delar() : SpellScriptLoader("spell_bh_cleanse_quel_delar") { }

	class spell_bh_cleanse_quel_delar_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_bh_cleanse_quel_delar_SpellScript);

		void OnEffect(SpellEffIndex effIndex)
		{
			if (Unit* caster = GetCaster())
				if (Creature* creature = caster->FindNearestCreature(NPC_TRIGGER, 200.0f, true))
					if (npc_queldelar_sunwell_plateau::npc_queldelar_sunwell_plateauAI* creatureAI = CAST_AI(npc_queldelar_sunwell_plateau::npc_queldelar_sunwell_plateauAI, creature->AI()))
						creatureAI->BeginScene();
		}

		void Register()
		{
			OnEffectLaunch += SpellEffectFn(spell_bh_cleanse_quel_delar_SpellScript::OnEffect, EFFECT_0, SPELL_EFFECT_SEND_EVENT);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_bh_cleanse_quel_delar_SpellScript();
	}
};

void AddSC_queldelar()
{
	new npc_thalorien();
	new npc_sunwell_warder();
	new npc_queldelar_sunwell_plateau();
	new spell_bh_cleanse_quel_delar();
}
