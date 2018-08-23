#include "..\e2daction.h"
#include "..\e2dnode.h"


e2d::MoveBy::MoveBy(float duration, Vector2 vector)
	: FiniteTimeAction(duration)
{
	_deltaPos = vector;
}

void e2d::MoveBy::_init()
{
	FiniteTimeAction::_init();

	if (_target)
	{
		_prevPos = _startPos = _target->position();
	}
}

void e2d::MoveBy::_update()
{
	FiniteTimeAction::_update();

	if (_target)
	{
		Point currentPos = _target->position();
		Vector2 diff = currentPos - _prevPos;
		_startPos = _startPos + diff;

		Point newPos = _startPos + (_deltaPos * _delta);
		_target->position(newPos);

		_prevPos = newPos;
	}
}

e2d::MoveBy * e2d::MoveBy::clone() const
{
	return new (e2d::autorelease) MoveBy(_duration, _deltaPos);
}

e2d::MoveBy * e2d::MoveBy::reverse() const
{
	return new (e2d::autorelease) MoveBy(_duration, -_deltaPos);
}