# Project name, used to build executable
NAME := mercury

# Builds list of .cpp files in src/ and src/*/
SOURCES := $(wildcard src/**/*.cpp src/*.cpp)

# Builds object list from sources, substitues .cpp for .o
OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))

# Directories to include for source files
INCLUDE_DIRS := src

# Add optimisations and remove debugging symbols
CPPFLAGS +=-g -Wall -Wextra -Wno-multichar

.PHONY: all build clean

all: $(NAME)

$(NAME) : $(OBJECTS)
	@mkdir -p bin
	@$(LINK.cc) $(OBJECTS) -o bin/$(NAME) -lPocoNet

# Remove $(NAME) from bin/
clean:
	@rm -f bin/$(NAME)
	@rm -f $(OBJECTS)
