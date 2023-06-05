#include "gui_application.hpp"
#include "gui_callback.hpp"

#include <iostream>
#include <stdexcept>

void Application::draw()
{
	SDL_SetRenderTarget(m_renderer, nullptr);
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0xFF);
	SDL_RenderClear(m_renderer);

	for (Vertex& vertex : m_matrix)
	{
		int colorIndex;

		if (!vertex.is_active)
			colorIndex = 1;
		else switch (vertex.state)
		{
			case OPEN:   colorIndex = 2; break;
			case CLOSED: colorIndex = 3; break;
			case PATH:   colorIndex = 4; break;
			default:     colorIndex = 0;
		}

		auto& color = g_colors[colorIndex];
		SDL_SetRenderDrawColor(m_renderer, std::get<0>(color), std::get<1>(color), std::get<2>(color), 0xFF);

		const float x = static_cast<float>(vertex.xpos * g_vertexSize);
		const float y = static_cast<float>(vertex.ypos * g_vertexSize);

		SDL_FRect rect{ x, y, g_vertexSize, g_vertexSize };
		m_viewport.rect_to_screen(rect, rect);

		if (rect.w >= g_gridThreshold)
		{
			rect.x += 1.0f; rect.w -= 2.0f;
			rect.y += 1.0f; rect.h -= 2.0f;
		}

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

	SDL_Rect rect{ 0, m_windowHeight - m_iconHeight, m_windowWidth, m_iconHeight };
	SDL_SetRenderDrawColor(m_renderer, g_fontBgColor.r, g_fontBgColor.g, g_fontBgColor.b, g_fontBgColor.a);
	SDL_RenderFillRect(m_renderer, &rect);

	for (Widget* crr = m_widgetList; crr != nullptr; crr = crr->m_next)
		crr->draw();

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

	static const char* s_prevDescription = nullptr;
	m_description = nullptr;

	bool handled = false;
	for (Widget* crr = m_widgetList; crr != nullptr; crr = crr->m_next)
		handled |= crr->handle_event(sdlEvent);

	if (s_prevDescription != m_description)
	{
		s_prevDescription = m_description;
		access_widget<Text>(T_DESCRIPTION).set(m_description);
	}

	if (m_execute != nullptr)
	{
		void (*ptr)(Application&) = m_execute;
		m_execute = nullptr;

		(*ptr)(*this);
		refresh_buttons();
	}

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

	xmouse /= g_vertexSize;
	ymouse /= g_vertexSize;

	if (xmouse >= m_matrixWidth || ymouse >= m_matrixHeight)
		goto next_event;

	Vertex& vertex = m_matrix[ymouse * m_matrixWidth + xmouse];
	const bool isFlag = &vertex == m_vertStart || &vertex == m_vertStop;

	switch (m_mode)
	{
		case B_FLAG_START:
		if (vertex.is_active && !isFlag)
			{ m_vertStart = &vertex; refresh_buttons(); }
		break;

		case B_FLAG_STOP:
		if (vertex.is_active && !isFlag)
			{ m_vertStop = &vertex; refresh_buttons(); }
		break;

		case B_ACTIVATE: vertex.is_active = true; break;
		case B_DEACTIVATE: vertex.is_active = isFlag;
	}

	goto next_event;
}

void Application::load_font()
{
	unload_font();
	m_font = TTF_OpenFont(g_fontPath, g_fontSize);

	if (m_font == nullptr)
		m_isRunning = false; // TODO: Add exceptions
}

void Application::unload_font()
{
	if (m_font != nullptr)
	{
		TTF_CloseFont(m_font);
		m_font = nullptr;
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
	m_vertStart = nullptr;
	m_vertStop = nullptr;
	m_graph.graph = nullptr;

	m_matrix.clear();
	m_graph.util_uninit();

	try
	{
		m_graph.init_by_file_template(g_defaultTemplatePath);

		m_matrixWidth = m_graph.get_xsize();
		m_matrixHeight = m_graph.get_ysize();

		m_matrix.resize(m_matrixWidth * m_matrixHeight);

		int i = 0;
		for (int y = 0; y < m_matrixHeight; y++)
			for (int x = 0; x < m_matrixWidth; x++)
				m_matrix[i++] = *m_graph.get_vertex(x, y);

		if (m_graph.start != nullptr)
			m_vertStart = m_matrix.data() + m_graph.start->ypos * m_matrixWidth + m_graph.start->xpos;

		if (m_graph.stop != nullptr)
			m_vertStop = m_matrix.data() + m_graph.stop->ypos * m_matrixWidth + m_graph.stop->xpos;

		if (m_vertStart == m_vertStop)
			m_vertStop = nullptr;
	}
	catch (const std::exception& error)
	{
		std::cerr << error.what() << '\n';
		m_matrix.resize(m_matrixWidth * m_matrixHeight);

		int i = 0;
		for (int y = 0; y < m_matrixHeight; y++)
			for (int x = 0; x < m_matrixWidth; x++)
				m_matrix[i++].SetPos(x, y);
	}

	m_graph.util_uninit();
	m_graph.graph = m_matrix.data();
	m_graph.set_size(m_matrixWidth, m_matrixHeight);
}

void Application::create_main_window()
{
	destroy_window();
	constexpr auto pos = SDL_WINDOWPOS_UNDEFINED;

	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(0, &dm);

	int width = m_matrixWidth * g_vertexSize;
	int height = m_matrixHeight * g_vertexSize;

	const float xmax = static_cast<float>(dm.w * g_xResFactor);
	const float ymax = static_cast<float>(dm.h * g_yResFactor);
	const float scale = std::min(1.0f, std::min(xmax / width, ymax / height));

	width *= scale;
	height *= scale;

	if (width < g_xMinRes)
		width = g_xMinRes;

	if (height < g_yMinRes)
		height = g_yMinRes;

	height += g_windowExtraHeight;

	m_windowWidth = width;
	m_windowHeight = height;
	m_defaultScale = scale;
	m_viewport.m_scale = scale;

	m_window = SDL_CreateWindow(g_windowTitle, pos, pos, width, height, 0);
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
	access_widget<Button>(B_DIAGONAL, DIAGONAL, x += m_iconWidth, y, &callback_diagonal);
	access_widget<Button>(B_ASTAR, ASTAR, x += m_iconWidth, y, &callback_sel_astar);
	access_widget<Button>(B_DIJKSTRA, DIJKSTRA, x += m_iconWidth, y, &callback_sel_dijkstra);
	access_widget<Text>(T_DESCRIPTION, x += m_iconWidth + g_descriptionXOffset, y += g_descriptionYOffset);
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
	Button& diag    = access_widget<Button>(B_DIAGONAL);
	Button& astar   = access_widget<Button>(B_ASTAR);
	Button& dijk    = access_widget<Button>(B_DIJKSTRA);

	play.m_textureID = m_isSolving ? STOP : PLAY;
	play.m_enabled = m_vertStart != nullptr && m_vertStop != nullptr;

	const bool b = !m_isSolving;

	reset.m_enabled   = b;
	cursor.m_enabled  = b;
	fstart.m_enabled  = b;
	fstop.m_enabled   = b;
	activ.m_enabled   = b;
	deactiv.m_enabled = b;
	resize.m_enabled  = b;
	random.m_enabled  = b;
	load.m_enabled    = b;
	diag.m_enabled    = b;
	astar.m_enabled   = b;
	dijk.m_enabled    = b;

	cursor.m_selected  = m_mode == B_CURSOR;
	fstart.m_selected  = m_mode == B_FLAG_START;
	fstop.m_selected   = m_mode == B_FLAG_STOP;
	activ.m_selected   = m_mode == B_ACTIVATE;
	deactiv.m_selected = m_mode == B_DEACTIVATE;

	diag.m_selected    = m_isDiagonal;
	astar.m_selected   = !m_isDijkstra;
	dijk.m_selected    = m_isDijkstra;
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
	unload_font();

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();

	m_graph.graph = nullptr;
}

void Application::run()
{
	load_font();
	create_matrix();
	create_main_window();
	refresh_buttons();

	while (m_isRunning)
	{
		update();
		draw();
	}
}
