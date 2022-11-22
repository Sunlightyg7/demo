#include <podofo.h>

#include <iostream>
#include <string>
#include <stack>
#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/pointer.h"

using namespace PoDoFo;
using namespace rapidjson;

void ExtractText(PdfMemDocument* pDocument, PdfPage* pPage);
void initInvoiceVec(std::vector<std::string>& vec);
std::string WstringToString(std::wstring wstr);

Document document;

int main()
{
	document.SetObject();
	Pointer("/code").Set(document, 0);
	Pointer("/message").Set(document, "�ɹ�");
	Pointer("/content/count").Set(document, 1);
	// �����0�������Ķ���
	Pointer("/content/kpxx/0/xh").Set(document, 1);

	try {
		const char* filename = "test.pdf";

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
	fopen_s(&fp, "output.json", "wb"); // non-Windows use "w"
	char writeBuffer[1024];
	memset(writeBuffer, '\0', sizeof(writeBuffer));
	FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
	Writer<FileWriteStream> file_writer(os);
	document.Accept(file_writer);
	if (NULL != fp)
		fclose(fp);
	return 0;
}

void ExtractText(PdfMemDocument* pDocument, PdfPage* pPage)
{
	PdfVariant       var;
	EPdfContentsType eType;
	PdfContentsTokenizer tokenizer(pPage);
	const char* pszToken = NULL;

	PdfFont* pCurFont = NULL;
	// Helvetica ���� STSong-light ��������
	PdfName chinese_font_name = "STSong-Light";
	std::stack<PdfVariant> stack;
	std::vector<std::string> vec_attribute_name;
	int commodity_index = 0;
	unsigned attribute_name_index = 0;
	bool bTextBlock = false;
	bool is_num_print = false;
	bool is_commodity_print = false;

	setlocale(LC_ALL, "");
	initInvoiceVec(vec_attribute_name);

	while (tokenizer.ReadNext(eType, pszToken, var))
	{
		if (eType == ePdfContentsType_Keyword)
		{
			if (strcmp(pszToken, "BT") == 0)
			{
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
				if (strcmp(pszToken, "Tj") == 0 ||
					strcmp(pszToken, "'") == 0)
				{
					if (stack.size() < 1)
					{
						fprintf(stderr, "WARNING: Expects one argument for '%s', ignoring\n", pszToken);
						continue;
					}

					// Ŀǰֻд�˽���������Ʊ
					std::string prefix = "/content/kpxx/0/";
					if (is_commodity_print)
					{
						// /content/kpxx/0/fyxm/0/xxxxx
						prefix = "/content/kpxx/0/fyxm/";
						prefix.append(std::to_string(commodity_index));
						prefix.push_back('/');
						int i = 0;
					}

					// ÿ4���ַ����һ��ʮ������������ת��ΪUnicode���������
					if (pCurFont != NULL && chinese_font_name == pCurFont->GetBaseFont())
					{
						is_num_print = true;
						std::wstring invoice_data;
						for (unsigned n = 0; n < tokenizer.m_UnicodeString.size(); n += 4)
						{
							std::string unicode_num_16 = tokenizer.m_UnicodeString.substr(n, 4);
							char* end;
							int unicode_num_10 = strtol(unicode_num_16.c_str(), &end, 16);
							invoice_data.push_back((wchar_t)unicode_num_10);
						}

						stack.pop();
						// ��Ʊ�������⴦��
						if (0 == invoice_data.compare(L"��") ||
							0 == invoice_data.compare(L"��") ||
							0 == invoice_data.compare(L"��"))
						{
							continue;
						}

						if (0 == vec_attribute_name[attribute_name_index].compare("skip"))
						{
							++attribute_name_index;
							continue;
						}

						// ��ʼѭ��������Ʒ��Ϣ
						if (0 == vec_attribute_name[attribute_name_index].compare("bz"))
							is_commodity_print = true;

						prefix.append(vec_attribute_name[attribute_name_index]);
						Pointer(prefix.c_str()).Set(document, WstringToString(invoice_data).c_str());
						std::wcout << invoice_data << std::endl;
						++attribute_name_index;
						continue;
					}

					PdfString pdfstr = stack.top().GetString();
					stack.pop();
					if (is_num_print)
					{
						// ��Ʊ���ڴ���
						static int date_index = 0;
						static std::string date;
						if (0 == vec_attribute_name[attribute_name_index].compare("kprq"))
						{
							date.append(pdfstr.GetString());
							++date_index;
							if (date_index != 3)
								date.append("-");
							if (date_index == 3)
							{
								std::cout << date << std::endl;
								prefix.append(vec_attribute_name[attribute_name_index]);
								Pointer(prefix.c_str()).Set(document, date.c_str());
								++attribute_name_index;
							}
							continue;
						}

						// ���������浽Json�ķ�Ʊ����
						if (0 == vec_attribute_name[attribute_name_index].compare("skip"))
						{
							++attribute_name_index;
							continue;
						}

						prefix.append(vec_attribute_name[attribute_name_index]);
						Pointer(prefix.c_str()).Set(document, pdfstr.GetString());

						std::cout << pdfstr.GetString() << std::endl;
						// ѭ������N����Ʒ����Ʒ��ϢΪ��ƱPDF�������Ϣ������꼴��PDF��������
						if (0 == vec_attribute_name[attribute_name_index].compare("se"))
						{
							attribute_name_index = 26;
							++commodity_index;
							continue;
						}
						++attribute_name_index;
					}
				}
			}
		}
		else if (eType == ePdfContentsType_Variant)
			stack.push(var);
		else
			PODOFO_RAISE_ERROR(ePdfError_InternalLogic); // Impossible; type must be keyword or variant
	}
}

void initInvoiceVec(std::vector<std::string>& vec)
{
	// initialized the vec with title of invoice data
	vec.push_back("skip"); // ���� ��Ʊ��

	vec.push_back("fpdm"); // ��Ʊ����
	vec.push_back("fphm"); // ��Ʊ����
	vec.push_back("kprq"); // ��Ʊ���� ���⴦��
	vec.push_back("jym"); // У����
	vec.push_back("jqbh"); // �������/˰�ط��������

	// ����������
	vec.push_back("skip"); // ����
	vec.push_back("skip"); // ����
	vec.push_back("skip"); // ����
	vec.push_back("skip"); // ����

	vec.push_back("skr"); // �տ���
	vec.push_back("fhr"); // ������
	vec.push_back("kpr"); // ��Ʊ��

	vec.push_back("skip"); // ���� ˰�ۺϼ� ��д
	vec.push_back("jshj"); // ˰�ۺϼ� Сд
	vec.push_back("hjje"); // �ϼƽ��
	vec.push_back("hjse"); // �ϼ�˰��

	vec.push_back("ghdwsbh"); // ������λʶ���
	vec.push_back("ghdwmc"); // ������λ����
	vec.push_back("ghdwdzdh"); // ������λ��ַ�绰
	vec.push_back("ghdwyhzh"); // ������λ�����˺�

	vec.push_back("xhdwsbh"); // ������λʶ���
	vec.push_back("xhdwmc"); // ������λ���� 
	vec.push_back("xhdwdzdh"); // ������λ��ַ�绰
	vec.push_back("xhdwyhzh"); // ������λ�����˺�
	vec.push_back("bz"); // ��ע
	//vec.push_back("zsfs"); // ��˰��ʽ
	//vec.push_back("fpzt"); // ��Ʊ״̬
	//vec.push_back("scbz"); // �ϴ���־
	//vec.push_back("skm"); 
	//vec.push_back("tspz"); // ����Ʊ�ֱ�ʶ

	// fyxm 
	vec.push_back("spmc"); // ��Ʒ����
	vec.push_back("ggxh"); // ����ͺ�
	vec.push_back("dw"); // ��λ
	vec.push_back("spsl"); // ��Ʒ����
	vec.push_back("dj"); // ����
	vec.push_back("je"); // ���
	vec.push_back("sl"); // ˰��
	vec.push_back("se"); // ˰��
	//vec.push_back("fphxz");
	//vec.push_back("spsm");
	//vec.push_back("hsbz");
	//vec.push_back("spbm");
	//vec.push_back("zxbm");
	//vec.push_back("yhzcbs");
	//vec.push_back("lslbs");
	//vec.push_back("zzstsgl");

	// qtxm
	//vec.push_back("sl");
	//vec.push_back("je");
	//vec.push_back("se");

	//vec.push_back("zhsl");
	//vec.push_back("jshj");
	//vec.push_back("jmbbh");
	//vec.push_back("zyspmc");
	//vec.push_back("spsm");
	//vec.push_back("qdbz");
	//vec.push_back("ssyf"); 
	//vec.push_back("kpjh");
	//vec.push_back("tzdbh");
	//vec.push_back("yfpdm");
	//vec.push_back("yfphm");
	//vec.push_back("zfrq");
	//vec.push_back("zfr");
	//vec.push_back("qmcs");
	//vec.push_back("qmz");
	//vec.push_back("ykfsje");
	//vec.push_back("ewm");
}

std::string WstringToString(std::wstring wstr)
{
	// support chinese
	std::string res;
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

// Unicode��ת����
//char a[] = "26376"; //  0x6708 ��
//int i;
//setlocale(LC_ALL, "");
//sscanf(a, "%u", &i); // ת��Ϊ���� 
//wprintf(L"%c", (wchar_t)i); // ת��ΪUnicode�ַ�
