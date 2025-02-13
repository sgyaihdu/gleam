include ../Makefile.param

########### SRCS & INC ############
SMP_SRCS := $(wildcard $(PWD)/*.c)

TARGET := sample_mcf

TARGET_PATH := $(PWD)

MPI_LIBS += $(HI_REL_LIB)/libhi_mpi_mcf.a
MPI_LIBS += $(HI_REL_LIB)/libhi_mpi_mcf_calibrate.a

# compile linux or LITEOS
include $(PWD)/../$(ARM_ARCH)_$(OSTYPE).mak
