#ifndef EXCEPTION_INC
#define EXCEPTION_INC

#include <Windows.h>
#include <string>

#define exc_(msg, deepMsg) exc(msg, deepMsg, __LINE__, __FILE__)
#define deepExc_(msg) exc(msg, __LINE__, __FILE__)

const unsigned long DEAD = 0xDEADBEEF;
const unsigned long LIVE = 0xBEEFBEEF;

class exc : public std::exception
{
private:
	unsigned long startGuard_;
	std::string msg_;
	exc* const deepMsg_;
	int line_;
	std::string file_;
	bool top_;
	unsigned long endGuard_;

	bool ok();
	std::string getDeepMsg();

public:
	exc() = delete;
	exc(std::string msg, exc* deepMsg, int line, const char* file);
	exc(std::string msg, int line, const char* file);
	exc(const char* msg, exc* deepMsg, int line, const char* file);
	exc(const char* msg, int line, const char* file);
	~exc();

	virtual void what();
};

exc::exc(std::string msg, exc* deepMsg, int line, const char* file) :
	exc(msg.c_str(), deepMsg, line, file)
{}

exc::exc(std::string msg, int line, const char* file) :
	exc(msg.c_str(), line, file)
{}

exc::exc(const char* msg, exc* deepMsg, int line, const char* file) :
	startGuard_(LIVE),
	msg_(msg),
	deepMsg_(deepMsg),
	line_(line),
	file_(file),
	top_(true),
	endGuard_(LIVE)
{
	deepMsg_->top_ = false;
}

exc::exc(const char* msg, int line, const char* file) :
	startGuard_(LIVE),
	msg_(msg),
	deepMsg_(NULL),
	line_(line),
	file_(file),
	top_(true),
	endGuard_(LIVE)
{}

exc::~exc()
{
	if (top_ && ok())
	{
		if (deepMsg_)
		{
			deepMsg_->top_ = true;
			delete deepMsg_;
		}

		startGuard_ = DEAD;
		endGuard_ = DEAD;
	}
}

bool exc::ok()
{
	if (startGuard_ == LIVE && endGuard_ == LIVE) return true;
	else return false;
}

std::string exc::getDeepMsg()
{
	if (ok())
	{
		if (deepMsg_) return msg_ +
			"(line [" + std::to_string(line_) + "], " +
			"file [" + file_ + "]): "
			+ deepMsg_->getDeepMsg();
		else return msg_ +
			"(line [" + std::to_string(line_) + "], "
			"file [" + file_ + "]). Kill the program?";
	}

	return "";
}

void exc::what()
{
	if (ok() && top_)
	{
		std::string deepMsg = getDeepMsg();
		if (MessageBox(NULL, std::wstring(deepMsg.begin(), deepMsg.end()).c_str(),
			L"ERROR", MB_YESNO | MB_ICONEXCLAMATION) == IDYES) exit(1);
	}
}

#endif