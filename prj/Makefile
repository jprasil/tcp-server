#Project root directory (Makefile and project files for Eclipse IDE)
PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

#Test if BUILD_MODE is defined
ifndef BUILD_MODE
#BUILD_MODE = debug
BUILD_MODE = run
endif

ifeq ($(BUILD_MODE), debug)
	CFLAGS += -g -D_DEBUG
else ifeq ($(BUILD_MODE), run)
	CFLAGS += -O2
else
	$(error Build mode $(BUILD_MODE) not supported by this Makefile)
endif

#C++ source files
CPPSRCS = main.cpp \
			help.cpp \
			tcp_server.cpp \
			signal_manager.cpp \
			daemon.cpp \
			hw_monitor.cpp \
			pit.cpp \
			comm.cpp

#C++ object files			
CPPOBJS = $(CPPSRCS:.cpp=.o)
#C++ object files with build path prefix
BLD_CPPOBJS = $(patsubst %, $(BUILDDIR)/%, $(CPPOBJS))
#Included directories
INCDIRS = $(PROJECT_ROOT)../inc \
			$(PROJECT_ROOT)../inc/modules
#Build directory			
BUILDDIR = $(PROJECT_ROOT)Build/$(BUILD_MODE)
#Flags for compiler			
CFLAGS += $(patsubst %,-I%, $(INCDIRS))
#Linked libraries
LIBS = -lpthread



.PHONY: makedirs
.PHONY: clean

all: makedirs tcp_server

	
makedirs:
	$(MKDIR) $(BUILDDIR)


tcp_server: $(BLD_CPPOBJS)
	$(CXX) -o $(BUILDDIR)/$@ $^ $(LIBS)
	
$(BUILDDIR)/%.o: $(PROJECT_ROOT)../src/%.cpp
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o $@ $< $(LIBS)
	

MKDIR = mkdir -p


clean:
	rm -fr $(BUILDDIR)/tcp_server $(BLD_CPPOBJS)
