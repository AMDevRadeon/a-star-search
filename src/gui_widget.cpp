#include "gui_widget.hpp"
#include "gui_application.hpp"

Application* Widget::s_appPointer = nullptr;
int Button::s_selTextureID = SELECTED;
int Button::s_hovTextureID = HOVER;
int Button::s_disTextureID = DISABLED;

void Button::draw()
{
	SDL_Rect src{ m_textureID * m_app.m_iconWidth, 0, m_app.m_iconWidth, m_app.m_iconHeight };
	const SDL_Rect dest{ m_xpos, m_ypos, m_app.m_iconWidth, m_app.m_iconHeight };

	SDL_RenderCopy(m_app.m_renderer, m_app.m_icons, &src, &dest);

	if (!m_enabled)
	{
		src.x = s_disTextureID * m_app.m_iconWidth;
		SDL_RenderCopy(m_app.m_renderer, m_app.m_icons, &src, &dest);
	}
	else
	{
		if (m_hover)
		{
			src.x = s_hovTextureID * m_app.m_iconWidth;
			SDL_RenderCopy(m_app.m_renderer, m_app.m_icons, &src, &dest);
		}

		if (m_selected)
		{
			src.x = s_selTextureID * m_app.m_iconWidth;
			SDL_RenderCopy(m_app.m_renderer, m_app.m_icons, &src, &dest);
		}
	}
}

bool Button::handle_event(SDL_Event& sdlEvent)
{
	bool ret = false;
	int xmouse, ymouse;
	SDL_GetMouseState(&xmouse, &ymouse);

	switch (sdlEvent.type)
	{
		case SDL_MOUSEBUTTONDOWN:
		if (m_enabled && m_hover && m_callback != nullptr)
		{
			(*m_callback)(m_app);
			ret = true;
		}
		break;

		case SDL_MOUSEMOTION:
		m_hover = xmouse >= m_xpos && xmouse < m_xpos + m_app.m_iconWidth && ymouse >= m_ypos && ymouse < m_ypos + m_app.m_iconHeight;
		ret = m_hover;
	}

	return ret;
}
