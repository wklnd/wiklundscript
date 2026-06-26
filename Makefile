CC      = cc
CFLAGS  = -Wall -Wextra -std=c11 -g -D_POSIX_C_SOURCE=200809L -I.
TARGET  = wscript
SRCDIR  = src
NATIVEDIR = native
NATIVE_HDR = $(NATIVEDIR)/native.h
SRCS    = $(SRCDIR)/main.c \
          $(SRCDIR)/lexer.c \
          $(SRCDIR)/ast.c \
          $(SRCDIR)/parser.c \
          $(SRCDIR)/interpreter.c
OBJS    = $(SRCS:.c=.o)
RAYLIB_PREFIX  = /opt/homebrew/opt/raylib
NATIVE_TARGETS = $(NATIVEDIR)/libDateTime.so $(NATIVEDIR)/libTime.so $(NATIVEDIR)/libRandom.so $(NATIVEDIR)/libString.so $(NATIVEDIR)/libIO.so $(NATIVEDIR)/libMath.so $(NATIVEDIR)/libColor.so $(NATIVEDIR)/libcolor.so $(NATIVEDIR)/libGraphics.so

# ─── Build ────────────────────────────────────────────────────────────────────

all: $(TARGET) $(NATIVE_TARGETS)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

$(NATIVEDIR)/libDateTime.so: $(NATIVEDIR)/DateTime.c $(NATIVE_HDR)
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $<

$(NATIVEDIR)/libTime.so: $(NATIVEDIR)/Time.c $(NATIVE_HDR)
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $<

$(NATIVEDIR)/libRandom.so: $(NATIVEDIR)/Random.c $(NATIVE_HDR)
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $<

$(NATIVEDIR)/libString.so: $(NATIVEDIR)/String.c $(NATIVE_HDR)
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $<

$(NATIVEDIR)/libIO.so: $(NATIVEDIR)/IO.c $(NATIVE_HDR)
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $<

$(NATIVEDIR)/libMath.so: $(NATIVEDIR)/Math.c $(NATIVE_HDR)
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $< -lm

$(NATIVEDIR)/libColor.so: $(NATIVEDIR)/Color.c $(NATIVE_HDR)
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $<

$(NATIVEDIR)/libcolor.so: $(NATIVEDIR)/Color.c $(NATIVE_HDR)
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $<

$(NATIVEDIR)/libGraphics.so: $(NATIVEDIR)/Graphics.c $(NATIVE_HDR)
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $< \
		-I$(RAYLIB_PREFIX)/include \
		-L$(RAYLIB_PREFIX)/lib \
		-lraylib \
		-framework CoreVideo -framework IOKit -framework Cocoa \
		-framework GLUT -framework OpenGL

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ─── Run ──────────────────────────────────────────────────────────────────────

run: $(TARGET)
	./$(TARGET)

# ─── Clean ───────────────────────────────────────────────────────────────────

clean:
	rm -f $(OBJS) $(TARGET) $(NATIVE_TARGETS)

.PHONY: all run clean