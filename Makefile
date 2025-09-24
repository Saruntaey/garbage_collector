CC = gcc
CFLAGS = -Wall -Wextra -Werror -g
TARGET = exe
OUTDIR = objs
SRCS = $(shell find . -type f -name '*.c')
OBJS = $(patsubst ./%.c, $(OUTDIR)/%.o, $(SRCS))

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

$(OUTDIR)/%.o: %.c | $(OUTDIR)
	$(CC) -c $(CFLAGS) -o $@ $<

$(OUTDIR):
	mkdir -p $@

.PHONY: clean run

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)
