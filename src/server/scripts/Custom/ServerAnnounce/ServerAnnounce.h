struct ServerAnnounceTemplate
{
	uint32 entry;
	uint8 flag;
	uint8 announceType;
	std::string text;
};
extern std::vector<ServerAnnounceTemplate> ServerAnnounceVec;

class ServerAnnounce
{
public:
	static ServerAnnounce* instance()
	{
		static ServerAnnounce instance;
		return &instance;
	}
	void Load();
	const char* Format(const char *format, ...);
	std::string GetItemLink(uint32 entry);
	std::string GetNameLink(Player* player);
	void Announce(Player* announcer,uint32 entry, uint8 flag);
	void CreatureRespawn(Creature* creature);
};
#define sServerAnnounce ServerAnnounce::instance()