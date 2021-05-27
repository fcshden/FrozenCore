#pragma execution_character_set("utf-8")
#include "QuickResponse.h"
#include "../CommonFunc/CommonFunc.h"
#include "../Reward/Reward.h"
#include "../Switch/Switch.h"
#include "../CustomEvent/Event.h"

std::vector<QuickResponseTemplate> QuickResponseVec;
std::vector<uint32/*guid*/> QuickResponsePlayerVec;
std::vector<uint32 /*guid*/> CorrectResponsePlayerVec;


void QuickResponse::Load()
{
	QuickResponseVec.clear();
	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 奖励模板ID,问题文本,正确选项,选项A,选项B,选项C,选项D FROM __抢答" :
		"SELECT rewId,question,correctAnswer,answerA,answerB,answerC,answerD FROM _quick_response");
	if (!result) return;
	do
	{
		Field* fields = result->Fetch();
		QuickResponseTemplate Temp;
		Temp.rewId			= fields[0].GetUInt32();
		Temp.question		= fields[1].GetString();
		Temp.correctAnswer	= fields[2].GetString();
		Temp.answerA		= fields[3].GetString();
		Temp.answerB		= fields[4].GetString();
		Temp.answerC		= fields[5].GetString();
		Temp.answerD		= fields[6].GetString();
		QuickResponseVec.push_back(Temp);
	} while (result->NextRow());

	//初始化
	SetAnswerFlag(true);
	SetAnswer("A");
	SetRewId(0);
}

void QuickResponse::Anounce()
{
	uint32 len = QuickResponseVec.size();
	uint32 urand_i = urand(0, len -1);

	for (size_t i = 0; i < len; i++)
	{
		if (i == urand_i)
		{
			QuickResponsePlayerVec.clear();
			CorrectResponsePlayerVec.clear();

			SetAnswerFlag(false);
			SetAnswer(QuickResponseVec[i].correctAnswer);
			SetRewId(QuickResponseVec[i].rewId);

			std::ostringstream ossQuestion;
			ossQuestion << "|cFFFF1717[抢答]|r " << QuickResponseVec[i].question;
			sWorld->SendServerMessage(SERVER_MSG_STRING, ossQuestion.str().c_str());

			if (!QuickResponseVec[i].answerA.empty())
			{
				std::ostringstream oss;
				oss << "|cFFFF1717[抢答]|r " << "[ A ] " << QuickResponseVec[i].answerA;
				sWorld->SendServerMessage(SERVER_MSG_STRING, oss.str().c_str());
			}

			if (!QuickResponseVec[i].answerB.empty())
			{
				std::ostringstream oss;
				oss << "|cFFFF1717[抢答]|r " << "[ B ] " << QuickResponseVec[i].answerB;
				sWorld->SendServerMessage(SERVER_MSG_STRING, oss.str().c_str());
			}

			if (!QuickResponseVec[i].answerC.empty())
			{
				std::ostringstream oss;
				oss << "|cFFFF1717[抢答]|r " << "[ C ] " << QuickResponseVec[i].answerC;
				sWorld->SendServerMessage(SERVER_MSG_STRING, oss.str().c_str());
			}

			if (!QuickResponseVec[i].answerD.empty())
			{
				std::ostringstream oss;
				oss << "|cFFFF1717[抢答]|r " << "[ D ] " << QuickResponseVec[i].answerD;
				sWorld->SendServerMessage(SERVER_MSG_STRING, oss.str().c_str());
			}
			break;
		}
	}
}

void QuickResponse::Reward()
{
	if (CorrectResponsePlayerVec.empty())
		return;

	sQuickResponse->SetAnswerFlag(true);

	for (std::vector<uint32>::iterator iter = CorrectResponsePlayerVec.begin(); iter != CorrectResponsePlayerVec.end(); ++iter)
	{
		Player* pl = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(*iter, 0, HIGHGUID_PLAYER));

		if (pl)
		{
			sRew->Rew(pl, GetRewId());
			std::ostringstream oss;
			std::string nameLink, nameLinkWithColor;
			sCF->GetNameLink(pl, nameLink, nameLinkWithColor);
			oss << "|cFFFF1717[抢答]|r " << nameLinkWithColor << "抢答正确,真是才高八斗！";
			sWorld->SendServerMessage(SERVER_MSG_STRING, oss.str().c_str());
		}

	}

	CorrectResponsePlayerVec.clear();
}

class QuickResponseScript : public CommandScript
{
public:
	QuickResponseScript() : CommandScript("QuickResponseScript") { }

	static bool HandleQuickResponseCommand(ChatHandler * pChat, const char * msg)
	{
		if (!*msg)
			return true;

		Player * player = pChat->GetSession()->GetPlayer();

		if (!player)
			return true;
		
		if (!sGameEventMgr->IsActiveEvent(EVENT_ID_QUICK_RESPONSE) || sQuickResponse->GetAnswerFlag())
		{
			player->GetSession()->SendAreaTriggerMessage("|cFFFF1717[抢答]|r 已结束！");
			return true;
		}

		if (std::find(QuickResponsePlayerVec.begin(), QuickResponsePlayerVec.end(), player->GetGUIDLow()) != QuickResponsePlayerVec.end())
		{
			player->GetSession()->SendAreaTriggerMessage("|cFFFF1717[抢答]|r 禁止重复答题！");
			return true;
		}
		
		std::string answer = msg;

		std::transform(answer.begin(), answer.end(), answer.begin(), ::toupper);

		if (strcmp(answer.c_str(), "A") != 0 && strcmp(answer.c_str(), "B") != 0 && strcmp(answer.c_str(), "C") != 0 && strcmp(answer.c_str(), "D") != 0)
		{
			ChatHandler(player->GetSession()).PSendSysMessage("|cFFFF1717[抢答]|r 正确答题格式为：.qd A 或 .qd a");
			return true;
		}

		QuickResponsePlayerVec.push_back(player->GetGUIDLow());

		std::string correctAnswer = sQuickResponse->GetAnswer();

		if (strcmp(correctAnswer.c_str(), answer.c_str()) == 0)
		{
			CorrectResponsePlayerVec.push_back(player->GetGUIDLow());

			if (CorrectResponsePlayerVec.size() >= sSwitch->GetValue(ST_QUICK_RESPONSE))
				sGameEventMgr->StopEvent(EVENT_ID_QUICK_RESPONSE, false, false);
		}

		return true;
	}

    std::vector<ChatCommand> GetCommands() const
	{
		static std::vector<ChatCommand> QuickResponseCommandTable =
		{
			{ "qd", SEC_PLAYER, true, &HandleQuickResponseCommand, "" }
		};

		return QuickResponseCommandTable;
	}
};

void AddSC_QuickResponse()
{
	new QuickResponseScript();
}
