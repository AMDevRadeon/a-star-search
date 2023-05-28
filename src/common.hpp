#pragma once

enum State
{
	AVAILABLE, // dystans nieskonczony
	OPEN,      // dystans okreslony
	CLOSED,    // wezel wylaczony z algorytmu
	PATH       // droga powrotna
};

struct Vertex
{
	int xpos = 0;
	int ypos = 0;

	double fcost = 0; // gcost + hcost
	double gcost = 0; // dystans od poczatku
	double hcost = 0; // heurystyka

	bool is_active = true;
	Vertex* previous = nullptr;
	State state = AVAILABLE;

	void Reset();
};

inline void Vertex::Reset()
{
	fcost = 0;
	gcost = 0;
	hcost = 0;

	is_active = true;
	previous = nullptr;
	state = AVAILABLE;
}

void solve_astar(Vertex* vertices, Vertex* start, Vertex* stop, int xsize, int ysize, bool diagonal_allowed, void (*callback)());

