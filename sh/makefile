-include ../build.mk

NAME    := sh
SRC_DIR := src
SRC     := $(wildcard $(SRC_DIR)/*.c)
OBJS    := $(patsubst %.c,$(BUILD)/$(NAME)/%.o,$(SRC))
TARGET  := $(BUILD)/$(NAME)/$(NAME)
BIN     := $(ROOTFS)/bin/$(NAME)

all: $(TARGET)
$(TARGET): $(OBJS)
	@$(RISCVCC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $(TARGET)
	@echo RISCVCC $(NAME)
	@cp $(TARGET) $(BIN)
	@echo COPY $(NAME) TO ROOTFS