// DataView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Ani_Data_Serever_PC.h"
#include "AddrView.h"
#include "EZini.h"

// CAddrView

IMPLEMENT_DYNCREATE(CAddrView, CFormView)
#define IF_CHECK_TIMER 0


CAddrView::CAddrView()
	: CFormView(CAddrView::IDD)
{
	m_iSelectPLCNum = 0;
	m_iSelectPCNum = 0;

	for (int ii = 0; ii < PCTOTLACOUNT; ii++)
		SetValue(ii);
}

CAddrView::~CAddrView()
{
}

void CAddrView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IN_LABEL01, m_addrBitPLCAddr[0]);
	DDX_Control(pDX, IDC_IN_LABEL02, m_addrBitPLCAddr[1]);
	DDX_Control(pDX, IDC_IN_LABEL03, m_addrBitPLCAddr[2]);
	DDX_Control(pDX, IDC_IN_LABEL04, m_addrBitPLCAddr[3]);
	DDX_Control(pDX, IDC_IN_LABEL05, m_addrBitPLCAddr[4]);
	DDX_Control(pDX, IDC_IN_LABEL06, m_addrBitPLCAddr[5]);
	DDX_Control(pDX, IDC_IN_LABEL07, m_addrBitPLCAddr[6]);
	DDX_Control(pDX, IDC_IN_LABEL08, m_addrBitPLCAddr[7]);
	DDX_Control(pDX, IDC_IN_LABEL09, m_addrBitPLCAddr[8]);
	DDX_Control(pDX, IDC_IN_LABEL10, m_addrBitPLCAddr[9]);
	DDX_Control(pDX, IDC_IN_LABEL11, m_addrBitPLCAddr[10]);
	DDX_Control(pDX, IDC_IN_LABEL12, m_addrBitPLCAddr[11]);
	DDX_Control(pDX, IDC_IN_LABEL13, m_addrBitPLCAddr[12]);
	DDX_Control(pDX, IDC_IN_LABEL14, m_addrBitPLCAddr[13]);
	DDX_Control(pDX, IDC_IN_LABEL15, m_addrBitPLCAddr[14]);
	DDX_Control(pDX, IDC_IN_LABEL16, m_addrBitPLCAddr[15]);
	DDX_Control(pDX, IDC_IN_LABEL17, m_addrWordPLCAddr[0]);
	DDX_Control(pDX, IDC_IN_LABEL18, m_addrWordPLCAddr[1]);
	DDX_Control(pDX, IDC_IN_LABEL19, m_addrWordPLCAddr[2]);
	DDX_Control(pDX, IDC_IN_LABEL20, m_addrWordPLCAddr[3]);
	DDX_Control(pDX, IDC_IN_LABEL21, m_addrWordPLCAddr[4]);
	DDX_Control(pDX, IDC_IN_LABEL22, m_addrWordPLCAddr[5]);
	DDX_Control(pDX, IDC_IN_LABEL23, m_addrWordPLCAddr[6]);
	DDX_Control(pDX, IDC_IN_LABEL24, m_addrWordPLCAddr[7]);

	DDX_Control(pDX, IDC_INPUT01, m_addrBitPLCValue[0]);
	DDX_Control(pDX, IDC_INPUT02, m_addrBitPLCValue[1]);
	DDX_Control(pDX, IDC_INPUT03, m_addrBitPLCValue[2]);
	DDX_Control(pDX, IDC_INPUT04, m_addrBitPLCValue[3]);
	DDX_Control(pDX, IDC_INPUT05, m_addrBitPLCValue[4]);
	DDX_Control(pDX, IDC_INPUT06, m_addrBitPLCValue[5]);
	DDX_Control(pDX, IDC_INPUT07, m_addrBitPLCValue[6]);
	DDX_Control(pDX, IDC_INPUT08, m_addrBitPLCValue[7]);
	DDX_Control(pDX, IDC_INPUT09, m_addrBitPLCValue[8]);
	DDX_Control(pDX, IDC_INPUT10, m_addrBitPLCValue[9]);
	DDX_Control(pDX, IDC_INPUT11, m_addrBitPLCValue[10]);
	DDX_Control(pDX, IDC_INPUT12, m_addrBitPLCValue[11]);
	DDX_Control(pDX, IDC_INPUT13, m_addrBitPLCValue[12]);
	DDX_Control(pDX, IDC_INPUT14, m_addrBitPLCValue[13]);
	DDX_Control(pDX, IDC_INPUT15, m_addrBitPLCValue[14]);
	DDX_Control(pDX, IDC_INPUT16, m_addrBitPLCValue[15]);
	DDX_Control(pDX, IDC_INPUT17, m_addrWordPLCValue[0]);
	DDX_Control(pDX, IDC_INPUT18, m_addrWordPLCValue[1]);
	DDX_Control(pDX, IDC_INPUT19, m_addrWordPLCValue[2]);
	DDX_Control(pDX, IDC_INPUT20, m_addrWordPLCValue[3]);
	DDX_Control(pDX, IDC_INPUT21, m_addrWordPLCValue[4]);
	DDX_Control(pDX, IDC_INPUT22, m_addrWordPLCValue[5]);
	DDX_Control(pDX, IDC_INPUT23, m_addrWordPLCValue[6]);
	DDX_Control(pDX, IDC_INPUT24, m_addrWordPLCValue[7]);

	DDX_Control(pDX, IDC_INNAME1, m_addrBitPLCName[0]);
	DDX_Control(pDX, IDC_INNAME2, m_addrBitPLCName[1]);
	DDX_Control(pDX, IDC_INNAME3, m_addrBitPLCName[2]);
	DDX_Control(pDX, IDC_INNAME4, m_addrBitPLCName[3]);
	DDX_Control(pDX, IDC_INNAME5, m_addrBitPLCName[4]);
	DDX_Control(pDX, IDC_INNAME6, m_addrBitPLCName[5]);
	DDX_Control(pDX, IDC_INNAME7, m_addrBitPLCName[6]);
	DDX_Control(pDX, IDC_INNAME8, m_addrBitPLCName[7]);
	DDX_Control(pDX, IDC_INNAME9, m_addrBitPLCName[8]);
	DDX_Control(pDX, IDC_INNAME10, m_addrBitPLCName[9]);
	DDX_Control(pDX, IDC_INNAME11, m_addrBitPLCName[10]);
	DDX_Control(pDX, IDC_INNAME12, m_addrBitPLCName[11]);
	DDX_Control(pDX, IDC_INNAME13, m_addrBitPLCName[12]);
	DDX_Control(pDX, IDC_INNAME14, m_addrBitPLCName[13]);
	DDX_Control(pDX, IDC_INNAME15, m_addrBitPLCName[14]);
	DDX_Control(pDX, IDC_INNAME16, m_addrBitPLCName[15]);
	DDX_Control(pDX, IDC_INNAME17, m_addrWordPLCName[0]);
	DDX_Control(pDX, IDC_INNAME18, m_addrWordPLCName[1]);
	DDX_Control(pDX, IDC_INNAME19, m_addrWordPLCName[2]);
	DDX_Control(pDX, IDC_INNAME20, m_addrWordPLCName[3]);
	DDX_Control(pDX, IDC_INNAME21, m_addrWordPLCName[4]);
	DDX_Control(pDX, IDC_INNAME22, m_addrWordPLCName[5]);
	DDX_Control(pDX, IDC_INNAME23, m_addrWordPLCName[6]);
	DDX_Control(pDX, IDC_INNAME24, m_addrWordPLCName[7]);

	DDX_Control(pDX, IDC_OUT_LABEL01, m_addrBitPCAddr[0]);
	DDX_Control(pDX, IDC_OUT_LABEL02, m_addrBitPCAddr[1]);
	DDX_Control(pDX, IDC_OUT_LABEL03, m_addrBitPCAddr[2]);
	DDX_Control(pDX, IDC_OUT_LABEL04, m_addrBitPCAddr[3]);
	DDX_Control(pDX, IDC_OUT_LABEL05, m_addrBitPCAddr[4]);
	DDX_Control(pDX, IDC_OUT_LABEL06, m_addrBitPCAddr[5]);
	DDX_Control(pDX, IDC_OUT_LABEL07, m_addrBitPCAddr[6]);
	DDX_Control(pDX, IDC_OUT_LABEL08, m_addrBitPCAddr[7]);
	DDX_Control(pDX, IDC_OUT_LABEL09, m_addrBitPCAddr[8]);
	DDX_Control(pDX, IDC_OUT_LABEL10, m_addrBitPCAddr[9]);
	DDX_Control(pDX, IDC_OUT_LABEL11, m_addrBitPCAddr[10]);
	DDX_Control(pDX, IDC_OUT_LABEL12, m_addrBitPCAddr[11]);
	DDX_Control(pDX, IDC_OUT_LABEL13, m_addrBitPCAddr[12]);
	DDX_Control(pDX, IDC_OUT_LABEL14, m_addrBitPCAddr[13]);
	DDX_Control(pDX, IDC_OUT_LABEL15, m_addrBitPCAddr[14]);
	DDX_Control(pDX, IDC_OUT_LABEL16, m_addrBitPCAddr[15]);
	DDX_Control(pDX, IDC_OUT_LABEL17, m_addrWordPCAddr[0]);
	DDX_Control(pDX, IDC_OUT_LABEL18, m_addrWordPCAddr[1]);
	DDX_Control(pDX, IDC_OUT_LABEL19, m_addrWordPCAddr[2]);
	DDX_Control(pDX, IDC_OUT_LABEL20, m_addrWordPCAddr[3]);
	DDX_Control(pDX, IDC_OUT_LABEL21, m_addrWordPCAddr[4]);
	DDX_Control(pDX, IDC_OUT_LABEL22, m_addrWordPCAddr[5]);
	DDX_Control(pDX, IDC_OUT_LABEL23, m_addrWordPCAddr[6]);
	DDX_Control(pDX, IDC_OUT_LABEL24, m_addrWordPCAddr[7]);

	DDX_Control(pDX, IDC_OUTPUT01, m_addrBitPCValue[0]);
	DDX_Control(pDX, IDC_OUTPUT02, m_addrBitPCValue[1]);
	DDX_Control(pDX, IDC_OUTPUT03, m_addrBitPCValue[2]);
	DDX_Control(pDX, IDC_OUTPUT04, m_addrBitPCValue[3]);
	DDX_Control(pDX, IDC_OUTPUT05, m_addrBitPCValue[4]);
	DDX_Control(pDX, IDC_OUTPUT06, m_addrBitPCValue[5]);
	DDX_Control(pDX, IDC_OUTPUT07, m_addrBitPCValue[6]);
	DDX_Control(pDX, IDC_OUTPUT08, m_addrBitPCValue[7]);
	DDX_Control(pDX, IDC_OUTPUT09, m_addrBitPCValue[8]);
	DDX_Control(pDX, IDC_OUTPUT10, m_addrBitPCValue[9]);
	DDX_Control(pDX, IDC_OUTPUT11, m_addrBitPCValue[10]);
	DDX_Control(pDX, IDC_OUTPUT12, m_addrBitPCValue[11]);
	DDX_Control(pDX, IDC_OUTPUT13, m_addrBitPCValue[12]);
	DDX_Control(pDX, IDC_OUTPUT14, m_addrBitPCValue[13]);
	DDX_Control(pDX, IDC_OUTPUT15, m_addrBitPCValue[14]);
	DDX_Control(pDX, IDC_OUTPUT16, m_addrBitPCValue[15]);
	DDX_Control(pDX, IDC_OUTPUT17, m_addrWordPCValue[0]);
	DDX_Control(pDX, IDC_OUTPUT18, m_addrWordPCValue[1]);
	DDX_Control(pDX, IDC_OUTPUT19, m_addrWordPCValue[2]);
	DDX_Control(pDX, IDC_OUTPUT20, m_addrWordPCValue[3]);
	DDX_Control(pDX, IDC_OUTPUT21, m_addrWordPCValue[4]);
	DDX_Control(pDX, IDC_OUTPUT22, m_addrWordPCValue[5]);
	DDX_Control(pDX, IDC_OUTPUT23, m_addrWordPCValue[6]);
	DDX_Control(pDX, IDC_OUTPUT24, m_addrWordPCValue[7]);

	DDX_Control(pDX, IDC_OUTNAME1, m_addrBitPCName[0]);
	DDX_Control(pDX, IDC_OUTNAME2, m_addrBitPCName[1]);
	DDX_Control(pDX, IDC_OUTNAME3, m_addrBitPCName[2]);
	DDX_Control(pDX, IDC_OUTNAME4, m_addrBitPCName[3]);
	DDX_Control(pDX, IDC_OUTNAME5, m_addrBitPCName[4]);
	DDX_Control(pDX, IDC_OUTNAME6, m_addrBitPCName[5]);
	DDX_Control(pDX, IDC_OUTNAME7, m_addrBitPCName[6]);
	DDX_Control(pDX, IDC_OUTNAME8, m_addrBitPCName[7]);
	DDX_Control(pDX, IDC_OUTNAME9, m_addrBitPCName[8]);
	DDX_Control(pDX, IDC_OUTNAME10, m_addrBitPCName[9]);
	DDX_Control(pDX, IDC_OUTNAME11, m_addrBitPCName[10]);
	DDX_Control(pDX, IDC_OUTNAME12, m_addrBitPCName[11]);
	DDX_Control(pDX, IDC_OUTNAME13, m_addrBitPCName[12]);
	DDX_Control(pDX, IDC_OUTNAME14, m_addrBitPCName[13]);
	DDX_Control(pDX, IDC_OUTNAME15, m_addrBitPCName[14]);
	DDX_Control(pDX, IDC_OUTNAME16, m_addrBitPCName[15]);
	DDX_Control(pDX, IDC_OUTNAME17, m_addrWordPCName[0]);
	DDX_Control(pDX, IDC_OUTNAME18, m_addrWordPCName[1]);
	DDX_Control(pDX, IDC_OUTNAME19, m_addrWordPCName[2]);
	DDX_Control(pDX, IDC_OUTNAME20, m_addrWordPCName[3]);
	DDX_Control(pDX, IDC_OUTNAME21, m_addrWordPCName[4]);
	DDX_Control(pDX, IDC_OUTNAME22, m_addrWordPCName[5]);
	DDX_Control(pDX, IDC_OUTNAME23, m_addrWordPCName[6]);
	DDX_Control(pDX, IDC_OUTNAME24, m_addrWordPCName[7]);

	DDX_Control(pDX, IDB_TITLE_PLC_BIT, m_addrBitPLCTitle);
	DDX_Control(pDX, IDB_TITLE_PLC_WORD, m_addrWordPLCTitle);
	DDX_Control(pDX, IDB_TITLE_PC_BIT, m_addrBitPCTitle);
	DDX_Control(pDX, IDB_TITLE_PC_WORD, m_addrWordPCTitle);
}

BEGIN_EVENTSINK_MAP(CAddrView, CFormView)
	ON_EVENT(CAddrView, IDC_IN_PREV, DISPID_CLICK, CAddrView::OnClickPLCPrev, VTS_NONE)
	ON_EVENT(CAddrView, IDC_IN_NEXT, DISPID_CLICK, CAddrView::OnClickPLCNext, VTS_NONE)
	ON_EVENT(CAddrView, IDC_IN_PREV2, DISPID_CLICK, CAddrView::OnClickPCPrev2, VTS_NONE)
	ON_EVENT(CAddrView, IDC_IN_NEXT2, DISPID_CLICK, CAddrView::OnClickPCNext2, VTS_NONE)
END_EVENTSINK_MAP()


BEGIN_MESSAGE_MAP(CAddrView, CFormView)
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CAddrView 진단입니다.

#ifdef _DEBUG
void CAddrView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CAddrView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


BOOL CAddrView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CAddrView::SetValue(int iValue)
{
	CString strFilePath;
	switch (iValue)
	{
	case PLCBIT: strFilePath = DATA_PLC_ADDR_PATH_BIT; break;
	case PLCWORD: strFilePath = DATA_PLC_ADDR_PATH_WORD; break;
	case PCBIT: strFilePath = DATA_PC_ADDR_PATH_BIT; break;
	case PCWORD: strFilePath = DATA_PC_ADDR_PATH_WORD; break;
	}


	EZIni ini(strFilePath);

	std::vector<CString> listOfSectionNames;
	ini.EnumSectionNames(listOfSectionNames);

	int ii = 0;
	for (auto title = listOfSectionNames.begin(); title != listOfSectionNames.end(); ++title)
	{
		AddrData dataBit;
		dataBit.m_iNum = ii;
		std::vector<CString> listOfKeyNames;
		ini[*title].EnumKeyNames(listOfKeyNames);
		dataBit.m_strAddrTitle = *title;

		for (auto list = listOfKeyNames.begin(); list != listOfKeyNames.end(); ++list)
		{
			dataBit.m_strAddrName.push_back(*list);
			dataBit.m_strAddr.push_back(ini[*title][*list]);
		}

		switch (iValue)
		{
		case PLCBIT: m_addrPLCDataBit.push_back(dataBit);; break;
		case PLCWORD: m_addrPLCDataWord.push_back(dataBit);; break;
		case PCBIT: m_addrPCDataBit.push_back(dataBit);; break;
		case PCWORD: m_addrPCDataWord.push_back(dataBit);; break;
		}
		ii++;
	}
}

void CAddrView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	SeteValuePLCReset();
	SeteValuePCReset();

	SetTimer(IF_CHECK_TIMER, 100, NULL);
}

void CAddrView::OnTimer(UINT_PTR nIDEvent)
{
	if (this->IsWindowVisible() == FALSE)
		return;

	if (nIDEvent == IF_CHECK_TIMER)
	{
		UpdateData();
		SetAddrData();
	}

	CFormView::OnTimer(nIDEvent);
}

void CAddrView::SetAddrData()
{
	int addr;
	long wordValue;
	PanelData pPanelData;
	FpcIDData pFpcData;
	ModelData pModelName;
	CString strValue;

	m_addrBitPLCTitle.SetCaption(m_addrPLCDataBit[m_iSelectPLCNum].m_strAddrTitle);

	for (int ii = 0; ii < m_addrPLCDataBit[m_iSelectPLCNum].m_strAddr.size(); ii++)
	{
		m_addrBitPLCAddr[ii].SetCaption(m_addrPLCDataBit[m_iSelectPLCNum].m_strAddr[ii]);
		m_addrBitPLCName[ii].SetCaption(m_addrPLCDataBit[m_iSelectPLCNum].m_strAddrName[ii]);
		addr = String2Hex(m_addrPLCDataBit[m_iSelectPLCNum].m_strAddr[ii]);
		long value = theApp.m_pEqIf->m_pMNetH->GetComViewBitData(addr);
		strValue.Format(_T("%d"), value);
		m_addrBitPLCValue[ii].SetCaption(strValue);
	}

	m_addrWordPLCTitle.SetCaption(m_addrPLCDataWord[m_iSelectPLCNum].m_strAddrTitle);

	for (int ii = 0; ii < m_addrPLCDataWord[m_iSelectPLCNum].m_strAddr.size(); ii++)
	{
		m_addrWordPLCAddr[ii].SetCaption(m_addrPLCDataWord[m_iSelectPLCNum].m_strAddr[ii]);
		m_addrWordPLCName[ii].SetCaption(m_addrPLCDataWord[m_iSelectPLCNum].m_strAddrName[ii]);
		addr = String2Hex(m_addrPLCDataWord[m_iSelectPLCNum].m_strAddr[ii]);

		if (m_addrPLCDataWord[m_iSelectPLCNum].m_strAddrName[ii].Find(_T("CELLID")) > 0)
		{
			theApp.m_pEqIf->m_pMNetH->GetAddrWordData(addr, &pPanelData, sizeof(PanelData));
			m_addrWordPLCValue[ii].SetCaption(CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData)));
		}
		else if (m_addrPLCDataWord[m_iSelectPLCNum].m_strAddrName[ii].Find(_T("FPCID")) > 0)
		{
			theApp.m_pEqIf->m_pMNetH->GetAddrWordData(addr, &pFpcData, sizeof(FpcIDData));
			m_addrWordPLCValue[ii].SetCaption(CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData)));
		}
		else
		{
			if (m_addrPLCDataWord[m_iSelectPLCNum].m_strAddrName[ii].Find(_T("MODEL")) > 0)
			{
				theApp.m_pEqIf->m_pMNetH->GetComViewWordData(addr, &m_shortValue);
				strValue.Format(_T("%d"), m_shortValue);
				m_addrWordPLCValue[ii].SetCaption(strValue);
				
			}
			else
			{
				if (m_addrPLCDataWord[m_iSelectPLCNum].m_strAddrName[ii].Find(_T("NAME")) > 0)
				{
					theApp.m_pEqIf->m_pMNetH->GetAddrWordModelData(addr, &pModelName);
					m_addrWordPLCValue[ii].SetCaption(CStringSupport::ToWString(pModelName.m_strModlName, sizeof(pModelName.m_strModlName)));
				}
				else
				{
					theApp.m_pEqIf->m_pMNetH->GetComViewWordData(addr, &wordValue);
					strValue.Format(_T("%d"), wordValue);
					m_addrWordPLCValue[ii].SetCaption(strValue);
				}
			}
		}
	}


	m_addrBitPCTitle.SetCaption(m_addrPCDataBit[m_iSelectPCNum].m_strAddrTitle);

	for (int ii = 0; ii < m_addrPCDataBit[m_iSelectPCNum].m_strAddr.size(); ii++)
	{
		m_addrBitPCAddr[ii].SetCaption(m_addrPCDataBit[m_iSelectPCNum].m_strAddr[ii]);
		m_addrBitPCName[ii].SetCaption(m_addrPCDataBit[m_iSelectPCNum].m_strAddrName[ii]);
		addr = String2Hex(m_addrPCDataBit[m_iSelectPCNum].m_strAddr[ii]);
		long value = theApp.m_pEqIf->m_pMNetH->GetComViewBitData(addr);
		strValue.Format(_T("%d"), value);
		m_addrBitPCValue[ii].SetCaption(strValue);
	}

	m_addrWordPCTitle.SetCaption(m_addrPCDataWord[m_iSelectPCNum].m_strAddrTitle);

	for (int ii = 0; ii < m_addrPCDataWord[m_iSelectPCNum].m_strAddr.size(); ii++)
	{
		m_addrWordPCAddr[ii].SetCaption(m_addrPCDataWord[m_iSelectPCNum].m_strAddr[ii]);
		m_addrWordPCName[ii].SetCaption(m_addrPCDataWord[m_iSelectPCNum].m_strAddrName[ii]);
		addr = String2Hex(m_addrPCDataWord[m_iSelectPCNum].m_strAddr[ii]);

		if (m_addrPCDataWord[m_iSelectPCNum].m_strAddrName[ii].Find(_T("CELLID")) > 0)
		{
			theApp.m_pEqIf->m_pMNetH->GetAddrWordData(addr, &pPanelData, sizeof(PanelData));
			m_addrWordPCValue[ii].SetCaption(CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData)));
		}
		else if (m_addrPCDataWord[m_iSelectPCNum].m_strAddrName[ii].Find(_T("FPCID")) > 0)
		{
			theApp.m_pEqIf->m_pMNetH->GetAddrWordData(addr, &pFpcData, sizeof(FpcIDData));
			m_addrWordPCValue[ii].SetCaption(CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData)));
		}
		else if (m_addrPCDataWord[m_iSelectPCNum].m_strAddrName[ii].Find(_T("ESD")) > 0)
		{
			theApp.m_pEqIf->m_pMNetH->GetComViewWordData(addr, &wordValue);
			m_addrWordPCValue[ii].SetCaption(CStringSupport::ToShorString(wordValue));
		}
		else
		{
			theApp.m_pEqIf->m_pMNetH->GetComViewWordData(addr, &wordValue);
			strValue.Format(_T("%d"), wordValue);
			m_addrWordPCValue[ii].SetCaption(strValue);
		}
	}
}

void CAddrView::OnClickPLCPrev()
{
	if (m_iSelectPLCNum == 0)
		m_iSelectPLCNum = m_addrPLCDataBit.size() - 1;
	else if (m_iSelectPLCNum > 0)
		m_iSelectPLCNum--;

	SeteValuePLCReset();
	SetAddrData();
}

void CAddrView::OnClickPLCNext()
{
	if (m_iSelectPLCNum == m_addrPLCDataBit.size() - 1)
		m_iSelectPLCNum = 0;	
	else if (m_iSelectPLCNum < m_addrPLCDataBit.size() - 1)
		m_iSelectPLCNum++;

	SeteValuePLCReset();
	SetAddrData();
}


void CAddrView::OnClickPCPrev2()
{
	if (m_iSelectPCNum == 0)
		m_iSelectPCNum = m_addrPCDataBit.size() - 1;
	else if (m_iSelectPCNum > 0)
		m_iSelectPCNum--;

	SeteValuePCReset();
	SetAddrData();
}


void CAddrView::OnClickPCNext2()
{
	if (m_iSelectPCNum == m_addrPCDataBit.size() - 1)
		m_iSelectPCNum = 0;
	else if (m_iSelectPCNum < m_addrPCDataBit.size() - 1)
		m_iSelectPCNum++;

	SeteValuePCReset();
	SetAddrData();
}

void CAddrView::SeteValuePLCReset()
{
	for (int ii = 0; ii < 16; ii++)
	{
		m_addrBitPLCName[ii].SetCaption(_T(""));
		m_addrBitPLCValue[ii].SetCaption(_T(""));
		m_addrBitPLCAddr[ii].SetCaption(_T(""));
	}


	for (int ii = 0; ii < 8; ii++)
	{
		m_addrWordPLCName[ii].SetCaption(_T(""));
		m_addrWordPLCValue[ii].SetCaption(_T(""));
		m_addrWordPLCAddr[ii].SetCaption(_T(""));
	}
}

void CAddrView::SeteValuePCReset()
{
	for (int ii = 0; ii < 16; ii++)
	{
		m_addrBitPCName[ii].SetCaption(_T(""));
		m_addrBitPCValue[ii].SetCaption(_T(""));
		m_addrBitPCAddr[ii].SetCaption(_T(""));
	}


	for (int ii = 0; ii < 8; ii++)
	{
		m_addrWordPCName[ii].SetCaption(_T(""));
		m_addrWordPCValue[ii].SetCaption(_T(""));
		m_addrWordPCAddr[ii].SetCaption(_T(""));
	}
}

void CAddrView::StringChanged()
{
	StringChnageMsg(IDC_IN_PREV, _T("이전"), _T("PREVIOUS"), _T("以前的"));
	StringChnageMsg(IDC_IN_NEXT, _T("다음"), _T("NEXT"), _T("下一个"));
	StringChnageMsg(IDC_IN_PREV2, _T("이전"), _T("PREVIOUS"), _T("以前的"));
	StringChnageMsg(IDC_IN_NEXT2, _T("다음"), _T("NEXT"), _T("下一个"));
}

void CAddrView::StringChnageMsg(int btn, CString strKor, CString strEng, CString strChi)
{
	CString msg;
	switch (theApp.m_iLanguageSelect)
	{
	case KOR:msg = strKor; break;
	case ENG:msg = strEng; break;
	case CHI:msg = strChi; break;
	}
	((CBtnEnh*)GetDlgItem(btn))->SetCaption(msg);
}