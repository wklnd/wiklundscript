CC      = cc
CFLAGS  = -Wall -Wextra -std=c11 -g -D_POSIX_C_SOURCE=200809L
TARGET  = wscript
SRCDIR  = src
SRCS    = $(SRCDIR)/main.c \
          $(SRCDIR)/lexer.c \
          $(SRCDIR)/ast.c \
          $(SRCDIR)/parser.c \
          $(SRCDIR)/interpreter.c
OBJS    = $(SRCS:.c=.o)

# ─── Build ────────────────────────────────────────────────────────────────────

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ─── Run ──────────────────────────────────────────────────────────────────────

run: $(TARGET)
	./$(TARGET)

# ─── Clean ───────────────────────────────────────────────────────────────────

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: run clean