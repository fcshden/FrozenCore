#include "Chat.h"
#include "ScriptMgr.h"
#include "AccountMgr.h"
#include "ArenaTeamMgr.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "Group.h"
#include "GuildMgr.h"
#include "InstanceSaveMgr.h"
#include "Language.h"
#include "MovementGenerator.h"
#include "ObjectAccessor.h"
#include "Opcodes.h"
#include "SpellAuras.h"
#include "TargetedMovementGenerator.h"
#include "WeatherMgr.h"
#include "ace/INET_Addr.h"
#include "Player.h"
#include "Pet.h"
#include "LFG.h"
#include "GroupMgr.h"
#include "BattlegroundMgr.h"
#include "MapManager.h"



enum SoulStoneExSlotType
{
    SSEX_XQ = 1,
    SSEX_FG = 2,
    SSEX_CX = 3,
};

struct SoulStoneExDate
{
    uint32 page;
    uint32 itemid1;
    uint32 itemid2;
    uint32 itemid3;
    uint32 itemid4;
    uint32 itemid5;
    uint32 itemid6;
    uint32 itemid7;
};

struct SoulStoneExItemType
{
	uint32 page;
	uint32 type;
};

struct SoulStoneExPlayerData
{
    uint32 guid;
    uint32 page;
    uint32 itemid1;
    uint32 itemid2;
    uint32 itemid3;
    uint32 itemid4;
    uint32 itemid5;
    uint32 itemid6;
    uint32 itemid7;
};


struct SoulStoneExBuyReqData
{
    uint32 page;
    uint32 itemid1;
    uint32 val1;
    uint32 itemid2;
    uint32 val2;
    uint32 itemid3;
    uint32 val3;
    uint32 itemid4;
    uint32 val4;
    uint32 itemid5;
    uint32 val5;
    uint32 vip;
    uint32 junxian;
    uint32 zhuansheng;
    uint32 douqival;
    uint32 money;
    uint32 jihuoshuxingid;
};

class SoulStoneEx
{
public:
    static SoulStoneEx * instance()
    {
        static SoulStoneEx instance;
        return &instance;
    }

    void Load();
    void ReLoad();
    void SendMutualData(Player * player);
    void SendPlayerDataForPage(Player * player, uint32 page);
    void SavePlayerAllDate(Player * player);
    void CreatePlayerDate(Player * player);
    void SavePlayerSlotToSTL(Player * player, uint32 page, uint32 slot,uint32 itemid);
    void SavePlayerSlotToBase(Player * player, uint32 page, uint32 slot,uint32 itemid);
    void AddOrRemovePlayerBuff(Player * player, uint32 itemid,uint32 olditemid = 0, SoulStoneExSlotType t = SSEX_XQ);
    void PlayerBuyPage(Player * player, uint32 page);
    uint32 GetPlayerMaxPage(Player * player);
    //uint32 GetItemType(uint32 itemid);
	uint32 GetItemType(uint32 itemid);
	int32 GetItemPage(uint32 itemid);

    uint32 GetPageActi(uint32 page);
    uint32 GetOldItemId(Player * player, uint32 page, uint32 slot);
    void SetPlayerMsg(Player * player, std::string msg) { if (!player) return; uint32 guid = player->GetGUIDLow(); _SoulStoneExPlayerMsgMap[guid] = msg; }
    std::string GetPlayerMsg(Player * player) { if (!player) return ""; uint32 guid = player->GetGUIDLow(); return _SoulStoneExPlayerMsgMap[guid]; }
    void SendAllActiData(Player * player);
    void SendActiDataToClient(Player * player, uint32 page);
    void SendUnActiDataToClient(Player * player, uint32 page);
    void SetActi(Player * player, uint32 page);
    void RemoveActi(Player * player, uint32 page);
    void CanInSetToSlot(Player * player, uint32 page, uint32 slot, uint32 itemid, std::string msg);
    void CanRemoveSlot(Player * player, uint32 page, uint32 slot, std::string msg);
    bool IfSoulStoneItem(uint32 item) { auto itr = _SoulStoneExItemTypeMap.find(item);  if (itr == _SoulStoneExItemTypeMap.end())return false; else return true; }
    bool CanSetActi(Player * player, uint32 page);
    bool IsActi(Player * player,uint32 page);
    bool IsData(Player * player);
    bool IsBuyPageReq(Player * player,uint32 page);
    void RemoveReq(Player * player, uint32 page);
    void SendBuyIsOkPage(Player * player, uint32 page);
    void SetActiId(Player * player, uint32 page, uint32 itemid);
    void AgainAddLoseBuff(Player * player);
	void UpdatePlayerDate(Player * player);

	std::vector<std::string> split(std::string str, std::string pattern);
    uint32 GetExActiId(Player * player, uint32 page);


    typedef std::multimap<uint32, SoulStoneExPlayerData> SoulStonePlayerDataMap;
    typedef std::map<uint32, SoulStoneExBuyReqData>  SoulStoneBuyReqDataMap;
	typedef std::map<uint32, SoulStoneExItemType> SoulStoneItemTypeMap;
    typedef std::map<uint32, std::string> SoulStonePlayerMsgMap;
    typedef std::multimap<uint32, SoulStoneExDate> SoulStoneExDateMap;

private:

    SoulStonePlayerDataMap _SoulStoneExPlayerDataMap;
    SoulStoneBuyReqDataMap _SoulStoneExBuyReqDataMap;
    SoulStoneItemTypeMap _SoulStoneExItemTypeMap;
    SoulStonePlayerMsgMap _SoulStoneExPlayerMsgMap;
    SoulStoneExDateMap _SoulStoneExExDateMap;
	uint32 limitpageval;
};

#define sSoulStoneEx SoulStoneEx::instance()
