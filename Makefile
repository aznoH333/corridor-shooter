# Makefile

CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

SRCDIR := ./src
SRCS := $(wildcard $(SRCDIR)/*.c)
OBJS := $(SRCS:$(SRCDIR)/%.c=$(SRCDIR)/%.o)
TARGET := game

PKG_CONFIG_FLAGS := $(shell pkg-config --libs --cflags raylib)

ifeq ($(MACOS),1)
  CC := cc
  CFLAGS := $(PKG_CONFIG_FLAGS)  
  LDFLAGS := $(PKG_CONFIG_FLAGS)
endif

.PHONY: run_all all mac run clean

# Default goal: build, run, then clean (in that sequence)
run_all:
	$(MAKE) all
	./$(TARGET)
	$(MAKE) clean

# convenience: make mac -> build for macOS
mac:	
	@$(MAKE) MACOS=1

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(TARGET)

$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

