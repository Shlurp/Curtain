PROJECT_NAME = curtain
CC = gcc
INCLUDE_DIR = ./include
SRC_DIR = ./src
OBJ_DIR = ./obj

CFLAGS = -Wall -I$(INCLUDE_DIR)

DEPS = $(wildcard $(INCLUDE_DIRS)/*.h)

__OBJ = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/*/*.c)
_OBJ = $(notdir $(__OBJ))
OBJ = $(patsubst %.c,$(OBJ_DIR)/%.o,$(_OBJ))

all:: $(OBJ_DIR) $(PROJECT_NAME)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/*/%.c $(DEPS)
	$(CC) -c -g $(CFLAGS) $< -o$@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	$(CC) -c -g $(CFLAGS) $< -o$@

$(PROJECT_NAME): $(OBJ)
	$(CC) -g $(CFLAGS) $^ -o$@ $(LIBS)

clean:
	rm -r $(OBJ_DIR)/*.o 

.PHONY: all clean