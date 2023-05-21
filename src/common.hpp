#include <cstdlib>
#include <cstdio>
#include <stdexcept>

#define TEMPLATES "../templates"

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

class Graph {
		int xsize;
		int ysize;
	public:
		Vertex* graph;
		Vertex* start;
		Vertex* stop;

		int get_xsize();
		int get_ysize();
		Vertex* get_vertex(int, int); // Wskaźnik do wierzchołka (x, y): wspólrzędne
									  // zaczynają się od (0, 0) w lewym górnym rogu
		//void set_active_nodes(bool*); // Ustawia przeszkody na grafie
		
		Graph(int, int);
		Graph(FILE*);
		~Graph();
};

void solve_astar(Vertex* vertices, Vertex* start, Vertex* stop, int xsize, int ysize, bool diagonal_allowed, void (*callback)());

