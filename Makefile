
NAME      		= webserv

CC          	= clang++
CFLAGS      	= -Wall -Wextra -Werror -Wpedantic -std=c++11 -O0 -g
# -Wsign-conversion
# CFLAGS      	= -Wall -Wextra -Werror -Wuninitialized -O3

SRCEXT      	= cpp
SRCDIR      	= src
OBJEXT      	= o
HEADEREXT		= hpp
HEADERDIR		= include
BUILDDIR    	= obj

LIBS			=
INCLUDES		= -I$(HEADERDIR)
LINK			=

OBJ_IN_DIR 		= $(BUILDDIR)/*.$(OBJEXT)
SRC 			= $(shell find $(SRCDIR) -name '*.$(SRCEXT)')
HEADERS 		= $(shell find $(HEADERDIR) -name '*.$(HEADEREXT)')
OBJ				= $(foreach src,$(SRC),$(BUILDDIR)/$(notdir $(src:.$(SRCEXT)=.$(OBJEXT))))

SILECE_MAKE 	= | grep -v -E ".*Leaving directory|.*Entering directory"
VPATH 			= $(shell find $(SRCDIR) -type d | tr '\n' ':' | sed -E 's/(.*):/\1/')
.SUFFIXES:

all: $(NAME) cgi/input

$(NAME): $(BUILDDIR)/ $(OBJ) $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJ_IN_DIR) $(LIBS) -o $(NAME) $(LINK)

$(BUILDDIR)/%.$(OBJEXT): %.$(SRCEXT) $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $(BUILDDIR)/$(notdir $@)

cgi/input: cgi/input.c
	gcc -Wall -Wextra cgi/input.c -o cgi/input

# sources


clean:
ifneq ($(BUILDDIR),.)
	/bin/rm -rf $(BUILDDIR)/
endif

fclean: | clean
	/bin/rm -f $(NAME)

re: | fclean all

$(BUILDDIR)/:
	mkdir -p $(BUILDDIR)

silent:
	@$(MAKE) > /dev/null

.PHONY: all clean fclean re silent
