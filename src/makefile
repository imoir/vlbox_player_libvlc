
CC         := gcc
CXX        := g++
CXXFLAGS   := -std=c++17 -pthread -g -O2 -Wall
CFLAGS     := -g -O2 -Wall
LDFLAGS    :=
LDLIBS     := -g -O2 -Wall -lvlc -Wl,-rpath='$$ORIGIN' -Wl,--allow-shlib-undefined
PROGRAMS   := player
OBJDIR	   := ../build
BINDIR     := ../bin

all: $(BINDIR)/player

$(BINDIR)/player: $(OBJDIR)/Main.o $(OBJDIR)/Commander.o $(OBJDIR)/Configuration.o $(OBJDIR)/Helpers.o
	@mkdir -p $(BINDIR)
	$(CXX) $(LDFLAGS) $(OBJDIR)/Main.o $(OBJDIR)/Commander.o $(OBJDIR)/Configuration.o $(OBJDIR)/Helpers.o -o $@ $(LDLIBS)

$(OBJDIR)/Main.o: Main.cpp $(OBJDIR)/Commander.o $(OBJDIR)/Configuration.o $(OBJDIR)/Helpers.o
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -c -o $@ Main.cpp $(LDLIBS)

$(OBJDIR)/Commander.o: Commander.cpp Commander.h $(OBJDIR)/Configuration.o
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -c -o $@ Commander.cpp $(LDLIBS)

$(OBJDIR)/Helpers.o: Helpers.cpp Helpers.h
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -c -o $@ Helpers.cpp $(LDLIBS)

$(OBJDIR)/Configuration.o: Configuration.cpp Configuration.h
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -c -o $@ Configuration.cpp $(LDLIBS)

clean:
	rm -Rf $(PROGRAMS) $(BINDIR) $(OBJDIR)

new: clean all
