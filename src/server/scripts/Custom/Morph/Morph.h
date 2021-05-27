#define MORPH_SPELLID 8

struct MorphTemplate
{
	uint32 displayId;
	std::string description;
	float scale;
	uint32 duration;
	uint8 skin;
	uint8 face;
	uint8 hair;
	uint8 haircolor;
	uint8 facialhair;
};

extern std::unordered_map<uint32, MorphTemplate> MorphMap;

struct CharInfoTemplate
{
	uint8 skin;
	uint8 face;
	uint8 hair;
	uint8 haircolor;
	uint8 facialhair;
	uint8 race;
	uint8 gender;
	uint32 displayId;
	uint32 morphId;
	uint32 duration;
};

extern std::unordered_map<uint32, CharInfoTemplate> CharInfoMap;
class Morph
{
public:
	static Morph* instance()
	{
		static Morph instance;
		return &instance;
	}
	void Load();
	void UpdateCharInfoMap(Player* player);

	uint32 GetMorphId(Player* player);
	void SetMorphId(Player* player, uint32 morphId);
	void SetMorphDuration(Player* player, uint32 duration);
	uint32 GetMorphDuration(Player* player);

	void Reset(Player* player);
	void Mor(Player* player);
	void DeMor(Player* player);

	uint32 GetDisplayId(uint32 morphId);
	std::string GetDescription(uint32 morphId);
	uint32 GetDuration(uint32 morphId);

	void AddGossip(Player* player, Object* obj);
	void Action(Player*player, uint32 action, Object*obj);
private:

};
//#define sMorph Morph::instance()