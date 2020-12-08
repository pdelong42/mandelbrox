SRC=$(wildcard src/*.c)
OBJ=$(patsubst src/%.c,obj/%.o,$(SRC))
STATIC=$(patsubst src/%.c,bin/%-static,$(SRC))
DYNAMIC=$(patsubst src/%.c,bin/%,$(SRC))
DIRS=bin obj

LDFLAGS += -lpthread -lc -lm
#CFLAGS += -I ./include
#CFLAGS += -DDEBUG

.PHONY: clean static dynamic prep

default: static dynamic

static: $(STATIC)

dynamic: $(DYNAMIC)

$(OBJ): obj/%.o: src/%.c prep
	cc -c -o $@ $< $(CFLAGS)

$(STATIC): bin/%-static: obj/%.o prep
	cc -o $@ $< $(LDFLAGS) -static

$(DYNAMIC): bin/%: obj/%.o prep
	cc -o $@ $< $(LDFLAGS)

clean:
	@rm -rvf $(DIRS)

prep:
	@mkdir -vp $(DIRS)
