#ifndef __Conf_H
#define __Conf_H
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

struct CConfParameters
{
    uint32 confid;
    std::string conftxt;
};

struct CConfText
{
    uint32 confid;
    std::string conftxt;
};

class ConfMgr
{
public:
    ConfMgr();
    ~ConfMgr();

    std::string Get1Conftxt(uint32 entry)
    {
        for (auto itr = m_confParameters.begin(); itr != m_confParameters.end(); ++itr)
            if (itr->confid == entry)
                return itr->conftxt;
        return "";
    }
    
    bool GetConfBool(uint32 entry, uint32 index)
    {
        std::string param = Get1Conftxt(entry);

        if (param.empty())
            return false;

        bool abc;
        Tokenizer confalls(param, '#');
        if (confalls.size() > 1)
        {
            if (index <= confalls.size())
            {
                if (confalls[index - 1] == "开" || confalls[index - 1] == "true" || confalls[index - 1] == "1" || confalls[index - 1] == "TRUE")
                    abc = true;
                else
                    abc = false;
            }
            else
            {

                if (confalls[0] == "开" || confalls[0] == "true" || confalls[0] == "1" || confalls[0] == "TRUE")
                    abc = true;
                else
                    abc = false;
            }
        }
        else
        {
            if (confalls[0] == "开" || confalls[0] == "true" || confalls[0] == "1" || confalls[0] == "TRUE")
                abc = true;
            else
                abc = false;
        }

        return abc;
    }

    std::string GetConfText(uint32 entry, uint32 index)
    {
        std::string param = Get1Conftxt(entry);
        if (param.empty())
            return "";

        std::string abc;
        Tokenizer confalls(param, '#');
        if (confalls.size() > 1)
        {
            if (index <= confalls.size())
                abc = confalls[index - 1];
            else
                abc = confalls[0];
        }
        else
            abc = confalls[0];

        return abc;
    }

    float GetConfFloat(uint32 entry, uint32 index)
    {
        std::string param = Get1Conftxt(entry);
        if (param.empty())
            return 0.0f;

        float abc;
        Tokenizer confalls(param, '#');
        if (confalls.size() > 1)
        {
            if (index <= confalls.size())
                abc = atof(confalls[index - 1]);
            else
                abc = atof(confalls[0]);
        }
        else
            abc = atof(confalls[0]);

        return abc;
    }

    int32 GetConfInt(uint32 entry, uint32 index)
    {
        std::string param = Get1Conftxt(entry);
        if (param.empty())
            return 0;

        int32 abc;
        Tokenizer confalls(param, '#');
        if (confalls.size() > 1)
        {
            if (index <= confalls.size())
                abc = atoi(confalls[index - 1]);
            else
                abc = atoi(confalls[0]);
        }
        else
            abc = atoi(confalls[0]);

        return abc;
    }

    typedef std::vector<CConfParameters>  CConfParametersContainer;
    CConfParametersContainer m_confParameters;


    std::string Get2Conftxt(uint32 entry)
    {
        for (auto itr = m_conftexts.begin(); itr != m_conftexts.end(); ++itr)
            if (itr->confid == entry)
                return itr->conftxt;
        return "";
    }


    typedef std::vector<CConfText>  CConfTextContainer;
    CConfTextContainer m_conftexts;

    void Load();

protected:
private:
};

#define sConfMgr ACE_Singleton<ConfMgr, ACE_Null_Mutex>::instance()
#endif


