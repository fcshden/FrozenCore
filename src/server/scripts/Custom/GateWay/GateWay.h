
extern std::vector<std::string/*ban mac*/> MacBanedVec;

class GateWay
{
public:
	static GateWay* instance()
	{
		static GateWay instance;
		return &instance;
	}

	void Load();
	bool IsMacBaned(std::string macAdress);
	void BanPc(std::string charName);
	void UnBanPc(std::string charName);
private:

};
#define sGateWay GateWay::instance()