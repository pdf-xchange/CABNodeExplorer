#include "stdafx.h"
#include "CabInst.h"


CCabInst::CCabInst()
{
	m_Inst.CoCreateInstance(__uuidof(PXV::PXV_Inst));

	IUnknownPtr pEx;
	m_Inst->GetExtension(L"AFS", &pEx);
	pEx->QueryInterface(&m_AFSInst);
	pEx = nullptr;

	PXV::IStringPtr prefsPathStr;
	m_Inst->CreateString(L"HKCU\\Software\\MyApp\\PDFXEdit", &prefsPathStr);

	m_Inst->Init(nullptr, L"", prefsPathStr, nullptr, nullptr, 0, nullptr);

	PXV::ICabNodePtr rootNode;
	m_Inst->get_Settings(&rootNode);
	m_rootNode.Attach(rootNode);
}


CCabInst::~CCabInst()
{
	m_Inst->Shutdown(0);
}

void CCabInst::SettLoad(LPCWSTR strFile)
{
	if (!m_Inst)
		return;

	//m_strFile = strFile;
	HRESULT hr = 0;
	do
	{
		long nID = 0;
		hr = m_Inst->Str2ID(L"op.settings.import", FALSE, &nID);
		BreakOnFailure0(hr);
		PXV::IOperationPtr op;
		hr = m_Inst->CreateOp(nID, &op);
		BreakOnFailure0(hr);
		PXV::ICabPtr rootNode;
		op->get_Params(&rootNode);
		_cab_node_t rootParams(rootNode);
		rootParams[L"Options.History"] = false;
		PXV::IAFS_FileSysPtr pFs;
		m_AFSInst->get_DefaultFileSys(&pFs);
		PXV::IAFS_NamePtr pName;
		pFs->StringToName((LPWSTR)strFile, 0, nullptr, &pName);
		rootParams[L"Input"] = (IUnknown*)pName;
		hr = op->Do(0);
		BreakOnFailure0(hr);
	} while (false);
	ATLASSERT(hr == S_OK);
}

int CCabInst::TreeItemImage(_cab_node_t& pNode)
{
	return 0;
}

int CCabInst::TreeItemSelectedImage(_cab_node_t& pNode)
{
	return 0;
}

bool CCabInst::CanSelect()
{
	return !m_strSelectPath.IsEmpty();
}

CStringW CCabInst::GetSelect()
{
	return m_strSelectPath;
}

void CCabInst::SetSelect(LPCWSTR str)
{
	m_strSelectPath = str;
}

