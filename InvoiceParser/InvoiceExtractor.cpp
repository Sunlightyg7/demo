#include "InvoiceExtractor.h"

Document document;

// 区域坐标，区域名
map<vector<unsigned>, string> mapAreaCoordinates;
// 商品信息区域坐标，商品信息区域名(如"spmc")
map<vector<unsigned>, string> mapCommodityCoordinate;
// <商品信息名, Y坐标>，商品信息  如 <"spmc", 201>, "餐饮服务"
multimap<pair<string, double>, string> mapCommodityData;
CInvoiceData InvoiceData;

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

	return 0;
}

void ExtractText(PdfMemDocument* pDocument, PdfPage* pPage)
{
	PdfVariant       var;
	EPdfContentsType eType;
	PdfContentsTokenizer tokenizer(pPage);
	const char* pszToken = NULL;

	// <区域名, X或Y坐标>，数据字符串 
	multimap<string, pair<double, string>> mapAreaData;
	PdfFont* pCurFont = NULL;
	stack<PdfVariant> stack;
	string DataString;
	bool bTextBlock = false;
	bool IsHex = false;
	double dCurPosY = 0;
	double dCurPosX = 0;

	setlocale(LC_ALL, ""); // 不设置就不能将字符串数字转为Unicode，如wchar_t Unicode = "56db"，Unicode为"四"
	mapAreaCoordinates.clear();
	mapCommodityCoordinate.clear();
	mapCommodityData.clear();
	InvoiceData.clear();
	// 待添加分辨发票格式的代码
	InitInvoiceCoordinates_2();

	// 提取PDF的数据，每一部分数据块为BT开头，ET结尾
	while (tokenizer.ReadNext(eType, pszToken, var))
	{
		if (eType == ePdfContentsType_Keyword)
		{
			if (strcmp(pszToken, "BT") == 0)
			{
				dCurPosY = 0;
				dCurPosX = 0;
				DataString.clear();
				bTextBlock = true;
				IsHex = false;
			}
			else if (strcmp(pszToken, "ET") == 0)
			{
				if (!IsHex) // 排除PDF的十六进制数据 如<0134>，因为没写十六进制数据处理
				{
					for (auto& AreaData : mapAreaCoordinates)
					{
						vector<unsigned> vecAreaCoor{ AreaData.first };

						// 判断在哪个数据区域坐标里，按区域添加发票数据
						if ((vecAreaCoor[0] <= dCurPosX && dCurPosX <= vecAreaCoor[1]) &&
							(vecAreaCoor[2] <= dCurPosY && dCurPosY <= vecAreaCoor[3]))
						{
							string AreaName = AreaData.second;
							if ("购买方" == AreaName)
								int a = 0;
							pair<double, string> value;

							// 商品数据处理。因为商品信息分名称、数量等不同区域，所以要单独处理
							if ("商品区" == AreaName)
							{
								for (auto& CommodityArea : mapCommodityCoordinate)
								{
									vector<unsigned> vecCommodityAreaCoor{ CommodityArea.first };
									// 判断具体在商品哪个区域，如 名称区域 数量区域 单价区域等
									if ((vecCommodityAreaCoor[0] <= dCurPosX && dCurPosX <= vecCommodityAreaCoor[1]) &&
										(vecCommodityAreaCoor[2] <= dCurPosY && dCurPosY <= vecCommodityAreaCoor[3]))
									{
										string CommodityAreaName = CommodityArea.second;
										pair<string, double> pair{ CommodityAreaName , dCurPosY };
										mapCommodityData.insert(make_pair(pair, DataString));
									}
								}
								continue;
							}

							// 判断是用Y坐标保存还是X坐标，合计、价税合计、开票人用X坐标，其余Y坐标保存
							if (!vecAreaCoor[4])
								value = make_pair(dCurPosY, DataString);
							else
								value = make_pair(dCurPosX, DataString);

							mapAreaData.insert(make_pair(AreaName, value));
						}
					}
				}

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

					// 在同一个BT ET数据块里，若Td或Tm为如下形式
					// 474.69 335.18 Td
					// 17.97 0 Td
					// 则后面的坐标为第一个坐标的相对坐标，需加上
					dCurPosY += stack.top().GetReal();
					stack.pop();
					dCurPosX += stack.top().GetReal();
					stack.pop();
				}
				else if (strcmp(pszToken, "Tm") == 0)
				{
					if (stack.size() < 6)
					{
						fprintf(stderr, "WARNING: Expects six argument for 'Tm', ignoring\n");
						continue;
					}

					dCurPosY += stack.top().GetReal();
					stack.pop();
					dCurPosX += stack.top().GetReal();
					stack.pop();
				}
				else if (strcmp(pszToken, "Tj") == 0 || strcmp(pszToken, "'") == 0)
				{
					if (stack.size() < 1)
					{
						fprintf(stderr, "WARNING: Expects one argument for '%s', ignoring\n", pszToken);
						continue;
					}

					PdfString pdfstr = stack.top().GetString();
					stack.pop();

					if (!pdfstr.IsHex()) // 排除PDF的十六进制数据 如<0134>，因为没写十六进制数据处理
						DataString += GetDataString(pdfstr, pCurFont);
					else
						IsHex = true;
				}
			}
		}
		else if (eType == ePdfContentsType_Variant)
			stack.push(var);
		else
			PODOFO_RAISE_ERROR(ePdfError_InternalLogic); // Impossible; type must be keyword or variant
	}

	InvoiceData.AppendAreaData(mapAreaData);
	InvoiceData.AppendCommodityData(mapCommodityData);
	InvoiceData.CreateJson("output.json");
}

// 获取PDF的数据，格式为 (123654)或({231d325a5c8c266234d0})
// 若有到Unicode字符串的映射表，则查找映射表找到所对应Unicode，然后转换为字符
string GetDataString(PdfString pdfstr, PdfFont* pFont)
{
	const PdfEncoding* pEncoding = pFont->GetEncoding();
	PdfString UnicodeCodeString;

	// 判断数据是否为数字，若否则转为Unicode
	// 字符串的m_bWasEmbedded为true
	if (pFont->m_bWasEmbedded)
	{
		// 若有ToUnicode表，则获取所映射到的Unicode，然后转换为字符
		if (pEncoding->m_bToUnicodeIsLoaded)
			UnicodeCodeString = pEncoding->ConvertToUnicode(pdfstr.GetUnicode(), pFont).GetUnicode();
		else
			UnicodeCodeString = pdfstr.GetUnicode();

		// 获取Unicode码
		const pdf_utf16be* utf16be = UnicodeCodeString.GetUnicode();
		wstring UnicodeStringW;
		for (int n = 0; n != pdfstr.GetUnicodeLength(); ++n)
		{
			// 因为直接取pdf_utf16be会先取低字节，所以要交换高字节和低字节位置再取
			wchar_t UnicodeCharW = pEncoding->GetCharCode(utf16be[n]);
			UnicodeStringW.push_back(UnicodeCharW);
		}

		return WstringToString(UnicodeStringW);
	}
	else
		return pdfstr.GetString();
}

// 添加发票信息，商品信息将单独处理
void CInvoiceData::AppendAreaData(multimap<string, pair<double, string>>& mapAreaData)
{
	multimap<double, string, std::greater<double>> mapSortData;
	vector<string> vecAreaName{ "发票信息", "机器编号", "购买方", "销售方", "密码区", "合计", "价税合计", "备注", "发票人" };

	// 将发票数据添加到InvoiceData
	for (string AreaName : vecAreaName)
	{
		// 获取相应名称的map key的范围
		auto AreaDataRange = mapAreaData.equal_range(AreaName);
		// 数据添加到新的map，利用map的自动排序，根据Y或X坐标从高到低自动排序
		for (auto& n = AreaDataRange.first; n != AreaDataRange.second; ++n)
			mapSortData.insert(make_pair(n->second.first, n->second.second));

		if (mapSortData.empty())
			continue;

		if ("发票信息" == AreaName)
		{
			vector<string> vecTitle = { "fpdm", "fphm", "kprq", "jym" };
			unsigned index = 0;
			unsigned num = 0;
			for (auto& data : mapSortData)
			{
				++num;
				auto res = InvoiceData.mapData.find(vecTitle[index]);
				if (InvoiceData.mapData.end() == res)
					InvoiceData.mapData.insert(make_pair(vecTitle[index], data.second));
				else
					res->second.append(data.second);

				if (mapSortData.size() > 4 && num >= 3 && num < 8)
					continue;

				++index;
			}
			mapSortData.clear();
		}

		if ("机器编号" == AreaName)
		{
			for (auto& data : mapSortData)
				InvoiceData.mapData.insert(make_pair("jqbh", data.second));
			mapSortData.clear();
		}

		if ("购买方" == AreaName)
		{
			vector<string> vecVarName{ "ghdwmc", "ghdwsbh", "ghdwdzdh", "ghdwyhzh" };
			HandleCorpInfo(vecVarName, InvoiceData.mapData, mapSortData);
			mapSortData.clear();
		}

		if ("销售方" == AreaName)
		{
			vector<string> vecVarName{ "xhdwmc", "xhdwsbh", "xhdwdzdh", "xhdwyhzh" };
			HandleCorpInfo(vecVarName, InvoiceData.mapData, mapSortData);
			mapSortData.clear();
		}

		if ("密码区" == AreaName)
		{
			for (auto& pwd : mapSortData)
			{
				auto res = InvoiceData.mapData.find("skm");
				if (InvoiceData.mapData.end() == res)
					InvoiceData.mapData.insert(make_pair("skm", pwd.second));
				else
					res->second.append(pwd.second); // 添加到之前添加的密码后面
			}
			mapSortData.clear();
		}

		if ("合计" == AreaName)
		{
			vector<string> vecTitle = { "hjse", "hjje" };
			unsigned index = 0;
			for (auto& data : mapSortData)
			{
				InvoiceData.mapData.insert(make_pair(vecTitle[index], data.second));
				++index;
			}
			mapSortData.clear();
		}

		if ("价税合计" == AreaName)
		{
			vector<string> vecTitle = { "jshj", "jshjdx" };
			unsigned index = 0;
			for (auto& data : mapSortData)
			{
				// 第二种发票格式会读取发票数据价税合计里的标题  "（小写）"	，先强行去掉
				if ("（小写）" == data.second)
					continue;
				InvoiceData.mapData.insert(make_pair(vecTitle[index], data.second));
				++index;
			}
			mapSortData.clear();
		}

		if ("备注" == AreaName)
		{
			for (auto& data : mapSortData)
			{
				auto res = InvoiceData.mapData.find("bz");
				if (InvoiceData.mapData.end() == res)
					InvoiceData.mapData.insert(make_pair("bz", data.second));
				else
					res->second.append(data.second);
			}
			mapSortData.clear();
		}

		if ("发票人" == AreaName)
		{
			vector<string> vecTitle = { "kpr", "fhr", "skr" };
			unsigned index = 0;
			for (auto& data : mapSortData)
			{
				// 第二个发票格式会读取发票数据里的标题  开票人、复核、收款人	，先强行去掉
				if ("开 票 人:" == data.second || "复 核:" == data.second)
					continue;
				InvoiceData.mapData.insert(make_pair(vecTitle[index], data.second));
				++index;
			}
			mapSortData.clear();
		}
	}
}

// 添加商品信息
void CInvoiceData::AppendCommodityData(multimap<pair<string, double>, string>& mapCommodityData)
{
	unsigned count = 0;
	unsigned index = 0;

	// 计算有几件商品
	for (auto& data : mapCommodityData)
	{
		string title = data.first.first;
		if (title == mapCommodityData.cbegin()->first.first)
		{
			vecCommodityData.push_back(map<string, string>());
			++count;
		}
		else
			break;
	}

	// 添加商品信息
	for (auto& data : mapCommodityData)
	{
		string title = data.first.first;
		string DataString = data.second;

		// "·"字符会转成"?"，去除
		if ("spmc" == title)
			EraseQuestionMark(DataString);

		vecCommodityData[index].insert(make_pair(title, DataString));

		++index;
		// 若有多件商品，则按序号循环添加商品信息
		if (count == index)
			index = 0;
	}
}

// 添加购买方和销售方信息
void CInvoiceData::HandleCorpInfo(vector<string>& vecTitleName, map<string, string>& map, multimap<double, string, std::greater<double>>& mapSortData)
{
	bool HasIdentifyNumber = false;
	bool HasBankName = false;
	unsigned index = 0;

	for (auto& data : mapSortData)
	{
		// 处理名称有两行的情况。名称有两行时，读取出来会有两行而不是一行，所以用识别码只有数字和字母来判断是不是识别码来读取两行，地址电话同理
		if (!HasIdentifyNumber && IsIdentifyNumber(data.second))
		{
			HasIdentifyNumber = true;
			++index;
		}

		//  处理地址电话有两行的情况
		if (!HasBankName && IsBankName(data.second))
		{
			HasBankName = true;
			++index;
		}

		EraseQuestionMark(data.second);
		auto res = InvoiceData.mapData.find(vecTitleName[index]);
		if (InvoiceData.mapData.end() == res)
			InvoiceData.mapData.insert(make_pair(vecTitleName[index], data.second));
		else
			res->second.append(data.second);

		if (HasIdentifyNumber)
		{
			HasIdentifyNumber = false;
			++index;
		}
	}
}

// 是否是纳税人识别号，识别号由大写字母和数字组成
bool  CInvoiceData::IsIdentifyNumber(string data)
{
	bool HasLetter = false;
	for (char c : data)
	{
		if (c > 'A' && c < 'Z')
			HasLetter = true;
		if (c < '0' || c > 'Z' || (c > '9' && c < 'A'))
			return false;
	}
	if (HasLetter)
		return true;
	else
		return false;
}

// 判断是否包含"银行"两字，判断是不是开户行数据
bool  CInvoiceData::IsBankName(string data)
{
	if (data.find("银行") == string::npos)
		return false;
	return true;
}

// 根据发票Json格式编写，json框架为RapidJson
void CInvoiceData::CreateJson(string FilePath)
{
	string prefix = "/content/kpxx/0/";
	unsigned CommodityIndex = 0;

	for (auto& data : mapData)
	{
		string subprefix{ prefix + data.first };
		Pointer(subprefix.c_str()).Set(document, data.second.c_str());
	}

	for (auto& mapData : vecCommodityData)
	{
		for (auto& data : mapData)
		{
			char index = CommodityIndex + '0';
			prefix = "/content/kpxx/0/fyxm/" + string{ index } + "/";
			prefix.append(data.first);
			Pointer(prefix.c_str()).Set(document, data.second.c_str());

		}
		++CommodityIndex;
	}

	StringBuffer sb;
	PrettyWriter<StringBuffer> writer(sb);
	document.Accept(writer);
	puts(sb.GetString());

	FILE* fp = NULL;
	fopen_s(&fp, FilePath.c_str(), "wb");
	char writeBuffer[1024];
	memset(writeBuffer, '\0', sizeof(writeBuffer));
	FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
	Writer<FileWriteStream> file_writer(os);
	document.Accept(file_writer);
	if (NULL != fp)
		fclose(fp);
}

void CInvoiceData::clear()
{
	vecCommodityData.clear();
	mapData.clear();
}

// wstring 转换为 string
string WstringToString(wstring wstr)
{
	string res;
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), nullptr, 0, nullptr, nullptr);
	if (len <= 0)
		return res;

	char* buffer = new char[len + 1];
	if (buffer == nullptr)
		return res;

	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, nullptr, nullptr);
	buffer[len] = '\0';
	res.append(buffer);
	delete[] buffer;
	return res;
}

// 有的字符如 "·" 转换时会转换成 "?" 手动去掉
void EraseQuestionMark(string& str)
{
	for (auto pos = str.find('?'); pos != string::npos;)
	{
		str.erase(pos, 1);
		pos = str.find('?');
	}
}

// 一
// 由iText 5.4.3 生成的发票的区域范围
void InitInvoiceCoordinates_1()
{
	// map<vector<unsigned>, string> mapAreaCoordinates;

	// vecAreaCoor[4] = 0 获取Y坐标
	// vecAreaCoor[4] = 1 获取X坐标
	vector<unsigned> vecAreaCoor;

	// *********************发票信息初始化*********************
	// 区域初始化	X：485-546	Y：307-359
	vecAreaCoor.push_back(485);
	vecAreaCoor.push_back(546);
	vecAreaCoor.push_back(307);
	vecAreaCoor.push_back(359);
	vecAreaCoor.push_back(0);
	mapAreaCoordinates.insert(make_pair(vecAreaCoor, "发票信息"));
	vecAreaCoor.clear();

	// *********************机器编号初始化*********************
	// 区域初始化	X：68-77	Y：306-315
	vecAreaCoor.push_back(68);
	vecAreaCoor.push_back(77);
	vecAreaCoor.push_back(306);
	vecAreaCoor.push_back(315);
	vecAreaCoor.push_back(0);
	mapAreaCoordinates.insert(make_pair(vecAreaCoor, "机器编号"));
	vecAreaCoor.clear();

	// *********************购买方初始化*********************
	// 区域初始化	X：113-124		Y：243-300
	vecAreaCoor.push_back(113);
	vecAreaCoor.push_back(124);
	vecAreaCoor.push_back(243);
	vecAreaCoor.push_back(300);
	vecAreaCoor.push_back(0);
	mapAreaCoordinates.insert(make_pair(vecAreaCoor, "购买方"));
	vecAreaCoor.clear();

	// *********************销售方初始化*********************
	// 区域初始化	X： 113-124		Y：37-93
	vecAreaCoor.push_back(113);
	vecAreaCoor.push_back(124);
	vecAreaCoor.push_back(37);
	vecAreaCoor.push_back(93);
	vecAreaCoor.push_back(0);
	mapAreaCoordinates.insert(make_pair(vecAreaCoor, "销售方"));
	vecAreaCoor.clear();

	// *********************密码区初始化*********************
	// 区域初始化 X：396-404	Y：244-294
	vecAreaCoor.push_back(396);
	vecAreaCoor.push_back(404);
	vecAreaCoor.push_back(244);
	vecAreaCoor.push_back(294);
	vecAreaCoor.push_back(0);
	mapAreaCoordinates.insert(make_pair(vecAreaCoor, "密码区"));
	vecAreaCoor.clear();

	// *********************商品区初始化*********************
	// 区域初始化 X：30-590	Y：138-224
	vecAreaCoor.push_back(30);
	vecAreaCoor.push_back(590);
	vecAreaCoor.push_back(138);
	vecAreaCoor.push_back(224);
	vecAreaCoor.push_back(0); // 占位
	mapAreaCoordinates.insert(make_pair(vecAreaCoor, "商品区"));
	vecAreaCoor.clear();

	// 各数据初始化
	// 商品名称	X：30-170	Y：138-224
	vecAreaCoor.push_back(30);
	vecAreaCoor.push_back(170);
	vecAreaCoor.push_back(138);
	vecAreaCoor.push_back(224);
	mapCommodityCoordinate.insert(make_pair(vecAreaCoor, "spmc"));
	vecAreaCoor.clear();

	// 商品规格型号	X：175-240	Y：138-224
	vecAreaCoor.push_back(175);
	vecAreaCoor.push_back(240);
	vecAreaCoor.push_back(138);
	vecAreaCoor.push_back(224);
	mapCommodityCoordinate.insert(make_pair(vecAreaCoor, "ggxh"));
	vecAreaCoor.clear();

	// 商品单位	X：249-283	Y：138-224
	vecAreaCoor.push_back(249);
	vecAreaCoor.push_back(283);
	vecAreaCoor.push_back(138);
	vecAreaCoor.push_back(224);
	mapCommodityCoordinate.insert(make_pair(vecAreaCoor, "dw"));
	vecAreaCoor.clear();

	// 商品数量 	X：287-338	Y：138-224
	vecAreaCoor.push_back(287);
	vecAreaCoor.push_back(338);
	vecAreaCoor.push_back(138);
	vecAreaCoor.push_back(224);
	mapCommodityCoordinate.insert(make_pair(vecAreaCoor, "spsl"));
	vecAreaCoor.clear();

	// 商品单价 	X：342-400	Y：138-224
	vecAreaCoor.push_back(342);
	vecAreaCoor.push_back(400);
	vecAreaCoor.push_back(138);
	vecAreaCoor.push_back(224);
	mapCommodityCoordinate.insert(make_pair(vecAreaCoor, "dj"));
	vecAreaCoor.clear();

	// 商品金额 	X：405-476	Y：138-224
	vecAreaCoor.push_back(405);
	vecAreaCoor.push_back(476);
	vecAreaCoor.push_back(138);
	vecAreaCoor.push_back(224);
	mapCommodityCoordinate.insert(make_pair(vecAreaCoor, "je"));
	vecAreaCoor.clear();

	// 商品税率 	X：481-507	Y：138-224
	vecAreaCoor.push_back(481);
	vecAreaCoor.push_back(507);
	vecAreaCoor.push_back(138);
	vecAreaCoor.push_back(224);
	mapCommodityCoordinate.insert(make_pair(vecAreaCoor, "sl"));
	vecAreaCoor.clear();

	// 商品税额 X：515-600	Y：138-224
	vecAreaCoor.push_back(515);
	vecAreaCoor.push_back(600);
	vecAreaCoor.push_back(138);
	vecAreaCoor.push_back(224);
	mapCommodityCoordinate.insert(make_pair(vecAreaCoor, "se"));
	vecAreaCoor.clear();

	// *********************合计初始化*********************
	// 区域初始化 X：405-600		Y：121-130
	vecAreaCoor.push_back(405);
	vecAreaCoor.push_back(600);
	vecAreaCoor.push_back(121);
	vecAreaCoor.push_back(130);
	vecAreaCoor.push_back(1);
	mapAreaCoordinates.insert(make_pair(vecAreaCoor, "合计"));
	vecAreaCoor.clear();

	// *********************价税合计初始化*********************
	// 区域初始化 X：185-600		Y：100-110
	vecAreaCoor.push_back(185);
	vecAreaCoor.push_back(600);
	vecAreaCoor.push_back(100);
	vecAreaCoor.push_back(110);
	vecAreaCoor.push_back(1);
	mapAreaCoordinates.insert(make_pair(vecAreaCoor, "价税合计"));
	vecAreaCoor.clear();

	// *********************备注初始化*********************
	// 区域初始化	X：377-600	Y：40-94
	vecAreaCoor.push_back(377);
	vecAreaCoor.push_back(600);
	vecAreaCoor.push_back(40);
	vecAreaCoor.push_back(94);
	vecAreaCoor.push_back(0);
	mapAreaCoordinates.insert(make_pair(vecAreaCoor, "备注"));
	vecAreaCoor.clear();

	// *********************收款人、复核、开票人初始化*********************
	// 区域初始化 X：72-375	Y：20-30
	vecAreaCoor.push_back(72);
	vecAreaCoor.push_back(375);
	vecAreaCoor.push_back(20);
	vecAreaCoor.push_back(30);
	vecAreaCoor.push_back(1);
	mapAreaCoordinates.insert(make_pair(vecAreaCoor, "发票人"));
	vecAreaCoor.clear();
}


// 二
// 另一种发票的区域
void InitInvoiceCoordinates_2()
{
	// map<vector<unsigned>, string> mapAreaCoordinates;

	// vecAreaCoor[4] = 0 获取Y坐标
	// vecAreaCoor[4] = 1 获取X坐标
	vector<unsigned> vecAreaCoor;

	// *********************发票信息初始化*********************
	// 区域初始化	X：471-532	Y：317-367
	vecAreaCoor.push_back(471);
	vecAreaCoor.push_back(532);
	vecAreaCoor.push_back(317);
	vecAreaCoor.push_back(367);
	vecAreaCoor.push_back(0);
	mapAreaCoordinates.insert(make_pair(vecAreaCoor, "发票信息"));
	vecAreaCoor.clear();

	// *********************机器编号初始化*********************
	// 区域初始化	X：64-75		Y：312-322
	vecAreaCoor.push_back(64);
	vecAreaCoor.push_back(75);
	vecAreaCoor.push_back(312);
	vecAreaCoor.push_back(322);
	vecAreaCoor.push_back(0);
	mapAreaCoordinates.insert(make_pair(vecAreaCoor, "机器编号"));
	vecAreaCoor.clear();

	// *********************购买方初始化*********************
	// 区域初始化	X：107-348	Y：250-308
	vecAreaCoor.push_back(107);
	vecAreaCoor.push_back(348);
	vecAreaCoor.push_back(250);
	vecAreaCoor.push_back(308);
	vecAreaCoor.push_back(0);
	mapAreaCoordinates.insert(make_pair(vecAreaCoor, "购买方"));
	vecAreaCoor.clear();

	// *********************销售方初始化*********************
	// 区域初始化	X：107-348
	vecAreaCoor.push_back(107);
	vecAreaCoor.push_back(348);
	vecAreaCoor.push_back(37);
	vecAreaCoor.push_back(93);
	vecAreaCoor.push_back(0);
	mapAreaCoordinates.insert(make_pair(vecAreaCoor, "销售方"));
	vecAreaCoor.clear();

	// *********************密码区初始化*********************
	// 区域初始化 X：365-372	Y：252-301
	vecAreaCoor.push_back(365);
	vecAreaCoor.push_back(372);
	vecAreaCoor.push_back(252);
	vecAreaCoor.push_back(301);
	vecAreaCoor.push_back(0);
	mapAreaCoordinates.insert(make_pair(vecAreaCoor, "密码区"));
	vecAreaCoor.clear();

	// *********************商品区初始化*********************
	// 区域初始化	X：22-600	Y：135-233
	vecAreaCoor.push_back(22);
	vecAreaCoor.push_back(600);
	vecAreaCoor.push_back(135);
	vecAreaCoor.push_back(233);
	vecAreaCoor.push_back(0); // 占位
	mapAreaCoordinates.insert(make_pair(vecAreaCoor, "商品区"));
	vecAreaCoor.clear();

	// 各数据初始化
	// 商品名称	X：22-158	Y：135-233
	vecAreaCoor.push_back(22);
	vecAreaCoor.push_back(158);
	vecAreaCoor.push_back(135);
	vecAreaCoor.push_back(233);
	mapCommodityCoordinate.insert(make_pair(vecAreaCoor, "spmc"));
	vecAreaCoor.clear();

	// 商品规格型号	X：171-240	Y：135-233
	vecAreaCoor.push_back(171);
	vecAreaCoor.push_back(240);
	vecAreaCoor.push_back(135);
	vecAreaCoor.push_back(233);
	mapCommodityCoordinate.insert(make_pair(vecAreaCoor, "ggxh"));
	vecAreaCoor.clear();

	// 商品单位	X：243-266	Y：135-233
	vecAreaCoor.push_back(243);
	vecAreaCoor.push_back(266);
	vecAreaCoor.push_back(135);
	vecAreaCoor.push_back(233);
	mapCommodityCoordinate.insert(make_pair(vecAreaCoor, "dw"));
	vecAreaCoor.clear();

	// 商品数量 	X：271-329	Y：135-233
	vecAreaCoor.push_back(271);
	vecAreaCoor.push_back(329);
	vecAreaCoor.push_back(135);
	vecAreaCoor.push_back(233);
	mapCommodityCoordinate.insert(make_pair(vecAreaCoor, "spsl"));
	vecAreaCoor.clear();

	// 商品单价 X：330-385	Y：135-233
	vecAreaCoor.push_back(330);
	vecAreaCoor.push_back(385);
	vecAreaCoor.push_back(135);
	vecAreaCoor.push_back(233);
	mapCommodityCoordinate.insert(make_pair(vecAreaCoor, "dj"));
	vecAreaCoor.clear();

	// 商品金额 X：390-473	Y：135-233
	vecAreaCoor.push_back(390);
	vecAreaCoor.push_back(473);
	vecAreaCoor.push_back(135);
	vecAreaCoor.push_back(233);
	mapCommodityCoordinate.insert(make_pair(vecAreaCoor, "je"));
	vecAreaCoor.clear();

	// 商品税率 	X：475-502	Y：135-233
	vecAreaCoor.push_back(475);
	vecAreaCoor.push_back(502);
	vecAreaCoor.push_back(135);
	vecAreaCoor.push_back(233);
	mapCommodityCoordinate.insert(make_pair(vecAreaCoor, "sl"));
	vecAreaCoor.clear();

	// 商品税额 X：507-600	Y：135-233
	vecAreaCoor.push_back(507);
	vecAreaCoor.push_back(600);
	vecAreaCoor.push_back(135);
	vecAreaCoor.push_back(233);
	mapCommodityCoordinate.insert(make_pair(vecAreaCoor, "se"));
	vecAreaCoor.clear();

	// *********************合计初始化*********************
	// 区域初始化	X：436-580	Y：125-131
	vecAreaCoor.push_back(436);
	vecAreaCoor.push_back(580);
	vecAreaCoor.push_back(125);
	vecAreaCoor.push_back(131);
	vecAreaCoor.push_back(1);
	mapAreaCoordinates.insert(make_pair(vecAreaCoor, "合计"));
	vecAreaCoor.clear();

	// *********************价税合计初始化*********************
	// 区域初始化 X：181-520		Y：106-114
	vecAreaCoor.push_back(181);
	vecAreaCoor.push_back(520);
	vecAreaCoor.push_back(106);
	vecAreaCoor.push_back(114);
	vecAreaCoor.push_back(1);
	mapAreaCoordinates.insert(make_pair(vecAreaCoor, "价税合计"));
	vecAreaCoor.clear();

	// *********************备注初始化*********************
	// 区域初始化	X：358-590	Y：42-100
	vecAreaCoor.push_back(358);
	vecAreaCoor.push_back(590);
	vecAreaCoor.push_back(42);
	vecAreaCoor.push_back(100);
	vecAreaCoor.push_back(0);
	mapAreaCoordinates.insert(make_pair(vecAreaCoor, "备注"));
	vecAreaCoor.clear();

	// *********************收款人、复核、开票人初始化*********************
	// 区域初始化 X：77-370 	Y：30-36
	vecAreaCoor.push_back(77);
	vecAreaCoor.push_back(370);
	vecAreaCoor.push_back(30);
	vecAreaCoor.push_back(36);
	vecAreaCoor.push_back(1);
	mapAreaCoordinates.insert(make_pair(vecAreaCoor, "发票人"));
	vecAreaCoor.clear();
}

// Unicode码转中文
//char a[] = "26376"; //  0x6708 月
//int i;
//setlocale(LC_ALL, "");
//sscanf(a, "%u", &i); // 转换为数字 
//wprintf(L"%c", (wchar_t)i); // 转换为Unicode字符
