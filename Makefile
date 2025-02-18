###########################
# Flags de la compilación #
###########################

CC=	gcc

#########################
# Flags para debbugging #
########################

CFLAGS= 	-Wall -Wextra -Wdouble-promotion -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -Werror -fsanitize=undefined -std=gnu23 -O0 -ggdb 

###############
# Compilación #
###############

PROG=		simsf

SRC=		node.c main.c

SRCOBJS=	$(SRC:.c=.o)

.SUFFIXES:.c .o

all:		$(PROG)

$(PROG):	$(SRCOBJS)
			$(CC) $(CFLAGS) $(SRCOBJS) -o $@ 
			rm -f $(SRCOBJS)

.c.o:
		$(CC) -c $(CFLAGS) $< -o $@

clean: 
		rm -f $(PROG) *~