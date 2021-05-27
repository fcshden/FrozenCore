struct CTOPCustom
{
	uint32 entry;
	uint32 pl1;
	uint32 pl2;
};

struct CTopSys
{
	uint32 entry;
	std::string text;
	uint32 itemid;
	uint32 buffs;
};

class CharPvpTop
{
public: //完全访问权限
	CharPvpTop();
	~CharPvpTop();

	static CharPvpTop* instance()
	{
		static CharPvpTop instance;
		return &instance;
	}

	
	typedef std::map<uint32, CTOPCustom> CTOPCustom_t;
	CTOPCustom_t VCtopCustom;

	void AddTopTeam(uint32 entry, uint32 pl1, uint32 pl2)
	{
		CTOPCustom sc;
		sc.entry = entry;
		sc.pl1 = pl1;
		sc.pl2 = pl2;
		VCtopCustom[entry] = sc;
	}
	std::vector<uint32> _charTopDataMap;
	std::vector<uint32> _charTopMaxMap;
	void RollTopTeamID();
	void UpdateCharTopData();
	void SengPVPgo(uint32 entry);
	uint64 topGobGuid;
	time_t pvptime;
	void PVPupdate();
	void SendTopTitle(bool isin);
	uint32 GetTopMax() const { return _charTopMaxMap.size(); }
	void DeleteTopData(uint32 guid)
	{
		for (std::vector<uint32>::iterator iter = _charTopMaxMap.begin(); iter != _charTopMaxMap.end();)
		{
			if (*iter == guid)
				iter = _charTopMaxMap.erase(iter);
			else
				iter++;
		}
	}
	bool Isintop(uint32 guid)
	{
		for (std::vector<uint32>::iterator iter = _charTopMaxMap.begin(); iter != _charTopMaxMap.end(); ++iter)
		{
			if (*iter == guid)
				return true;
		}
		return false;
	}
	bool isfirst;
	bool topevent;
	uint32 pvpcount;


	typedef std::map<uint32, CTopSys> CTopSys_t;
	CTopSys_t VCTopSys;
	const CTopSys * FindTopSys(const uint32  entryID)
	{
		CTopSys_t::const_iterator It = VCTopSys.find(entryID);

		return (It != VCTopSys.end() ? &It->second : NULL);
	}

	std::vector<uint32> topbufflists;
	void SendAndClearTopBuff(Player * pl, bool onlyph = false);
	GameObject* SpawnGob(uint32 guid, bool created);
	typedef std::map<uint32, uint32> PlayerTopPhMap;
	PlayerTopPhMap m_playertopph;
	void LoadTopSys();
private:

};
#define sCharPvpTop CharPvpTop::instance()
