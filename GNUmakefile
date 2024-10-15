# for muscle memory if nothing else

# read requires_sudo.txt files for lists of programs that need sudo
HEADERS := $(shell ls *.hpp)
SOURCES := $(shell find t ein -name '*.[ch]pp' -o '*.cppm' -noth -path './gen/*' -print)
BUILD_TYPE := RelWithDebInfo  # for release
PRESET := native
# BUILD_TYPE := Debug  # for debugging
MAKEFLAGS += --no-print-directory -j
CMAKELISTS := CMakeLists.txt t/CMakeLists.txt $(shell find ein -type f -name CMakeLists.txt)
TESTS := $(notdir $(wildcard t/t_*.cpp))
PHONY := all build clean run test tags
EXES := 

# run `make DEBUG=1` to see these
ifdef DEBUG
$(info BUILD_TYPE := $(BUILD_TYPE))
$(info MAKEFLAGS := $(MAKEFLAGS))
$(info TESTS = $(TESTS))
$(info PHONY = $(PHONY))
$(info RUN = $(RUN))
$(info CMAKE_DEFINES = $(CMAKE_DEFINES))
endif

all: build

gen/ein.png: gen
	@echo "\nDependency diagram available as gen/ein.png"

gen/alderaan.png: gen
	@dot -Tpng -o gen/alderaan.png gen/alderaan.dot

build: gen
	@cmake --build gen -j
	@bin/ninjatracing gen/.ninja_log > gen/trace.json

run: $(RUN)

gen: $(CMAKELISTS)
	@mkdir -p gen
	@cmake --preset $(PRESET) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) --graphviz=gen/ein.dot
	@touch gen

lint:
	@git --no-pager diff --check origin/main HEAD -- \
		':!lib/**' \
		|| { echo "Trailing whitespace detected. Please run 'make format'."; exit 1; }
	@for file in $(shell git diff --name-only origin/main HEAD); do \
		if [ -f "$$file" ] && [ -n "$$(tail -c 1 "$$file")" ]; then \
			echo "Missing newline at end of file: $$file. Please run 'make format'"; \
			exit 1; \
		fi; \
	done

format:
	bin/remove_whitespace.sh

clean:
	@rm -rf gen tags

tags:
	@find . -name '*.[ch]pp' -o -name '*.cppm' -type f -not -path './gen/*' -exec ctags {} +
	@echo tags updated

gen/t_%: gen $(HEADERS) $(SOURCES)
	@cmake --build gen --target $(notdir $@) -j

t_%: gen/t_%
	@gen/$@

define EXE_TEMPLATE
gen/$(1): gen $(HEADERS) $(SOURCES)
	@cmake --build gen --target $(1) -j

$(1): gen/$(1)
	@gen/$(1)
endef

$(foreach exe,$(EXES),$(eval $(call EXE_TEMPLATE,$(exe))))

test: all
	@ctest --test-dir gen --output-on-failure -j

.PHONY: $(PHONY)
