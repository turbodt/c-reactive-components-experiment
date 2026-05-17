LIB_NAME = xre
SRC_DIR = ./src

INCLUDES = \
	-I./external/uthash \
	-I./include \
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

LIB_DIR = $(realpath ./)/lib
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

tests:
	$(MAKE) clean
	$(MAKE) all
	$(MAKE) clean -C ./tests
	$(MAKE) -C ./tests \
		MAIN_DIR=$(realpath ./) \
		EXTERNAL_INCLUDES="$(INCLUDES)" \
		EXTERNAL_LIBRARIES="-L$(LIB_DIR) -l$(LIB_NAME)"

clean:
	$(MAKE) clean -C ./tests
	rm -rf $(OBJS_DIR)
	rm -rf $(LIB_DIR)

run-tests: tests
	./tests/bin/tests

.PHONY: all clean tests run-tests
