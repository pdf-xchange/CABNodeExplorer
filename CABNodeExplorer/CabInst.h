#pragma once
class CCabInst
{
public:
	CCabInst();
	~CCabInst();


	t_pInst m_Inst;
	_cab_node_t m_rootNode;

	int TreeItemImage(_cab_node_t& pNode);
	int TreeItemSelectedImage(_cab_node_t& pNode);

	bool CanSelect();
	CStringW GetSelect();
	void SetSelect(LPCWSTR str);
	CStringW m_strSelectPath;
};

