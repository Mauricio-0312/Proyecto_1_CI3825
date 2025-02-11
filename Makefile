###########################
# Flags de la compilación #
###########################

CC=	gcc

# Flags para debbugging
CFLAGS= 	-Wall -Wextra -Wdouble-promotion -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -Werror -fsanitize=undefined -O0 -ggdb 
#-std=gnu2x
		

###############
# Compilación #
###############

PROG=		simsf

SRC=		node.c main.c

SRCOBJS=	$(SRC:.c=.o)

#LIBS=		-lm

.SUFFIXES:.c .o

all:		$(PROG)

$(PROG):	$(SRCOBJS)
		$(CC) $(CFLAGS) $(SRCOBJS) $(LIBS) -o $@ 

.c.o:
		$(CC) -c $(CFLAGS) $< -o $@

clean:
		rm -f $(PROG) *.o simsf