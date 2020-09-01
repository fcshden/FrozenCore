#ifndef __CUSTOM_H
#define __CUSTOM_H
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
#include "LootMgr.h"
#include "Item.h"
#include "ObjectMgr.h"


class CustomMgr
{
public: //完全访问权限
	CustomMgr();
	~CustomMgr();

    std::string Format(char const *format, ...)
    {
        if (format)
        {
            va_list ap;
            char str[2048];
            va_start(ap, format);
            vsnprintf(str, 2048, format, ap);
            va_end(ap);

            std::string msg(str);
            return str;
        }
        return NULL;
    }

    void LoadAllCustomData();
protected:
private:
};

#define sCustomMgr ACE_Singleton<CustomMgr, ACE_Null_Mutex>::instance()

#endif

