################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include startup/subdir.mk
-include source/subdir.mk
-include drivers/freertos/subdir.mk
-include drivers/subdir.mk
-include board/subdir.mk
-include amazon-freertos/portable/subdir.mk
-include amazon-freertos/TraceRecorder/streamports/Jlink_RTT/subdir.mk
-include amazon-freertos/TraceRecorder/streamports/File/subdir.mk
-include amazon-freertos/TraceRecorder/streamports/ARM_ITM/subdir.mk
-include amazon-freertos/TraceRecorder/subdir.mk
-include amazon-freertos/FreeRTOS/portable/subdir.mk
-include amazon-freertos/FreeRTOS/subdir.mk
-include CMSIS/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: TP4_SO2.axf

# Tool invocations
TP4_SO2.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -Xlinker -Map="TP4_SO2.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -Xlinker --sort-section=alignment -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -T TP4_SO2_Debug.ld -o "TP4_SO2.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) TP4_SO2.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "TP4_SO2.axf"; # arm-none-eabi-objcopy -v -O binary "TP4_SO2.axf" "TP4_SO2.bin" ; # checksum -p MK64FN1M0xxx12 -d "TP4_SO2.bin";
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets
