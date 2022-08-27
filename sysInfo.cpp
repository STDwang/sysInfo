#include <iostream>
#include <ctime>
#include <time.h> 
#include <Windows.h>
#include <atlstr.h>
#include <string>
#include <math.h>
#pragma warning(disable:4996)
#include<regex>
#pragma comment(lib,"Urlmon.lib")
using namespace std;
//系统语言
namespace
{
	struct countryLang
	{
		LANGID langId;//
		char country[16];//国际代码缩写
		char lang[32];//
		char userLang[32];//
	};

	const countryLang COUNTRY_LANG[] =
	{
		{0x0804, "CN", "zh", "zh_CN"},//中国
		{0x0409, "US", "en", "en_US"},//美国
		{0x0412, "KR", "ko", "ko_KR"},//韩国
		{0x0411, "JP", "ja", "ja_JP"},//日本
		{0x0421, "ID", "en", "en_ID"},//印度尼西亚
		{0x0407, "DE", "de", "de_DE"},//德国
		{0x040c, "FR", "fr", "fr_Fr"},//法国
		{0x0410, "IT", "it", "it_IT"},//意大利
		{0x0416, "BR", "pt", "pt_BR"},//葡萄牙
		{0x0c0a, "ES", "es", "es_Es"},//西班牙
		{0x0419, "RU", "ru", "ru_RU"},//俄国
		{0x041f, "TR", "tr", "tr_TR"},//土耳其
		{0x042a, "VI", "vi", "vi_VI"},//越南
		{0x041e, "TH", "en", "en_TH"},//泰国
		{0x0809, "GB", "en", "en_US"} //英国
	};
}

string UTF8ToGB(const char* str)
{
	string result;
	WCHAR* strSrc;
	LPSTR szRes;

	//获得临时变量的大小
	int i = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	strSrc = new WCHAR[i + 1];
	MultiByteToWideChar(CP_UTF8, 0, str, -1, strSrc, i);

	//获得临时变量的大小
	i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
	szRes = new CHAR[i + 1];
	WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, NULL, NULL);

	result = szRes;
	delete[]strSrc;
	delete[]szRes;

	return result;
}

bool GetDefaultLang(string& country, string& lang, string& userLang)
{
	bool bFind = false;
	LANGID lang_id = GetSystemDefaultLangID();
	//LANGID lang_id = GetUserDefaultUILanguage();//"win10>设置>语言>Windows显示语言"
	int size = sizeof(COUNTRY_LANG) / sizeof(COUNTRY_LANG[0]);

	char _country[16] = { 0 };//国际代码缩写
	strcpy_s(_country, "US");
	char _lang[32] = { 0 };//系统语言
	strcpy_s(_lang, "en");
	char _userlang[32] = { 0 };//用户语言
	strcpy_s(_userlang, "en_US");

	for (int i = 0; i < size; i++)
	{
		if (lang_id == COUNTRY_LANG[i].langId)
		{
			memset(_country, 0, sizeof(_country));
			memset(_lang, 0, sizeof(_lang));
			memset(_userlang, 0, sizeof(_userlang));
			strcpy(_country, COUNTRY_LANG[i].country);
			strcpy(_lang, COUNTRY_LANG[i].lang);
			strcpy(_userlang, COUNTRY_LANG[i].userLang);
			bFind = true;
			break;
		}
	}
	country = std::string(_country);
	lang = std::string(_lang);
	userLang = std::string(_userlang);
	return bFind;
}

string GetTimeZone() {
	time_t now = time(0);
	char* local = ctime(&now);
	cout << "local time: " << local; //本地 时间

	//转换UTC时间
	tm* gm = gmtime(&now);
	char* utc = asctime(gm);
	cout << "UTC time:" << utc;   //UTC 时间 

	int timeDistance = (now - mktime(gm)) / 3600;
	string zone;
	if (timeDistance > 0)				zone = "东";
	else if (timeDistance < 0)			zone = "西";
	else								zone = "东西";
	string zoneNumber;
	if (timeDistance == 0)				zoneNumber = "十二区";
	else if (abs(timeDistance) == 1)	zoneNumber = "一区";
	else if (abs(timeDistance) == 2)	zoneNumber = "二区";
	else if (abs(timeDistance) == 3)	zoneNumber = "三区";
	else if (abs(timeDistance) == 4)	zoneNumber = "四区";
	else if (abs(timeDistance) == 5)	zoneNumber = "五区";
	else if (abs(timeDistance) == 6)	zoneNumber = "六区";
	else if (abs(timeDistance) == 7)	zoneNumber = "七区";
	else if (abs(timeDistance) == 8)	zoneNumber = "八区";
	else if (abs(timeDistance) == 9)	zoneNumber = "九区";
	else if (abs(timeDistance) == 10)	zoneNumber = "十区";
	else if (abs(timeDistance) == 11)	zoneNumber = "十一区";

	return UTF8ToGB((zone + zoneNumber).c_str());
}

string getIP(const string& url) {
	HRESULT ret = URLDownloadToFileA(NULL, url.c_str(), ".\\tmp.txt", 0, NULL); //下载网页到tmp.txt文件中
	if (ret != S_OK) { //如果下载失败返回NULL
		return "";
	}
	//下载成功,读取文本内容
	FILE* file;
	errno_t err = fopen_s(&file, ".\\tmp.txt", "r");
	if (err != 0) {
		return "";
	}
	fseek(file, 0, SEEK_END);
	int nSize = ftell(file);
	fseek(file, 0, SEEK_SET);
	std::string buf;
	buf.resize(nSize + 1);
	fread((char*)buf.c_str(), sizeof(char), nSize, file);
	fclose(file);

	buf = buf.substr(nSize / 2);
	int begin, end;

	string ip = buf.substr(buf.find("AiWenIpData"));
	begin = ip.find(">") + 1;;
	end = ip.find("<");
	ip = ip.substr(begin, end - begin);

	string location = buf.substr(buf.find("Whwtdhalf w45-0 lh45"));
	begin = location.find("<em>") + 4;
	end = location.find("</em>");
	location = location.substr(begin, end - begin);

	DeleteFileA(".\\tmp.txt"); //匹配完成,可以删除下载的文件了
	return ip + " " + UTF8ToGB(location.c_str());
}

int main()
{
	std::string country, lang, userLang;
	GetDefaultLang(country, lang, userLang);
	cout << "sys lang:" << lang << endl;
	cout << "user Lang:" << userLang << endl;

	cout << "time zone:" << GetTimeZone() << endl;

	cout << getIP("https://ip.chinaz.com/") << endl;

	getchar();

	return 0;
}
