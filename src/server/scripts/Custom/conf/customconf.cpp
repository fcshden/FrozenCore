#pragma execution_character_set("utf-8")
#include "customconf.h"

ConfMgr::ConfMgr()
{
    m_confParameters.clear();
    m_conftexts.clear();
}

ConfMgr::~ConfMgr()
{
    
}
 
void ConfMgr::Load()
{
    m_confParameters.clear();
    m_conftexts.clear();
    QueryResult confsys;
    int count = 0;
    int count2 = 0;

    confsys = WorldDatabase.PQuery("SELECT 配置ID,配置数据 FROM _配置_基础");
    if (confsys)
    {
        do
        {
            uint32 confid = confsys->Fetch()[0].GetUInt32();

            CConfParameters tmpConf;
            tmpConf.confid = confid;
            tmpConf.conftxt = confsys->Fetch()[1].GetString();

            m_confParameters[confid] = tmpConf;
            count++;
        } while (confsys->NextRow());
    }
    sLog->outString(">> 读取功能数据表[_配置_基础],共%u条数据读取加载...", count);

    confsys = WorldDatabase.PQuery("SELECT 配置ID,配置数据 FROM _配置_文本");
    if (confsys)
    {
        do
        {
            uint32 confid = confsys->Fetch()[0].GetUInt32();

            CConfText tmpConf;
            tmpConf.confid = confid;
            tmpConf.conftxt = confsys->Fetch()[1].GetString();

            m_conftexts[confid] = tmpConf;
            count2++;
        } while (confsys->NextRow());
    }
    sLog->outString(">> 读取功能数据表[_配置_文本],共%u条数据读取加载...", count2);
}
