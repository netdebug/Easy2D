#include "..\e2daction.h"
#include "..\e2dnode.h"


e2d::ScaleBy::ScaleBy(float duration, float scale)
	: FiniteTimeAction(duration)
{
	_deltaX = scale;
	_deltaY = scale;
}

e2d::ScaleBy::ScaleBy(float duration, float scaleX, float scaleY)
	: FiniteTimeAction(duration)
{
	_deltaX = scaleX;
	_deltaY = scaleY;
}

void e2d::ScaleBy::_init()
{
	FiniteTimeAction::_init();

	if (_target)
	{
		_startScaleX = _target->scaleX();
		_startScaleY = _target->scaleY();
	}
}

void e2d::ScaleBy::_update()
{
	FiniteTimeAction::_update();

	if (_target)
	{
		_target->scale(_startScaleX + _deltaX * _delta, _startScaleY + _deltaY * _delta);
	}
}

e2d::ScaleBy * e2d::ScaleBy::clone() const
{
	return new (e2d::autorelease) ScaleBy(_duration, _deltaX, _deltaY);
}

e2d::ScaleBy * e2d::ScaleBy::reverse() const
{
	return new (e2d::autorelease) ScaleBy(_duration, -_deltaX, -_deltaY);
}