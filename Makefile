TARGET_NAME = test

EXTERNAL_INCLUDES = \

EXTERNAL_LIBRARIES = \

LOCAL_INCLUDES = \

LOCAL_LIBRARIES = \

#
#
#

CC = gcc

CFLAGS = -Wall -Wextra -fPIC -g $(EXTERNAL_INCLUDES) $(LOCAL_INCLUDES)

LDFLAGS = $(LOCAL_LIBRARIES) $(EXTERNAL_LIBRARIES)

SRC_DIR = src
SRC = $(SRC_DIR)/main.c

OBJ_DIR = $(PWD)/build
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

TARGET = $(PWD)/$(TARGET_NAME)

#
#
#

all: gui $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm -f $(TARGET)
	-rm -rf $(OBJ_DIR)

.PHONY: all clean gui
