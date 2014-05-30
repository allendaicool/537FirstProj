################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FindAndParse.c \
../ProcessCreation.c \
../buildGraph.c 

OBJS += \
./FindAndParse.o \
./ProcessCreation.o \
./buildGraph.o 

C_DEPS += \
./FindAndParse.d \
./ProcessCreation.d \
./buildGraph.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


