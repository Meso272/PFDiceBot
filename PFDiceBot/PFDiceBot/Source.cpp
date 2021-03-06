/*
* CoolQ Demo for VC++
* Api Version 9
* Written by Coxxs & Thanks for the help of orzFly
*/

#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <sstream>
#include <iostream>
#include <ctime>
#include "cqp.h"
#include "appmain.h" //应用AppID等信息，请正确填写，否则酷Q可能无法加载

using namespace std;

int ac = -1; //AuthCode 调用酷Q的方法时需要用到
bool enabled = false;

char HELP[]= ".r {表达式}\n 例如：.r 3d6";

/*
* 返回应用的ApiVer、Appid，打包后将不会调用
*/
CQEVENT(const char*, AppInfo, 0)() {
	return CQAPPINFO;
}


/*
* 接收应用AuthCode，酷Q读取应用信息后，如果接受该应用，将会调用这个函数并传递AuthCode。
* 不要在本函数处理其他任何代码，以免发生异常情况。如需执行初始化代码请在Startup事件中执行（Type=1001）。
*/
CQEVENT(int32_t, Initialize, 4)(int32_t AuthCode) {
	ac = AuthCode;
	return 0;
}


/*
* Type=1001 酷Q启动
* 无论本应用是否被启用，本函数都会在酷Q启动后执行一次，请在这里执行应用初始化代码。
* 如非必要，不建议在这里加载窗口。（可以添加菜单，让用户手动打开窗口）
*/
CQEVENT(int32_t, __eventStartup, 0)() {
	srand((unsigned)time(NULL));
	return 0;
}


/*
* Type=1002 酷Q退出
* 无论本应用是否被启用，本函数都会在酷Q退出前执行一次，请在这里执行插件关闭代码。
* 本函数调用完毕后，酷Q将很快关闭，请不要再通过线程等方式执行其他代码。
*/
CQEVENT(int32_t, __eventExit, 0)() {

	return 0;
}

/*
* Type=1003 应用已被启用
* 当应用被启用后，将收到此事件。
* 如果酷Q载入时应用已被启用，则在_eventStartup(Type=1001,酷Q启动)被调用后，本函数也将被调用一次。
* 如非必要，不建议在这里加载窗口。（可以添加菜单，让用户手动打开窗口）
*/
CQEVENT(int32_t, __eventEnable, 0)() {
	enabled = true;
	return 0;
}


/*
* Type=1004 应用将被停用
* 当应用被停用前，将收到此事件。
* 如果酷Q载入时应用已被停用，则本函数*不会*被调用。
* 无论本应用是否被启用，酷Q关闭前本函数都*不会*被调用。
*/
CQEVENT(int32_t, __eventDisable, 0)() {
	enabled = false;
	return 0;
}

template<typename T> string toString(const T& t) {
	ostringstream oss;  //创建一个格式化输出流
	oss << t;             //把值传递如流中
	return oss.str();
}

int toInt(const string &str) {
	stringstream ss(str);
	int n;
	ss >> n;
	return n;
}

int myrandom(int max) {
	return max-(1 + rand() % max)+1;
}

vector<string> getSplit(const char *msg, const char * deli) {
	CQ_addLog(ac, CQLOG_DEBUG, "split_msg", msg);
	char *msgc = new char[strlen(msg)];
	strcpy(msgc, msg + 1);
	vector<string> resultVec;
	char* context;
	char* tmpStr = strtok_s(msgc, deli, &context);
	while (tmpStr != NULL) {
		resultVec.push_back(string(tmpStr));
		tmpStr = strtok_s(NULL, deli, &context);
	}
	delete[] msgc;
	return resultVec;
}

/*
* CalRoll 计算骰子
* str 格式：ndm
*/
string CalRoll(string str) {
	size_t pos = str.find("d");
	if (pos == str.npos) return "";
	int n = 0, m = 0;
	string result = "";
	stringstream calexp(str);
	char op;
	if (pos == 0) {
		n = 1;
	} else {
		calexp >> n;
	}
	if (!(calexp >> op)) return "";
	if (op != 'd') return "";
	calexp >> m;
	int temp = n;
	if (temp != 0 && m != 0) {
		int cal = 0;
		int dice = myrandom(m);
		cal += dice;
		result += toString(dice);
		n--;
		while (n--) {
			dice = myrandom(m);
			cal += dice;
			result += " + " + toString(dice);
		}
		if (n != 1) {
			result += " = " + toString(cal);
		}
	}
	return result;
}

/*
* Type=21 私聊消息
* subType 子类型，11/来自好友 1/来自在线状态 2/来自群 3/来自讨论组
*/
CQEVENT(int32_t, __eventPrivateMsg, 24)(int32_t subType, int32_t msgId, int64_t fromQQ, const char *msg, int32_t font) {

	//如果要回复消息，请调用酷Q方法发送，并且这里 return EVENT_BLOCK - 截断本条消息，不再继续处理  注意：应用优先级设置为"最高"(10000)时，不得使用本返回值
	//如果不回复消息，交由之后的应用/过滤器处理，这里 return EVENT_IGNORE - 忽略本条消息
	if (msg[0] == '.') {
		if (strlen(msg) > 1) {
			vector<string> resultVec = getSplit(msg, " ");
			if (resultVec[0] == "r") {
				string res = CalRoll(resultVec[1]);
				CQ_sendPrivateMsg(ac, fromQQ, res.c_str());
			} else if (resultVec[0] == "help" || resultVec[0] == "h") {
				CQ_sendPrivateMsg(ac, fromQQ, HELP);
			}
		}
	}
	return EVENT_IGNORE;
}


/*
* Type=2 群消息
*/
CQEVENT(int32_t, __eventGroupMsg, 36)(int32_t subType, int32_t msgId, int64_t fromGroup, int64_t fromQQ, const char *fromAnonymous, const char *msg, int32_t font) {
	if (msg[0] == '.') {
		if (strlen(msg) > 1) {
			vector<string> resultVec = getSplit(msg, " ");
			if (resultVec[0] == "r") {
				string res = CalRoll(resultVec[1]);
				CQ_sendGroupMsg(ac, fromGroup, res.c_str());
			} else if (resultVec[0] == "help" || resultVec[0] == "h") {
				CQ_sendGroupMsg(ac, fromGroup, HELP);
			}
		}
	}
	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=4 讨论组消息
*/
CQEVENT(int32_t, __eventDiscussMsg, 32)(int32_t subType, int32_t msgId, int64_t fromDiscuss, int64_t fromQQ, const char *msg, int32_t font) {
	if (msg[0] == '.') {
		if (strlen(msg) > 1) {
			vector<string> resultVec = getSplit(msg, " ");
			if (resultVec[0] == "r") {
				string res = CalRoll(resultVec[1]);
				CQ_sendDiscussMsg(ac, fromDiscuss, res.c_str());
			} else if (resultVec[0] == "help" || resultVec[0] == "h") {
				CQ_sendDiscussMsg(ac, fromDiscuss, HELP);
			}
		}
	}
	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}

/*
* Type=301 请求-好友添加
* msg 附言
* responseFlag 反馈标识(处理请求用)
*/
CQEVENT(int32_t, __eventRequest_AddFriend, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, const char *responseFlag) {

	CQ_setFriendAddRequest(ac, responseFlag, REQUEST_ALLOW, "");

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}