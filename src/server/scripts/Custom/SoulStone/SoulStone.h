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



enum SoulStoneSlotType
{
    SS_XQ = 1,
    SS_FG = 2,
    SS_CX = 3,
};

//struct SoulStoneExDate
//{
//    uint32 page;
//    uint32 itemid1;
//    uint32 itemid2;
//    uint32 itemid3;
//    uint32 itemid4;
//    uint32 itemid5;
//    uint32 itemid6;
//    uint32 itemid7;
//};

struct SoulStoneItemType
{
	uint32 page;
	uint32 type;
};


struct SoulStonePlayerData
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


struct SoulStoneBuyReqData
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

class SoulStone
{
public:
    static SoulStone * instance()
    {
        static SoulStone instance;
        return &instance;
    }

    void setPlayerDataForPage(Player * player);
    void Load();
    void ReLoad();
    void SendMutualData(Player * player);
    void SendPlayerDataForPage(Player * player, uint32 page);
    void SavePlayerAllDate(Player * player);
    void CreatePlayerDate(Player * player);
    void SavePlayerSlotToSTL(Player * player, uint32 page, uint32 slot,uint32 itemid);
    void SavePlayerSlotToBase(Player * player, uint32 page, uint32 slot,uint32 itemid);
    void AddOrRemovePlayerBuff(Player * player, uint32 itemid,uint32 olditemid = 0, SoulStoneSlotType t = SS_XQ);
    void PlayerBuyPage(Player * player, uint32 page);
    uint32 GetPlayerMaxPage(Player * player);
    //uint32 GetItemType(uint32 itemid);
	uint32 GetItemType(uint32 itemid);
	int32 GetItemPage(uint32 itemid);

    uint32 GetPageActi(uint32 page);
    uint32 GetOldItemId(Player * player, uint32 page, uint32 slot);
    void SetPlayerMsg(Player * player, std::string msg) { if (!player) return; uint32 guid = player->GetGUIDLow(); _SoulStonePlayerMsgMap[guid] = msg; }
    std::string GetPlayerMsg(Player * player) { if (!player) return ""; uint32 guid = player->GetGUIDLow(); return _SoulStonePlayerMsgMap[guid]; }
    void SendAllActiData(Player * player);
    void SendActiDataToClient(Player * player, uint32 page);
    void SendUnActiDataToClient(Player * player, uint32 page);
    void SetActi(Player * player, uint32 page);
    void RemoveActi(Player * player, uint32 page);
    void CanInSetToSlot(Player * player, uint32 page, uint32 slot, uint32 itemid, std::string msg);
    void CanRemoveSlot(Player * player, uint32 page, uint32 slot, std::string msg);
    bool IfSoulStoneItem(uint32 item) { auto itr = _SoulStoneItemTypeMap.find(item);  if (itr == _SoulStoneItemTypeMap.end())return false; else return true; }
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


    typedef std::multimap<uint32, SoulStonePlayerData> SoulStonePlayerDataMap;
    typedef std::map<uint32, SoulStoneBuyReqData>  SoulStoneBuyReqDataMap;
	typedef std::map<uint32, SoulStoneItemType> SoulStoneItemTypeMap;
    typedef std::map<uint32, std::string> SoulStonePlayerMsgMap;
    //typedef std::multimap<uint32, SoulStoneExDate> SoulStoneExDateMap;

private:

    SoulStonePlayerDataMap _SoulStonePlayerDataMap;
    SoulStoneBuyReqDataMap _SoulStoneBuyReqDataMap;
    SoulStoneItemTypeMap _SoulStoneItemTypeMap;
    SoulStonePlayerMsgMap _SoulStonePlayerMsgMap;
    //SoulStoneExDateMap _SoulStoneExDateMap;
	uint32 limitpageval;
};

#define sSoulStone SoulStone::instance()
