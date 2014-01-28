################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../lib.o 

C_SRCS += \
../BikeAVL.c \
../LinkedList.c \
../MessageAVL.c \
../SignalAVL.c \
../avl.c \
../datalogger.c \
../lib.c \
../main.c \
../parser.c \
../translator.c 

OBJS += \
./BikeAVL.o \
./LinkedList.o \
./MessageAVL.o \
./SignalAVL.o \
./avl.o \
./datalogger.o \
./lib.o \
./main.o \
./parser.o \
./translator.o 

C_DEPS += \
./BikeAVL.d \
./LinkedList.d \
./MessageAVL.d \
./SignalAVL.d \
./avl.d \
./datalogger.d \
./lib.d \
./main.d \
./parser.d \
./translator.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


