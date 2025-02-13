
include ../Makefile.param

SMP_SRCS := $(wildcard *.c) $(wildcard $(PWD)/common/*.c)

CFLAGS += -I$(HI_REL_INC)
MPI_LIBS += $(HI_REL_LIB)/libhi_mpi_otp.a
MPI_LIBS += $(HI_REL_LIB)/libhi_mpi_km.a
MPI_LIBS += $(HI_REL_LIB)/libhi_mpi_cipher.a

TARGET := sample_ssl_aes

CFLAGS	+= -DCONFIG_CRYPTO_CHIP_HI3519DV500

ifeq ($(CONFIG_LLVM), y)
	CFLAGS += -fsanitize=address
else
	CFLAGS += -fstack-protector-strong
endif

TARGET_PATH := $(PWD)

#COMM_OBJ := $(filter-out $(PWD)/../common/sample_comm_venc.o, $(COMM_OBJ))

# compile linux or LITEOS
include $(PWD)/../$(ARM_ARCH)_$(OSTYPE).mak
