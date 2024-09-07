CC= $${HOME}/riscv64-linux-musl-cross/bin/riscv64-linux-musl-gcc

ifeq ($(OS),Windows_NT) 
RM = del /Q /F
CP = copy /Y
ifdef ComSpec
SHELL := $(ComSpec)
endif
ifdef COMSPEC
SHELL := $(COMSPEC)
endif
else
RM = rm -rf
CP = cp -f
endif

BIN=rasterizer
CXXFLAGS=-O3 -g -Wall -Wextra -Wno-unknown-pragmas -Wno-deprecated-copy -nodefaultlibs -fno-rtti -fno-exceptions
#CXXFLAGS += -pg
CFLAGS=$(CXXFLAGS)
CPPFLAGS=-IIncludes -IHeaders -MMD

LDLIBS=-lc -lgcc

# PROGRAM OBJS
OBJS=  Sources/Main.o
OBJS+= Sources/Rasterizer.o
OBJS+= Sources/RenderThread.o
OBJS+= Sources/Maths/Maths.o
OBJS+= Sources/Resources/ModelLoader.o

DEPS=$(OBJS:.o=.d)

all: $(BIN)

#-include $(DEPS)

%.o: %.cpp
	$(CC) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@

$(BIN): $(OBJS)
	$(CC) $(CXXFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

clean:
	@echo "Clean project"
	-$(RM) -f $(BIN) $(OBJS) $(DEPS)

.PHONY: clean
