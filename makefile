# 體感平衡迷宮 — SDCC makefile（沿用課堂實驗的結構）
#   make        編譯，產生 ./main.hex
#   make clean  清除 obj/ 和 main.hex
#   make size   顯示記憶體使用量（RAM 很小，每次加功能都要看）

OBJ_DIR=obj
OBJ_PATH=./obj/
SRC_PATH_FROM_OBJ=./src
INC_PATH_FROM_OBJ=./inc

OUTPUT_FLAG=-o ./obj/
C_FLAG=-I $(INC_PATH_FROM_OBJ) $(CFLAGS_EXTRA)

CC:=sdcc
ixh2hex:=packihx
hex_file:=main.hex
ihx_file:=main.ihx
main_rel:=main.rel

cur_path:=${wildcard $(SRC_PATH_FROM_OBJ)/*.c}
cur_src:=${notdir $(cur_path)}
cur_obj:=${patsubst %.c, %.rel, $(cur_src)}
final_rel:=${filter-out $(main_rel), $(cur_obj)}
inc_path:=${wildcard $(INC_PATH_FROM_OBJ)/*.h}

ifneq ($(OBJ_DIR), $(wildcard $(OBJ_DIR)))
	SHELL_HACK := $(shell mkdir $(OBJ_DIR))
endif

vpath %.rel $(OBJ_DIR)
vpath %.ihx $(OBJ_DIR)
vpath %.c $(SRC_PATH_FROM_OBJ)

all:$(ihx_file)
	$(ixh2hex) $(OBJ_PATH)$(ihx_file) > $(OBJ_PATH)$(hex_file)
	@cp $(OBJ_PATH)$(hex_file) .

$(ihx_file):$(cur_obj)
	cd $(OBJ_PATH) && $(CC) $(main_rel) $(final_rel)

%.rel: %.c $(inc_path)
	$(CC) -c $(SRC_PATH_FROM_OBJ)/$*.c $(C_FLAG) $(OUTPUT_FLAG)

size: all
	@grep -A 30 "Internal RAM layout" $(OBJ_PATH)main.mem | tail -n 12

.PHONY: clean size
clean:
	@rm -rf $(OBJ_PATH)
	@rm -f $(hex_file)
	@echo remove obj and main.hex
