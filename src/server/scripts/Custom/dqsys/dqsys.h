#ifndef __DQSYS_H
#define __DQSYS_H
#include "Common.h"
#include "Timer.h"
#include <ace/Singleton.h>
#include <ace/Atomic_Op.h>
#include "SharedDefines.h"
#include "Util.h"
#include <atomic>
#include <map>
#include <set>
#include <list>
#pragma execution_character_set("utf-8")

//斗气
struct Customconf
{
    uint32 entry;
    std::string conftxt;
};

struct CDQParameters
{
    uint32 dqlevel;
    std::string dqtitlestring;
    uint32 dqxp;
    uint32 dqpoint;
    uint32 itemcost;
    bool upcast;
    uint32 addtalent;
};

class DqSysMgr
{
public:
    DqSysMgr();
    ~DqSysMgr();

    std::string GetConftxt(uint32 entry)
    {
        if (GetConf(entry))
            return GetConf(entry)->conftxt;
        return NULL;
    }

    typedef std::unordered_map<uint32, Customconf> CustomconfMap;
    CustomconfMap Custom_confs;


    Customconf const* GetConf(uint32 entry) const
    {
        CustomconfMap::const_iterator itr = Custom_confs.find(entry);
        if (itr != Custom_confs.end())
            return &itr->second;
        return NULL;
    }

    void AddCustomConf(uint32 entry, std::string txt)
    {
        Customconf sc;
        sc.entry = entry;
        sc.conftxt = txt;
        Custom_confs[entry] = sc;
    }

    int32 GetConfInt(uint32 entry, uint32 index)
    {
        std::string param = GetConftxt(entry);
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

    std::string GetConfText(uint32 entry, ...)
    {
        if (GetConftxt(entry).c_str())
        {
            va_list ap;
            char szStr[1024];
            szStr[0] = '\0';
            va_start(ap, entry);
            vsnprintf(szStr, 1024, GetConftxt(entry).c_str(), ap);
            va_end(ap);

            std::string aaaaaaa = std::string(szStr);
            std::string strsrc = "@";
            std::string strdst = "\n";
            std::string::size_type pos = 0;
            std::string::size_type srclen = strsrc.size();
            std::string::size_type dstlen = strdst.size();

            while ((pos = aaaaaaa.find(strsrc, pos)) != std::string::npos)
            {
                aaaaaaa.replace(pos, srclen, strdst);
                pos += dstlen;
            }
            return aaaaaaa;
        }
        return "";
    }

    void SetDqData()
    {
        dqjs = GetConfInt(2, 1);
        maxdqjs = GetConfInt(2, 2);

        dqbj = GetConfInt(3, 1);
        maxdqbj = GetConfInt(3, 2);

        dqrx = GetConfInt(4, 1);
        maxdqrx = GetConfInt(4, 2);

        dqjz = GetConfInt(5, 1);
        maxdqjz = GetConfInt(5, 2);

        dqds = GetConfInt(6, 1);
        maxdqds = GetConfInt(6, 2);

        dqzj = GetConfInt(7, 1);
        maxdqzj = GetConfInt(7, 2);

        dqhj = GetConfInt(8, 1);
        maxdqhj = GetConfInt(8, 2);

        dqll = GetConfInt(9, 1);
        maxdqll = GetConfInt(9, 2);

        dqmj = GetConfInt(10, 1);
        maxdqmj = GetConfInt(10, 2);

        dqzl = GetConfInt(11, 1);
        maxdqzl = GetConfInt(11, 2);

        dqnl = GetConfInt(12, 1);
        maxdqnl = GetConfInt(12, 2);

        dqjs6 = GetConfInt(13, 1);
        maxdqjs6 = GetConfInt(13, 2);

        dqfsct = GetConfInt(14, 1);
        maxdqfsct = GetConfInt(14, 2);

        dqwlct = GetConfInt(15, 1);
        maxdqwlct = GetConfInt(15, 2);
    }
    //=============斗气================
    CDQParameters const *GetDQParameters(uint32 index) const { return (index) <= m_dqlevel ? &m_dqParameters[index] : NULL; }
    uint32 GetMaxdqLevel()const { return m_dqlevel - 1; }
    CDQParameters *m_dqParameters;
    uint32 m_dqlevel;


    uint32 dqjs, maxdqjs;//急速
    uint32 dqbj, maxdqbj;//暴击
    uint32 dqrx, maxdqrx;//韧性
    uint32 dqjz, maxdqjz;//精准
    uint32 dqds, maxdqds;//躲闪
    uint32 dqzj, maxdqzj;//招架
    uint32 dqhj, maxdqhj;//护甲
    uint32 dqll, maxdqll;// 力量
    uint32 dqmj, maxdqmj;//敏捷
    uint32 dqzl, maxdqzl;//智力
    uint32 dqnl, maxdqnl;//耐力
    uint32 dqjs6, maxdqjs6;//精神
    uint32 dqfsct, maxdqfsct;
    uint32 dqwlct, maxdqwlct;

    uint32 Getdqxp(Player * pl) const { return pl->dq_xp; }
    uint32 Getdqll(Player * pl) const { return pl->dq_ll; }
    uint32 Getdqmj(Player * pl) const { return pl->dq_mj; }
    uint32 Getdqzl(Player * pl) const { return pl->dq_zl; }
    uint32 Getdqnl(Player * pl) const { return pl->dq_nl; }
    uint32 Getdqjs6(Player * pl) const { return pl->dq_js6; }
    uint32 Getdqjs(Player * pl) const { return pl->dq_js; }
    uint32 Getdqbj(Player * pl) const { return pl->dq_bj; }
    uint32 Getdqrx(Player * pl) const { return pl->dq_rx; }
    uint32 Getdqjz(Player * pl) const { return pl->dq_jz; }
    uint32 Getdqds(Player * pl) const { return pl->dq_ds; }
    uint32 Getdqzj(Player * pl) const { return pl->dq_zj; }
    uint32 Getdqhj(Player * pl) const { return pl->dq_hj; }
    uint32 Getdqwlct(Player * pl) const { return pl->dq_wlct; }
    uint32 Getdqfsct(Player * pl) const { return pl->dq_fsct; }

    void Modifydqxp(Player * pl, int32 haaa)
    {
        pl->dq_xp = pl->dq_xp + haaa;
        OnPlayerUpdateDQXP(pl);
    }

    void Modifydqll(Player * pl, int32 haaa) { pl->dq_ll = pl->dq_ll + haaa; }
    void Modifydqmj(Player * pl, int32 haaa) { pl->dq_mj = pl->dq_mj + haaa; }
    void Modifydqzl(Player * pl, int32 haaa) { pl->dq_zl = pl->dq_zl + haaa; }
    void Modifydqnl(Player * pl, int32 haaa) { pl->dq_nl = pl->dq_nl + haaa; }
    void Modifydqjs6(Player * pl, int32 haaa) { pl->dq_js6 = pl->dq_js6 + haaa; }
    void Modifydqjs(Player * pl, int32 haaa) { pl->dq_js = pl->dq_js + haaa; }
    void Modifydqbj(Player * pl, int32 haaa) { pl->dq_bj = pl->dq_bj + haaa; }
    void Modifydqrx(Player * pl, int32 haaa) { pl->dq_rx = pl->dq_rx + haaa; }
    void Modifydqjz(Player * pl, int32 haaa) { pl->dq_jz = pl->dq_jz + haaa; }
    void Modifydqds(Player * pl, int32 haaa) { pl->dq_ds = pl->dq_ds + haaa; }
    void Modifydqzj(Player * pl, int32 haaa) { pl->dq_zj = pl->dq_zj + haaa; }
    void Modifydqhj(Player * pl, int32 haaa) { pl->dq_hj = pl->dq_hj + haaa; }
    void Modifydqwlct(Player * pl, int32 haaa) { pl->dq_wlct = pl->dq_wlct + haaa; }
    void Modifydqfsct(Player * pl, int32 haaa) { pl->dq_fsct = pl->dq_fsct + haaa; }

    void resetdq(Player * pl)
    {
        AppDQlevel(pl, false);
        uint32 shuxing = 0;
        for (uint32 i = 0; i < Getdqlevel(pl) + 1; i++)
        {
            shuxing = shuxing + GetDQParameters(i)->dqpoint;
        }
        pl->dq_shuxing = shuxing;
        pl->dq_ll = 0;
        pl->dq_mj = 0;
        pl->dq_zl = 0;
        pl->dq_nl = 0;
        pl->dq_js = 0;
        pl->dq_bj = 0;
        pl->dq_rx = 0;
        pl->dq_jz = 0;
        pl->dq_ds = 0;
        pl->dq_zj = 0;
        pl->dq_hj = 0;
        pl->dq_js6 = 0;
        pl->dq_wlct = 0;
        pl->dq_fsct = 0;
    }

    uint32 Getdqlevel(Player * pl) const { return pl->dq_level; }
    uint32 Getdqshuzong(Player * pl) const
    {
        return Getdqjs(pl) + Getdqbj(pl) + Getdqrx(pl) + Getdqjz(pl) + Getdqds(pl) + Getdqzj(pl) + Getdqhj(pl) + Getdqll(pl) + Getdqmj(pl) + Getdqzl(pl) + Getdqnl(pl) + Getdqjs6(pl)+ Getdqwlct(pl) + Getdqfsct(pl);
    }

    uint32 Getdqshuxing(Player * pl) const { return pl->dq_shuxing; }

    std::string GetdqTitleStd(Player * pl)
    {
        return GetDQParameters(Getdqlevel(pl))->dqtitlestring;
    }

    void Setdqxp(Player * pl, int32 haha)
    {
        pl->dq_xp = haha;
        OnPlayerUpdateDQXP(pl);
    }

    void Modifydqlevel(Player * pl, int32 haha)
    {
        pl->dq_level = haha + pl->dq_level;

        //sCustomMgr->CheckPlayerUpitem(this);
        OnPlayerUpdateDQXP(pl);
        //sCustomMgr->CheckAchDQCom(this, pl->dq_level);
        pl->InitTalentForLevel();

        CharacterDatabase.PExecute("update _玩家_斗气 set 升级时间 = %i where 玩家 = %u", time(NULL), pl->GetGUIDLow());
    }

    void Modifydqshuxing(Player * pl, int32 zshuxing) { pl->dq_shuxing = zshuxing + pl->dq_shuxing; }

    void AppDQlevel(Player * pl, bool applystat)
    {
        uint32 shu36 = dqjs * Getdqjs(pl); //急速
        uint32 shu32 = dqbj * Getdqbj(pl); //暴击
        uint32 shu35 = dqrx * Getdqrx(pl); //韧性
        uint32 shu37 = dqjz * Getdqjz(pl); //精准
        uint32 shu13 = dqds * Getdqds(pl); //躲闪
        uint32 shu14 = dqzj * Getdqzj(pl); //招架
        uint32 shu50 = dqhj * Getdqhj(pl); //护甲
        uint32 shu4 = dqll * Getdqll(pl);  // 力量
        uint32 shu3 = dqmj * Getdqmj(pl);  //敏捷
        uint32 shu5 = dqzl * Getdqzl(pl);  //智力
        uint32 shu7 = dqnl * Getdqnl(pl);  //耐力
        uint32 shu6 = dqjs6 * Getdqjs6(pl);  //精神
        uint32 shu44 = dqwlct * Getdqwlct(pl);  //精神
        uint32 shu47 = dqfsct * Getdqfsct(pl);  //精神

        pl->ApplyRatingMod(CR_HASTE_MELEE, int32(shu36), applystat);
        pl->ApplyRatingMod(CR_HASTE_RANGED, int32(shu36), applystat);
        pl->ApplyRatingMod(CR_HASTE_SPELL, int32(shu36), applystat);
        pl->ApplyRatingMod(CR_CRIT_MELEE, int32(shu32), applystat);
        pl->ApplyRatingMod(CR_CRIT_RANGED, int32(shu32), applystat);
        pl->ApplyRatingMod(CR_CRIT_SPELL, int32(shu32), applystat);
        pl->ApplyRatingMod(CR_CRIT_TAKEN_MELEE, int32(shu35), applystat);
        pl->ApplyRatingMod(CR_CRIT_TAKEN_RANGED, int32(shu35), applystat);
        pl->ApplyRatingMod(CR_CRIT_TAKEN_SPELL, int32(shu35), applystat);
        pl->ApplyRatingMod(CR_EXPERTISE, int32(shu37), applystat);
        pl->ApplyRatingMod(CR_DODGE, int32(shu13), applystat);
        pl->ApplyRatingMod(CR_PARRY, int32(shu14), applystat);

        pl->HandleStatModifier(UNIT_MOD_ARMOR, BASE_VALUE, float(shu50), applystat);
        pl->HandleStatModifier(UNIT_MOD_STAT_STRENGTH, BASE_VALUE, float(shu4), applystat);
        pl->ApplyStatBuffMod(STAT_STRENGTH, float(shu4), applystat);
        pl->HandleStatModifier(UNIT_MOD_STAT_AGILITY, BASE_VALUE, float(shu3), applystat);
        pl->ApplyStatBuffMod(STAT_AGILITY, float(shu3), applystat);
        pl->HandleStatModifier(UNIT_MOD_STAT_INTELLECT, BASE_VALUE, float(shu5), applystat);
        pl->ApplyStatBuffMod(STAT_INTELLECT, float(shu5), applystat);
        pl->HandleStatModifier(UNIT_MOD_STAT_STAMINA, BASE_VALUE, float(shu7), applystat);
        pl->ApplyStatBuffMod(STAT_STAMINA, float(shu7), applystat);
        pl->HandleStatModifier(UNIT_MOD_STAT_SPIRIT, BASE_VALUE, float(shu6), applystat);
        pl->ApplyStatBuffMod(STAT_SPIRIT, float(shu6), applystat);
        pl->ApplyRatingMod(CR_ARMOR_PENETRATION, int32(shu44), applystat);
        pl->ApplySpellPenetrationBonus(shu47, applystat);
    }

    void Load();

    void OnPlayerHelloDQ(Player * player);
    void OnPlayerSelectDQ(Player * player, uint32 sender, uint32 action, std::string code);
    bool CheckPldqcount(Player * player, uint32 count, uint32 type);
    void OnPlayerUpdateDQXP(Player * player);

protected:
private:
};

#define sDqSysMgr ACE_Singleton<DqSysMgr, ACE_Null_Mutex>::instance()
#endif

