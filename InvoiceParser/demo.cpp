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
using std::make_pair;

void ExtractText(PdfMemDocument* pDocument, PdfPage* pPage);
void InitInvoiceCoordinates_1();
string UnicodeToString(const string& UnicodeString);
string WstringToString(wstring wstr);
void HandleCommodityInformation(map<string, map<double, string>>& map);


Document document;

// 第一项保存区域坐标，第二项保存区域内的所有发票数据标题和数据坐标
map<vector<unsigned>, map<string, vector<unsigned>>> mapCoordinates;

int main(int argc, char* argv[])
{
	document.SetObject();
	Pointer("/code").Set(document, 0);
	Pointer("/message").Set(document, "成功");
	Pointer("/content/count").Set(document, 1);
	// 数组第0个索引的对象
	Pointer("/content/kpxx/0/xh").Set(document, 1);

	try {
		const char* filename = argv[1]; 

		try {
			PdfMemDocument document(filename);

			int nCount = document.GetPageCount();
			for (int i = 0; i < nCount; ++i)
			{
				PdfPage* pPage = document.GetPage(i);
				ExtractText(&document, pPage);
			}
		}
		catch (PdfError& e) {
			fprintf(stderr, "Error: An error %i ocurred during processing the pdf file.\n", e.GetError());
			e.PrintErrorMsg();
			return e.GetError();
		}
	}
	catch (PdfError& e) {
		std::cerr << "Error: An error " << e.GetError() << " ocurred." << std::endl;
		e.PrintErrorMsg();
		return e.GetError();
	}

	StringBuffer sb;
	PrettyWriter<StringBuffer> writer(sb);
	document.Accept(writer);
	puts(sb.GetString());

	FILE* fp = NULL;
	fopen_s(&fp, "output.json", "wb");
	char writeBuffer[1024];
	memset(writeBuffer, '\0', sizeof(writeBuffer));
	FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
	Writer<FileWriteStream> file_writer(os);
	document.Accept(file_writer);
	if (NULL != fp)
		fclose(fp);
	system("pause");
	return 0;
}

void ExtractText(PdfMemDocument* pDocument, PdfPage* pPage)
{
	PdfVariant       var;
	EPdfContentsType eType;
	PdfContentsTokenizer tokenizer(pPage);
	const char* pszToken = NULL;

	PdfFont* pCurFont = NULL;
	stack<PdfVariant> stack;
	map<string, map<double, string>> mapCommodityInfo; // 8 element: map Y Pos - info
	unsigned DateIndex = 0;
	bool bTextBlock = false;
	bool IsCommodity = false;
	double dCurPosY = 0;
	double dCurPosX = 0;

	setlocale(LC_ALL, "");
	mapCoordinates.clear();
	InitInvoiceCoordinates_1();

	while (tokenizer.ReadNext(eType, pszToken, var))
	{
		if (eType == ePdfContentsType_Keyword)
		{
			if (strcmp(pszToken, "BT") == 0)
			{
				dCurPosY = 0;
				dCurPosX = 0;
				bTextBlock = true;
			}
			else if (strcmp(pszToken, "ET") == 0)
			{
				if (!bTextBlock)
					fprintf(stderr, "WARNING: Found ET without BT!\n");
			}

			if (bTextBlock)
			{
				if (strcmp(pszToken, "Tf") == 0)
				{
					if (stack.size() < 2)
					{
						fprintf(stderr, "WARNING: Expects two arguments for 'Tf', ignoring\n");
						pCurFont = NULL;
						continue;
					}

					stack.pop();
					PdfName fontName = stack.top().GetName();
					PdfObject* pFont = pPage->GetFromResources(PdfName("Font"), fontName);
					if (!pFont)
					{
						PODOFO_RAISE_ERROR_INFO(ePdfError_InvalidHandle, "Cannot create font!");
					}

					pCurFont = pDocument->GetFont(pFont);
					if (!pCurFont)
					{
						fprintf(stderr, "WARNING: Unable to create font for object %" PDF_FORMAT_INT64 " %" PDF_FORMAT_INT64 " R\n",
							static_cast<pdf_int64>(pFont->Reference().ObjectNumber()),
							static_cast<pdf_int64>(pFont->Reference().GenerationNumber()));
					}
				}
				else if (strcmp(pszToken, "Td") == 0)
				{
					if (stack.size() < 2)
					{
						fprintf(stderr, "WARNING: Expects two argument for 'Td', ignoring\n");
						continue;
					}

					dCurPosY = stack.top().GetReal();
					stack.pop();
					dCurPosX = stack.top().GetReal();
					stack.pop();
				}
				else if (strcmp(pszToken, "Tm") == 0)
				{
					if (stack.size() < 6)
					{
						fprintf(stderr, "WARNING: Expects six argument for 'Tm', ignoring\n");
						continue;
					}

					dCurPosY = stack.top().GetReal();
					stack.pop();
					dCurPosX = stack.top().GetReal();
					stack.pop();
				}
				else if (strcmp(pszToken, "Tj") == 0 ||
						 strcmp(pszToken, "'") == 0)
				{
					if (stack.size() < 1)
					{
						fprintf(stderr, "WARNING: Expects one argument for '%s', ignoring\n", pszToken);
						continue;
					}

					string prefix = "/content/kpxx/0/";
					PdfString pdfstr = stack.top().GetString();
					stack.pop();

					if (!pdfstr.IsHex()) // 排除发票数据的标题
					{
						if (pCurFont)
						{
							const PdfEncoding* encoding = pCurFont->GetEncoding();
							auto& table = encoding->m_toUnicode;
							// 1.str to int_10
							// 2.search int_10 get Unicode_code_10
							// 3.Unicode_code_10 to str_16
							// 4.str_16 to Unicode

							// 这里没有读取数字，所以数字不会显示
							if (!table.empty())
							{
								// 每4个截断一次
								string res;
								for (unsigned n = 0; n < tokenizer.m_DataString.size(); n += 4)
								{
									string substr = tokenizer.m_DataString.substr(n, 4);
									unsigned int_10 = std::stoi(substr, nullptr, 16);
									auto iter = table.find(int_10);
									if (table.end() != iter)
									{
										pdf_utf16be res_10 = iter->second;
										char str_16[8] = "";
										_itoa_s(res_10, str_16, 16);
										string unicode = UnicodeToString(str_16);
										res.append(unicode);
									}
								}
								std::cout << res;
								int a = 0;
							}
						}

						// 商品区域 X：30-590	Y：138-224
						if ((30  <= dCurPosX && dCurPosX <= 590) &&
							 138 <= dCurPosY && dCurPosY <= 224)
							IsCommodity = true;

						for (auto& AreaData : mapCoordinates)
						{
							vector<unsigned> vecAreaCoor{ AreaData.first };

							// 判断在哪个区域坐标里
							if ((vecAreaCoor[0] <= dCurPosX && dCurPosX <= vecAreaCoor[1]) &&
								(vecAreaCoor[2] <= dCurPosY && dCurPosY <= vecAreaCoor[3]))
							{
								map<string, vector<unsigned>> mapData{ AreaData.second };

								// 比较区域内的坐标，判断哪个字段的范围
								for (auto& data : mapData)
								{
									string DataName = data.first;
									vector<unsigned> DataCoor{ data.second };

									// 判断在哪个字段的范围
									if ((DataCoor[0] <= dCurPosX && dCurPosX <= DataCoor[1]) &&
										(DataCoor[2] <= dCurPosY && dCurPosY <= DataCoor[3]))
									{
										prefix.append(DataName);
										string DataValue;

										// 判断是否是Unicode编码
										if(0 == DataCoor[4])
											DataValue = pdfstr.GetString();
										else if (1 == DataCoor[4])
										{
											DataValue = UnicodeToString(tokenizer.m_DataString);
											// 去除字符串里解码错误的问号
											for (size_t pos = DataValue.find('?'); pos != string::npos;)
											{
												DataValue.erase(pos, 1);
												pos = DataValue.find('?');
											}
										}
										// 解析日期：2022年10月11日
										else if (2 == DataCoor[4])
										{
											if (DateIndex % 2 == 0)
												DataValue = pdfstr.GetString();
											else
												DataValue = UnicodeToString(tokenizer.m_DataString);
											++DateIndex;
										}

										// 添加商品信息
										if (IsCommodity)
										{
											// map<string, map<double, string>> mapCommodityInfo; // DataName - (Y Pos - info)
											map<double, string> mapTemp{ make_pair(dCurPosY, DataValue) };

											auto iter = mapCommodityInfo.find(DataName);
											// 若已添加商品名称的Key，则添加信息到iter->second，否则insert会被忽略
											if(mapCommodityInfo.end() == iter)
												mapCommodityInfo.insert(make_pair(DataName, mapTemp));
											else
											{
												map<double, string>& value{ iter->second };
												value.insert(make_pair(dCurPosY, DataValue));
											}

											continue;
										}

										// 判断json是否已存在，若存在则将数据附加到json中已存在数据后，使有多行数据的字段能够正确写入json
										if (nullptr == Pointer(prefix.c_str()).Get(document))
											Pointer(prefix.c_str()).Set(document, DataValue.c_str());
										else
										{
											Value* pValue = Pointer(prefix.c_str()).Get(document);
											string temp = pValue->GetString();
											temp.append(DataValue);
											Pointer(prefix.c_str()).Set(document, temp.c_str());
										}

										IsCommodity = false;
									}
								}
							}
						}
					}
				}
			}
		}
		else if (eType == ePdfContentsType_Variant)
			stack.push(var);
		else
			PODOFO_RAISE_ERROR(ePdfError_InternalLogic); // Impossible; type must be keyword or variant
	}

	HandleCommodityInformation(mapCommodityInfo);
}

void HandleCommodityInformation(map<string, map<double, string>>& map)
{
	//map<string, map<double, string>> mapCommodityInfo; // DataName - (Y Pos - info)
	string prefix;
	unsigned CommodityIndex = 0;

	for (auto& mapInfo : map)
	{
		prefix = "/content/kpxx/0/fyxm/0/";
		prefix.append(mapInfo.first);

		for (auto& value : mapInfo.second)
		{
			prefix.at(21) = CommodityIndex + '0';

			// 处理税率 "免税" 的情况
			if ("sl" == mapInfo.first)
			{
				if (map.end() != map.find("se"))
				{
					auto seIter = map.at("se").cbegin();
					for (unsigned n = 0; n < CommodityIndex; ++n)
						++seIter;

					if ("***" == seIter->second)
						Pointer(prefix.c_str()).Set(document, "免税");
					else
						Pointer(prefix.c_str()).Set(document, value.second.c_str());
				}
			}
			else
				Pointer(prefix.c_str()).Set(document, value.second.c_str());

			++CommodityIndex;
		}

		CommodityIndex = 0;
	}
}

// 每4个字符组成一个十六进制数，并转换为Unicode编码的中文
string UnicodeToString(const string& UnicodeString)
{
	wstring invoice_data;

	for (unsigned n = 0; n < UnicodeString.size(); n += 4)
	{
		string unicode_num_16 = UnicodeString.substr(n, 4);
		char* end;
		int unicode_num_10 = strtol(unicode_num_16.c_str(), &end, 16);
		invoice_data.push_back((wchar_t)unicode_num_10);
	}

	return WstringToString(invoice_data);
}

string WstringToString(wstring wstr)
{
	// support chinese
	string res;
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), nullptr, 0, nullptr, nullptr);
	if (len <= 0) {
		return res;
	}
	char* buffer = new char[len + 1];
	if (buffer == nullptr) {
		return res;
	}
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, nullptr, nullptr);
	buffer[len] = '\0';
	res.append(buffer);
	delete[] buffer;
	return res;
}

// 初始化字段区域和字段坐标
void InitInvoiceCoordinates_1()
{
	// map<vector<unsigned>, map<string, vector<unsigned>>> mapCoordinates;
	vector<unsigned> vecAreaCoor;
	map<string, vector<unsigned>> mapData;
	vector<unsigned> vecDataCoor; // 最后一位用来判断是否是Unicode编码，1为是，0为否，2为开票日期特殊处理
	
	// *********************发票信息初始化*********************
	// 区域初始化	X：485-546	Y：307-359
	vecAreaCoor.push_back(485);
	vecAreaCoor.push_back(546);
	vecAreaCoor.push_back(307);
	vecAreaCoor.push_back(359);

	// 各数据初始化
	// 发票代码		X：485-492	Y：352-359
	vecDataCoor.push_back(485);
	vecDataCoor.push_back(492);
	vecDataCoor.push_back(352);
	vecDataCoor.push_back(359);
	vecDataCoor.push_back(0);
	mapData.insert(make_pair("fpdm", vecDataCoor));
	vecDataCoor.clear();

	// 发票号码		X：485-492	Y：337-344
	vecDataCoor.push_back(485);
	vecDataCoor.push_back(492);
	vecDataCoor.push_back(337);
	vecDataCoor.push_back(344);
	vecDataCoor.push_back(0);
	mapData.insert(make_pair("fphm", vecDataCoor));
	vecDataCoor.clear();

	// 开票日期 X：485-546 	Y：322-329
	vecDataCoor.push_back(485);
	vecDataCoor.push_back(546);
	vecDataCoor.push_back(322);
	vecDataCoor.push_back(329);
	vecDataCoor.push_back(2); // 2为开票日期特殊处理标志
	mapData.insert(make_pair("kprq", vecDataCoor));
	vecDataCoor.clear();

	// 校验码	X：485-492	Y：307-314
	vecDataCoor.push_back(485);
	vecDataCoor.push_back(492);
	vecDataCoor.push_back(307);
	vecDataCoor.push_back(314);
	vecDataCoor.push_back(0); 
	mapData.insert(make_pair("jym", vecDataCoor));
	vecDataCoor.clear();

	mapCoordinates.insert(make_pair(vecAreaCoor, mapData));
	mapData.clear();
	vecAreaCoor.clear();

	// *********************购买方初始化*********************
	// 区域初始化	X：113-124		Y：243-300
	vecAreaCoor.push_back(113);
	vecAreaCoor.push_back(124);
	vecAreaCoor.push_back(243);
	vecAreaCoor.push_back(300);

	// 各数据初始化
	// 购买方名称	X： 113-124		Y：287-300
	vecDataCoor.push_back(113);
	vecDataCoor.push_back(124);
	vecDataCoor.push_back(287);
	vecDataCoor.push_back(300);
	vecDataCoor.push_back(1);
	mapData.insert(make_pair("ghdwmc", vecDataCoor));
	vecDataCoor.clear();

	// 购买方识别号	X： 113-124		Y：275-280
	vecDataCoor.push_back(113);
	vecDataCoor.push_back(124);
	vecDataCoor.push_back(275);
	vecDataCoor.push_back(280);
	vecDataCoor.push_back(0);
	mapData.insert(make_pair("ghdwsbh", vecDataCoor));
	vecDataCoor.clear();

	// 购买方地址电话	X： 113-124		Y：258-271
	vecDataCoor.push_back(113);
	vecDataCoor.push_back(124);
	vecDataCoor.push_back(258);
	vecDataCoor.push_back(271);
	vecDataCoor.push_back(1);
	mapData.insert(make_pair("ghdwdzdh", vecDataCoor));
	vecDataCoor.clear();

	// 购买方开户行及账号	X： 113-124		Y：243-256
	vecDataCoor.push_back(113);
	vecDataCoor.push_back(124);
	vecDataCoor.push_back(243);
	vecDataCoor.push_back(256);
	vecDataCoor.push_back(1);
	mapData.insert(make_pair("ghdwyhzh", vecDataCoor));
	vecDataCoor.clear();

	mapCoordinates.insert(make_pair(vecAreaCoor, mapData));
	mapData.clear();
	vecAreaCoor.clear();

	// *********************密码区初始化*********************
	// 区域初始化 X：396-404	Y：244-294
	vecAreaCoor.push_back(396);
	vecAreaCoor.push_back(404);
	vecAreaCoor.push_back(244);
	vecAreaCoor.push_back(294);

	// 各数据初始化
	// 密码区 X：396-404	Y：244-294
	vecDataCoor.push_back(396);
	vecDataCoor.push_back(404);
	vecDataCoor.push_back(244);
	vecDataCoor.push_back(294);
	vecDataCoor.push_back(0);
	mapData.insert(make_pair("skm", vecDataCoor));
	vecDataCoor.clear();

	mapCoordinates.insert(make_pair(vecAreaCoor, mapData));
	mapData.clear();
	vecAreaCoor.clear();

	// *********************商品初始化*********************
	// 区域初始化 X：30-590	Y：138-224
	vecAreaCoor.push_back(30);
	vecAreaCoor.push_back(590);
	vecAreaCoor.push_back(138);
	vecAreaCoor.push_back(224);

	// 各数据初始化
	// 商品名称	X：30-170	Y：138-224
	vecDataCoor.push_back(30);
	vecDataCoor.push_back(170);
	vecDataCoor.push_back(138);
	vecDataCoor.push_back(224);
	vecDataCoor.push_back(1);
	mapData.insert(make_pair("spmc", vecDataCoor));
	vecDataCoor.clear();

	// 商品规格型号	X：175-240	Y：138-224
	vecDataCoor.push_back(175);
	vecDataCoor.push_back(240);
	vecDataCoor.push_back(138);
	vecDataCoor.push_back(224);
	vecDataCoor.push_back(1);
	mapData.insert(make_pair("ggxh", vecDataCoor));
	vecDataCoor.clear();

	// 商品单位	X：249-283	Y：138-224
	vecDataCoor.push_back(249);
	vecDataCoor.push_back(283);
	vecDataCoor.push_back(138);
	vecDataCoor.push_back(224);
	vecDataCoor.push_back(1);
	mapData.insert(make_pair("dw", vecDataCoor));
	vecDataCoor.clear();

	// 商品数量 	X：287-338	Y：138-224
	vecDataCoor.push_back(287);
	vecDataCoor.push_back(338);
	vecDataCoor.push_back(138);
	vecDataCoor.push_back(224);
	vecDataCoor.push_back(0);
	mapData.insert(make_pair("spsl", vecDataCoor));
	vecDataCoor.clear();

	// 商品单价 	X：342-400	Y：138-224
	vecDataCoor.push_back(342);
	vecDataCoor.push_back(400);
	vecDataCoor.push_back(138);
	vecDataCoor.push_back(224);
	vecDataCoor.push_back(0);
	mapData.insert(make_pair("dj", vecDataCoor));
	vecDataCoor.clear();

	// 商品金额 	X：405-476	Y：138-224
	vecDataCoor.push_back(405);
	vecDataCoor.push_back(476);
	vecDataCoor.push_back(138);
	vecDataCoor.push_back(224);
	vecDataCoor.push_back(0);
	mapData.insert(make_pair("je", vecDataCoor));
	vecDataCoor.clear();

	// 商品税率 	X：481-507	Y：138-224
	vecDataCoor.push_back(481);
	vecDataCoor.push_back(507);
	vecDataCoor.push_back(138);
	vecDataCoor.push_back(224);
	vecDataCoor.push_back(0);
	mapData.insert(make_pair("sl", vecDataCoor));
	vecDataCoor.clear();

	// 商品税额 X：515-600	Y：138-224
	vecDataCoor.push_back(515);
	vecDataCoor.push_back(600);
	vecDataCoor.push_back(138);
	vecDataCoor.push_back(224);
	vecDataCoor.push_back(0);
	mapData.insert(make_pair("se", vecDataCoor));
	vecDataCoor.clear();

	mapCoordinates.insert(make_pair(vecAreaCoor, mapData));
	mapData.clear();
	vecAreaCoor.clear();

	// *********************合计初始化*********************
	// 区域初始化 X：405-600		Y：121-130
	vecAreaCoor.push_back(405);
	vecAreaCoor.push_back(600);
	vecAreaCoor.push_back(121);
	vecAreaCoor.push_back(130);

	// 各数据初始化
	// 合计金额 X：405-476	Y：121-130
	vecDataCoor.push_back(405);
	vecDataCoor.push_back(476);
	vecDataCoor.push_back(121);
	vecDataCoor.push_back(130);
	vecDataCoor.push_back(1);
	mapData.insert(make_pair("hjje", vecDataCoor));
	vecDataCoor.clear();

	// 合计税额 X：515-600	Y：121-130
	vecDataCoor.push_back(515);
	vecDataCoor.push_back(600);
	vecDataCoor.push_back(121);
	vecDataCoor.push_back(130);
	vecDataCoor.push_back(1);
	mapData.insert(make_pair("hjse", vecDataCoor));
	vecDataCoor.clear();

	mapCoordinates.insert(make_pair(vecAreaCoor, mapData));
	mapData.clear();
	vecAreaCoor.clear();

	// *********************价税合计初始化*********************
	// 区域初始化 X：185-600		Y：100-110
	vecAreaCoor.push_back(185);
	vecAreaCoor.push_back(600);
	vecAreaCoor.push_back(100);
	vecAreaCoor.push_back(110);

	// 各数据初始化
	// 价税合计大写 X：185-195	Y：100-110
	vecDataCoor.push_back(185);
	vecDataCoor.push_back(195);
	vecDataCoor.push_back(100);
	vecDataCoor.push_back(110);
	vecDataCoor.push_back(1);
	mapData.insert(make_pair("jshjdx", vecDataCoor));
	vecDataCoor.clear();

	// 价税合计小写 X：470-600	Y：100-110
	vecDataCoor.push_back(470);
	vecDataCoor.push_back(600);
	vecDataCoor.push_back(100);
	vecDataCoor.push_back(110);
	vecDataCoor.push_back(1);
	mapData.insert(make_pair("jshj", vecDataCoor));
	vecDataCoor.clear();

	mapCoordinates.insert(make_pair(vecAreaCoor, mapData));
	mapData.clear();
	vecAreaCoor.clear();

	// *********************机器编号初始化*********************
	// 区域初始化	X：68-77	Y：306-315
	vecAreaCoor.push_back(68);
	vecAreaCoor.push_back(77);
	vecAreaCoor.push_back(306);
	vecAreaCoor.push_back(315);

	// 各数据初始化
	// 机器编号	X：68-77	Y：306-315
	vecDataCoor.push_back(68);
	vecDataCoor.push_back(77);
	vecDataCoor.push_back(306);
	vecDataCoor.push_back(315);
	vecDataCoor.push_back(0);
	mapData.insert(make_pair("jqbh", vecDataCoor));
	vecDataCoor.clear();

	mapCoordinates.insert(make_pair(vecAreaCoor, mapData));
	mapData.clear();
	vecAreaCoor.clear();

	// *********************销售方初始化*********************
	// 区域初始化	X： 113-124		Y：37-93
	vecAreaCoor.push_back(113);
	vecAreaCoor.push_back(124);
	vecAreaCoor.push_back(37);
	vecAreaCoor.push_back(93);

	// 销售方名称	X： 113-124		Y：80-93
	vecDataCoor.push_back(113);
	vecDataCoor.push_back(124);
	vecDataCoor.push_back(80);
	vecDataCoor.push_back(93);
	vecDataCoor.push_back(1);
	mapData.insert(make_pair("xhdwmc", vecDataCoor));
	vecDataCoor.clear();

	// 销售方识别号	X： 113-124		Y：69-74
	vecDataCoor.push_back(113);
	vecDataCoor.push_back(124);
	vecDataCoor.push_back(69);
	vecDataCoor.push_back(74);
	vecDataCoor.push_back(0);
	mapData.insert(make_pair("xhdwsbh", vecDataCoor));
	vecDataCoor.clear();

	// 销售方地址电话	X： 113-124		Y：52-64
	vecDataCoor.push_back(113);
	vecDataCoor.push_back(124);
	vecDataCoor.push_back(52);
	vecDataCoor.push_back(64);
	vecDataCoor.push_back(1);
	mapData.insert(make_pair("xhdwdzdh", vecDataCoor));
	vecDataCoor.clear();

	// 销售方开户行及账号	X： 113-124		Y：37-50
	vecDataCoor.push_back(113);
	vecDataCoor.push_back(124);
	vecDataCoor.push_back(37);
	vecDataCoor.push_back(50);
	vecDataCoor.push_back(1);
	mapData.insert(make_pair("xhdwyhzh", vecDataCoor));
	vecDataCoor.clear();

	mapCoordinates.insert(make_pair(vecAreaCoor, mapData));
	mapData.clear();
	vecAreaCoor.clear();

	// *********************备注初始化*********************
	// 区域初始化	X：377-600	Y：40-94
	vecAreaCoor.push_back(377);
	vecAreaCoor.push_back(600);
	vecAreaCoor.push_back(40);
	vecAreaCoor.push_back(94);

	// 各数据初始化
	// 备注	X：377-600	Y：40-94
	vecDataCoor.push_back(377);
	vecDataCoor.push_back(600);
	vecDataCoor.push_back(40);
	vecDataCoor.push_back(94);
	vecDataCoor.push_back(1);
	mapData.insert(make_pair("bz", vecDataCoor));
	vecDataCoor.clear();

	mapCoordinates.insert(make_pair(vecAreaCoor, mapData));
	mapData.clear();
	vecAreaCoor.clear();

	// *********************收款人、复核、开票人初始化*********************
	// 区域初始化 X：72-375	Y：20-30
	vecAreaCoor.push_back(72);
	vecAreaCoor.push_back(375);
	vecAreaCoor.push_back(20);
	vecAreaCoor.push_back(30);

	// 各数据初始化
	// 收款人 X：72-82	Y：20-30
	vecDataCoor.push_back(72);
	vecDataCoor.push_back(82);
	vecDataCoor.push_back(20);
	vecDataCoor.push_back(30);
	vecDataCoor.push_back(1);
	mapData.insert(make_pair("skr", vecDataCoor));
	vecDataCoor.clear();

	// 复核人 X：211-220	  Y：20-30
	vecDataCoor.push_back(211);
	vecDataCoor.push_back(220);
	vecDataCoor.push_back(20);
	vecDataCoor.push_back(30);
	vecDataCoor.push_back(1);
	mapData.insert(make_pair("fhr", vecDataCoor));
	vecDataCoor.clear();

	// 开票人 X：364-373  Y：20-30
	vecDataCoor.push_back(364);
	vecDataCoor.push_back(373);
	vecDataCoor.push_back(20);
	vecDataCoor.push_back(30);
	vecDataCoor.push_back(1);
	mapData.insert(make_pair("kpr", vecDataCoor));
	vecDataCoor.clear();

	mapCoordinates.insert(make_pair(vecAreaCoor, mapData));
	mapData.clear();
	vecAreaCoor.clear();
}

// Unicode码转中文
//char a[] = "26376"; //  0x6708 月
//int i;
//setlocale(LC_ALL, "");
//sscanf(a, "%u", &i); // 转换为数字 
//wprintf(L"%c", (wchar_t)i); // 转换为Unicode字符
