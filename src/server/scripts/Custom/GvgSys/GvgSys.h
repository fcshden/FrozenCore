
struct gvgconf
{
	uint32 id;
	uint32 mapid;
	float m_x;
	float m_y;
	float m_z;
	float m_o;
	float min_x;
	float max_x;
	float min_y;
	float max_y;
	float min_z;
	float max_z;
	std::string areaids;
	uint32 itemid;
};


class GvgSys
{
public: //完全访问权限
	GvgSys();
	~GvgSys();

	static GvgSys* instance()
	{
		static GvgSys instance;
		return &instance;
	}

	//==============工会战============
	uint32 m_guildId1;   //守城工会ID
	uint32 m_guildId2;   //
	uint32 m_gvgtime;
	typedef std::map<uint32, gvgconf> gvgconf_t;
	gvgconf_t Vgvgconf;
	const gvgconf * Findgvg(const uint32 entry)
	{
		gvgconf_t::const_iterator It = Vgvgconf.find(entry);

		return (It != Vgvgconf.end() ? &It->second : NULL);
	}
	void LoadgvgSys();
	void UpdateGvGevent();
	bool IsInDistGC(Player * pl);
	bool IsInAreaGC(Player * pl);
	uint32 getGuildId1() { return m_guildId1; }
	uint32 getGuildId2() { return m_guildId2; }
	void setGuildId1(uint32 guildId) { m_guildId1 = guildId; }
	void setGuildId2(uint32 guildId) { m_guildId2 = guildId; }
	bool GCevent;

	uint32 GCpoint;
	uint32 GCtime;
	bool IsGuildvsGuild()
	{
		if (m_guildId1 > 0 && m_guildId2 > 0)
			return true;
		else
			return false;

	}
	bool IsGCevent()
	{
		if (GCevent)
			return true;
		else
			return false;
	}
	void StopEventSys(uint16 event_id);
	void StartEventSys(uint16 event_id);
	void SendGVGItem(Player * pl);

private:

};
#define sGvgSys GvgSys::instance()
