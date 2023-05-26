#pragma once

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <vector>
#include <queue>
#include <algorithm>
#include <cmath>
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

		/**
		 * Inicjalizuje obiekt Graph pobierając dane z wzorca 
		 * umieszczonego w pliku o podanej ścieżce.
		 * 
		 * @param {char*} filepath - Ścieżka do pliku
		 */
		void init_by_file_template(const char* filepath);
		
		/**
		 * Inicjalizuje obiekt Graph pobierając dane otrzymane 
		 * z interfejsu graficznego.
		 * 
		 * @param {int} xsize - Szerokość grafu,
		 * @param {int} ysize - Wysokość grafu,
		 * @param {bool*} activity_setup - Wskaźnik do tablicy o rozmiarach grafu zawierającej informację o układzie przeszkód,
		 * @param {int} xcoord_start, ycoord_start, xcoord_stop, ycoord_stop - Współrzędne (x,y) punktu początkowego i końcowego działania algorytmu.
		 */
		void init_by_user_interface(int xsize, int ysize, bool* activity_setup, int xcoord_start, int ycoord_start,
																				int xcoord_stop, int ycoord_stop);
		void init_by_random();

		/**
		 * Pobierz szerokość grafu.
		 */
		int get_xsize();

		/**
		 * Pobierz wysokość grafu.
		 */
		int get_ysize();

		/**
		 * Zwraca wskaźnik do wierzchołka obiektu Graph o podanych współrzędnych.
		 * Numerowanie zaczyna się od lewego górnego rogu od współrzędnych (0,0).
		 * @param {int} xcoord - Współrzędna x wierzchołka,
		 * @param {int} ycoord - Współrzędna y wierzchołka.
		 * @returns Vertex* - Zwraca wskażnik jeżeli wspołrzędne są w zakresie [0, xsize) i [0, ysize). Zwraca nullptr w przeciwnym wypadku.
		 */
		Vertex* get_vertex(int xcoord, int ycoord);
		
		/**
		 * Sprawdź, czy graf został już zainicjowany.
		 */
		int util_check_init();

		/**
		 * Sprowadź graf do stanu sprzed inicjalizacji
		 */
		void util_uninit();
		
		Graph();
		~Graph();
};

void solve_astar(Graph& graph, bool diagonal_allowed, void (*callback)());

void DEBUG_callback_astar();
