# for muscle memory if nothing else

# read requires_sudo.txt files for lists of programs that need sudo
HEADERS := $(shell find src -name '*.hpp' -print)
SOURCES := $(shell find t src -name '*.cpp' -o -name '*.cppm' -print)
BUILD_TYPE := RelWithDebInfo  # for release
PRESET := native
# BUILD_TYPE := Debug  # for debugging
MAKEFLAGS += --no-print-directory -j
CMAKELISTS := CMakeLists.txt t/CMakeLists.txt $(shell find src -type f -name CMakeLists.txt)
TESTS := $(notdir $(wildcard t/t_*.cpp))
PHONY := all build clean run test tags
REPO := https://github.com/ekmett/ein/
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

#docs: gen/docs

png: gen/ein.png
	@echo "\nDependency diagram available as gen/ein.png"

gen/ein.png: gen
	@dot -Tpng -o gen/ein.png gen/ein.dot

build: gen
	@cmake --build gen -j
	@bin/ninjatracing gen/.ninja_log | tee gen/docs/html/ein-build.trace > gen/trace.json

#gen/docs: gen $(SOURCES) $(HEADERS)
#	@clang-doc-19 --executor=all-TUs compile_commands.json --output=gen/doc --format=md --repository=$(REPO) --project-name=$(PROJECT) -p gen

run: $(RUN)

gen: $(CMAKELISTS)
	@mkdir -p gen
	@cmake --preset $(PRESET) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) --graphviz=gen/ein.dot
	@touch gen

lint:
	@git --no-pager diff --check origin/main HEAD \
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
