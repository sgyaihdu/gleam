
include ../Makefile.param

SMP_SRCS := $(wildcard *.c) $(wildcard $(PWD)/common/*.c)
TARGET := sample_venc_bt1120

TARGET_PATH := $(PWD)

COMM_OBJ := $(filter-out $(PWD)/../common/sample_comm_venc.o, $(COMM_OBJ))

# compile linux or LITEOS
include $(PWD)/../$(ARM_ARCH)_$(OSTYPE).mak
