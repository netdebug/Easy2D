#include "..\e2dbase.h"
#include "..\e2dtool.h"
#include "..\e2dmanager.h"

using namespace e2d;

e2d::autorelease_t const e2d::autorelease = e2d::autorelease_t();

void * operator new(size_t size, e2d::autorelease_t const &) E2D_NOEXCEPT
{
	void* p = ::operator new(size, std::nothrow);
	if (p)
	{
		GC::instance()->autorelease(static_cast<Ref*>(p));
	}
	return p;
}

void operator delete(void * block, e2d::autorelease_t const &) E2D_NOEXCEPT
{
	::operator delete (block, std::nothrow);
}


e2d::GC::GC()
	: _notifyed(false)
	, _cleanup(false)
	, _pool()
{
}

e2d::GC::~GC()
{
	// ɾ�����ж���
	this->clear();

	// ���ͼƬ����
	Image::clearCache();
}


void e2d::GC::flush()
{
	if (!_notifyed)
		return;

	_notifyed = false;
	for (auto iter = _pool.begin(); iter != _pool.end();)
	{
		if ((*iter)->refCount() <= 0)
		{
			delete (*iter);
			iter = _pool.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void e2d::GC::clear()
{
	_cleanup = true;

	Game::instance()->clearAllScenes();
	Timer::instance()->clearAllTasks();
	ActionManager::instance()->clearAll();

	for (const auto& ref : _pool)
	{
		delete ref;
	}
	_pool.clear();
	_cleanup = false;
}

e2d::GC * e2d::GC::instance()
{
	static GC _instance;
	return &_instance;
}

void e2d::GC::autorelease(Ref * ref)
{
	if (ref)
	{
		_pool.insert(ref);
	}
}

void e2d::GC::safeRelease(Ref* ref)
{
	if (_cleanup)
		return;

	if (ref)
	{
		ref->release();
		_notifyed = true;
	}
}
