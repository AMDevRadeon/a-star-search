#pragma once
#include <SDL2/SDL.h>

class Application;

class Widget
{
	protected:
	Application& m_app;

	private:
	int m_id = -1;
	Widget* m_next;
	static Application* s_appPointer;

	protected:
	Widget() : m_app(*s_appPointer) {}

	public:
	virtual ~Widget() { delete m_next; }

	virtual void draw() {}
	virtual void handle_event(SDL_Event&) {}

	friend class Application;
};

struct Button : public Widget
{
	bool m_enabled = true;
	bool m_hover = false;
	bool m_selected = false;

	int m_xpos = 0;
	int m_ypos = 0;
	int m_textureID = 0;

	void (*m_callback)(Application&) = nullptr;

	static int s_selTextureID;
	static int s_hovTextureID;
	static int s_disTextureID;

	Button() = default;
	~Button() = default;

	void draw() override;
	void handle_event(SDL_Event& sdlEvent) override;
};
