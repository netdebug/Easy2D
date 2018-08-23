#include "..\e2devent.h"
#include "..\e2dbase.h"

e2d::MouseEvent::MouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, float dpi)
	: _message(message)
	, _wParam(wParam)
	, _lParam(lParam)
	, _type(Type(message))
{
	_pos.x = ((float)(short)LOWORD(lParam)) * 96.f / dpi;
	_pos.y = ((float)(short)HIWORD(lParam)) * 96.f / dpi;
}

float e2d::MouseEvent::positionX() const
{
	return _pos.x;
}

float e2d::MouseEvent::positionY() const
{
	return _pos.y;
}

e2d::Point e2d::MouseEvent::position() const
{
	return _pos;
}

bool e2d::MouseEvent::isShiftDown() const
{
	return GET_KEYSTATE_WPARAM(_wParam) == MK_SHIFT;
}

bool e2d::MouseEvent::isCtrlDown() const
{
	return GET_KEYSTATE_WPARAM(_wParam) == MK_CONTROL;
}

float e2d::MouseEvent::wheelDelta() const
{
	return static_cast<float>(GET_WHEEL_DELTA_WPARAM(_wParam));
}

bool e2d::MouseEvent::isLButtonDown() const
{
	return GET_KEYSTATE_WPARAM(_wParam) == MK_LBUTTON;
}

bool e2d::MouseEvent::isRButtonDown() const
{
	return GET_KEYSTATE_WPARAM(_wParam) == MK_RBUTTON;
}

bool e2d::MouseEvent::isMButtonDown() const
{
	return GET_KEYSTATE_WPARAM(_wParam) == MK_MBUTTON;
}

e2d::MouseEvent::Type e2d::MouseEvent::type() const
{
	return _type;
}
