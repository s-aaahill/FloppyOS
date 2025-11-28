BUILD_DIR?=build
CC?=gcc
CFLAGS?=-ggdb

SOURCES_C=$(wildcard *.c)
OBJECTS_C=$(patsubst %.c, $(BUILD_DIR)/tools/fat_obj/%.o, $(SOURCES_C))

.PHONY: all fat clean always

all: fat

fat: $(BUILD_DIR)/tools/fat

$(BUILD_DIR)/tools/fat: $(OBJECTS_ASM) $(OBJECTS_C)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/tools/fat $(OBJECTS_C)

$(BUILD_DIR)/tools/fat_obj/%.o: %.c always
	$(CC) $(CFLAGS) -c -o $@ $<

always:
	mkdir -p $(BUILD_DIR)/tools/fat_obj

clean:
	rm -f $(BUILD_DIR)/tools/fat
	rm -rf $(BUILD_DIR)/tools/fat_obj