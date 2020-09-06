#ifndef __Need_H
#define __Need_H
#include "Common.h"
#include "Timer.h"
#include <ace/Singleton.h>
#include <ace/Atomic_Op.h>
#include "SharedDefines.h"
#include "QueryResult.h"
#include "Callback.h"
#include "Util.h"
#include <atomic>
#include <map>
#include <set>
#include <list>
#pragma execution_character_set("utf-8")

//所有需求表

struct CNeedParameters
{
    uint32 needid;
    uint32 classmask;
    uint32 racemask;
    uint32 level;
    uint32 viplevel;
    uint32 dqlevel;
    uint32 achpoint;
    
    std::vector<uint32> m_achneeds; //成就组
    std::vector<uint32> m_spellneeds; //技能组
    std::vector<uint32> m_auraneeds; //BUFF组
    std::vector<uint32> m_questhasneeds; //接取任务组
    std::vector<uint32> m_questcomneeds; //完成任务组
    std::vector<uint32> m_haveitems; //需求物品分组
    std::vector<uint32> m_haveitemcounts;


    uint32 jfcost;
    uint32 gbcost;
    uint32 honorcost;
    uint32 arenacost;
    std::vector<uint32> m_itemcosts; //消耗物品分组
    std::vector<uint32> m_itemcostcounts;

};

class NeedMgr
{
public:
    NeedMgr();
    ~NeedMgr();

     CNeedParameters const * GetNeedParameters(uint32 needid)
    {
        CNeedParametersContainer::const_iterator itr = m_needParameters.find(needid);
        if (itr != m_needParameters.end())
            return &itr->second;
        return nullptr;
    }

    typedef std::unordered_map<uint32, CNeedParameters>  CNeedParametersContainer;
    CNeedParametersContainer m_needParameters;

    void Load();
    bool Candothis(Player * pl, uint32 needid, uint8 count);
    void Needcost(Player * pl, uint32 needid, uint8 count);

protected:
private:
};
#define sNeedMgr ACE_Singleton<NeedMgr, ACE_Null_Mutex>::instance()
#endif

