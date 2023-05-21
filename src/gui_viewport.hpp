#pragma once
#include <SDL2/SDL.h>

struct Viewport
{
	float m_scale = 1.0f;
	float m_xoffset = 0;
	float m_yoffset = 0;

	template <typename WRectType, typename SRectType>
	void rect_to_screen(const WRectType& from, SRectType& to) const;

	template <typename SType, typename WType>
	void to_world(SType xScreen, SType yScreen, WType& xWorld, WType& yWorld) const;

	template <typename WType, typename SType>
	void to_screen(WType xWorld, WType yWorld, SType& xScreen, SType& yScreen) const;
};

template <typename WRectType, typename SRectType>
void Viewport::rect_to_screen(const WRectType& from, SRectType& to) const
{
	to.x = static_cast<decltype(to.x)>((from.x - m_xoffset) * m_scale);
	to.y = static_cast<decltype(to.y)>((from.y - m_yoffset) * m_scale);

	to.w = static_cast<decltype(to.w)>(from.w * m_scale);
	to.h = static_cast<decltype(to.h)>(from.h * m_scale);
}

template <typename SType, typename WType>
void Viewport::to_world(SType xScreen, SType yScreen, WType& xWorld, WType& yWorld) const
{
	xWorld = static_cast<WType>(xScreen / m_scale + m_xoffset);
	yWorld = static_cast<WType>(yScreen / m_scale + m_yoffset);
}

template <typename WType, typename SType>
void Viewport::to_screen(WType xWorld, WType yWorld, SType& xScreen, SType& yScreen) const
{
	xScreen = static_cast<SType>((xWorld - m_xoffset) * m_scale);
	yScreen = static_cast<SType>((yWorld - m_yoffset) * m_scale);
}
