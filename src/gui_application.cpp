#include "gui_application.hpp"

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
			case PATH:   colorIndex = 4; break;
		}

		auto color = g_colors[colorIndex];
		SDL_SetRenderDrawColor(m_renderer, std::get<0>(color), std::get<1>(color), std::get<2>(color), 0xFF);

		SDL_Rect rect{ vertex.xpos * g_defaultVertexSize, vertex.ypos * g_defaultVertexSize, g_defaultVertexSize, g_defaultVertexSize };
		m_viewport.rect_to_screen(rect, rect);
		SDL_RenderFillRect(m_renderer, &rect);
	}

	SDL_RenderPresent(m_renderer);
}

void Application::update()
{
	SDL_Event sdlEvent{};
	SDL_WaitEvent(&sdlEvent);

	if (sdlEvent.type == SDL_QUIT)
		m_isRunning = false;

	m_viewport.update(sdlEvent);
	draw();
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
			m_matrix[index].state = static_cast<State>(index % 4); // TODO: Remove!
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

	if (width < g_xMinRes)
		width = g_xMinRes;
	else if (width > xMaxRes)
		width = xMaxRes;

	if (height < g_yMinRes)
		height = g_yMinRes;
	else if (height > yMaxRes)
		height = yMaxRes;

	m_window = SDL_CreateWindow("A* Pathfinding", pos, pos, width, height, 0);
	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
}

void Application::destroy_window()
{
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

	while (m_isRunning) update();
}
