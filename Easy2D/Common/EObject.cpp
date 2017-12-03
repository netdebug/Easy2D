#include "..\ebase.h"
#include "..\emanagers.h"

e2d::EObject::EObject()
	: m_nRefCount(0)
	, m_bManaged(false)
{
	EObjectManager::add(this);	// ���ö�������ͷų���
}

e2d::EObject::~EObject()
{
}

// ���ü�����һ
void e2d::EObject::retain()
{
	m_nRefCount++;
}

// ���ü�����һ
void e2d::EObject::release()
{
	m_nRefCount--;
	// ֪ͨ���������ˢ��
	EObjectManager::notifyFlush();
}