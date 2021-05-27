extern uint32 MAX_PLAYERS;

extern uint32  START_PLAYERS;

struct FakePlayersTemplate
{
	uint32 ID;
	std::string pname;                       // player name
	std::string gname;                       // guild name
	uint32 lvl;                              // player level
	uint32 class_;                           // player class
	uint32 race;                             // player race
	uint8 gender;                            // player gender
	uint32 pzoneid;
	bool online;
	uint32 hr;
	uint32 vip;
};
extern std::vector<FakePlayersTemplate> FakePlayersVec;

#define MAX_FAKE_PLAYERS_ZONE 10
extern uint32 FakePlayerZones[MAX_FAKE_PLAYERS_ZONE];

class FakePlayers
{
public:
	static FakePlayers* instance()
	{
		static FakePlayers instance;
		return &instance;
	}
	void Load();
	uint32 GenerateZoneId();

	void LoginOrLogout();
	void LevelUp();
	void UpdateZone();



	bool isSameTeamId(Player* player, std::string name);
	bool Logout(std::string name);


private:

};
#define sFakePlayers FakePlayers::instance()