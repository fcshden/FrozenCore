/*
The Zxtreme - Dev Core
*/
#pragma execution_character_set("utf-8")
#include <cstring>
#include <string.h>
#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ArenaTeam.h"
#include "ArenaTeamMgr.h"
#include "World.h"

enum enus
{
    ARENA_MAX_RESULTS = 10,
    ARENA_2V2_LADDER = GOSSIP_ACTION_INFO_DEF + 1,
    ARENA_3V3_LADDER = GOSSIP_ACTION_INFO_DEF + 2,
    ARENA_5V5_LADDER = GOSSIP_ACTION_INFO_DEF + 3,
    ARENA_GOODBYE = GOSSIP_ACTION_INFO_DEF + 4,
    ARENA_START_TEAM_LOOKUP = GOSSIP_ACTION_INFO_DEF + 5,
    ARENA_HELP = GOSSIP_ACTION_INFO_DEF + 9999,
};

class arena_top_teams : public CreatureScript
{
    private:
    uint32 optionToTeamType(uint32 option) 
    {
        uint32 teamType;
        switch(option)
        {
            case ARENA_2V2_LADDER: 
                teamType = 2;
            break;
                    
            case ARENA_3V3_LADDER:
                teamType = 3; 
            break;
                
            case ARENA_5V5_LADDER:
                teamType = 1; 
            break;
        }
        return teamType;
    }
        
    uint32 teamTypeToOption(uint32 teamType) 
    {
        uint32 option;
        switch(teamType) 
        {
            case 2: 
                option = ARENA_2V2_LADDER; 
            break;
                    
            case 3: 
                option = ARENA_3V3_LADDER; 
            break;
                    
            case 5: 
                option = ARENA_5V5_LADDER; 
            break;
        }
        return option;
    }
        
    std::string raceToString(uint8 race) 
    {
        std::string race_s = "Unknown";
        switch (race)
        {
            case RACE_HUMAN:            
                race_s = "人类";       
            break;
                
            case RACE_ORC:              
                race_s = "兽人";         
            break;
                
            case RACE_DWARF:           
                race_s = "矮人";       
            break;
                
            case RACE_NIGHTELF:         
                race_s = "暗夜精灵";   
            break;
                
            case RACE_UNDEAD_PLAYER:   
                race_s = "亡灵";      
            break;
                
            case RACE_TAUREN:          
                race_s = "牛头人";      
            break;
                
            case RACE_GNOME:            
                race_s = "Gnome";       
            break;
                
            case RACE_TROLL:            
                race_s = "巨魔";       
            break;
                
            case RACE_BLOODELF:         
                race_s = "血精灵";   
            break;
                
            case RACE_DRAENEI:          
                race_s = "德莱尼";     
            break;
        }
        return race_s;
    }
        
    std::string classToString(uint8 Class) 
    {
        std::string Class_s = "Unknown";
        switch (Class)
        {
            case CLASS_WARRIOR:         
                Class_s = "战士";        
            break;
                
            case CLASS_PALADIN:        
                Class_s = "圣骑士";       
            break;
                
            case CLASS_HUNTER:         
                Class_s = "猎人";        
            break;
                
            case CLASS_ROGUE:          
                Class_s = "盗贼";          
            break;
                
            case CLASS_PRIEST:
                Class_s = "牧师";
            break;
                
            case CLASS_DEATH_KNIGHT:
                Class_s = "死亡骑士";
            break;
                
            case CLASS_SHAMAN: 
                Class_s = "萨满";       
            break;
                
            case CLASS_MAGE: 
                Class_s = "法师";          
            break;
                
            case CLASS_WARLOCK: 
                Class_s = "术士";       
            break;
                
            case CLASS_DRUID:          
                Class_s = "德鲁伊";          
            break;
        }
        return Class_s;
    }
        
    std::string getWinPercent(uint32 wins, uint32 losses)
    {
        uint32 totalGames = wins + losses;
        if (totalGames == 0)
            return "0%";
            
        std::stringstream buf;
        uint32 percentage = (wins * 100) / totalGames;
        buf << percentage << "%";
        return buf.str();
    }

    public:
        arena_top_teams() : CreatureScript("arena_top_teams"){}
        
    bool OnGossipHello(Player *player, Creature *creature)
    {
        player->ADD_GOSSIP_ITEM(4,"|cff00ff00|TInterface\\icons\\spell_chargepositive:26|t|r 竞技场排名?", GOSSIP_SENDER_MAIN, ARENA_HELP);
        player->ADD_GOSSIP_ITEM(4,"|cff00ff00|TInterface\\icons\\Achievement_Arena_2v2_7:26|t|r 竞技场2v2", GOSSIP_SENDER_MAIN, ARENA_2V2_LADDER);
        player->ADD_GOSSIP_ITEM(4,"|cff00ff00|TInterface\\icons\\Achievement_Arena_3v3_7:26|t|r 竞技场3v3", GOSSIP_SENDER_MAIN, ARENA_3V3_LADDER);
        player->ADD_GOSSIP_ITEM(4,"|cff00ff00|TInterface\\icons\\Achievement_Arena_5v5_7:26|t|r 竞技场1v1", GOSSIP_SENDER_MAIN, ARENA_5V5_LADDER);
        player->ADD_GOSSIP_ITEM(4,"|cff00ff00|TInterface\\icons\\spell_chargenegative:26|t|r 返回", GOSSIP_SENDER_MAIN, ARENA_GOODBYE);
        player->SEND_GOSSIP_MENU(90085, creature->GetGUID());
        return true;
    }
        
    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction) 
    {
        player->PlayerTalkClass->ClearMenus();
        switch(uiAction) 
        {
            case ARENA_GOODBYE:
            {
                player->PlayerTalkClass->SendCloseGossip();
                break;
            }

            case ARENA_HELP:
            {
				ChatHandler(player->GetSession()).PSendSysMessage("|cffff6060[Information]:|r 这个NPC显示服务器的角斗士 (Top 10).简单的点击团队名称和看到团队细节");
																		  
				break;

			}

			case ARENA_2V2_LADDER:
			case ARENA_5V5_LADDER:
			case ARENA_3V3_LADDER:
            {
                uint32 teamType = optionToTeamType(uiAction);
                QueryResult result = CharacterDatabase.PQuery("SELECT arenaTeamId, name, rating FROM arena_team WHERE type = '%u' ORDER BY rating DESC LIMIT %u;", teamType, ARENA_MAX_RESULTS);
                    
                if(!result) 
                {
                    player->ADD_GOSSIP_ITEM(7, "返回", GOSSIP_SENDER_MAIN, ARENA_GOODBYE);
                    player->SEND_GOSSIP_MENU(1, creature->GetGUID());
                } 
                else
                {
                    std::string name;
                    uint32 teamId, rating, rank = 1;
                    player->ADD_GOSSIP_ITEM(0,"顶级竞技场名单 - 角斗士:", GOSSIP_SENDER_MAIN, ARENA_GOODBYE);
                    do 
                    {
                        Field *fields = result->Fetch();
                        teamId = fields[0].GetUInt32();
                        name = fields[1].GetString();
                        rating = fields[2].GetUInt32();
                            
                        std::stringstream buffer;
                        buffer << rank << ". " << name;
                        buffer << ": " << "|cFF1E90FF" << rating << "|r" << " 评级!";
                        player->ADD_GOSSIP_ITEM(4, buffer.str(), GOSSIP_SENDER_MAIN, ARENA_START_TEAM_LOOKUP + teamId);
                            
                        rank++;
                    } 
                    while(result->NextRow());
                    player->ADD_GOSSIP_ITEM(7, "返回", GOSSIP_SENDER_MAIN, ARENA_GOODBYE);
                    player->SEND_GOSSIP_MENU(90086, creature->GetGUID());
                }
                break;
            }
            
            default:
            {
                if (uiAction > ARENA_START_TEAM_LOOKUP) 
                {
                    uint32 teamId = uiAction - ARENA_START_TEAM_LOOKUP;
					QueryResult result = CharacterDatabase.PQuery("SELECT name, rating, seasonWins, seasonGames - seasonWins, weekWins, weekGames - weekWins, rank, captainGuid , type FROM arena_team WHERE arenaTeamId = '%u'", teamId);
                        
                    if(!result) 
                    {
                        player->GetSession()->SendNotification("竞技场队未找到...");
                        player->PlayerTalkClass->SendCloseGossip();
                        return true;
                    }
                        
                    Field *fields = result->Fetch();
                    std::string name = fields[0].GetString();
                    uint32 rating = fields[1].GetUInt32();
                    uint32 seasonWins = fields[2].GetUInt32();
                    uint32 seasonLosses = fields[3].GetUInt32();
                    uint32 weekWins = fields[4].GetUInt32();
                    uint32 weekLosses = fields[5].GetUInt32();
                    uint32 rank = fields[6].GetUInt32();
                    uint32 captainGuid = fields[7].GetUInt32();
                    uint32 type = fields[8].GetUInt32();
                    uint32 parentOption = teamTypeToOption(type);
                        
                    std::string seasonWinPercentage = getWinPercent(seasonWins, seasonLosses);
                    std::string weekWinPercentage = getWinPercent(weekWins, weekLosses);
                        
                    std::stringstream buf;
                    buf << "战队名字: " << "|cFF1E90FF" << name << "|r";
                    player->ADD_GOSSIP_ITEM(7, buf.str(), GOSSIP_SENDER_MAIN, parentOption);
                    buf.str("");
                    
                    buf << "等级: " << "|cFF1E90FF" << rating << "|r" << " (排名: " << "|cFF1E90FF" << rank << "|r" << ", Type: " << "|cFF1E90FF" << type << "v" << type << "|r"")";
                    player->ADD_GOSSIP_ITEM(4, buf.str(), GOSSIP_SENDER_MAIN, parentOption);
                    buf.str("");
                    
                    buf << "本周: " << "|cFF1E90FF" << weekWins << "-" << weekLosses << "|r"" (" << "|cFF1E90FF" << weekWinPercentage << "|r" << "win)"; 
                    player->ADD_GOSSIP_ITEM(4, buf.str(), GOSSIP_SENDER_MAIN, parentOption);
                    buf.str("");
                    
                    buf << "全赛季: " << "|cFF1E90FF" << seasonWins << "-" << seasonLosses << "|r" << " (" << "|cFF1E90FF" << seasonWinPercentage << "|r" << " win)"; 
                    player->ADD_GOSSIP_ITEM(4, buf.str(), GOSSIP_SENDER_MAIN, parentOption);
                        
                    QueryResult members = CharacterDatabase.PQuery("SELECT  a.guid, a.personalRating, a.weekWins, a.weekGames - a.weekWins, a.seasonWins, a.seasonGames - a.seasonWins, c.name, c.race, c.class, c.level FROM arena_team_member a LEFT JOIN characters c ON c.guid = a.guid WHERE arenaTeamId = '%u' ORDER BY a.guid = '%u' DESC, a.seasonGames DESC, c.name ASC", teamId, captainGuid);
                    if(!members) 
                    {
                        player->ADD_GOSSIP_ITEM(7, "没有发现团队成员…？", GOSSIP_SENDER_MAIN, parentOption);
                    } 
                    else 
                    {
                        uint32 memberPos = 1;
                        uint32 memberCount = members->GetRowCount();
                        uint32 guid, personalRating, level;
                        std::string name, race, Class;
                            
                        buf.str("");
                        buf << "      --- " << memberCount << " 团队" << ((memberCount == 1) ? " 成员" : " 成员") << " 发现" << " ---";
                        player->ADD_GOSSIP_ITEM(0, buf.str(), GOSSIP_SENDER_MAIN, parentOption);
                        do 
                        {
                            fields = members->Fetch();
                            guid = fields[0].GetUInt32();
                            personalRating = fields[1].GetUInt32();
                            weekWins= fields[2].GetUInt32();
                            weekLosses = fields[3].GetUInt32();
                            seasonWins = fields[4].GetUInt32();
                            seasonLosses = fields[5].GetUInt32();
                            name = fields[6].GetString();
                            race = raceToString(fields[7].GetUInt8());
                            Class = classToString(fields[8].GetUInt8());
                            level = fields[9].GetUInt32();
                                
                            seasonWinPercentage = getWinPercent(seasonWins, seasonLosses);
                            weekWinPercentage = getWinPercent(weekWins, weekLosses);
                                
                            buf.str(""); 
                            buf << memberPos << ". "; 
                            if (guid == captainGuid) 
                                buf <<  "队 队长 ";
                                
                            
                            buf << race << " " << Class << ", " << "|cFF1E90FF" << personalRating << "|r" << " 个人等级!";
                            player->ADD_GOSSIP_ITEM(4, buf.str(), GOSSIP_SENDER_MAIN, parentOption);
                            buf.str("");
                            
                            buf << "胜利: " << "|cFF1E90FF" << weekWins << "-" << weekLosses << "|r" << " (" << "|cFF1E90FF" << weekWinPercentage << "|r" << " 胜利), " << "|cFF1E90FF" << (weekWins + weekLosses) << "|r" << " played!"; 
                            player->ADD_GOSSIP_ITEM(4, buf.str(), GOSSIP_SENDER_MAIN, parentOption);
                            buf.str("");
                            
                            buf << "赛季: " << "|cFF1E90FF" << seasonWins << "-" << seasonLosses << "|r" << " (" << "|cFF1E90FF" << seasonWinPercentage << "|r" << " 胜利), " << "|cFF1E90FF" << (seasonWins + seasonLosses) << "|r" << " played!"; 
                            player->ADD_GOSSIP_ITEM(4, buf.str(), GOSSIP_SENDER_MAIN, parentOption);
                            memberPos++;
                        } 
                        while(members->NextRow());
                    }
                    player->SEND_GOSSIP_MENU(90087, creature->GetGUID());
                }
            }
        }
        return true;
    }
};

void AddSC_arena_top_teams()
{
    new arena_top_teams();
}
