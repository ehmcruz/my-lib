HEADERS = $(wildcard src/*.h) $(wildcard src/*.h)
CPPFLAGS = -I./include -Wall -ggdb -std=c++23

all: pool stl timer trigger
	@echo "My-lib compiled! yes!"

pool: $(HEADERS) lib/pool-alloc.cpp tests/test-pool-alloc.cpp
	g++ lib/pool-alloc.cpp tests/test-pool-alloc.cpp -o test-pool-alloc $(CPPFLAGS)

stl: $(HEADERS) lib/pool-alloc.cpp tests/test-stl-alloc.cpp
	g++ lib/pool-alloc.cpp tests/test-stl-alloc.cpp -o test-stl-alloc $(CPPFLAGS)

timer: $(HEADERS) tests/test-timer.cpp
	g++ tests/test-timer.cpp -o test-timer $(CPPFLAGS)

trigger: $(HEADERS) tests/test-trigger.cpp
	g++ tests/test-trigger.cpp -o test-trigger $(CPPFLAGS)

clean:
	- rm -rf test-pool-alloc test-stl-alloc test-timer