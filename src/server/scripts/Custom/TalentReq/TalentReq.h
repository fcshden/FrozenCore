
/*
CREATE TABLE characters_talent_req( guid INT DEFAULT 0, spellId INT DEFAULT 0 );
CREATE TABLE _talent_req( comment VARCHAR(128), spellId INT UNSIGNED NOT NULL DEFAULT 0, reqId INT UNSIGNED NOT NULL DEFAULT 0, PRIMARY KEY (spellId) );
*/

extern std::unordered_map<uint32, uint32>TalentReqMap;

class TalentReq
{
public:
	static TalentReq* instance()
	{
		static TalentReq instance;
		return &instance;
	}

	void Load();
	bool SendAcceptOrCancel(Player* player, uint32 spellid);
	void DoAction(Player* player);
private:

};
#define sTalentReq TalentReq::instance()