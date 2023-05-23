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
		if (&vertex == m_vertStart || &vertex == m_vertStop)
		{
			const int index = &vertex == m_vertStart ? FLAG_START_2 : FLAG_STOP_2;
			const SDL_Rect src{ index * m_iconWidth, 0, m_iconWidth, m_iconHeight };

			rect.x -= 1.0f; rect.w += 2.0f;
			rect.y -= 1.0f; rect.h += 2.0f;

			SDL_RenderCopyF(m_renderer, m_icons, &src, &rect);
		}
	}

	for (Widget* crr = m_widgetList; crr != nullptr; crr = crr -> m_next)
		crr -> draw();

	SDL_RenderPresent(m_renderer);
}

void Application::update()
{
	static bool s_isPressed = false;

	bool pending;
	SDL_Event sdlEvent{};

	if (m_isSolving)
	{
		next_event:
		pending = SDL_PollEvent(&sdlEvent);
	}
	else
		pending = SDL_WaitEvent(&sdlEvent);

	if (!pending)
		return;

	if (sdlEvent.type == SDL_QUIT)
	{
		m_isRunning = false;
		m_isSolving = false;
		return;
	}

	if (sdlEvent.type == SDL_KEYDOWN && !m_isSolving)
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

	if (handled || m_isSolving)
		goto next_event;

	if (m_mode == B_CURSOR)
	{
		m_viewport.update(sdlEvent);
		goto next_event;
	}

	int xmouse, ymouse;

	SDL_GetMouseState(&xmouse, &ymouse);
	m_viewport.to_world(xmouse, ymouse, xmouse, ymouse);

	if (!s_isPressed || xmouse < 0 || ymouse < 0)
		goto next_event;

	xmouse /= g_defaultVertexSize;
	ymouse /= g_defaultVertexSize;

	if (xmouse >= m_matrixWidth || ymouse >= m_matrixHeight)
		goto next_event;

	Vertex& vertex = m_matrix[ymouse * m_matrixWidth + xmouse];
	const bool isFlag = &vertex == m_vertStart || &vertex == m_vertStop;

	switch (m_mode)
	{
		case B_FLAG_START:
		if (vertex.is_active && !isFlag)
			m_vertStart = &vertex;
		break;

		case B_FLAG_STOP:
		if (vertex.is_active && !isFlag)
			m_vertStop = &vertex;
		break;

		case B_ACTIVATE: vertex.is_active = true; break;
		case B_DEACTIVATE: vertex.is_active = isFlag;
	}

	goto next_event;
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
	access_widget<Button>(B_RESET, RESET, x += m_iconWidth, y, &callback_reset);
	access_widget<Button>(B_CURSOR, CURSOR, x += m_iconWidth, y, &callback_cursor);
	access_widget<Button>(B_FLAG_START, FLAG_START, x += m_iconWidth, y, &callback_flag_start);
	access_widget<Button>(B_FLAG_STOP, FLAG_STOP, x += m_iconWidth, y, &callback_flag_stop);
	access_widget<Button>(B_ACTIVATE, ACTIVATE, x += m_iconWidth, y, &callback_activate);
	access_widget<Button>(B_DEACTIVATE, DEACTIVATE, x += m_iconWidth, y, &callback_deactivate);
	access_widget<Button>(B_RESIZE, RESIZE, x += m_iconWidth, y, &callback_resize);
	access_widget<Button>(B_RANDOMIZE, RANDOMIZE, x += m_iconWidth, y, &callback_randomize);
	access_widget<Button>(B_LOAD, LOAD, x += m_iconWidth, y, &callback_load);
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

void Application::refresh_buttons()
{
	Button& play    = access_widget<Button>(B_PLAY);
	Button& reset   = access_widget<Button>(B_RESET);
	Button& cursor  = access_widget<Button>(B_CURSOR);
	Button& fstart  = access_widget<Button>(B_FLAG_START);
	Button& fstop   = access_widget<Button>(B_FLAG_STOP);
	Button& activ   = access_widget<Button>(B_ACTIVATE);
	Button& deactiv = access_widget<Button>(B_DEACTIVATE);
	Button& resize  = access_widget<Button>(B_RESIZE);
	Button& random  = access_widget<Button>(B_RANDOMIZE);
	Button& load    = access_widget<Button>(B_LOAD);

	const bool b = !m_isSolving;
	play.m_textureID = m_isSolving ? STOP : PLAY;

	reset.m_enabled   = b;
	cursor.m_enabled  = b;
	fstart.m_enabled  = b;
	fstop.m_enabled   = b;
	activ.m_enabled   = b;
	deactiv.m_enabled = b;
	resize.m_enabled  = b;
	random.m_enabled  = b;
	load.m_enabled    = b;

	cursor.m_selected  = m_mode == B_CURSOR;
	fstart.m_selected  = m_mode == B_FLAG_START;
	fstop.m_selected   = m_mode == B_FLAG_STOP;
	activ.m_selected   = m_mode == B_ACTIVATE;
	deactiv.m_selected = m_mode == B_DEACTIVATE;
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
	refresh_buttons();

	while (m_isRunning)
	{
		update();
		draw();
	}
}
