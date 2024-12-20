# This is named GNUmakefile to prevent it from being clobbered in case you do an in-directory
# build by accident. Not that I would ever do such a thing. Nope.

HEADERS := $(shell find src -name '*.hpp' -print)
SOURCES := $(shell find src -name '*.cpp' -print)
BUILD_TYPE := RelWithDebInfo  # for release
PRESET := native
# BUILD_TYPE := Debug  # for debugging
MAKEFLAGS += --no-print-directory -j
CMAKELISTS := CMakeLists.txt $(shell find src -type f -name CMakeLists.txt)
PHONY := all build distclean clean run test tags
REPO := https://github.com/ekmett/ein
LOGLEVEL :=
# LOGLEVEL := --log-level=TRACE
PROJECT := ein
EXES :=
PORT := 8000

# run `make DEBUG=1` to see these
ifdef DEBUG
$(info BUILD_TYPE := $(BUILD_TYPE))
$(info MAKEFLAGS := $(MAKEFLAGS))
$(info PHONY = $(PHONY))
$(info RUN = $(RUN))
$(info CMAKE_DEFINES = $(CMAKE_DEFINES))
endif

all: build

serve: build
	@echo Running http doc server in the foreground on port $(PORT)
	@python3 -m http.server $(PORT) -d gen/doc/html

server-start: build
	@echo Running http doc server in the background on port $(PORT)
	@nohup python3 -m http.server $(PORT) -d gen/doc/html &

server-stop:
	@pkill -f "python3 -m http.server $(PORT) -d gen/doc/html"

run: $(RUN)

gen: $(CMAKELISTS) CMakeGraphVizOptions.cmake
	@mkdir -p gen/dotfiles
	@cmake $(LOGLEVEL) --preset $(PRESET) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) --graphviz=gen/dotfiles/ein.dot
	@bin/adjust_dotfiles.sh gen/dotfiles gen/doc/patched-dotfiles
	@touch gen

gen/doc/patched-dotfiles: gen
	@bin/adjust_dotfiles.sh gen/dotfiles gen/doc/patched-dotfiles

gen/ein.png: gen
	@dot -Tpng -o gen/ein.png gen/doc/patched-dotfiles/ein.dot

png: gen/ein.png
	@echo "\nDependency diagram available as gen/ein.png"

build: gen
	@cmake --build gen -j
	@bin/ninjatracing gen/.ninja_log | tee gen/doc/html/ein-build.trace > gen/trace.json || @echo TRACE FAILED

lint:
	bin/check_whitespace.sh

format:
	bin/remove_whitespace.sh

clean:
	@rm -rf gen tags nohup.out

distclean: clean
	@rm -rf lib/cache .ccache/?

tags:
	@find . -name '*.[ch]pp' -o -name '*.cppm' -type f -not -path './gen/*' -exec ctags {} +
	@echo tags updated


define EXE_TEMPLATE
gen/bin/$(1): gen $(HEADERS) $(SOURCES)
	@cmake --build gen --target $(1) -j

$(1): gen/bin/$(1)
	@gen/bin/$(1)
endef
$(foreach exe,$(EXES),$(eval $(call EXE_TEMPLATE,$(exe))))

test: all
	@ctest --test-dir gen --output-on-failure -j

coverage: test
	@cmake --build gen -j --target coverage

.PHONY: $(PHONY)
