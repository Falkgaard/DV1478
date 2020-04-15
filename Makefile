VULKAN_SDK_PATH = /home/falk/Code/libs/Vulkan/1.1.130.0/x86_64
CFLAGS = -std=c++2a -I$(VULKAN_SDK_PATH)/include
LDFLAGS = -L$(VULKAN_SDK_PATH)/lib `pkg-config --static --libs glfw3` -lvulkan -lfmt -G -O0 -D_DEBUG -D_VERBOSE -Wall -Wformat -Wno-unused-variable

TestBinary: ./src/main.cpp
	@clang++ $(CFLAGS) -o TestBinary ./src/main.cpp $(LDFLAGS)

.PHONY: test clean

test: TestBinary
	@./TestBinary

clean:
	@rm -f TestBinary

remake: clean TestBinary

# EOF
