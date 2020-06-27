CC = g++
CPPFLAGS = -std=c++17 -Wall -fsanitize=address
LDFLAGS = -lglfw.3.3 -lGLEW.2.1.0 -framework OpenGL

INC_DIR = inc
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
GFX_DIR = gfx

SOURCES = $(wildcard $(SRC_DIR)/*.cpp $(SRC_DIR)/$(GFX_DIR)/*.cpp)
OBJECTS = $(subst $(SRC_DIR), $(OBJ_DIR), $(SOURCES:.cpp=.o))
TARGET = $(BIN_DIR)/visualizer

folders:
	mkdir -p $(INC_DIR) & mkdir -p $(INC_DIR)/$(GFX_DIR) & mkdir -p $(SRC_DIR) & mkdir -p $(SRC_DIR)/$(GFX_DIR) & mkdir -p $(OBJ_DIR) & mkdir -p $(OBJ_DIR)/$(GFX_DIR) & mkdir -p $(BIN_DIR)

all: folders $(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c $(CPPFLAGS) $^ -o $@ -I$(INC_DIR) -I$(INC_DIR)/$(GFX_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/$(GFX_DIR)/%.cpp
	$(CC) -c $(CPPFLAGS) $^ -o $@ -I$(INC_DIR) -I$(INC_DIR)/$(GFX_DIR)

$(TARGET): $(OBJECTS)
	$(CC) $(CPPFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJ_DIR)/*.o $(OBJ_DIR)/$(GFX_DIR)/*.o $(BIN_DIR)/*

.PHONY: folders all clean
.PRECIOUS: $(OBJ_DIR)/%.o

# ---Flags---
# -std=c++11 use the C++11 standard
# -Wall output all warnings
# -fsanitize=address use ASan
# -l[libname] points to a library
# -p makes intermediate dirs, idempotent
# -o specify name of executable
# -c to object file
# -f forces the operation
# -I search dir for headers
# ---Vars---
# $@ the target file
# $^ the source files