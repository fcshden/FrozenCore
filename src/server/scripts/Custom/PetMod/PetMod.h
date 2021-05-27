struct PetModTemplate
{
	float HpAddPct;
	float DmgAddPct;
	float SpAddPct;
	float ArmorLimit;
};

extern std::unordered_map<uint8, PetModTemplate> PetModMap;

class PetMod
{
public:
	static PetMod* instance()
	{
		static PetMod instance;
		return &instance;
	}
	void Load();
	void GetHpMod(Guardian* guardian, float &value);
	void GetMinDmgMod(Guardian* guardian, float &mindamage, float &maxdamage);
	float GetSpAddPct(Unit* owner);
	void GetArmorLimit(Guardian* guardian, float &value);
private:

};
#define sPetMod PetMod::instance()