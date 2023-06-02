#pragma once
#include <SDL2/SDL.h>

struct Application;

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
	virtual bool handle_event(SDL_Event&) { return false; }

	friend struct Application;
};

struct Button : public Widget
{
	bool m_enabled = true;
	bool m_hover = false;
	bool m_selected = false;

	int m_textureID = 0;
	int m_xpos = 0;
	int m_ypos = 0;

	void (*m_callback)(Application&) = nullptr;

	static int s_selTextureID;
	static int s_hovTextureID;
	static int s_disTextureID;

	Button() = default;
	Button(int txID, int x, int y, void (*callback)(Application&))
		: m_textureID(txID), m_xpos(x), m_ypos(y), m_callback(callback) {}

	~Button() = default;

	void draw() override;
	bool handle_event(SDL_Event& sdlEvent) override;
};

struct Text : public Widget
{
	int m_xpos = 0;
	int m_ypos = 0;
	int m_xsize = 0;
	int m_ysize = 0;

	SDL_Texture* m_text = nullptr;

	Text() = default;
	Text(int x, int y) : m_xpos(x), m_ypos(y) {}
	Text(int x, int y, const char* text) : m_xpos(x), m_ypos(y) { set(text); }
	~Text() { clear(); }

	void clear();
	void draw() override;
	void set(const char* text);
};
