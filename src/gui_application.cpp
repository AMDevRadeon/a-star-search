#include "gui_application.hpp"
#include "gui_callback.hpp"

void Application::draw()
{
	SDL_SetRenderTarget(m_renderer, nullptr);
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0xFF);
	SDL_RenderClear(m_renderer);

	for (Vertex& vertex : m_matrix)
	{
		int colorIndex = 0;

		if (!vertex.is_active)
			colorIndex = 1;
		else switch (vertex.state)
		{
			case OPEN:   colorIndex = 2; break;
			case CLOSED: colorIndex = 3; break;
			case PATH:   colorIndex = 4;
		}

		auto& color = g_colors[colorIndex];
		SDL_SetRenderDrawColor(m_renderer, std::get<0>(color), std::get<1>(color), std::get<2>(color), 0xFF);

		const float x = static_cast<float>(vertex.xpos * g_defaultVertexSize);
		const float y = static_cast<float>(vertex.ypos * g_defaultVertexSize);

		SDL_FRect rect{ x, y, g_defaultVertexSize, g_defaultVertexSize };
		m_viewport.rect_to_screen(rect, rect);

		rect.x += 1.0f; rect.w -= 2.0f;
		rect.y += 1.0f; rect.h -= 2.0f;

		if (rect.w <= 0)
			break;

		SDL_RenderFillRectF(m_renderer, &rect);
	}

	for (Widget* crr = m_widgetList; crr != nullptr; crr = crr -> m_next)
		crr -> draw();

	SDL_RenderPresent(m_renderer);
}

void Application::update()
{
	static bool s_isPressed = false;

	SDL_Event sdlEvent{};
	SDL_WaitEvent(&sdlEvent);

	if (sdlEvent.type == SDL_QUIT)
		m_isRunning = false;

	if (sdlEvent.type == SDL_KEYDOWN)
	{
		if (sdlEvent.key.keysym.sym == SDLK_TAB)
		{
			m_viewport.m_xoffset = 0;
			m_viewport.m_yoffset = 0;
			m_viewport.m_scale = m_defaultScale;
		}
	}
	else if (sdlEvent.type == SDL_MOUSEBUTTONDOWN)
		s_isPressed = true;
	else if (sdlEvent.type == SDL_MOUSEBUTTONUP)
		s_isPressed = false;

	bool handled = false;
	for (Widget* crr = m_widgetList; crr != nullptr; crr = crr -> m_next)
		handled |= crr -> handle_event(sdlEvent);

	if (handled)
		return;

	if (m_mode == B_CURSOR)
	{
		m_viewport.update(sdlEvent);
		return;
	}

	int xmouse, ymouse;

	SDL_GetMouseState(&xmouse, &ymouse);
	m_viewport.to_world(xmouse, ymouse, xmouse, ymouse);

	if (!s_isPressed || xmouse < 0 || ymouse < 0)
		return;

	xmouse /= g_defaultVertexSize;
	ymouse /= g_defaultVertexSize;

	if (xmouse >= m_matrixWidth || ymouse >= m_matrixHeight)
		return;

	const int index = ymouse * m_matrixWidth + xmouse;
	Vertex& vertex = m_matrix[index];

	switch (m_mode)
	{
		case B_ACTIVATE: vertex.is_active = true; break;
		case B_DEACTIVATE: vertex.is_active = false;
	}
}

void Application::load_icons()
{
	unload_icons();
	SDL_Surface* surface = IMG_Load(g_iconsPath);

	if (surface == nullptr)
	{
		m_isRunning = false; // TODO: Add exceptions
		return;
	}

	m_icons = SDL_CreateTextureFromSurface(m_renderer, surface);

	SDL_SetTextureBlendMode(m_icons, SDL_BLENDMODE_BLEND);
	SDL_FreeSurface(surface);
	SDL_QueryTexture(m_icons, nullptr, nullptr, &m_iconWidth, &m_iconHeight);

	m_iconCount = m_iconWidth / m_iconHeight;
	m_iconWidth = m_iconHeight;
}

void Application::unload_icons()
{
	if (m_icons != nullptr)
	{
		SDL_DestroyTexture(m_icons);
		m_icons = nullptr;
	}
}

void Application::create_matrix()
{
	m_matrix.clear();
	m_matrix.resize(m_matrixWidth * m_matrixHeight);

	int index = 0;
	for (int y = 0; y < m_matrixHeight; y++)
	{
		for (int x = 0; x < m_matrixWidth; x++)
		{
			m_matrix[index].xpos = x;
			m_matrix[index].ypos = y;
			index++;
		}
	}
}

void Application::create_main_window()
{
	destroy_window();
	constexpr auto pos = SDL_WINDOWPOS_UNDEFINED;

	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(0, &dm);

	const int xMaxRes = dm.w * g_xResFactor;
	const int yMaxRes = dm.h * g_yResFactor;

	int width = m_matrixWidth * g_defaultVertexSize;
	int height = m_matrixHeight * g_defaultVertexSize;

	m_defaultScale = 1.0f;

	if (width < g_xMinRes)
		width = g_xMinRes;
	else if (width > xMaxRes)
	{
		m_defaultScale = xMaxRes / static_cast<float>(width);
		width = xMaxRes;
	}

	if (height < g_yMinRes)
		height = g_yMinRes;
	else if (height > yMaxRes)
	{
		m_defaultScale = std::min(m_defaultScale, yMaxRes / static_cast<float>(height));
		height = yMaxRes;
	}

	height += g_windowExtraHeight;

	m_viewport.m_scale = m_defaultScale;
	m_window = SDL_CreateWindow("A* Pathfinding", pos, pos, width, height, 0);
	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);

	load_icons();

	int x = -m_iconWidth;
	int y = height - m_iconHeight;

	access_widget<Button>(B_PLAY, PLAY, x += m_iconWidth, y, &callback_play);
	access_widget<Button>(B_REPEAT, REPEAT, x += m_iconWidth, y, &callback_repeat);
	access_widget<Button>(B_CURSOR, CURSOR, x += m_iconWidth, y, &callback_cursor);
	access_widget<Button>(B_FLAG_START, FLAG_START, x += m_iconWidth, y, &callback_flag_start);
	access_widget<Button>(B_FLAG_STOP, FLAG_STOP, x += m_iconWidth, y, &callback_flag_stop);
	access_widget<Button>(B_ACTIVATE, ACTIVATE, x += m_iconWidth, y, &callback_activate);
	access_widget<Button>(B_DEACTIVATE, DEACTIVATE, x += m_iconWidth, y, &callback_deactivate);
	access_widget<Button>(B_RESIZE, RESIZE, x += m_iconWidth, y, &callback_resize);
	access_widget<Button>(B_RANDOMIZE, RANDOMIZE, x += m_iconWidth, y, &callback_randomize);
}

void Application::destroy_window()
{
	unload_icons();

	if (m_widgetList != nullptr)
	{
		delete m_widgetList;
		m_widgetList = nullptr;
	}

	if (m_renderer != nullptr)
	{
		SDL_DestroyRenderer(m_renderer);
		m_renderer = nullptr;
	}

	if (m_window != nullptr)
	{
		SDL_DestroyWindow(m_window);
		m_window = nullptr;
	}
}

void Application::refresh_mode()
{
	access_widget<Button>(B_CURSOR).m_selected = m_mode == B_CURSOR;
	access_widget<Button>(B_FLAG_START).m_selected = m_mode == B_FLAG_START;
	access_widget<Button>(B_FLAG_STOP).m_selected = m_mode == B_FLAG_STOP;
	access_widget<Button>(B_ACTIVATE).m_selected = m_mode == B_ACTIVATE;
	access_widget<Button>(B_DEACTIVATE).m_selected = m_mode == B_DEACTIVATE;
}

Application::Application()
{
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
}

Application::~Application()
{
	destroy_window();

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void Application::run()
{
	create_main_window();
	create_matrix();
	refresh_mode();

	while (m_isRunning)
	{
		update();
		draw();
	}
}
