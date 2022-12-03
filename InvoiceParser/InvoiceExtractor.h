#pragma once
#include <podofo.h>

#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <map>

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/pointer.h"

using namespace PoDoFo;
using namespace rapidjson;
using std::vector;
using std::string;
using std::wstring;
using std::stack;
using std::map;
using std::multimap;
using std::pair;
using std::make_pair;

void ExtractText(PdfMemDocument* pDocument, PdfPage* pPage);
void InitInvoiceCoordinates_1();
void InitInvoiceCoordinates_2();
string GetDataString(PdfString pdfstr, PdfFont* pFont);
string WstringToString(wstring wstr);
void EraseQuestionMark(string& str);

class CInvoiceData
{
public:
	vector<map<string, string>> vecCommodityData; // 包含商品信息 如 "spmc"  -  "印刷品*一日一练口算题卡:一年级"
	map<string, string> mapData;

	void AppendAreaData(multimap<string, pair<double, string>>& mapAreaData);
	void AppendCommodityData(multimap<pair<string, double>, string>& mapCommodityData);
	void HandleCorpInfo(vector<string>& vecTitleName, map<string, string>& map, multimap<double, string, std::greater<double>>& mapSortData);
	inline bool IsIdentifyNumber(string data);
	bool IsBankName(string data);
	void CreateJson(string FilePath);
	void clear();
};