BIN = ./bin/game
INCLUDES = -I./src/*.h -I./src/libsys

CFLAGS = -std=c99 -Wall -Wno-missing-braces -g -DDEBUG -DMEM_DEBUG -O3 -mavx2

BASE_SRCS = src/libsys/*.c \
			src/libsys/audio/*.c \
			src/libsys/graphics/*.c \
			src/*.c

ifeq ($(shell uname),Darwin)
	CC = $(shell brew --prefix llvm)/bin/clang
	CFLAGS += -DPLATFORM_MACOS -fopenmp=libomp -mllvm -force-vector-width=8
	LDFLAGS = -framework Cocoa

	SRCS = src/libsys/platforms/macos.m $(BASE_SRCS)
endif
ifeq ($(shell uname),Linux)
	CC = clang
	CFLAGS += -DPLATFORM_LINUX -fopenmp=libomp -mllvm -force-vector-width=8
	LDFLAGS = -lX11 -lpthread -lm -ldl

	SRCS = src/libsys/platforms/linux.c $(BASE_SRCS)
endif
ifeq ($(shell uname),Windows)
	CC = x86_64-w64-mingw32-gcc
	CFLAGS += -DPLATFORM_WINDOWS -mprefer-vector-width=128
	LDFLAGS = -lmingw32 -mwindows

	SRCS = src/libsys/platforms/windows.c $(BASE_SRCS)
endif

dke:
	$(CC) $(INCLUDES) $(CFLAGS) $(LDFLAGS) $(SRCS) -o $(BIN)/$@

run:
	cd $(BIN) && ./dke

clean:
	rm -f $(BIN)/dke
