CC = g++
CFLAGS = -Wall -Wextra -Wpedantic -std=c++17 -O2 -lSDL2 -lSDL2_ttf -lSDL2_image
SRCDIR = src
LIBDIR = lib
OBJDIR = obj

a-star-search: $(OBJDIR)/astar.o $(OBJDIR)/gui_application.o $(OBJDIR)/gui_callback.o $(OBJDIR)/gui_viewport.o $(OBJDIR)/gui_widget.o $(OBJDIR)/main.o $(OBJDIR)/sfd.o
	$(CC) $(CFLAGS) $^ -o $@


$(OBJDIR)/astar.o : $(SRCDIR)/astar.cpp $(SRCDIR)/common.hpp
	mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/gui_application.o : $(SRCDIR)/gui_application.cpp $(SRCDIR)/gui_application.hpp $(SRCDIR)/gui_callback.hpp $(SRCDIR)/gui_global.hpp
	mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/gui_callback.o : $(SRCDIR)/gui_callback.cpp $(SRCDIR)/gui_callback.hpp $(SRCDIR)/gui_application.hpp $(LIBDIR)/sfd/sfd.h $(SRCDIR)/gui_global.hpp
	mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/gui_viewport.o : $(SRCDIR)/gui_viewport.cpp $(SRCDIR)/gui_viewport.hpp $(SRCDIR)/gui_global.hpp
	mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/gui_widget.o : $(SRCDIR)/gui_widget.cpp $(SRCDIR)/gui_widget.hpp $(SRCDIR)/gui_application.hpp $(SRCDIR)/gui_global.hpp
	mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/main.o : $(SRCDIR)/main.cpp $(SRCDIR)/gui_application.hpp
	mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/sfd.o : $(LIBDIR)/sfd/sfd.c $(LIBDIR)/sfd/sfd.h
	mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f obj/*.o