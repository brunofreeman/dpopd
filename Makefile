CC = g++
CPPFLAGS = -Wall -std=c++11
#LDFLAGS = -framework OpenGL -framework GLUT
LDFLAGS =

INC_DIR = inc
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(subst $(SRC_DIR), $(OBJ_DIR), $(SOURCES:.cpp=.o))
TARGET = $(BIN_DIR)/visualizer

folders:
	mkdir -p $(INC_DIR) & mkdir -p $(SRC_DIR) & mkdir -p $(OBJ_DIR) & mkdir -p $(BIN_DIR)

all: folders $(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c $(CPPFLAGS) $^ -o $@ $(LDFLAGS) -I$(INC_DIR)

$(TARGET): $(OBJECTS)
	$(CC) $(CPPFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJ_DIR)/* $(BIN_DIR)/*

.PHONY: folders all clean
.PRECIOUS: $(OBJ_DIR)/%.o

# ---Flags---
# -p makes intermediate dirs, idempotent
# -o specify name of executable
# -c to object file
# -f forces the operation
# -I search dir for headers
# ---Vars---
# $@ the target file
# $^ the source files