//#pragma execution_character_set("utf-8")
//#include "FunctionCollection.h"
//#include "../Custom/HonorRank/HonorRank.h"
//#include "../Lottery/Lottery.h"
//#include "../CommonFunc/CommonFunc.h"
//#include "../Recruit/Recruit.h"
//#include "../CustomEvent/Event.h"
//#include "Guild.h"
//#include "../Requirement/Requirement.h"
//#include "../Reward/Reward.h"
//#include "../Quest/QuestMod.h"
//#include "../VIP/VIP.h"
//#include "Group.h"
//#include "MapManager.h"
//#include "../ResetInstance/ResetInstance.h"
//#include "../Switch/Switch.h"
//#include "../Skill/Skill.h"
//#include "../Rank/Rank.h"
//#include "../Faction/Faction.h"
//#include "../Morph/Morph.h"
//#include "../GCAddon/GCAddon.h"
//#include "../LuckDraw/LuckDraw.h"
//#include "../SignIn/SignIn.h"
//#include "../Recovery/Recovery.h"
//#include "../StatPoints/StatPoints.h"
//#include "../Command/CustomCommand.h"
//#include "../Reincarnation/Reincarnation.h"
//#include "../String/myString.h"
//#include "../UnitMod/CharMod/CharMod.h"
//
//std::vector<FunctionTemplate> FunctionVec;
//
//std::vector<GossipTemplate> GossipVec;
//
//std::vector<GossipTemplate> AboveMenuVec;
//
//bool GreaterSort(GossipTemplate a, GossipTemplate b) { return (a.menuId <b.menuId); }
//
//void FunctionCollection::Load()
//{
//	FunctionVec.clear();
//	GossipVec.clear();
//	QueryResult result = WorldDatabase.PQuery("SELECT funcIndex,reqId,telePosId_A,telePosId_H,desReq FROM _function_index");
//	QueryResult result1 = WorldDatabase.PQuery("SELECT entry,type,menuId,prevMenuId,funcIndex, smallIconType, bigIconName,menuText,teamId FROM _function_menu");
//
//	if (!result || !result1) return;
//	do
//	{
//		Field* fields = result->Fetch();
//		FunctionTemplate FunctionTemp;
//		FunctionTemp.index = fields[0].GetUInt32();
//		FunctionTemp.reqId = fields[1].GetUInt32();
//
//		Tokenizer telePosId_A_Data(fields[2].GetString(), ' ');
//
//		for (Tokenizer::const_iterator itr = telePosId_A_Data.begin(); itr != telePosId_A_Data.end(); ++itr)
//			FunctionTemp.telePosId_A.push_back(atoi(*itr));
//
//		Tokenizer telePosId_H_Data(fields[3].GetString(), ' ');
//
//		for (Tokenizer::const_iterator itr = telePosId_A_Data.begin(); itr != telePosId_A_Data.end(); ++itr)
//			FunctionTemp.telePosId_H.push_back(atoi(*itr));
//
//		FunctionTemp.desReq = fields[4].GetBool();
//		FunctionVec.push_back(FunctionTemp);
//	} while (result->NextRow());
//
//	do
//	{
//		Field* fields = result1->Fetch();
//		GossipTemplate GossipTemp;
//		GossipTemp.entry = fields[0].GetUInt32();
//		GossipTemp.type = fields[1].GetUInt32();
//		GossipTemp.menuId = fields[2].GetUInt32();
//		GossipTemp.prevMenuId = fields[3].GetUInt32();
//		GossipTemp.funcIndex = fields[4].GetInt32();
//		GossipTemp.smallIconType = fields[5].GetUInt8();
//		GossipTemp.bigIconName = fields[6].GetString();
//		GossipTemp.menuText = fields[7].GetString();
//		GossipTemp.teamId = TeamId(fields[8].GetUInt32());
//		GossipVec.push_back(GossipTemp);
//	} while (result1->NextRow());
//
//
//	
//	sort(GossipVec.begin(), GossipVec.end(), GreaterSort);//升序排列
//
//}
//uint32 FunctionCollection::GetOjectType(Object* obj)
//{
//	switch (obj->GetTypeId())
//	{
//	case TYPEID_GAMEOBJECT:
//		return 2;
//	case TYPEID_UNIT:
//		return 1;
//	case TYPEID_ITEM:
//		return 0;
//	}
//
//	return 100;
//}
//uint32 FunctionCollection::DoAction(Player* player, FunctionTypes type, Object* obj, bool destroy)
//{
//	if (!obj)
//		return 0;
//
//	uint32 reqId = GetReqId(uint32(type));
//	WorldLocation telePos = GetTelePos(player,uint32(type));
//
//	if (reqId != 0 && !sReq->Check(player, reqId))
//		return 0;
//	
//	sReq->Des(player, reqId);
//
//	switch (type)
//	{
//	case FT_MOD_CHAR_NAME:
//		ChangeName(player);
//		break;
//	case FT_MOD_CHAR_RACE:
//		ChangeRace(player);
//		break;
//	case FT_MOD_CHAR_FACTION:
//		ChangeFaction(player);
//		break;
//	case FT_MOD_CHAR_CUSTOMIZE:
//		Customize(player);
//		break;
//	case FT_RESET_TALENTS:
//		RestTalents(player);
//		break;
//	case FT_UPGRADE_WEAPON_SKILLS:
//		UpgradeWeaponSkills(player);
//		break;
//	case FT_QUERY_TOKEN:
//		QueryToken(player);
//		break;
//	case FT_RANDOM_QUEST:
//		sQuestMod->AddRandomQuest(player);
//		break;
//	case FT_COMBATE_STOP:
//		CombateStop(player);
//		break;
//	case FT_ABTAIN_TIME_REWARD:
//		AbtainTimeReward(player);
//		break;
//	case FT_BUY_LOTTERY:
//		sLottery->AddLotteryGossip(player, obj);
//		break;
//	case FT_SHOW_ACTIVE_EVENTS:
//		ShowActiveEvents(player, obj);
//		break;
//	case FT_UPGRADE_HR:
//		sHR->AddNextHRMenu(player, obj);
//		break;
//	case FT_RECRUIT:
//		sRecruit->AddMainMenu(player, obj);
//		break;
//	case FT_BACK_MENU:
//		break;
//	case FT_QUEST_TELE:
//		sQuestMod->AddTeleGossip(player, obj);
//		break;
//	case FT_UPGRADE_VIP:
//		sVIP->AddGossip(player, obj);
//		break;
//	case FT_RESET_INSTANCE_ALL:
//		sCustomCommand->DoCommand(player, ".instance unbind all");
//		break;
//	case FT_HOME_BIND:
//		if (AreaTableEntry const* areaEntry = GetAreaEntryByAreaID(player->GetAreaId()))
//		{
//			if (player->GetMap()->Instanceable())
//			{
//				ChatHandler(player->GetSession()).PSendSysMessage("你处在副本、竞技场或战场中，炉石点不能绑定到当前位置：%s", areaEntry->area_name[4]);
//				return 0;
//			}
//
//			player->SetHomebind(player->GetWorldLocation(), player->GetAreaId());
//			ChatHandler(player->GetSession()).PSendSysMessage("炉石点已经绑定到当前位置：%s", areaEntry->area_name[4]);
//		}	
//		break;
//	case FT_HOME_TELE:
//		player->TeleportTo(player->m_homebindMapId, player->m_homebindX, player->m_homebindY, player->m_homebindZ, player->GetOrientation());
//		break;
//	case FT_RESET_INSTANCE:
//		sResetIns->AddGossip(player, obj);
//		break;
//	case FT_REPAIR:
//		player->DurabilityRepairAll(false, 0, false);
//		player->GetSession()->SendAreaTriggerMessage("修理完成");
//		break;
//	case FT_BANK:
//		player->GetSession()->SendShowBank(player->GetGUID());
//		break;
//	case FT_MAIL:
//		if (!player->HasSpellCooldown(54710))
//		{
//			player->CastSpell(player, 54710, true);
//			player->AddSpellCooldown(54710, 0, 90 * IN_MILLISECONDS);
//		}
//		else
//			player->GetSession()->SendNotification("邮箱功能冷却剩余时间%d秒", player->GetSpellCooldownDelay(54710) / IN_MILLISECONDS);
//		break;
//	case FT_CUSTOM_SKILL:
//		sCustomSkill->AddGossip(player, obj);
//		break;
//	case FT_RANK:
//		sRank->AddGossip(player, obj);
//		break;
//	case FT_FACTION:
//		sFaction->AddGossip(player, obj);
//		break;
//	case FT_RESET_DAILY_QUEST:
//		player->ResetDailyQuestStatus();
//		break;
//	case FT_LUCKDRAW:
//		sLuckDraw->OpenPanel(player);
//		break;
//	case FT_SIGNIN:
//		sSignIn->OpenPanel(player);
//		break;
//	case FT_RECOVERY:
//		sRecovery->OpenPanel(player);
//		break;
//	case FT_STATPOINTS:
//		sStatPoints->OpenPanel(player);
//		break;
//	case FT_REINCARNATION:
//		sReincarnation->AddGossip(player, obj);
//		break;
//	case FT_ALT_CLASS:
//		sCharMod->AddGossip(player, obj);
//		break;
//	default:
//		if (player->IsInCombat() && !player->IsGameMaster())
//			player->GetSession()->SendNotification("战斗状态无法传送！");
//		else
//		{
//			if (telePos.GetMapId() == 0 && telePos.GetPositionX() == 0 && telePos.GetPositionY() == 0 && telePos.GetPositionZ() == 0 && telePos.GetOrientation() == 0)
//				;
//			else
//				player->TeleportTo(telePos);
//		}	
//		break;
//	}
//
//
//	if (destroy)
//		player->DestroyItemCount(obj->GetEntry(), 1, true);
//
//	return 0;
//}
//
//void FunctionCollection::AddAboveMenu(Player* player, Object* obj, uint32 menuId)
//{
//	player->PlayerTalkClass->ClearMenus();
//
//	if (!obj)
//		return;
//
//	AboveMenuVec.clear();
//
//	uint32 prevMenuId = 0;
//	uint32 len = GossipVec.size();
//	for (size_t i = 0; i < len; i++)
//	{
//		if (menuId == GossipVec[i].menuId)
//		{
//			prevMenuId = GossipVec[i].prevMenuId;
//			break;
//		}
//	}
//	for (size_t i = 0; i < len; i++)
//	{
//		if (prevMenuId == GossipVec[i].menuId)
//		{
//			for (size_t j = 0; j < len; j++)
//			{
//				if (GossipVec[j].prevMenuId == GossipVec[i].prevMenuId && obj->GetEntry() == GossipVec[j].entry && GossipVec[j].type == sFC->GetOjectType(obj))
//				{
//					GossipTemplate GossipTemp;
//					GossipTemp.entry = GossipVec[j].entry;
//					GossipTemp.type = GossipVec[j].type;
//					GossipTemp.funcIndex = GossipVec[j].funcIndex;
//					GossipTemp.menuId = GossipVec[j].menuId;
//					GossipTemp.smallIconType = GossipVec[j].smallIconType;
//					GossipTemp.menuText = GossipVec[j].menuText;
//					GossipTemp.prevMenuId = GossipVec[j].prevMenuId;
//					GossipTemp.bigIconName = GossipVec[j].bigIconName;
//					AboveMenuVec.push_back(GossipTemp);
//				}
//			}
//
//			break;
//		}
//	}
//
//	player->PlayerTalkClass->ClearMenus();
//
//	if (!AboveMenuVec.empty())
//	{
//		for (size_t i = 0; i < AboveMenuVec.size(); i++)
//		{
//			uint32 funcIndex = AboveMenuVec[i].funcIndex;
//			uint32 reqId = sFC->GetReqId(funcIndex);
//			uint8 smallIconType = AboveMenuVec[i].smallIconType;
//			std::string text = AboveMenuVec[i].bigIconName.empty() ? AboveMenuVec[i].menuText : "|TInterface/ICONS/" + AboveMenuVec[i].bigIconName + ":30:30:0:0|t" + AboveMenuVec[i].menuText;
//			std::string notice = GossipVec[i].menuText;
//			TeamId teamId = GossipVec[i].teamId;
//
//			if (teamId == TEAM_NEUTRAL || player->GetTeamId() == teamId)
//			{
//				if (reqId != 0)
//					player->ADD_GOSSIP_ITEM_EXTENDED(smallIconType, text, AboveMenuVec[i].menuId, funcIndex, sReq->Notice(player, reqId, notice, ""), sReq->Golds(reqId), false);
//				else
//					player->ADD_GOSSIP_ITEM(smallIconType, text, AboveMenuVec[i].menuId, funcIndex);
//			}
//		}
//
//		if (obj->ToCreature())
//			player->SEND_GOSSIP_MENU(obj->GetEntry(), obj->GetGUID());
//		else
//			player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, obj->GetGUID());
//	}
//}
//
//uint32 FunctionCollection::GetReqId(uint32 funcIndex)
//{
//	uint32 len = FunctionVec.size();
//
//	for (size_t i = 0; i < len; i++)
//	{
//		if (funcIndex == FunctionVec[i].index)
//			return FunctionVec[i].reqId;
//	}
//
//	return 0;
//}
//
//
//bool FunctionCollection::GetDesReq(uint32 funcIndex)
//{
//	uint32 len = FunctionVec.size();
//
//	for (size_t i = 0; i <len; i++)
//	{
//		if (funcIndex == FunctionVec[i].index)
//			return FunctionVec[i].desReq;
//	}
//
//	return false;
//}
//
//uint32 FunctionCollection::GetPrevId(uint32 entry,uint32 funcIndex)
//{
//	uint32 len = GossipVec.size();
//
//	for (size_t i = 0; i <len; i++)
//		if (entry == GossipVec[i].entry && funcIndex == GossipVec[i].funcIndex)
//			return GossipVec[i].prevMenuId;
//
//	return 0;
//}
//
//uint32 FunctionCollection::GetExtraTPs(Player* player)
//{
//	QueryResult resultExtraTPs = CharacterDatabase.PQuery("SELECT extraTalentPoints FROM characters WHERE guid = '%u'", player->GetGUIDLow());
//	if (resultExtraTPs)
//		return resultExtraTPs->Fetch()[0].GetUInt32();
//	return 0;
//}
//
//void FunctionCollection::AddTalentPoints(Player* player,Object* obj)
//{
//	player->GetSession()->SendAreaTriggerMessage("获得额外一个天赋点");
//	player->SetFreeTalentPoints(player->GetFreeTalentPoints() + 1);
//	player->SendTalentsInfoData(false);
//	CharacterDatabase.DirectPExecute("UPDATE characters SET extraTalentPoints = extraTalentPoints + 1 WHERE guid = '%u'", player->GetGUIDLow());
//
//}
//
//void FunctionCollection::ChangeFaction(Player* player)
//{
//	PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ADD_AT_LOGIN_FLAG);
//	stmt->setUInt16(0, uint16(AT_LOGIN_CHANGE_FACTION));
//	player->SetAtLoginFlag(AT_LOGIN_CHANGE_FACTION);
//	stmt->setUInt32(1, player->GetGUIDLow());
//	CharacterDatabase.Execute(stmt);
//	player->GetSession()->SendAreaTriggerMessage("重新上线以完成更改！");
//
//	if (Guild* guild = player->GetGuild())
//		guild->HandleLeaveMember(player->GetSession());
//
//}
//
//void FunctionCollection::Customize(Player* player)
//{
//	PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ADD_AT_LOGIN_FLAG);
//	stmt->setUInt16(0, uint16(AT_LOGIN_CUSTOMIZE));
//	player->SetAtLoginFlag(AT_LOGIN_CUSTOMIZE);
//	stmt->setUInt32(1, player->GetGUIDLow());
//	CharacterDatabase.Execute(stmt);
//	player->GetSession()->SendAreaTriggerMessage("重新上线以完成更改！");
//}
//
//void FunctionCollection::ChangeName(Player* player)
//{
//	player->SetAtLoginFlag(AT_LOGIN_RENAME);
//	player->GetSession()->SendAreaTriggerMessage("重新上线以完成更改！");
//}
//
//void FunctionCollection::ChangeRace(Player* player)
//{
//	PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ADD_AT_LOGIN_FLAG);
//	stmt->setUInt16(0, uint16(AT_LOGIN_CHANGE_RACE));
//	player->SetAtLoginFlag(AT_LOGIN_CHANGE_RACE);
//	stmt->setUInt32(1, player->GetGUIDLow());
//	CharacterDatabase.Execute(stmt);
//	player->GetSession()->SendAreaTriggerMessage("重新上线以完成更改！");
//
//	if (Guild* guild = player->GetGuild())
//		guild->HandleLeaveMember(player->GetSession());
//}
//
//void FunctionCollection::RestTalents(Player* player)
//{
//	player->resetTalents(true);
//	player->SendTalentsInfoData(false);
//	player->GetSession()->SendAreaTriggerMessage("已重置角色天赋");
//}
//
//void FunctionCollection::UpgradeWeaponSkills(Player* player)
//{
//	player->UpdateSkillsToMaxSkillsForLevel();
//	player->GetSession()->SendAreaTriggerMessage("武器技能已提升至最大值");
//}
//
//void FunctionCollection::QueryToken(Player* player)
//{
//	std::ostringstream oss;
//	oss << "你当前的" << sString->GetText(CORE_STR_TYPES(STR_TOKEN) << "数量：" << sCF->GetTokenAmount(player);
//
//	player->GetSession()->SendAreaTriggerMessage(oss.str().c_str());
//	ChatHandler(player->GetSession()).PSendSysMessage(oss.str().c_str());
//}
//
//void FunctionCollection::CombateStop(Player* player)
//{
//	if (!player->canStopCombat && sSwitch->GetValue(ST_STOP_COMBAT_CD) != 0)
//	{
//		uint32 cd = player->stopCombatCD / IN_MILLISECONDS;
//		std::ostringstream oss;
//		oss << "脱战功能将在" << cd << "秒后冷却！";
//		player->GetSession()->SendNotification(oss.str().c_str());
//		return;
//	}
//		
//	player->ClearInCombat();
//	player->GetSession()->SendAreaTriggerMessage("已清除战斗状态");
//	player->canStopCombat = false;
//	player->stopCombatCD = sSwitch->GetValue(ST_STOP_COMBAT_CD) * IN_MILLISECONDS;
//}
//
//void FunctionCollection::AbtainTimeReward(Player* player)
//{
//	if (PlayerTimeRewType != 1)
//	{
//		if (PlayerTimeRewType != 2)
//			player->GetSession()->SendNotification("泡点奖励已关闭！");
//		else
//			player->GetSession()->SendNotification("领取泡点已关闭，请保持在线，等待系统发放奖励");
//
//		return;
//	}
//		
//
//	int32 unRewardNum = player->GetTotalPlayedTime() / 3600 - player->onlineRewardedCount;
//	if (unRewardNum <= 0)
//	{
//		uint32 minu = player->GetTotalPlayedTime() > 3600 ? uint32(60 - (float(player->GetTotalPlayedTime()) / 3600 - player->GetTotalPlayedTime() / 3600) * 60) : (60 - player->GetTotalPlayedTime() / 60);
//
//		std::ostringstream oss;
//		oss << minu << "分钟后可领取泡点";
//		player->GetSession()->SendNotification(oss.str().c_str());
//
//		ChatHandler(player->GetSession()).PSendSysMessage(oss.str().c_str());
//		return;
//	}
//	sCF->SetOnlineRewardedCount(player, unRewardNum + player->onlineRewardedCount);
//	for (int32 i = 0; i < unRewardNum; i++)
//	{
//		sRew->Rew(player, player->timeRewId);
//	}
//}
//
////获取商业技能数量
//
//void FunctionCollection::BuyLottery(Player* player, uint32 lotteryAmount, uint8 lotteryType)
//{
//	sLottery->PlayerBuyLottery(player, LotteryType(lotteryType), lotteryAmount);
//}
//
//void FunctionCollection::ShowActiveEvents(Player* player, Object* obj)
//{
//	sEvent->AddEventList(player, obj);
//}
//
//void FunctionCollection::Teleport(Player* player, uint32 map, float x, float y, float z, float o)
//{
//	player->TeleportTo(map, x, y, z, o);
//}
//
//WorldLocation FunctionCollection::GetTelePos(Player*player, uint32 funcIndex)
//{
//	uint32 telePosId = 0;
//
//	for (size_t i = 0; i < FunctionVec.size(); i++)
//		if (funcIndex == FunctionVec[i].index)
//		{
//			player->GetTeamId() == TEAM_ALLIANCE ? telePosId = FunctionVec[i].telePosId_A[urand(0, FunctionVec[i].telePosId_A.size() - 1)] : telePosId = FunctionVec[i].telePosId_H[urand(0, FunctionVec[i].telePosId_H.size() - 1)];
//			break;
//		}
//
//
//	std::unordered_map<uint32, PosTemplate>::iterator it = PosMap.find(telePosId);
//
//	if (it != PosMap.end())
//		return WorldLocation(it->second.map, it->second.x, it->second.y, it->second.z, it->second.o);
//
//	return WorldLocation(0, 0, 0, 0, 0);
//}
//
//void FunctionCollection::RestAllInstance(Player* player)
//{
//	/*if (Group* group = player->GetGroup())
//	{
//		if (group->IsLeader(player->GetGUID()))
//			group->ResetInstances(INSTANCE_RESET_ALL, false, player);
//	}
//	else
//		Player::ResetInstances(player->GetGUIDLow(), INSTANCE_RESET_ALL, false);
//
//	if (Group* group = player->GetGroup())
//	{
//		if (group->IsLeader(player->GetGUID()))
//			group->ResetInstances(INSTANCE_RESET_CHANGE_DIFFICULTY, true, player);
//	}
//	else
//		Player::ResetInstances(player->GetGUIDLow(), INSTANCE_RESET_CHANGE_DIFFICULTY, true);
//
//*/
//	for (uint8 i = 0; i < MAX_DIFFICULTY; ++i)
//	{
//
//		std::vector<InstanceSave*> toUnbind;
//		BoundInstancesMap const& m_boundInstances = sInstanceSaveMgr->PlayerGetBoundInstances(player->GetGUIDLow(), Difficulty(i));
//		for (BoundInstancesMap::const_iterator itr = m_boundInstances.begin(); itr != m_boundInstances.end(); ++itr)
//		{
//			InstanceSave* instanceSave = itr->second.save;
//			const MapEntry* entry = sMapStore.LookupEntry(itr->first);
//			if (!entry)
//				continue;
//
//			Map* map = sMapMgr->FindMap(instanceSave->GetMapId(), instanceSave->GetInstanceId());
//			if (!map || map->ToInstanceMap()->Reset(INSTANCE_RESET_GLOBAL))
//			{
//				player->SendResetInstanceSuccess(instanceSave->GetMapId());
//				toUnbind.push_back(instanceSave);
//			}
//			else
//				player->SendResetInstanceFailed(0, instanceSave->GetMapId());
//		}
//		for (std::vector<InstanceSave*>::const_iterator itr = toUnbind.begin(); itr != toUnbind.end(); ++itr)
//			sInstanceSaveMgr->UnbindAllFor(*itr);
//	}
//}
//
//void FunctionCollection::AddGossipOrDoAction(Player* player, Object* obj, uint32 sender, uint32 action)
//{
//	player->PlayerTalkClass->ClearMenus();
//
//	if (!obj)
//		return;
//
//#pragma region 任务传送
//	if (sender == SENDER_QUEST_TELE)
//	{
//		sQuestMod->Tele(player, action);
//		return;
//	}
//#pragma endregion
//
//#pragma region 重置副本
//	if (sender == SENDER_RESET_INS)
//	{
//		sResetIns->Action(player, action);
//		return;
//	}
//#pragma endregion
//
//#pragma region 招募处理
//	if (sender == SENDER_RECRUIT)
//	{
//		sFC->DoAction(player, FT_RECRUIT, obj);
//		return;
//	}
//#pragma endregion
//
//#pragma region 升级军衔处理
//	if (sender == SENDER_HR_MENU)
//	{
//		sFC->DoAction(player, FT_UPGRADE_HR, obj);
//		return;
//	}
//	
//	if (sender == SENDER_HR_UP)
//	{
//		sHR->SetHRTitle(player, action);
//		player->CLOSE_GOSSIP_MENU();
//		return;
//	}
//#pragma endregion
//
//#pragma region 升级VIP处理
//	if (sender == SENDER_VIP_CURR)
//	{
//		sFC->DoAction(player, FT_UPGRADE_VIP, obj);
//		return;
//	}
//
//	if (sender == SENDER_VIP_UP)
//	{
//		sVIP->Up(player);
//		player->CLOSE_GOSSIP_MENU();
//		return;
//	}
//#pragma endregion
//
//
//#pragma region 转生
//	if (sender == SENDER_REINCARNATION)
//	{
//		sReincarnation->DoAction(player, obj, action);
//		return;
//	}
//#pragma endregion
//
//#pragma region 事件
//	if (sender == SENDER_CUSTOM_EVENT)
//	{
//		sEvent->AcceptInvitation(player, action + 1000000);
//		player->CLOSE_GOSSIP_MENU();
//		return;
//	}
//#pragma endregion
//
//#pragma region CustomSkill
//	if (sender == SENDER_CUSTOM_SKILL)
//	{
//		sCustomSkill->Action(player, action, obj);
//		return;
//	}
//#pragma endregion
//
//#pragma region Rank
//	if (sender == SENDER_RANK)
//	{
//		sRank->Action(player, action, obj);
//		return;
//	}
//#pragma endregion
//
//#pragma region Faction
//	if (sender == SENDER_FACTION)
//	{
//		sFaction->Action(player, action, obj);
//		return;
//	}
//#pragma endregion
//
//#pragma region Morph
//	if (sender == SENDER_MORPH)
//	{
//		//sMorph->Action(player, action, obj);
//		return;
//	}
//#pragma endregion
//
//#pragma region 转职
//	if (sender == SENDER_ALT_CLASS)
//	{
//		sCharMod->ModClass(player, action);
//		return;
//	}
//#pragma endregion
//
//#pragma region 返回上级菜单、主菜单
//	if (action == uint32(FT_BACK_MENU))
//	{
//		AddAboveMenu(player, obj, sender);
//		return;
//	}
//
//	if (action == uint32(FT_MAIN_MENU))
//	{
//		AddGossipOrDoAction(player, obj, 0, 0);
//		return;
//	}
//#pragma endregion
//
//#pragma region 添加gossip或直接执行功能
//
//	for (size_t i = 0; i < GossipVec.size(); i++)
//	{
//		if (obj->GetEntry() == GossipVec[i].entry && GossipVec[i].type == sFC->GetOjectType(obj))
//		{
//			int32 prevMenuId = GossipVec[i].prevMenuId;
//			uint32 funcIndex = GossipVec[i].funcIndex;
//
//			switch (prevMenuId)
//			{
//				//doaction:1,do not destroy item 2,destroy item
//			case -1:
//				sFC->DoAction(player, FunctionTypes(funcIndex), obj);
//				return;
//			case -2:
//				if (IS_ITEM_GUID(obj->GetGUID()))
//					sFC->DoAction(player, FunctionTypes(funcIndex), obj,true);
//				else
//					sFC->DoAction(player, FunctionTypes(funcIndex), obj);
//				return;
//			default:
//				break;
//			}
//
//			//add gossip
//			uint8 smallIconType = GossipVec[i].smallIconType;
//			std::string text = GossipVec[i].bigIconName.empty() ? GossipVec[i].menuText : "|TInterface/ICONS/" + GossipVec[i].bigIconName + ":30:30:0:0|t" + GossipVec[i].menuText;
// 			std::string notice = GossipVec[i].menuText;
//
//			if (sender == prevMenuId)
//			{
//				uint32 reqId = sFC->GetReqId(funcIndex);
//				TeamId teamId = GossipVec[i].teamId;
//
//				if (teamId == TEAM_NEUTRAL || player->GetTeamId() == teamId)
//				{
//					if (reqId != 0)
//						player->ADD_GOSSIP_ITEM_EXTENDED(smallIconType, text, GossipVec[i].menuId, funcIndex, sReq->Notice(player, reqId, notice, ""), sReq->Golds(reqId), false);
//					else
//						player->ADD_GOSSIP_ITEM(smallIconType, text, GossipVec[i].menuId, funcIndex);
//				}		
//			}
//		}
//	}
//
//	if (obj->ToCreature())
//		player->SEND_GOSSIP_MENU(obj->GetEntry(), obj->GetGUID());
//	else
//		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, obj->GetGUID());
//
//#pragma endregion
//
//#pragma region 菜单中常用功能处理
//
//	if (action > uint32(FT_START))
//	{
//		sFC->DoAction(player, FunctionTypes(action),obj);
//
//		if (action != uint32(FT_ALT_CLASS) && action != uint32(FT_FACTION) && action != uint32(FT_RANK) && action != uint32(FT_CUSTOM_SKILL) && action != uint32(FT_RESET_INSTANCE) && action != uint32(FT_BUY_LOTTERY) && action != uint32(FT_UPGRADE_HR) && action != uint32(FT_RECRUIT) && action != uint32(FT_QUEST_TELE) && action != FT_UPGRADE_VIP && action != FT_REINCARNATION && action != FT_SHOW_ACTIVE_EVENTS)
//			player->CLOSE_GOSSIP_MENU();
//	}
//
//#pragma endregion
//}
//
///*
//class Func_ItemScript : public ItemScript
//{
//public:
//	Func_ItemScript() : ItemScript("Func_ItemScript") { }
//
//	bool OnUse(Player* player, Item* item, SpellCastTargets const& targets) override
//	{
//		sFC->AddGossipOrDoAction(player, item, 0, 0);
//		return true;
//	}
//
//	void OnGossipSelect(Player* player, Item* item, uint32 sender, uint32 action) override
//	{
//		if (sRecruit->AddSubMenuOrDoAction(player, item, sender, action))
//			return;
//		sFC->AddGossipOrDoAction(player, item, sender, action);
//	}
//
//	void OnGossipSelectCode(Player* player, Item* item, uint32 sender, uint32 action, const char* code) override
//	{
//		player->PlayerTalkClass->ClearMenus();
//
//		if (!*code)
//			return;
//
//		if (sRecruit->RecruitFriend(player, sender, code))
//			return;
//
//		if (sLottery->PlayerBuyLottery(player, LotteryType(sender), (uint32)atoi(code)))
//			return;
//	}
//
//};
//
//class Func_CreatureScript : public CreatureScript
//{
//public:
//	Func_CreatureScript() : CreatureScript("Func_CreatureScript") { }
//
//
//	bool OnGossipHello(Player* player, Creature* creature) override
//	{
//		sFC->AddGossipOrDoAction(player, creature, 0, 0);
//		return true;
//	}
//
//	bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
//	{
//		if (sRecruit->AddSubMenuOrDoAction(player, creature, sender, action))
//			return true;
//		sFC->AddGossipOrDoAction(player, creature, sender, action);
//		return true;
//	}
//
//	bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code) override
//	{ 
//		player->PlayerTalkClass->ClearMenus();
//
//		if (!*code)
//			return false;
//
//		if (sRecruit->RecruitFriend(player, sender, code))
//			return true;
//
//		if (sLottery->PlayerBuyLottery(player, LotteryType(sender), (uint32)atoi(code)))
//			return true;
//		return false;
//	}
//};
//
//class Func_GameObjectScript :public GameObjectScript
//{
//public:
//	Func_GameObjectScript() : GameObjectScript("Func_GameObjectScript") { }
//
//	bool OnGossipHello(Player* player, GameObject* go) override
//	{
//		sFC->AddGossipOrDoAction(player, go, 0, 0);
//		return true;
//	}
//
//	bool OnGossipSelect(Player* player, GameObject* go, uint32 sender, uint32 action) override
//	{
//		if (sRecruit->AddSubMenuOrDoAction(player, go, sender, action))
//			return true;
//		sFC->AddGossipOrDoAction(player, go, sender, action);
//		return true;
//	}
//
//	bool OnGossipSelectCode(Player* player, GameObject* go, uint32 sender, uint32 action, const char* code) override
//	{
//		player->PlayerTalkClass->ClearMenus();
//
//		if (!*code)
//			return false;
//
//		if (sRecruit->RecruitFriend(player, sender, code))
//			return true;
//
//		if (sLottery->PlayerBuyLottery(player, LotteryType(sender), (uint32)atoi(code)))
//			return true;
//		return false;
//	}
//};
//
//class Func_PlayerScript : PlayerScript
//{
//public:
//	Func_PlayerScript() : PlayerScript("Func_PlayerScript") {}
//	void OnLogin(Player* player, bool) override
//	{
//		sFC->SetCommercePoints(player);
//	}
//};
//*/
//void AddSC_FunctionCollection()
//{
//	//new Func_PlayerScript();
//	//new Func_ItemScript();
//	//new Func_CreatureScript();
//	//new Func_GameObjectScript();
//}
