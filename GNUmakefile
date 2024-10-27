# This is named GNUmakefile to prevent it from being clobbered in case you do an in-directory
# build by accident. Not that I would ever do such a thing. Nope.

HEADERS := $(shell find src -name '*.hpp' -print)
SOURCES := $(shell find t src -name '*.cpp' -o -name '*.cppm' -print)
BUILD_TYPE := RelWithDebInfo  # for release
PRESET := native
# BUILD_TYPE := Debug  # for debugging
MAKEFLAGS += --no-print-directory -j
CMAKELISTS := CMakeLists.txt t/CMakeLists.txt $(shell find src -type f -name CMakeLists.txt)
TESTS := $(notdir $(wildcard t/t_*.cpp))
PHONY := all build distclean clean run test tags
REPO := https://github.com/ekmett/ein
LOGLEVEL := --log-level=WARNING
PROJECT := ein
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

png: gen/ein.png
	@echo "\nDependency diagram available as gen/ein.png"

gen/ein.png: gen
	@dot -Tpng -o gen/ein.png gen/dotfiles/ein.dot

build: gen
	@cmake --build gen -j
	@bin/ninjatracing gen/.ninja_log | tee gen/doc/html/ein-build.trace > gen/trace.json

run: $(RUN)

gen: $(CMAKELISTS)
	@mkdir -p gen
	@mkdir -p gen/dotfiles
	@cmake $(LOGLEVEL) --preset $(PRESET) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) --graphviz=gen/dotfiles/ein.dot
	@touch gen

lint:
	bin/check_whitespace.sh

format:
	bin/remove_whitespace.sh

clean:
	@rm -rf gen tags

distclean: clean
	@rm -rf lib/cache

tags:
	@find . -name '*.[ch]pp' -o -name '*.cppm' -type f -not -path './gen/*' -exec ctags {} +
	@echo tags updated

gen/bin/t_%: gen $(HEADERS) $(SOURCES)
	@cmake --build gen --target $(notdir $@) -j

t_%: gen/bin/t_%
	@gen/$@

define EXE_TEMPLATE
gen/bin/$(1): gen $(HEADERS) $(SOURCES)
	@cmake --build gen --target $(1) -j

$(1): gen/bin/$(1)
	@gen/bin/$(1)
endef

$(foreach exe,$(EXES),$(eval $(call EXE_TEMPLATE,$(exe))))

test: all
	@ctest --test-dir gen --output-on-failure -j

.PHONY: $(PHONY)
