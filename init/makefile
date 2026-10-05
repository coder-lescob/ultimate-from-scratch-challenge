-include ../build.mk

NAME    := init
SRC_DIR := src
SRC     := $(wildcard $(SRC_DIR)/*.c)
OBJS    := $(patsubst %.c,$(BUILD)/$(NAME)/%.o,$(SRC))
TARGET  := $(BUILD)/$(NAME)/init
BIN     := $(ROOTFS)/init

all: $(TARGET)
$(TARGET): $(OBJS)
	@$(RISCVCC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $(TARGET)
	@echo RISCVCC $(NAME)
	@cp $(TARGET) $(BIN)
	@echo COPY $(NAME) TO ROOTFS