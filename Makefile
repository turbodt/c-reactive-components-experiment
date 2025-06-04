LIB_NAME = xre
SRC_DIR = ./src

INCLUDES = \
	-I./external/uthash \
	-I$(SRC_DIR) \

LIBRARIES :=

#
#
#

CC = gcc

CFLAGS = -Wall -Wextra -fPIC -g $(INCLUDES)

SRC = $(wildcard \
        $(SRC_DIR)/impl/*.c \
        $(SRC_DIR)/context/impl/*.c \
        $(SRC_DIR)/use_value/impl/*.c \
      )

OBJS_DIR = build
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJS_DIR)/%.o, $(SRC))

LIB_DIR = lib
SHARED_LIB = $(LIB_DIR)/lib$(LIB_NAME).so
STATIC_LIB = $(LIB_DIR)/lib$(LIB_NAME).a

#
#
#

all: $(SHARED_LIB) $(STATIC_LIB)

$(SHARED_LIB): $(OBJS) | $(LIB_DIR)
	$(CC) -shared -o $@ $^ $(LIBRARIES)

$(STATIC_LIB): $(OBJS) | $(LIB_DIR)
	mkdir -p $(LIB_DIR)
	ar rcs $(STATIC_LIB) $(OBJS)

$(OBJS_DIR)/%.o: src/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(LIB_DIR):
	mkdir -p $@

clean:
	rm -rf $(OBJS_DIR)
	rm -rf $(LIB_DIR)

.PHONY: all clean
