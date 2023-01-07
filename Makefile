CC := gcc
FLAGS := -Werror -pedantic -Wall
NAME := kr
SRC_DIR := ./
BUILD_DIR := ./bin
SRCS := $(wildcard ./*.c)
HDRS := $(wildcard ./*.h)
OBJECTS := $(BUILD_DIR)/$(notdir $(SRCS:.c=.o))
LIBS := -lncurses

.PHONY: main-build pre-build build run clean pack

all: build

pre-build:
	mkdir -p ./bin

build: pre-build
	$(MAKE) main-build

main-build: $(OBJECTS)
	$(CC) $(FLAGS) -o $(NAME) $^ $(LIBS)

$(BUILD_DIR)/%.o: %.c
	$(CC) $(FLAGS) -o $@ -c $^

clean:
	rm -f $(NAME)
	rm -rf $(BUILD_DIR)
