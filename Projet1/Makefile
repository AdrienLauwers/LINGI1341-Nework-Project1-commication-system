# See gcc/clang manual to understand all flags
CFLAGS += -std=c99 # Define which version of the C standard to use
CFLAGS += -Wall # Enable the 'all' set of warnings
CFLAGS += -Werror # Treat all warnings as error
CFLAGS += -Wshadow # Warn when shadowing variables
CFLAGS += -Wextra # Enable additional warnings
CFLAGS += -O2 -D_FORTIFY_SOURCE=2 # Add canary code, i.e. detect buffer overflows
CFLAGS += -fstack-protector-all # Add canary code to detect stack smashing
CFLAGS += -D_POSIX_C_SOURCE=201112L -D_XOPEN_SOURCE # feature_test_macros for getpot and getaddrinfo
LD_LIBRARY_PATH = $$HOME/local/lib:$$LD_LIBRARY_PATH

# We have no libraries to link against except libc, but we want to keep
# the symbols for debugging

LDFLAGS = -lcunit -lz -lm -Wl,-R$$HOME/local/lib -I$$HOME/local/include -L$$HOME/local/lib

# Default target
all: clean receiver sender tests

# If we run `make debug` instead, keep the debug symbols for gdb
# and define the DEBUG macro.
debug: CFLAGS += -g -DDEBUG -Wno-unused-parameter -fno-omit-frame-pointer
debug: clean receiver

# We use an implicit rule to build an executable named 'chat'
receiver: receiver.o socket.o server.o client.o packet_implem.o

sender: sender.o socket.o server.o client.o packet_implem.o

tests: test.o socket.o server.o client.o packet_implem.o


.PHONY: clean

clean:
	@rm -f receiver receiver.o socket.o server.o client.o
