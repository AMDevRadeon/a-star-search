#include "gui_viewport.hpp"

bool Viewport::update(SDL_Event& sdlEvent)
{
	int xmouse, ymouse;
	SDL_GetMouseState(&xmouse, &ymouse);

	switch (sdlEvent.type)
	{
	case SDL_MOUSEBUTTONDOWN:
		m_xstart = xmouse;
		m_ystart = ymouse;
		m_pressed = true;
		break;

	case SDL_MOUSEBUTTONUP:
		m_pressed = false;
		break;

	case SDL_MOUSEMOTION:
		if (!m_pressed) return false;
		move(xmouse, ymouse);
		break;

	case SDL_MOUSEWHEEL:
		scale(xmouse, ymouse, static_cast<float>(sdlEvent.wheel.y));
		break;

	default:
		return false;
	}

	return true;
}

void Viewport::move(int xmouse, int ymouse)
{
	m_xoffset += (m_xstart - xmouse) / m_scale;
	m_yoffset += (m_ystart - ymouse) / m_scale;

	m_xstart = xmouse;
	m_ystart = ymouse;
}

void Viewport::scale(int xmouse, int ymouse, float factor)
{
	const float before = 1.0f / m_scale;
	m_scale *= (g_scaleStep * factor) + 1;

	const float diff = before - 1.0f / m_scale;

	m_xoffset += xmouse * diff;
	m_yoffset += ymouse * diff;
}
