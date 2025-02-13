include ../Makefile.param

########### SRCS & INC ############
SMP_SRCS := $(wildcard $(PWD)/*.c) $(wildcard $(PWD)/aibnr/*.c) $(wildcard $(PWD)/aidrc/*.c) $(wildcard $(PWD)/ai3dnr/*.c) $(wildcard $(PWD)/common/*.c)

CFLAGS += -I$(PWD)/aibnr/
CFLAGS += -I$(PWD)/aidrc/
CFLAGS += -I$(PWD)/ai3dnr/
CFLAGS += -I$(PWD)/common/

CFLAGS += -I$(HI_REL_INC)
MPI_LIBS += $(HI_REL_LIB)/libhi_mpi_otp.a
MPI_LIBS += $(HI_REL_LIB)/libhi_mpi_km.a
MPI_LIBS += $(HI_REL_LIB)/libhi_mpi_cipher.a

CFLAGS	+= -DCONFIG_CRYPTO_CHIP_HI3519DV500
ifeq ($(CONFIG_LLVM), y)
	CFLAGS += -fsanitize=address
else
	CFLAGS += -fstack-protector-strong
endif

TARGET := sample_rtsp_ai_test_own_slite

TARGET_PATH := $(PWD)

COMM_OBJ := $(filter-out $(PWD)/../common/sample_comm_venc.o, $(COMM_OBJ))

#MPI_LIBS += $(HI_REL_LIB)/libmk_api.so

# compile linux or LITEOS
include $(PWD)/../$(ARM_ARCH)_$(OSTYPE).mak