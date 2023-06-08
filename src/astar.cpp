#include "common.hpp"

static const int movement[8][2] = {{0,1},{-1,0},{1,0},{0,-1},{-1,1},{1,1},{-1,-1},{1,-1}};
static const double dist[8] = {1.0, 1.0, 1.0, 1.0, 1.414213, 1.414213, 1.414213, 1.414213};

static void file_check_line_comment(FILE* fhook, const char comm) {
        char curr_char;

        curr_char = fgetc(fhook);
        while (curr_char == comm || curr_char == '\n') {
            while (curr_char != '\n' && curr_char != EOF) {
		        curr_char = fgetc(fhook);
		    }
            curr_char = fgetc(fhook);
        }
        ungetc(curr_char, fhook);
}

#ifdef ASTAR_DEBUG
void DEBUG_graphall_print(Graph &graph, Vertex *&vertex, Vertex *&neighbour_vertex) {
    int xsize = graph.get_xsize(), ysize = graph.get_ysize();
    Vertex* curr_vertex;
    /*
    printf("%d %d\n", xsize, ysize);
    for (int i = 0; i < ysize; i++) {
        for (int j = 0; j < xsize; j++) {
            curr_vertex = graph.get_vertex(j,i);
            if (curr_vertex->is_active) {
                printf("#");
            }
            else {
                printf("v");
            }
        }
        printf("\n");
    }
    */
    printf("\n");
    for (int i = 0; i < ysize; i++) {
        for (int j = 0; j < xsize; j++) {
            curr_vertex = graph.get_vertex(j,i);
            if (curr_vertex == neighbour_vertex) {
                printf("!");
            }
            else if (curr_vertex == graph.start) {
                printf("%%");
            }
            else if (curr_vertex == graph.stop) {
                printf("$");
            }
            else if (curr_vertex == vertex) {
                printf("*");
            }
            else if (!curr_vertex->is_active) {
                printf("@");
            }
            else if (curr_vertex->state == AVAILABLE) {
                printf("-");
            }
            else if (curr_vertex->state == OPEN) {
                printf("x");
            }
            else if (curr_vertex->state == CLOSED) {
                printf(":");
            }
            else if (curr_vertex->state == PATH) {
                printf("~");
            }
        }
        printf("\n");
    }
    printf("curr->fcost: %lf\n", vertex->fcost);
    printf("curr->gcost: %lf\n", vertex->gcost);
    printf("curr->hcost: %lf\n", vertex->hcost);
    printf("neigh->curr: (%d, %d)\n", neighbour_vertex->xpos, neighbour_vertex->ypos);
    printf("neigh->fcost: %lf\n", neighbour_vertex->fcost);
    printf("curr: (%d, %d)\n", vertex->xpos, vertex->ypos);
    if (vertex->previous) printf("curr->prev: (%d, %d)\n", vertex->previous->xpos, vertex->previous->ypos);

/*    
    for (int i = 0; i < ysize; i++) {
        for (int j = 0; j < xsize; j++) {
            curr_vertex = graph.get_vertex(j,i);
            printf("(%d, %d), ", curr_vertex->xpos, curr_vertex->ypos);
        }
        printf("\n");
    }

    printf("start:(%d %d), stop(%d, %d)", graph.start->xpos, graph.start->ypos, graph.stop->xpos, graph.stop->ypos);
    */
}

void DEBUG_graph_printwall(Graph &graph) {
    int xsize = graph.get_xsize(), ysize = graph.get_ysize();
    Vertex* curr_vertex;
    printf("%d %d\n", xsize, ysize);
    for (int i = 0; i < ysize; i++) {
        for (int j = 0; j < xsize; j++) {
            curr_vertex = graph.get_vertex(j,i);
            if (curr_vertex == graph.start) {
                printf("%%");
            }
            else if (curr_vertex == graph.stop) {
                printf("$");
            }
            else if (!curr_vertex->is_active) {
                printf("@");
            }
            else {
                printf("-");
            }
        }
        printf("\n");
    }
}


void DEBUG_callback_astar() {
    // char n;
    // scanf(" %c", &n);

    // printf("///////////////////////////////////////////////////////////////////\n");
    // printf("/=================================================================/\n");
    // printf("///////////////////////////////////////////////////////////////////\n");
}
#endif

Vertex* Graph::get_vertex(int xcoord, int ycoord) {
    if (xcoord < 0 || ycoord < 0 || xcoord >= this->xsize || ycoord >= this->ysize) {
        return (Vertex*)nullptr;
    }
    return &(this->graph[ycoord * this->xsize + xcoord]);
}

int Graph::get_xsize() {
    return this->xsize;
}

int Graph::get_ysize() {
    return this->ysize;
}

Graph::Graph() {
    this->xsize = -1;
    this->ysize = -1;
    this->graph = (Vertex*)nullptr;
    this->start = (Vertex*)nullptr;
    this->stop = (Vertex*)nullptr;
}

int Graph::util_check_init() {
    if (this->xsize < 1 || this->ysize < 1 || this->graph == (Vertex*)nullptr || this->start == (Vertex*)nullptr || this->stop == (Vertex*)nullptr) {
        throw std::runtime_error("INTERNAL ERROR: uninitialized Graph object.");
        return 0;
    }

    return 1;
}

void Graph::util_uninit() {
    this->xsize = -1;
    this->ysize = -1;
    if (this->graph) { 
        delete[] this->graph;
        this->graph = (Vertex*)nullptr;
    }
    this->start = (Vertex*)nullptr;
    this->stop = (Vertex*)nullptr;
}

void Graph::util_clear_pathfinding() {
    this->util_check_init();
    int i, j;

    for (i = 0; i < this->ysize; i++) {
        for (j = 0; j < this->xsize; j++) {
            this->get_vertex(j, i)->SoftReset();
        }
    }
}

void Graph::init_by_file_template(const char* filepath) {
    FILE* fhook;

    fhook = fopen(filepath, "r");

    if (fhook) {
        int i, j;
        char curr_char;
        Vertex* curr_vertex;

        this->util_uninit();

        file_check_line_comment(fhook, '#');
        if (feof(fhook)) throw std::runtime_error("FILE ERROR: Bad formatting. No template header.");
        fscanf(fhook, "%d%d", &(this->xsize), &(this->ysize));

        if (feof(fhook)) throw std::runtime_error("FILE ERROR: Bad formatting. No template body.");
        this->graph = new Vertex[(size_t)this->xsize * this->ysize];

        for (i = 0; i < this->ysize; i++) {
            file_check_line_comment(fhook, '#');
            if (feof(fhook)) throw std::runtime_error("FILE ERROR: Bad formatting. Defective body.");

            for (j = 0; j < this->xsize; j++) {
                curr_vertex = this->get_vertex(j,i);
                curr_vertex->xpos = j;
                curr_vertex->ypos = i;
                curr_char = fgetc(fhook);

                if (curr_char == '\n') throw std::runtime_error("FILE ERROR: Bad formatting. Defective body, unexpected line end.");

                switch (curr_char) {
                case '0':
                case '@':
                    curr_vertex->is_active = false;
                    break;
                case '1':
                case '-':
                    curr_vertex->is_active = true;
                    break;
                default:
                    throw std::runtime_error("FILE ERROR: Bad formatting. Unexpected symbol in template body.");
                    break;
                }
            }
            fgetc(fhook);
            if (feof(fhook)) throw std::runtime_error("FILE ERROR: Bad formatting. Defective body.");
        }

        file_check_line_comment(fhook, '#');
        if (feof(fhook)) throw std::runtime_error("FILE ERROR: Bad formatting. No template footer.");

        int coordinates[4];
        fscanf(fhook, "%d%d%d%d", &coordinates[0], &coordinates[1], &coordinates[2], &coordinates[3]);
        this->start = this->get_vertex(coordinates[0], coordinates[1]);
        this->stop = this->get_vertex(coordinates[2], coordinates[3]);

    }
    else {
        char _FILE_ERROR[42] = {'F', 'I', 'L', 'E', ' ', 'E', 'R', 'R', 'O', 'R', ':', ' ', '\0'};
        char *_errmessage = strcat(_FILE_ERROR, strerror(errno));
        throw std::runtime_error(_errmessage);
    }

    fclose(fhook);
}


void Graph::init_by_user_interface(int xsize, int ysize, bool* activity_setup, int xcoord_start, int ycoord_start,
																			   int xcoord_stop, int ycoord_stop) {
    
    int i, j;
    Vertex* curr_vertex;
    this->util_uninit();

    this->xsize = xsize;
    this->ysize = ysize;

    this->graph = new Vertex[(size_t)this->xsize * this->ysize];

    for (i = 0; i < this->ysize; i++) {
        for (j = 0; j < this->xsize; j++) {
            curr_vertex = this->get_vertex(j, i);
            curr_vertex->is_active = activity_setup[i * this->xsize + j];
            curr_vertex->xpos = j;
            curr_vertex->ypos = i;
        }
    }

    this->start = this->get_vertex(xcoord_start, ycoord_start);
    this->stop = this->get_vertex(xcoord_stop, ycoord_stop);
}


void Graph::init_by_random(int xsize, int ysize, double fill_percent) {
    int i, j;
    Vertex* curr_vertex;
    bool is_init = false;
    std::random_device rand_dev;
    std::mt19937 threshold(rand_dev());
    std::uniform_real_distribution<> wall_space(0,1);


    try { is_init = this->util_check_init(); }
    catch(const std::runtime_error&) {};
    
    if (!is_init) {
    this->util_uninit();

    this->xsize = xsize;
    this->ysize = ysize;

    this->graph = new Vertex[(size_t)this->xsize * this->ysize];
    for (i = 0; i < this->ysize; i++) {
        for (j = 0; j < this->xsize; j++) {
            curr_vertex = this->get_vertex(j, i);
            curr_vertex->xpos = j;
            curr_vertex->ypos = i;
        }
    }
    }
    else if (this->xsize != xsize || this->ysize != ysize) {
    this->util_uninit();

    this->xsize = xsize;
    this->ysize = ysize;

    this->graph = new Vertex[(size_t)this->xsize * this->ysize];
    for (i = 0; i < this->ysize; i++) {
        for (j = 0; j < this->xsize; j++) {
            curr_vertex = this->get_vertex(j, i);
            curr_vertex->xpos = j;
            curr_vertex->ypos = i;
        }
    }
    }

    for (i = 0; i < this->ysize; i++) {
        for (j = 0; j < this->xsize; j++) {
            this->get_vertex(j, i)->is_active = (wall_space(threshold) > fill_percent) ? 0 : 1;
        }
    }

    std::uniform_int_distribution<> xspace(0, this->xsize - 1);
    std::uniform_int_distribution<> yspace(0, this->ysize - 1);

    curr_vertex = this->get_vertex(xspace(threshold), yspace(threshold));
    curr_vertex->is_active = true;
    this->start = curr_vertex;
    do {
        curr_vertex = this->get_vertex(xspace(threshold), yspace(threshold));
    } while (this->start == curr_vertex);
    curr_vertex->is_active = true;
    this->stop = curr_vertex;

    #ifdef ASTAR_DEBUG
    DEBUG_graph_printwall(*this);
    #endif
}


Graph::~Graph() {
    if (this->graph) { 
        delete[] this->graph;
        this->graph = (Vertex*)nullptr;
    }
}


/*
 * ALGORYTMY SORTUJĄCE
 */

double euclidean_dist(Vertex *const& a, Vertex *const& b) {
    return std::sqrt(std::pow(a->xpos - b->xpos, 2.) + std::pow(a->ypos - b->ypos, 2.));
}


void traceback_path(Graph& graph) {
    Vertex* curr_vertex = graph.stop;
    curr_vertex->state = PATH;
    do {
        curr_vertex = curr_vertex->previous;
        curr_vertex->state = PATH;
    } while (curr_vertex != graph.start);
}

void solve_astar(Graph& graph, bool diagonal_allowed, void (*callback)()) {
    graph.util_check_init();

    int i;
    Vertex* curr_vertex, * neighbour_curr_vertex;
    double curr_neighbour_gcost;
    bool is_curr_path_optimal;
    std::vector<Vertex*> openset;

    openset.push_back(graph.start);

    while (!openset.empty()) {
        curr_vertex = openset.front();
        for (std::vector<Vertex*>::iterator ii = openset.begin() + 1; ii != openset.cend(); ii++ ) {
            if (ii.operator*()->fcost < curr_vertex->fcost) curr_vertex = ii.operator*();
        }

        if (curr_vertex == graph.stop) {
            fprintf(stderr, "[###] solve_astar(): Path found.\n");
            traceback_path(graph);
            #ifdef ASTAR_DEBUG
            DEBUG_graphall_print(graph, curr_vertex, neighbour_curr_vertex);
            #endif
            return;
        }

        openset.erase(std::find(openset.begin(), openset.end(), curr_vertex));
        curr_vertex->state = CLOSED;
        for (i = 0; i < ((diagonal_allowed) ? 8 : 4); i++) {
            neighbour_curr_vertex = graph.get_vertex(curr_vertex->xpos + movement[i][0], curr_vertex->ypos + movement[i][1]);

            if (neighbour_curr_vertex == (Vertex*)nullptr) continue;
            if (neighbour_curr_vertex->state == CLOSED || !neighbour_curr_vertex->is_active) continue;

            curr_neighbour_gcost = curr_vertex->gcost + dist[i];
            is_curr_path_optimal = false;

            if (std::find(openset.begin(), openset.end(), neighbour_curr_vertex) == openset.end()) {
                openset.push_back(neighbour_curr_vertex);
                neighbour_curr_vertex->hcost = euclidean_dist(neighbour_curr_vertex, graph.stop);
                is_curr_path_optimal = true;
            }
            else if (curr_neighbour_gcost < neighbour_curr_vertex->gcost) {
                is_curr_path_optimal = true;
            }

            neighbour_curr_vertex->state = OPEN;

            if (is_curr_path_optimal) {
                neighbour_curr_vertex->previous = curr_vertex;
                neighbour_curr_vertex->gcost = curr_neighbour_gcost;
                neighbour_curr_vertex->fcost = neighbour_curr_vertex->gcost + neighbour_curr_vertex->hcost;
            }
            #ifdef ASTAR_DEBUG
            DEBUG_graphall_print(graph, curr_vertex, neighbour_curr_vertex);
            #endif
            callback();
        }

    }
    fprintf(stderr, "[###] solve_astar(): No path was found.\n");
    return;
}

void solve_dijkstra(Graph& graph, bool diagonal_allowed, void (*callback)()) {
    graph.util_check_init();

    int i;
    Vertex* curr_vertex, * neighbour_curr_vertex;
    double curr_neighbour_gcost;
    bool is_curr_path_optimal;
    std::vector<Vertex*> openset;

    openset.push_back(graph.start);

    while (!openset.empty()) {
        curr_vertex = openset.front();
        for (std::vector<Vertex*>::iterator ii = openset.begin() + 1; ii != openset.cend(); ii++ ) {
            if (ii.operator*()->fcost < curr_vertex->fcost) curr_vertex = ii.operator*();
        }

        if (curr_vertex == graph.stop) {
            fprintf(stderr, "[###] solve_dijkstra(): Path found.\n");
            traceback_path(graph);
            #ifdef ASTAR_DEBUG
            DEBUG_graphall_print(graph, curr_vertex, neighbour_curr_vertex);
            #endif
            return;
        }

        openset.erase(std::find(openset.begin(), openset.end(), curr_vertex));
        curr_vertex->state = CLOSED;
        for (i = 0; i < ((diagonal_allowed) ? 8 : 4); i++) {
            neighbour_curr_vertex = graph.get_vertex(curr_vertex->xpos + movement[i][0], curr_vertex->ypos + movement[i][1]);

            if (neighbour_curr_vertex == (Vertex*)nullptr) continue;
            if (neighbour_curr_vertex->state == CLOSED || !neighbour_curr_vertex->is_active) continue;

            curr_neighbour_gcost = curr_vertex->gcost + dist[i];
            is_curr_path_optimal = false;

            if (std::find(openset.begin(), openset.end(), neighbour_curr_vertex) == openset.end()) {
                openset.push_back(neighbour_curr_vertex);
                is_curr_path_optimal = true;
            }
            else if (curr_neighbour_gcost < neighbour_curr_vertex->gcost) {
                is_curr_path_optimal = true;
            }

            neighbour_curr_vertex->state = OPEN;

            if (is_curr_path_optimal) {
                neighbour_curr_vertex->previous = curr_vertex;
                neighbour_curr_vertex->gcost = curr_neighbour_gcost;
                neighbour_curr_vertex->fcost = neighbour_curr_vertex->gcost;
            }
            #ifdef ASTAR_DEBUG
            DEBUG_graphall_print(graph, curr_vertex, neighbour_curr_vertex);
            #endif
            callback();
        }

    }
    fprintf(stderr, "[###] solve_dijkstra(): No path was found.\n");
    return;
}

