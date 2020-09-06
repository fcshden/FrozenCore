#ifndef __Rew_H
#define __Rew_H
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

struct CRewParameters
{
    uint32 rewid;
    uint32 jfrew;
    uint32 gbrew;
    uint32 honorrew;
    uint32 arenarew;
    uint32 xprew;
    uint32 dqxprew;
    std::vector<uint32> m_items; //奖励物品分组
    std::vector<uint32> m_itemcounts;
    std::vector<uint32> m_spellrews; //技能组
    std::vector<uint32> m_aurarews; //BUFF组
    std::vector<std::string> m_commands; //GM命令组

};

class RewMgr
{
public:
    RewMgr();
    ~RewMgr();

    CRewParameters const * GetRewParameters(uint32 rewid)
    {
        CRewParametersContainer::const_iterator itr = m_rewParameters.find(rewid);
        if (itr != m_rewParameters.end())
            return itr->second;
        return nullptr;
    }

    typedef std::unordered_map<uint32, CRewParameters*>  CRewParametersContainer;
    CRewParametersContainer m_rewParameters;

    void Load();
    void SendToPlayer(Player * pl, uint32 rewid, uint8 count);

protected:
private:
};
#define sRewMgr ACE_Singleton<RewMgr, ACE_Null_Mutex>::instance()
#endif

