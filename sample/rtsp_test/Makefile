include ../Makefile.param

########### SRCS & INC ############
SMP_SRCS := $(wildcard $(PWD)/*.c) $(wildcard $(PWD)/aibnr/*.c) $(wildcard $(PWD)/aidrc/*.c) $(wildcard $(PWD)/ai3dnr/*.c) $(wildcard $(PWD)/common/*.c)

CFLAGS += -I$(PWD)/aibnr/
CFLAGS += -I$(PWD)/aidrc/
CFLAGS += -I$(PWD)/ai3dnr/
CFLAGS += -I$(PWD)/common/

TARGET := sample_rtsp_test

TARGET_PATH := $(PWD)



COMM_OBJ := $(filter-out $(PWD)/../common/sample_comm_venc.o, $(COMM_OBJ))

MPI_LIBS += $(HI_REL_LIB)/libmk_api.so

# compile linux or LITEOS
include $(PWD)/../$(ARM_ARCH)_$(OSTYPE).mak