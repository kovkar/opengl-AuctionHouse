CPPFLAGS = -std=c++11
LDLIBS = -lGL -lGLU -lglut -lGLEW -lglfw

SOURCES = main.cpp application.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = auction_house

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS) 

$(OBJECTS): application.hpp include/stb_image.h

clean: 
	$(RM) ${OBJECTS} $(TARGET)

.PHONY: all clean