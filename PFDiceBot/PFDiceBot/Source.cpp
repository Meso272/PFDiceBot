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
#include "appmain.h" //Ӧ��AppID����Ϣ������ȷ��д�������Q�����޷�����

using namespace std;

int ac = -1; //AuthCode ���ÿ�Q�ķ���ʱ��Ҫ�õ�
bool enabled = false;

std::random_device rd; //���
std::mt19937 e(rd()); //�������

/*
* ����Ӧ�õ�ApiVer��Appid������󽫲������
*/
CQEVENT(const char*, AppInfo, 0)() {
	return CQAPPINFO;
}


/*
* ����Ӧ��AuthCode����Q��ȡӦ����Ϣ��������ܸ�Ӧ�ã���������������������AuthCode��
* ��Ҫ�ڱ��������������κδ��룬���ⷢ���쳣���������ִ�г�ʼ����������Startup�¼���ִ�У�Type=1001����
*/
CQEVENT(int32_t, Initialize, 4)(int32_t AuthCode) {
	ac = AuthCode;
	return 0;
}


/*
* Type=1001 ��Q����
* ���۱�Ӧ���Ƿ����ã������������ڿ�Q������ִ��һ�Σ���������ִ��Ӧ�ó�ʼ�����롣
* ��Ǳ�Ҫ����������������ش��ڡ����������Ӳ˵������û��ֶ��򿪴��ڣ�
*/
CQEVENT(int32_t, __eventStartup, 0)() {
	srand((unsigned)time(NULL));
	return 0;
}


/*
* Type=1002 ��Q�˳�
* ���۱�Ӧ���Ƿ����ã������������ڿ�Q�˳�ǰִ��һ�Σ���������ִ�в���رմ��롣
* ������������Ϻ󣬿�Q���ܿ�رգ��벻Ҫ��ͨ���̵߳ȷ�ʽִ���������롣
*/
CQEVENT(int32_t, __eventExit, 0)() {

	return 0;
}

/*
* Type=1003 Ӧ���ѱ�����
* ��Ӧ�ñ����ú󣬽��յ����¼���
* �����Q����ʱӦ���ѱ����ã�����_eventStartup(Type=1001,��Q����)�����ú󣬱�����Ҳ��������һ�Ρ�
* ��Ǳ�Ҫ����������������ش��ڡ����������Ӳ˵������û��ֶ��򿪴��ڣ�
*/
CQEVENT(int32_t, __eventEnable, 0)() {
	enabled = true;
	return 0;
}


/*
* Type=1004 Ӧ�ý���ͣ��
* ��Ӧ�ñ�ͣ��ǰ�����յ����¼���
* �����Q����ʱӦ���ѱ�ͣ�ã��򱾺���*����*�����á�
* ���۱�Ӧ���Ƿ����ã���Q�ر�ǰ��������*����*�����á�
*/
CQEVENT(int32_t, __eventDisable, 0)() {
	enabled = false;
	return 0;
}

template<typename T> string toString(const T& t) {
	ostringstream oss;  //����һ����ʽ�������
	oss << t;             //��ֵ����������
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

/*
* CalRoll ��������
* str ��ʽ��ndm
*/
string CalRoll(string str) {
	size_t pos = str.find("d");
	string result = "";
	if (isalnum(str[0])) {
		int n = toInt(str.substr(0, pos).c_str());
		int m = toInt(str.substr(pos + 1, str.size()).c_str());
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
		result += " = " + toString(cal);
	}
	if (str[0] == 'd') {
		result += str.substr(1, str.size());
	}
	return result;
}


/*
* Type=21 ˽����Ϣ
* subType �����ͣ�11/���Ժ��� 1/��������״̬ 2/����Ⱥ 3/����������
*/
CQEVENT(int32_t, __eventPrivateMsg, 24)(int32_t subType, int32_t msgId, int64_t fromQQ, const char *msg, int32_t font) {

	//���Ҫ�ظ���Ϣ������ÿ�Q�������ͣ��������� return EVENT_BLOCK - �ضϱ�����Ϣ�����ټ�������  ע�⣺Ӧ�����ȼ�����Ϊ"���"(10000)ʱ������ʹ�ñ�����ֵ
	//������ظ���Ϣ������֮���Ӧ��/���������������� return EVENT_IGNORE - ���Ա�����Ϣ
	if (msg[0] == '.') {
		if (strlen(msg) > 1) {
			char *msgc = new char[strlen(msg)];
			strcpy(msgc, msg+1);
			vector<string> resultVec;
			char* context;
			char* tmpStr = strtok_s(msgc, " ", &context);
			while (tmpStr != NULL) {
				resultVec.push_back(string(tmpStr));
				tmpStr = strtok_s(NULL, " ", &context);
			}
			delete [] msgc;
			if (resultVec.size() > 1) {
				if (resultVec[0] == "r") {
					string res = CalRoll(resultVec[1]);
					CQ_sendPrivateMsg(ac, fromQQ, res.c_str());
				}
			}
		}
	}
	return EVENT_IGNORE;
}


/*
* Type=2 Ⱥ��Ϣ
*/
CQEVENT(int32_t, __eventGroupMsg, 36)(int32_t subType, int32_t msgId, int64_t fromGroup, int64_t fromQQ, const char *fromAnonymous, const char *msg, int32_t font) {
	if (msg[0] == '.') {
		if (strlen(msg) > 1) {
			char *msgc = new char[strlen(msg)];
			strcpy(msgc, msg + 1);
			vector<string> resultVec;
			char* context;
			char* tmpStr = strtok_s(msgc, " ", &context);
			while (tmpStr != NULL) {
				resultVec.push_back(string(tmpStr));
				tmpStr = strtok_s(NULL, " ", &context);
			}
			delete[] msgc;
			if (resultVec.size() > 1) {
				if (resultVec[0] == "r") {
					string res = CalRoll(resultVec[1]);
					CQ_sendGroupMsg(ac, fromGroup, res.c_str());
				}
			}
		}
	}
	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=4 ��������Ϣ
*/
CQEVENT(int32_t, __eventDiscussMsg, 32)(int32_t subType, int32_t msgId, int64_t fromDiscuss, int64_t fromQQ, const char *msg, int32_t font) {
	if (msg[0] == '.') {
		if (strlen(msg) > 1) {
			char *msgc = new char[strlen(msg)];
			strcpy(msgc, msg + 1);
			vector<string> resultVec;
			char* context;
			char* tmpStr = strtok_s(msgc, " ", &context);
			while (tmpStr != NULL) {
				resultVec.push_back(string(tmpStr));
				tmpStr = strtok_s(NULL, " ", &context);
			}
			delete[] msgc;
			if (resultVec.size() > 1) {
				if (resultVec[0] == "r") {
					string res = CalRoll(resultVec[1]);
					CQ_sendGroupMsg(ac, fromDiscuss, res.c_str());
				}
			}
		}
	}
	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}

/*
* Type=301 ����-��������
* msg ����
* responseFlag ������ʶ(����������)
*/
CQEVENT(int32_t, __eventRequest_AddFriend, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, const char *responseFlag) {

	CQ_setFriendAddRequest(ac, responseFlag, REQUEST_ALLOW, "");

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}