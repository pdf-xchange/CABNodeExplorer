#include "stdafx.h"
#include "CabInst.h"


CCabInst::CCabInst()
{
	m_Inst.CoCreateInstance(__uuidof(PXV::PXV_Inst));
	m_Inst->Init(nullptr, L"", nullptr, nullptr, nullptr, 0, nullptr);

	PXV::ICabNodePtr rootNode;
	m_Inst->get_Settings(&rootNode);
	m_rootNode.Attach(rootNode);
}


CCabInst::~CCabInst()
{
	m_Inst->Shutdown(0);
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

