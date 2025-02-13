
include ../Makefile.param

SMP_SRCS := $(wildcard *.c) $(wildcard $(PWD)/common/*.c)

CFLAGS += -I$(PWD)/common/

TARGET := sample_region
CFLAGS += -I$(HI_REL_INC)

TARGET_PATH := $(PWD)

COMM_OBJ := $(filter-out $(PWD)/../common/sample_comm_venc.o, $(COMM_OBJ))

# compile linux or LITEOS
include $(PWD)/../$(ARM_ARCH)_$(OSTYPE).mak
