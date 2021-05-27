#include "Player.h"
#include "BattlegroundQueue.h"

class CFBG
{
public:
	static CFBG* instance()
	{
		static CFBG instance;
		return &instance;
	}
	void SetFaction(Player* player);

	bool SendCFBGChat(Player* player, uint32 msgtype, std::string message);

private:
	
};
#define sCFBG CFBG::instance()