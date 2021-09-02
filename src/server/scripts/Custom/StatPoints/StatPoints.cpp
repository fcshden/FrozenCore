#pragma execution_character_set("utf-8")
#include "StatPoints.h"
#include "../Custom/CommonFunc/CommonFunc.h"
#include "../GCAddon/GCAddon.h"

uint32 stat_muil[MAX_STAT_POINTS_TYPE];

void StatPoints::Load()
{
	for (size_t i = 0; i < MAX_STAT_POINTS_TYPE; i++)
		stat_muil[i] = 0;

	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 每点增加耐力,每点增加敏捷,每点增加力量,每点增加智力,每点增加精神,每点增加命中等级,每点增加爆击等级,每点增加攻击强度,每点增加护甲穿透,每点增加法术强度,每点增加法术穿透,每点增加急速等级 FROM __斗气" :
		"SELECT muil_stamina,muil_agility,muil_strength,muil_intellect,muil_spirit,muil_hit,muil_crit,muil_ap,muil_ape,muil_sp,muil_spe,muil_haste FROM _stat_points");
	if (!result)
		return;

	Field* fields = result->Fetch();

	for (size_t i = 1; i < MAX_STAT_POINTS_TYPE; i++)
		stat_muil[i] = fields[i - 1].GetUInt32();
}

void StatPoints::LoadPlayerData(Player* player)
{
	QueryResult result = CharacterDatabase.PQuery("SELECT stat_points FROM characters where guid = %d", player->GetGUIDLow());
	if (!result)
		return;

	std::string str = result->Fetch()[0].GetString();

	if (str.empty())
		return;

	std::vector<std::string> v = sCF->SplitStr(str, " ");

	if (v.empty())
		return;

	for (size_t i = 0; i < v.size(); i++)
		player->stat_points[i] = (uint32)atoi(v[i].c_str());
}

void StatPoints::UpdateDB(Player* player)
{
	std::ostringstream oss;

	for (size_t i = 0; i < MAX_STAT_POINTS_TYPE; i++)
		oss << player->stat_points[i] << " ";

	CharacterDatabase.PExecute("UPDATE characters SET stat_points = '%s'WHERE guid = %d", oss.str().c_str(), player->GetGUIDLow());
}

void StatPoints::UpdateDB(uint32 guid, uint32 statPoint)
{
	QueryResult result = CharacterDatabase.PQuery("SELECT stat_points FROM characters where guid = %d", guid);

	if (!result)
		return;

	Field* fields = result->Fetch();

	std::string str = fields[0].GetString();

	if (str.empty())
		return;

	uint32 stat_points[MAX_STAT_POINTS_TYPE];

	for (size_t i = 0; i < MAX_STAT_POINTS_TYPE; i++)
		stat_points[i] = 0;

	std::vector<std::string> v = sCF->SplitStr(str, " ");

	if (v.empty())
		return;

	for (size_t i = 0; i < v.size(); i++)
		stat_points[i] = (uint32)atoi(v[i].c_str());

	stat_points[SPT_TOTLAL] += statPoint;


	std::ostringstream oss;

	for (size_t i = 0; i < MAX_STAT_POINTS_TYPE; i++)
		oss << stat_points[i] << " ";

	CharacterDatabase.PExecute("UPDATE characters SET stat_points = '%s'WHERE guid = %d", oss.str().c_str(), guid);
}

void StatPoints::ResetPoints(Player* player)
{
	Apply(player, false);

	for (size_t i = SPT_TOTLAL; i < MAX_STAT_POINTS_TYPE; i++)
	{
		player->stat_points[SPT_TOTLAL] += player->stat_points[i];
		player->stat_points[i] = 0;
	}

	Apply(player, true);

	UpdateDB(player);
}

void StatPoints::Apply(Player* player, bool apply)
{
	//敏捷
	float agility = player->stat_points[SPT_AGILITY] * stat_muil[SPT_AGILITY];
	player->HandleStatModifier(UNIT_MOD_STAT_AGILITY, TOTAL_VALUE, agility, apply);
	player->ApplyStatBuffMod(STAT_AGILITY, agility, apply);
	//力量
	float strength = player->stat_points[SPT_STRENGTH] * stat_muil[SPT_STRENGTH];
	player->HandleStatModifier(UNIT_MOD_STAT_STRENGTH, TOTAL_VALUE, strength, apply);
	player->ApplyStatBuffMod(STAT_STRENGTH, strength, apply);
	//智力
	float intellect = player->stat_points[SPT_INTELLECT] * stat_muil[SPT_INTELLECT];
	player->HandleStatModifier(UNIT_MOD_STAT_INTELLECT, TOTAL_VALUE, intellect, apply);
	player->ApplyStatBuffMod(STAT_INTELLECT, intellect, apply);
	//精神
	float spirit = player->stat_points[SPT_SPIRIT] * stat_muil[SPT_SPIRIT];
	player->HandleStatModifier(UNIT_MOD_STAT_SPIRIT, TOTAL_VALUE, spirit, apply);
	player->ApplyStatBuffMod(STAT_SPIRIT, spirit, apply);
	//耐力
	float stamina = player->stat_points[SPT_STMAMINA] * stat_muil[SPT_STMAMINA];
	player->HandleStatModifier(UNIT_MOD_STAT_STAMINA, TOTAL_VALUE, stamina, apply);
	player->ApplyStatBuffMod(STAT_STAMINA, stamina, apply);
	//命中
	int32 hit = player->stat_points[SPT_HIT] * stat_muil[SPT_HIT];
	player->ApplyRatingMod(CR_HIT_MELEE, hit, apply);
	player->ApplyRatingMod(CR_HIT_RANGED, hit, apply);
	player->ApplyRatingMod(CR_HIT_SPELL, hit, apply);
	//暴击
	int32 crit = player->stat_points[SPT_CRIT] * stat_muil[SPT_CRIT];
	player->ApplyRatingMod(CR_CRIT_MELEE, crit, apply);
	player->ApplyRatingMod(CR_CRIT_RANGED, crit, apply);
	player->ApplyRatingMod(CR_CRIT_SPELL, crit, apply);
	//急速
	int32 haste = player->stat_points[SPT_HASTE] * stat_muil[SPT_HASTE];
	player->ApplyRatingMod(CR_HASTE_MELEE, haste, apply);
	player->ApplyRatingMod(CR_HASTE_RANGED, haste, apply);
	player->ApplyRatingMod(CR_HASTE_SPELL, haste, apply);
	//攻强
	float ap = player->stat_points[SPT_AP] * stat_muil[SPT_AP];
	player->HandleStatModifier(UNIT_MOD_ATTACK_POWER, TOTAL_VALUE, ap, apply);
	player->HandleStatModifier(UNIT_MOD_ATTACK_POWER_RANGED, TOTAL_VALUE, ap, apply);
	//甲穿
	int32 ape = player->stat_points[SPT_APE] * stat_muil[SPT_APE];
	player->ApplyRatingMod(CR_ARMOR_PENETRATION, ape, apply);
	//法强
	int32 sp = player->stat_points[SPT_SP] * stat_muil[SPT_SP];
	player->ApplySpellPowerBonus(sp, apply);
	//法穿
	int32 spe = player->stat_points[SPT_SPE] * stat_muil[SPT_SPE];
	player->ApplySpellPenetrationBonus(spe, apply);
}

void StatPoints::AddGosip(Player* player, Object* obj)
{
	std::ostringstream oss_cur_total;
	std::ostringstream oss_cur_stamina;
	std::ostringstream oss_cur_agility;
	std::ostringstream oss_cur_strength;
	std::ostringstream oss_cur_intellect;
	std::ostringstream oss_cur_spirit;
	std::ostringstream oss_cur_hit;
	std::ostringstream oss_cur_crit;
	std::ostringstream oss_cur_haste;
	std::ostringstream oss_cur_ap;
	std::ostringstream oss_cur_ape;
	std::ostringstream oss_cur_sp;
	std::ostringstream oss_cur_spe;

	oss_cur_total << "- " << player->stat_points[SPT_TOTLAL] << " 点数";
	oss_cur_stamina << "+ " << player->stat_points[SPT_STMAMINA] << " 耐力";
	oss_cur_agility << "+ " << player->stat_points[SPT_AGILITY] << " 敏捷";
	oss_cur_strength << "+ " << player->stat_points[SPT_STRENGTH] << " 力量";
	oss_cur_intellect << "+ " << player->stat_points[SPT_INTELLECT] << " 智力";
	oss_cur_spirit << "+ " << player->stat_points[SPT_SPIRIT] << " 精神";
	oss_cur_hit << "+ " << player->stat_points[SPT_HIT] << " 命中等级";
	oss_cur_crit << "+ " << player->stat_points[SPT_CRIT] << " 暴击等级";
	oss_cur_haste << "+ " << player->stat_points[SPT_HASTE] << " 急速等级";
	oss_cur_ap << "+ " << player->stat_points[SPT_AP] << " 攻击强度";
	oss_cur_ape << "+ " << player->stat_points[SPT_APE] << " 护甲穿透";
	oss_cur_sp << "+ " << player->stat_points[SPT_SP] << " 法术强度";
	oss_cur_spe << "+ " << player->stat_points[SPT_SPE] << " 法术穿透";

	std::ostringstream oss_stamina;
	std::ostringstream oss_agility;
	std::ostringstream oss_strength;
	std::ostringstream oss_intellect;
	std::ostringstream oss_spirit;
	std::ostringstream oss_hit;
	std::ostringstream oss_crit;
	std::ostringstream oss_haste;
	std::ostringstream oss_ap;
	std::ostringstream oss_ape;
	std::ostringstream oss_sp;
	std::ostringstream oss_spe;

	oss_stamina << "确定 + " << stat_muil[SPT_STMAMINA] << " 耐力";
	oss_agility << "确定 + " << stat_muil[SPT_AGILITY] << " 敏捷";
	oss_strength << "确定 + " << stat_muil[SPT_STRENGTH] << " 力量";
	oss_intellect << "确定 + " << stat_muil[SPT_INTELLECT] << " 智力";
	oss_spirit << "确定 + " << stat_muil[SPT_SPIRIT] << " 精神";
	oss_hit << "确定 + " << stat_muil[SPT_HIT] << " 命中等级";
	oss_crit << "确定 + " << stat_muil[SPT_CRIT] << " 暴击等级";
	oss_haste << "确定 + " << stat_muil[SPT_HASTE] << " 急速等级";
	oss_ap << "确定 + " << stat_muil[SPT_AP] << " 攻击强度";
	oss_ape << "确定 + " << stat_muil[SPT_APE] << " 护甲穿透";
	oss_sp << "确定 + " << stat_muil[SPT_SP] << " 法术强度";
	oss_spe << "确定 + " << stat_muil[SPT_SPE] << " 法术穿透";

	player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, oss_cur_total.str(), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

	if (stat_muil[SPT_STMAMINA] > 0)
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, oss_cur_stamina.str(), GOSSIP_SENDER_MAIN + 1, GOSSIP_ACTION_INFO_DEF);// , oss_stamina.str(), 0, false);
	if (stat_muil[SPT_AGILITY] > 0)
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, oss_cur_agility.str(), GOSSIP_SENDER_MAIN + 2, GOSSIP_ACTION_INFO_DEF);// , oss_agility.str(), 0, false);
	if (stat_muil[SPT_STRENGTH] > 0)
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, oss_cur_strength.str(), GOSSIP_SENDER_MAIN + 3, GOSSIP_ACTION_INFO_DEF);// , oss_strength.str(), 0, false);
	if (stat_muil[SPT_INTELLECT] > 0)
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, oss_cur_intellect.str(), GOSSIP_SENDER_MAIN + 4, GOSSIP_ACTION_INFO_DEF);// , oss_intellect.str(), 0, false);
	if (stat_muil[SPT_SPIRIT] > 0)
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, oss_cur_spirit.str(), GOSSIP_SENDER_MAIN + 5, GOSSIP_ACTION_INFO_DEF);// , oss_spirit.str(), 0, false);
	if (stat_muil[SPT_HIT] > 0)
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, oss_cur_hit.str(), GOSSIP_SENDER_MAIN + 6, GOSSIP_ACTION_INFO_DEF);// , oss_hit.str(), 0, false);
	if (stat_muil[SPT_CRIT] > 0)
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, oss_cur_crit.str(), GOSSIP_SENDER_MAIN + 7, GOSSIP_ACTION_INFO_DEF);// , oss_crit.str(), 0, false);
	if (stat_muil[SPT_HASTE] > 0)
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, oss_cur_haste.str(), GOSSIP_SENDER_MAIN + 8, GOSSIP_ACTION_INFO_DEF);// , oss_haste.str(), 0, false);
	if (stat_muil[SPT_AP] > 0)
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, oss_cur_ap.str(), GOSSIP_SENDER_MAIN + 9, GOSSIP_ACTION_INFO_DEF);// , oss_ap.str(), 0, false);
	if (stat_muil[SPT_APE] > 0)
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, oss_cur_ape.str(), GOSSIP_SENDER_MAIN + 10, GOSSIP_ACTION_INFO_DEF);// , oss_ape.str(), 0, false);
	if (stat_muil[SPT_SP] > 0)
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, oss_cur_sp.str(), GOSSIP_SENDER_MAIN + 11, GOSSIP_ACTION_INFO_DEF);// , oss_sp.str(), 0, false);
	if (stat_muil[SPT_SPE] > 0)
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, oss_cur_spe.str(), GOSSIP_SENDER_MAIN + 12, GOSSIP_ACTION_INFO_DEF);// , oss_spe.str(), 0, false);

	if (obj->ToCreature())
		player->SEND_GOSSIP_MENU(obj->GetEntry(), obj->GetGUID());
	else
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, obj->GetGUID());
}

void StatPoints::AddPoints(Player* player, uint32 sender)
{
	if (player->stat_points[SPT_TOTLAL] < 1)
	{
		player->GetSession()->SendNotification("点数不足！");
		player->CLOSE_GOSSIP_MENU();
		return;
	}

	Apply(player, false);

	switch (sender)
	{
	case GOSSIP_SENDER_MAIN + 1:
		player->stat_points[SPT_STMAMINA]++;
		player->stat_points[SPT_TOTLAL]--;
		break;
	case GOSSIP_SENDER_MAIN + 2:
		player->stat_points[SPT_AGILITY]++;
		player->stat_points[SPT_TOTLAL]--;
		break;
	case GOSSIP_SENDER_MAIN + 3:
		player->stat_points[SPT_STRENGTH]++;
		player->stat_points[SPT_TOTLAL]--;
		break;
	case GOSSIP_SENDER_MAIN + 4:
		player->stat_points[SPT_INTELLECT]++;
		player->stat_points[SPT_TOTLAL]--;
		break;
	case GOSSIP_SENDER_MAIN + 5:
		player->stat_points[SPT_SPIRIT]++;
		player->stat_points[SPT_TOTLAL]--;
		break;
	case GOSSIP_SENDER_MAIN + 6:
		player->stat_points[SPT_HIT]++;
		player->stat_points[SPT_TOTLAL]--;
		break;
	case GOSSIP_SENDER_MAIN + 7:
		player->stat_points[SPT_CRIT]++;
		player->stat_points[SPT_TOTLAL]--;
		break;
	case GOSSIP_SENDER_MAIN + 8:
		player->stat_points[SPT_HASTE]++;
		player->stat_points[SPT_TOTLAL]--;
		break;
	case GOSSIP_SENDER_MAIN + 9:
		player->stat_points[SPT_AP]++;
		player->stat_points[SPT_TOTLAL]--;
		break;
	case GOSSIP_SENDER_MAIN + 10:
		player->stat_points[SPT_APE]++;
		player->stat_points[SPT_TOTLAL]--;
		break;
	case GOSSIP_SENDER_MAIN + 11:
		player->stat_points[SPT_SP]++;
		player->stat_points[SPT_TOTLAL]--;
		break;
	case GOSSIP_SENDER_MAIN + 12:
		player->stat_points[SPT_SPE]++;
		player->stat_points[SPT_TOTLAL]--;
		break;
	default:
		break;
	}

	Apply(player, true);
	UpdateDB(player);
	player->CLOSE_GOSSIP_MENU();
}


class StatPoints_PlayerScript : public PlayerScript
{
public:
	StatPoints_PlayerScript() : PlayerScript("StatPoints_PlayerScript") {}

	void OnLogin(Player* player) override
	{
		sStatPoints->LoadPlayerData(player);
		sStatPoints->Apply(player, true);
	}

	void OnLogout(Player* player)
	{
		sStatPoints->UpdateDB(player);
	}

	//void OnCreatureKill(Player* player, Creature* /*killed*/)
	//{
	//	player->stat_points[SPT_TOTLAL]++;
	//}
};

class NPCStatPoints : public CreatureScript
{
public:
	NPCStatPoints() : CreatureScript("NPCStatPoints") { }
	bool OnGossipHello(Player* player, Creature* creature) override
	{
		sStatPoints->OpenPanel(player);

		return true;
		player->PlayerTalkClass->ClearMenus();

		sStatPoints->AddGosip(player, creature);

		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
	{
		player->PlayerTalkClass->ClearMenus();

		if (sender == GOSSIP_SENDER_MAIN)
		{
			sStatPoints->AddGosip(player, creature);
			return true;
		}

		sStatPoints->AddPoints(player, sender);

		player->PlayerTalkClass->ClearMenus();
		sStatPoints->AddGosip(player, creature);
		return true;
	}
};

void AddSC_StatPoints()
{
	new StatPoints_PlayerScript();
	new NPCStatPoints();
}


void StatPoints::OpenPanel(Player* player)
{
	std::ostringstream oss;

	oss << player->stat_points[SPT_TOTLAL] << "-斗气点数" << "#";

	if (stat_muil[SPT_STMAMINA] > 0)
		oss << stat_muil[SPT_STMAMINA] * player->stat_points[SPT_STMAMINA] << ":1:耐力-";
	if (stat_muil[SPT_AGILITY] > 0)
		oss << stat_muil[SPT_AGILITY] * player->stat_points[SPT_AGILITY] << ":2:敏捷-";
	if (stat_muil[SPT_STRENGTH] > 0)
		oss << stat_muil[SPT_STRENGTH] * player->stat_points[SPT_STRENGTH] << ":3:力量-";
	if (stat_muil[SPT_INTELLECT] > 0)
		oss << stat_muil[SPT_INTELLECT] * player->stat_points[SPT_INTELLECT] << ":4:智力-";
	if (stat_muil[SPT_SPIRIT] > 0)
		oss << stat_muil[SPT_SPIRIT] * player->stat_points[SPT_SPIRIT] << ":5:精神-";
	if (stat_muil[SPT_HIT] > 0)
		oss << stat_muil[SPT_HIT] * player->stat_points[SPT_HIT] << ":6:命中等级-";
	if (stat_muil[SPT_CRIT] > 0)
		oss << stat_muil[SPT_CRIT] * player->stat_points[SPT_CRIT] << ":7:暴击等级-";
	if (stat_muil[SPT_HASTE] > 0)
		oss << stat_muil[SPT_HASTE] * player->stat_points[SPT_HASTE] << ":8:急速等级-";
	if (stat_muil[SPT_AP] > 0)
		oss << stat_muil[SPT_AP] * player->stat_points[SPT_AP] << ":9:攻击强度-";
	if (stat_muil[SPT_APE] > 0)
		oss << stat_muil[SPT_APE] * player->stat_points[SPT_APE] << ":10:护甲穿透-";
	if (stat_muil[SPT_SP] > 0)
		oss << stat_muil[SPT_SP] * player->stat_points[SPT_SP] << ":11:法术强度-";
	if (stat_muil[SPT_SPE] > 0)
		oss << stat_muil[SPT_SPE] * player->stat_points[SPT_SPE] << ":12:法术穿透-";

	sGCAddon->SendPacketTo(player, "GC_S_STATPOINTS", oss.str());
}

void StatPoints::Ins(Player* player, uint32 id)
{
	if (player->stat_points[SPT_TOTLAL] < 1)
		return;

	Apply(player, false);

	switch (id)
	{
	case 1:
		player->stat_points[SPT_STMAMINA]++;
		player->stat_points[SPT_TOTLAL]--;
		break;
	case 2:
		player->stat_points[SPT_AGILITY]++;
		player->stat_points[SPT_TOTLAL]--;
		break;
	case 3:
		player->stat_points[SPT_STRENGTH]++;
		player->stat_points[SPT_TOTLAL]--;
		break;
	case 4:
		player->stat_points[SPT_INTELLECT]++;
		player->stat_points[SPT_TOTLAL]--;
		break;
	case 5:
		player->stat_points[SPT_SPIRIT]++;
		player->stat_points[SPT_TOTLAL]--;
		break;
	case 6:
		player->stat_points[SPT_HIT]++;
		player->stat_points[SPT_TOTLAL]--;
		break;
	case 7:
		player->stat_points[SPT_CRIT]++;
		player->stat_points[SPT_TOTLAL]--;
		break;
	case 8:
		player->stat_points[SPT_HASTE]++;
		player->stat_points[SPT_TOTLAL]--;
		break;
	case 9:
		player->stat_points[SPT_AP]++;
		player->stat_points[SPT_TOTLAL]--;
		break;
	case 10:
		player->stat_points[SPT_APE]++;
		player->stat_points[SPT_TOTLAL]--;
		break;
	case 11:
		player->stat_points[SPT_SP]++;
		player->stat_points[SPT_TOTLAL]--;
		break;
	case 12:
		player->stat_points[SPT_SPE]++;
		player->stat_points[SPT_TOTLAL]--;
		break;
	default:
		break;
	}

	Apply(player, true);
	UpdateDB(player);
	OpenPanel(player);
}

void StatPoints::Des(Player* player, uint32 id)
{
	Apply(player, false);

	switch (id)
	{
	case 1:
		if (player->stat_points[SPT_STMAMINA] < 1)
			break;
		player->stat_points[SPT_STMAMINA]--;
		player->stat_points[SPT_TOTLAL]++;
		break;
	case 2:
		if (player->stat_points[SPT_AGILITY] < 1)
			break;
		player->stat_points[SPT_AGILITY]--;
		player->stat_points[SPT_TOTLAL]++;
		break;
	case 3:
		if (player->stat_points[SPT_STRENGTH] < 1)
			break;
		player->stat_points[SPT_STRENGTH]--;
		player->stat_points[SPT_TOTLAL]++;
		break;
	case 4:
		if (player->stat_points[SPT_INTELLECT] < 1)
			break;
		player->stat_points[SPT_INTELLECT]--;
		player->stat_points[SPT_TOTLAL]++;
		break;
	case 5:
		if (player->stat_points[SPT_SPIRIT] < 1)
			break;
		player->stat_points[SPT_SPIRIT]--;
		player->stat_points[SPT_TOTLAL]++;
		break;
	case 6:
		if (player->stat_points[SPT_HIT] < 1)
			break;
		player->stat_points[SPT_HIT]--;
		player->stat_points[SPT_TOTLAL]++;
		break;
	case 7:
		if (player->stat_points[SPT_CRIT] < 1)
			break;
		player->stat_points[SPT_CRIT]--;
		player->stat_points[SPT_TOTLAL]++;
		break;
	case 8:
		if (player->stat_points[SPT_HASTE] < 1)
			break;
		player->stat_points[SPT_HASTE]--;
		player->stat_points[SPT_TOTLAL]++;
		break;
	case 9:
		if (player->stat_points[SPT_AP] < 1)
			break;
		player->stat_points[SPT_AP]--;
		player->stat_points[SPT_TOTLAL]++;
		break;
	case 10:
		if (player->stat_points[SPT_APE] < 1)
			break;
		player->stat_points[SPT_APE]--;
		player->stat_points[SPT_TOTLAL]++;
		break;
	case 11:
		if (player->stat_points[SPT_SP] < 1)
			break;
		player->stat_points[SPT_SP]--;
		player->stat_points[SPT_TOTLAL]++;
		break;
	case 12:
		if (player->stat_points[SPT_SPE] < 1)
			break;
		player->stat_points[SPT_SPE]--;
		player->stat_points[SPT_TOTLAL]++;
		break;
	default:
		break;
	}

	Apply(player, true);
	UpdateDB(player);
	OpenPanel(player);
}
