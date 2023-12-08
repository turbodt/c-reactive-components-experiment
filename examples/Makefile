SRC_DIR = src
EXAMPLES = misc example1

EXTERNAL_INCLUDES = -I../include
EXTERNAL_LIBRARIES = -L../lib -lxre
LOCAL_INCLUDES = -I./hooks -I./utils
LOCAL_LIBRARIES =

#
#
#

CC = gcc
CFLAGS = -Wall -Wextra -fPIC -g $(EXTERNAL_INCLUDES) $(LOCAL_INCLUDES)
LDFLAGS = $(LOCAL_LIBRARIES) $(EXTERNAL_LIBRARIES)

EXAMPLES_SRC = $(patsubst %, $(SRC_DIR)/%.c, $(EXAMPLES))
SHARED_SRC = \
      hooks/use_time.c \
      hooks/use_timer.c \
      hooks/use_timespec.c \
      utils/msleep.c \
      utils/screen.c \
      \
      utils/kbhit_unix_posix.c \
      utils/terminal_unix.c \

OBJ_DIR = build
SHARED_OBJS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SHARED_SRC))
OBJS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(EXAMPLES_SRC)) \
        $(SHARED_OBJS)

TARGET_DIR = bin
TARGETS = $(patsubst %, $(TARGET_DIR)/%, $(EXAMPLES))

#
#
#

all: $(TARGETS)

$(TARGETS): $(TARGET_DIR)/%: $(OBJ_DIR)/%.o $(SHARED_OBJS)
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/hooks/%.o: hooks/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/utils/%.o: utils/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm -f $(TARGETS)
	-rm -rf $(OBJ_DIR)

.PHONY: all clean
