#include "common.hpp"

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
void DEBUG_graphall_print(Graph &graph) {
    int xsize = graph.get_xsize(), ysize = graph.get_ysize();
    Vertex* curr_vertex;
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

    for (int i = 0; i < ysize; i++) {
        for (int j = 0; j < xsize; j++) {
            curr_vertex = graph.get_vertex(j,i);
            printf("(%d, %d), ", curr_vertex->xpos, curr_vertex->ypos);
        }
        printf("\n");
    }

    printf("start:(%d %d), stop(%d, %d)", graph.start->xpos, graph.start->ypos, graph.stop->xpos, graph.stop->ypos);
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

Graph::Graph(int xsize, int ysize) {
    this->xsize = xsize;
    this->ysize = ysize;
    this->graph = new Vertex[(size_t)this->xsize * this->ysize];

    div_t div_res;
    for (int i = 0; i < this->xsize * this->ysize; i++) {
        div_res = div(i, this->xsize);
        this->graph[i].xpos = div_res.rem;
        this->graph[i].ypos = div_res.quot;
    }
}

Graph::Graph(FILE* fhook) {
    if (fhook) {
        int i, j;
        char curr_char;
        Vertex* curr_vertex;

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

        #ifdef ASTAR_DEBUG
        DEBUG_graphall_print(*this);
        #endif
    }
    else {
        throw std::runtime_error("FILE ERROR: Empty file pointer");
    }
}

Graph::~Graph() {
    delete[] this->graph;
}

