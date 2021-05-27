extern std::unordered_map<std::string, uint32> CDKMap;

class CDKC
{
public:
	static CDKC* instance()
	{
		static CDKC instance;
		return &instance;
	}

	void Load();
	std::string Create();
	void Create(uint32 count, uint32 rewId, std::string comment);
	void OutPut();
	void AddGossip(Player* player, Object* obj);
	bool Redeem(Player* player, uint32 sender, uint32 action, std::string cdk);
private:

};
#define sCDK CDKC::instance()