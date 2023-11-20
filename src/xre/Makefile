TARGET_NAME = xre
SRC_DIR = ./src

EXTERNAL_INCLUDES = $(GLOBAL_INCLUDES) \

EXTERNAL_LIBRARIES = \

LOCAL_INCLUDES = -I$(SRC_DIR)

LOCAL_LIBRARIES = \

#
#
#

CC = gcc

CFLAGS = -Wall -Wextra -fPIC -g $(EXTERNAL_INCLUDES) $(LOCAL_INCLUDES)

LDFLAGS = $(LOCAL_LIBRARIES) $(EXTERNAL_LIBRARIES)

SRC = $(wildcard $(SRC_DIR)/*.c $(SRC_DIR)/use_value/*.c)

OBJ_DIR = build
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

LIB_DIR = lib
SHARED_LIB = $(LIB_DIR)/lib$(TARGET_NAME).so

#
#
#

all: $(SHARED_LIB)

$(SHARED_LIB): $(OBJ) | $(LIB_DIR)
	$(CC) -shared -o $@ $^

$(OBJ_DIR)/%.o: src/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(LIB_DIR):
	mkdir -p $@

clean:
	-rm -rf $(OBJ_DIR)
	-rm -rf $(LIB_DIR)

.PHONY: all clean
