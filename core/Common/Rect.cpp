#include "..\e2dcommon.h"

e2d::Rect::Rect(void)
	: origin()
	, size()
{
}

e2d::Rect::Rect(float x, float y, float width, float height)
	: origin(x, y)
	, size(width, height)
{
}

e2d::Rect::Rect(const Point& pos, const Size& size)
	: origin(pos)
	, size(size)
{
}

e2d::Rect::Rect(const Rect& other)
	: origin(other.origin)
	, size(other.size)
{
}

e2d::Rect& e2d::Rect::operator= (const Rect& other)
{
	origin = other.origin;
	size = other.size;
	return *this;
}

bool e2d::Rect::operator==(const Rect & rect) const
{
	return (origin == rect.origin) && (size == rect.size);
}

bool e2d::Rect::containsPoint(const Point& point) const
{
	if (point.x >= origin.x && point.x <= (origin.y + size.height)
		&& point.y >= origin.y && point.y <= (origin.y + size.height))
	{
		return true;
	}
	return false;
}

bool e2d::Rect::intersects(const Rect& rect) const
{
	return !((origin.x + size.width)			< rect.origin.x ||
			(rect.origin.x + rect.size.width)	<      origin.x ||
			(origin.y + size.height)			< rect.origin.y ||
			(rect.origin.y + rect.size.height)	<      origin.y);
}
