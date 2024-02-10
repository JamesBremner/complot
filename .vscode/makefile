#source file search paths
VPATH = ./src 

	# compiler include search paths
INCS = -I./include \
	-I../windex/include \
	-I../raven-set \

LIBS = \
	-lgdiplus -lgdi32 -lcomdlg32 -lstdc++fs \
	-mwindows

static_link_flags = -static-libstdc++ -static-libgcc -static -lComctl32

# folders
ODIR = ./.vscode/obj

OPT = -g
CCflags = -std=c++17 $(OPT) -D_mingw_

# sources
OBJ_complotter = \
	$(ODIR)/complotter.o 
OBJ_comdevicesim = \
	$(ODIR)/comdevicesim.o 

### COMPILE

$(ODIR)/%.o: %.cpp  
	g++ $< \
	-c -o $@  $(INCS) $(CCflags)

complotter: $(OBJ_complotter)
	g++ $(OPT) \
	-o bin/complotter.exe $^  \
	$(static_link_flags) $(LIBS)

comdevicesim: $(OBJ_comdevicesim)
	g++ $(OPT) \
	-o bin/comdevicesim.exe $^  \
	$(static_link_flags) $(LIBS)

rebuild: clean complotter comdevicesim
	
.PHONY: clean
clean:
	del /q .vscode\\obj\\*.o

