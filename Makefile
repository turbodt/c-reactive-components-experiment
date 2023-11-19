TARGET_NAME = test

SRC_DIR = ./src

EXTERNAL_INCLUDES = \
									-I$(PWD)/src/xre/include \

EXTERNAL_LIBRARIES = \
									 -L$(PWD)/src/xre/lib -lxre \

LOCAL_INCLUDES = -I$(SRC_DIR)

LOCAL_LIBRARIES = \

#
#
#

CC = gcc

CFLAGS = -Wall -Wextra -fPIC -g $(EXTERNAL_INCLUDES) $(LOCAL_INCLUDES)

LDFLAGS = $(LOCAL_LIBRARIES) $(EXTERNAL_LIBRARIES)

SRC = $(SRC_DIR)/main.c

OBJ_DIR = $(PWD)/build
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

TARGET = $(PWD)/$(TARGET_NAME)

#
#
#

all: xre $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

xre:
	$(MAKE) -C $(PWD)/src/xre GLOBAL_INCLUDES="$(EXTERNAL_INCLUDES)"

clean:
	-rm -f $(TARGET)
	-rm -rf $(OBJ_DIR)

clean-all: clean
	- $(MAKE) -C $(PWD)/src/xre clean

.PHONY: all clean clean-all