################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../AllSolution.cpp \
../Bucket.cpp \
../BucketTree.cpp \
../CPLX_test.cpp \
../RuleList.cpp \
../UnitTest.cpp 

OBJS += \
./AllSolution.o \
./Bucket.o \
./BucketTree.o \
./CPLX_test.o \
./RuleList.o \
./UnitTest.o 

CPP_DEPS += \
./AllSolution.d \
./Bucket.d \
./BucketTree.d \
./CPLX_test.d \
./RuleList.d \
./UnitTest.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


