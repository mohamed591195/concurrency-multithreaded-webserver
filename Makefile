# An admittedly primitive Makefile
# To compile, type "make" or make "all"
# To remove files, type "make clean"

#key components of a Makefile:
#1. variables
#2. suffixes
#3. targets and rules
#4. implicit rules

#1. the following are the variables (for the compiler, flags, and object files)
CC = gcc            # Specifies the C compiler to use
CFLAGS = -Wall -pthread     # Compiler flags (-Wall enables all warning messages) could be stands for warning all
OBJS = wserver.o wclient.o request.o io_helper.o tpool.o  # List of object files needed

#2. this line tells make about the file suffixes it should recognize (.c and .o)
.SUFFIXES: .c .o 

#3. the following are the rules and targets
#all: this is a special target, when you run "make" without any arguments, 
#it will build the first target in the file, which is "all"
#in this case, it will build all the specified programs (wserver, wclient, and spin.cgi)
all: wserver wclient spin.cgi

# wserver: This target depends on wserver.o, request.o, and io_helper.o. 
# The rule specifies how to build wserver using these object files.
wserver: wserver.o request.o io_helper.o tpool.o linked_list.o heap.o schedulers.o 
	$(CC) $(CFLAGS) -o wserver wserver.o request.o io_helper.o tpool.o linked_list.o heap.o schedulers.o

# wclient: This target depends on wclient.o and io_helper.o.
wclient: wclient.o io_helper.o
	$(CC) $(CFLAGS) -o wclient wclient.o io_helper.o

# spin.cgi: This target is built from spin.c.
spin.cgi: spin.c
	$(CC) $(CFLAGS) -o spin.cgi spin.c

# .c.o:: This is an implicit rule that tells make how to convert .c files to .o files. 
# The $@ is a placeholder for the target name (the .o file), 
# and $< is a placeholder for the first prerequisite (the .c file).
.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

# clean: This target is used to remove the compiled files and clean up the directory. 
# The - before rm suppresses errors if the files do not exist.
clean:
	-rm -f $(OBJS) wserver wclient spin.cgi