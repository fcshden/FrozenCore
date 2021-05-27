#pragma execution_character_set("utf-8")
#include "Morph.h"
#include "../FunctionCollection/FunctionCollection.h"

std::unordered_map<uint32, MorphTemplate> MorphMap;
std::unordered_map<uint32, CharInfoTemplate> CharInfoMap;

void Morph::Load()
{
	MorphMap.clear();
	QueryResult result = WorldDatabase.PQuery("SELECT id,displayId,description,scale,duration,skin,face,hair,haircolor,facialhair FROM _morph");
	if (!result)
		return;
	do
	{
		Field* fields = result->Fetch();	
		uint32 id = fields[0].GetUInt32();

		if (id == 0)
			continue;

		MorphTemplate Temp;
		Temp.displayId		= fields[1].GetUInt32();
		Temp.description	= fields[2].GetString();
		Temp.scale			= fields[3].GetUInt32();
		Temp.duration		= fields[4].GetUInt32();
		Temp.skin			= fields[5].GetUInt32();
		Temp.face			= fields[6].GetUInt32();
		Temp.hair			= fields[7].GetUInt32();
		Temp.haircolor		= fields[8].GetUInt32();
		Temp.facialhair		= fields[9].GetUInt32();
		MorphMap.insert(std::make_pair(id, Temp));

	} while (result->NextRow());
}

void Morph::UpdateCharInfoMap(Player* player)
{
	std::unordered_map<uint32, CharInfoTemplate>::iterator iter = CharInfoMap.find(player->GetGUIDLow());

	if (iter == CharInfoMap.end())
	{
		CharInfoTemplate Temp;
		Temp.skin = player->GetByteValue(PLAYER_BYTES, 0);
		Temp.face = player->GetByteValue(PLAYER_BYTES, 1);
		Temp.hair = player->GetByteValue(PLAYER_BYTES, 2);
		Temp.haircolor = player->GetByteValue(PLAYER_BYTES, 3);
		Temp.facialhair = player->GetByteValue(PLAYER_BYTES_2, 0);
		Temp.race = player->getRace();
		Temp.gender = player->getGender();
		Temp.displayId = player->GetDisplayId();
		Temp.morphId = 0;
		Temp.duration = 0;
		CharInfoMap.insert(std::make_pair(player->GetGUIDLow(), Temp));
	}
	else
	{
		iter->second.skin = player->GetByteValue(PLAYER_BYTES, 0);
		iter->second.face = player->GetByteValue(PLAYER_BYTES, 1);
		iter->second.hair = player->GetByteValue(PLAYER_BYTES, 2);
		iter->second.haircolor = player->GetByteValue(PLAYER_BYTES, 3);
		iter->second.facialhair = player->GetByteValue(PLAYER_BYTES_2, 0);
		iter->second.race = player->getRace();
		iter->second.gender = player->getGender();
		iter->second.displayId = player->GetDisplayId();
	}
}

std::string Morph::GetDescription(uint32 morphId)
{
	std::unordered_map<uint32, MorphTemplate>::iterator iter = MorphMap.find(morphId);

	if (iter != MorphMap.end())
		return iter->second.description;

	return "";
}

uint32 Morph::GetDuration(uint32 morphId)
{
	std::unordered_map<uint32, MorphTemplate>::iterator iter = MorphMap.find(morphId);

	if (iter != MorphMap.end())
		return iter->second.duration * IN_MILLISECONDS;

	return 0;
}

uint32 Morph::GetMorphId(Player* player)
{
	std::unordered_map<uint32, CharInfoTemplate>::iterator iter = CharInfoMap.find(player->GetGUIDLow());

	if (iter != CharInfoMap.end())
		return iter->second.morphId;

	return 0;
}

void Morph::SetMorphId(Player* player, uint32 morphId)
{
	std::unordered_map<uint32, CharInfoTemplate>::iterator iter = CharInfoMap.find(player->GetGUIDLow());

	if (iter != CharInfoMap.end())
		iter->second.morphId = morphId;
}

void Morph::SetMorphDuration(Player* player, uint32 duration)
{
	std::unordered_map<uint32, CharInfoTemplate>::iterator iter = CharInfoMap.find(player->GetGUIDLow());

	if (iter != CharInfoMap.end())
		iter->second.duration = duration;
}

uint32 Morph::GetMorphDuration(Player* player)
{
	std::unordered_map<uint32, CharInfoTemplate>::iterator iter = CharInfoMap.find(player->GetGUIDLow());

	if (iter != CharInfoMap.end())
		return iter->second.duration;

	return 0;
}

void Morph::Mor(Player* player)
{
	uint32 morphId = GetMorphId(player);

	if (morphId == 0)
		return;
	
	std::unordered_map<uint32, MorphTemplate>::iterator iter = MorphMap.find(morphId);

	if (iter != MorphMap.end())
	{
		uint32 displayId		= iter->second.displayId;
		float scale				= iter->second.scale;
		uint8 skin				= iter->second.skin;
		uint8 face				= iter->second.face;
		uint8 hair				= iter->second.hair;
		uint8 haircolor			= iter->second.haircolor;
		uint8 facialhair		= iter->second.facialhair;

		if (displayId == player->GetDisplayId())
			return;

		player->SetByteValue(PLAYER_BYTES, 0, skin);//skin
		player->SetByteValue(PLAYER_BYTES, 1, face);//face
		player->SetByteValue(PLAYER_BYTES, 2, hair); //hair
		player->SetByteValue(PLAYER_BYTES, 3, haircolor);//haircolor
		player->SetByteValue(PLAYER_BYTES_2, 0, facialhair);//facialhair

		uint8 race = 0;
		uint8 gender = 0;

		switch (displayId)
		{
		case 49:
			race = 1;
			gender = 0;
			break;
		case 50:
			race = 1;
			gender = 1;
			break;
		case 51:
			race = 2;
			gender = 0;
			break;
		case 52:
			race = 2;
			gender = 1;
			break;
		case 53:
			race = 3;
			gender = 0;
			break;
		case 54:
			race = 3;
			gender = 1;
			break;
		case 55:
			race = 4;
			gender = 0;
			break;
		case 56:
			race = 4;
			gender = 1;
			break;
		case 57:
			race = 5;
			gender = 0;
			break;
		case 58:
			race = 5;
			gender = 1;
			break;
		case 59:
			race = 6;
			gender = 0;
			break;
		case 60:
			race = 6;
			gender = 1;
			break;
		case 1563:
			race = 7;
			gender = 0;
			break;
		case 1564:
			race = 7;
			gender = 1;
			break;
		case 1478:
			race = 8;
			gender = 0;
			break;
		case 1479:
			race = 8;
			gender = 1;
			break;
		case 15476:
			race = 10;
			gender = 0;
			break;
		case 15475:
			race = 10;
			gender = 1;
			break;
		case 16125:
			race = 11;
			gender = 0;
			break;
		case 16126:
			race = 11;
			gender = 1;
			break;
		default:
			break;
		}

		uint32 bytes0 = 0;
		bytes0 |= race;										// race
		bytes0 |= player->getClass() << 8;                  // class
		bytes0 |= gender << 16;                             // gender
		player->SetUInt32Value(UNIT_FIELD_BYTES_0, bytes0);


		for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; ++slot)
			if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
				player->SetVisibleItemSlot(slot, item);

		player->SetDisplayId(displayId);
		player->SetNativeDisplayId(displayId);
		player->SetObjectScale(scale);
	}
}

void Morph::DeMor(Player* player)
{
	SetMorphId(player, 0);
	SetMorphDuration(player, 0);
	player->SetObjectScale(1.0f);
	Reset(player);
}

void Morph::Reset(Player* player)
{
	std::unordered_map<uint32, CharInfoTemplate>::iterator iter = CharInfoMap.find(player->GetGUIDLow());

	if (iter != CharInfoMap.end())
	{
		player->SetByteValue(PLAYER_BYTES, 0, iter->second.skin);//skin
		player->SetByteValue(PLAYER_BYTES, 1, iter->second.face);//face
		player->SetByteValue(PLAYER_BYTES, 2, iter->second.hair); //hair
		player->SetByteValue(PLAYER_BYTES, 3, iter->second.haircolor);//haircolor
		player->SetByteValue(PLAYER_BYTES_2, 0, iter->second.facialhair);//facialhair

		uint32 bytes0 = 0;
		bytes0 |= iter->second.race;				// race
		bytes0 |= player->getClass() << 8;          // class
		bytes0 |= iter->second.gender << 16;        // gender
		player->SetUInt32Value(UNIT_FIELD_BYTES_0, bytes0);

		player->SetDisplayId(iter->second.displayId);
		player->SetNativeDisplayId(iter->second.displayId);
	}
}

void Morph::AddGossip(Player* player, Object* obj)
{
	player->PlayerTalkClass->ClearMenus();

	for (std::unordered_map<uint32, MorphTemplate>::iterator iter = MorphMap.begin(); iter != MorphMap.end(); iter++)
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, iter->second.description, SENDER_MORPH, iter->first + SENDER_MORPH);
	
	if (obj->ToCreature())
		player->SEND_GOSSIP_MENU(obj->GetEntry(), obj->GetGUID());
	else
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, obj->GetGUID());
}

void Morph::Action(Player*player, uint32 action, Object*obj)
{
	player->RemoveAura(MORPH_SPELLID);
	SetMorphId(player, action - SENDER_MORPH);
	SetMorphDuration(player, GetDuration(action - SENDER_MORPH));
	player->CastSpell(player, MORPH_SPELLID);
	AddGossip(player, obj);
}
//
//class spell_morph : public SpellScriptLoader
//{
//public:
//	spell_morph() : SpellScriptLoader("spell_morph") { }
//
//	class spell_morph_AuraScript : public AuraScript
//	{
//		PrepareAuraScript(spell_morph_AuraScript);
//
//		void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*handle*/)
//		{
//			Unit* target = GetTarget();
//
//			if (!target || target->GetTypeId() != TYPEID_PLAYER)
//				return;
//
//			uint32 duration = sMorph->GetMorphDuration(target->ToPlayer());
//			SetMaxDuration(duration);
//			SetDuration(duration);
//			sMorph->Mor(target->ToPlayer());
//		}
//
//		void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*handle*/)
//		{
//			Unit* target = GetTarget();
//
//			if (!target || target->GetTypeId() != TYPEID_PLAYER)
//				return;
//
//			if (GetDuration() < 1)
//				sMorph->DeMor(target->ToPlayer());
//			else
//				sMorph->SetMorphDuration(target->ToPlayer(), GetDuration());
//		}
//
//		void Register()
//		{
//			OnEffectApply += AuraEffectApplyFn(spell_morph_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
//			OnEffectRemove += AuraEffectRemoveFn(spell_morph_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
//		}
//	};
//
//	AuraScript* GetAuraScript() const
//	{
//		return new spell_morph_AuraScript();
//	}
//};
//
//class MorPlayerScript : PlayerScript
//{
//public:
//	MorPlayerScript() : PlayerScript("MorPlayerScript") {}
//
//	void OnLogin(Player* player, bool /*first*/)
//	{
//		//sMorph->UpdateCharInfoMap(player);
//	}
//};
//
//void AddSC_Morph()
//{
//	new spell_morph();
//	new MorPlayerScript();
//}