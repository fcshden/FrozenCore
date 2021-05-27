struct QuickResponseTemplate
{
	uint32 rewId;
	std::string correctAnswer;
	std::string question;
	std::string answerA;
	std::string answerB;
	std::string answerC;
	std::string answerD;
};

extern std::vector<QuickResponseTemplate> QuickResponseVec;

extern std::vector<uint32/*guid*/> QuickResponsePlayerVec;

extern std::vector<uint32 /*guid*/> CorrectResponsePlayerVec;

class QuickResponse
{
public:
	static QuickResponse* instance()
	{
		static QuickResponse instance;
		return &instance;
	}
	void Load();
	void Anounce();
	bool GetAnswerFlag()
	{
		return _answereFlag;
	}

	void SetAnswerFlag(bool flag)
	{
		_answereFlag = flag;
	}

	void SetAnswer(std::string answer)
	{
		_answer = answer;
	}

	std::string GetAnswer()
	{
		return _answer;
	}

	void SetRewId(uint32 rewId)
	{
		_rewId = rewId;
	}

	uint32 GetRewId()
	{
		return _rewId;
	}

	void Reward();

private:
	std::string _answer;
	bool _answereFlag;
	uint32 _rewId;
};
#define sQuickResponse QuickResponse::instance()