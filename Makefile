CC = gcc
CC_FLAGS = -Iinc -Wall -Wextra -Wpedantic 
LD_FLAGS = -pthread 

TARGET_DIR = bin
SOURCE_DIR = src
BUILD_DIR = $(TARGET_DIR)/build
INCLUDE_DIR = inc
SCRIPT_DIR = tool

TARGET = $(TARGET_DIR)/linux-rfid-eac
SOURCES = $(wildcard $(SOURCE_DIR)/*.c) 
OBJECTS = $(subst $(SOURCE_DIR),$(BUILD_DIR),$(SOURCES:.c=.o))
HEADERS = $(wildcard $(INCLUDE_DIR)/*.h)

.PHONY: all run clean setup upload umod report

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
	@rm -f $(TARGET)
	@rm -f $(BUILD_DIR)/*.o

setup:
	@echo "Configuring ports"
	@sh $(SCRIPT_DIR)/sys/tty_setup.sh

upload:
	@echo "Uploading log"
	@sh $(SCRIPT_DIR)/remote/run-ul.sh

umod:
	@echo "Starting usermod"
	@sh $(SCRIPT_DIR)/umod/run.sh

report:
	@echo "Uploading report"
	@sh $(SCRIPT_DIR)/remote/run-rp.sh
