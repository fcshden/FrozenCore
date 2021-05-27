struct ZoneAuraTemplate
{
	uint32 aura;
	uint32 limitHP;
};
extern std::unordered_map<uint32/*zone*/, ZoneAuraTemplate> ZoneAuraMap;

class ZoneAura
{
public:
	static ZoneAura* instance()
	{
		static ZoneAura instance;
		return &instance;
	}
	void Load();
	int32 GetAuraStack(Map* map, uint32 zone, uint32 limitHP);
	void UpdateAura(Map* map);
private:
};
#define sZoneAura ZoneAura::instance()