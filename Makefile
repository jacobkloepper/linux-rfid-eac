CC = gcc
CC_FLAGS = -Iinc -Wall -Wpedantic 
LD_FLAGS = -pthread -lpthread

TARGET_DIR = bin
SOURCE_DIR = src
BUILD_DIR = $(TARGET_DIR)/build
INCLUDE_DIR = inc
SCRIPT_DIR = tools

TARGET = $(TARGET_DIR)/linux-rfid-eac
SOURCES = $(wildcard $(SOURCE_DIR)/*.c) 
OBJECTS = $(subst $(SOURCE_DIR),$(BUILD_DIR),$(SOURCES:.c=.o))
HEADERS = $(wildcard $(INCLUDE_DIR)/*.h)

.PHONY: all run clean setup upload

all: setup makedirs $(TARGET) run

$(TARGET): $(OBJECTS)
	@echo Linking $@
	@$(CC) $(LD_FLAGS) $(OBJECTS) -o $(TARGET)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c $(HEADERS)
	@echo Buliding $@
	@$(CC) -c $(CC_FLAGS) -o $@ $<

makedirs:
	@mkdir -p $(TARGET_DIR) $(SOURCE_DIR) $(BUILD_DIR)

run:
	./$(TARGET)

clean:
	@echo Cleaning
	@rm -f $(TARGET_DIR)/*
	@rm -f $(BUILD_DIR)/*.o

setup:
	@echo "Configuring ports"
	@sh $(SCRIPT_DIR)/setup.sh

upload:
	@echo "Uploading log"
	@sh tool/uploader/run.sh
