%.elf: $(OBJS) | $(BUILD_DIR)
	$(LD) -o $@ $(OBJS) ${SUB_OBJS} $(LDFLAGS)

%.hex: %.elf
	$(OBJCOPY) -O ihex $^ $@

%.bin: %.elf
	$(OBJCOPY) -O binary $^ $@

%.v: %.elf
	$(OBJCOPY) -O verilog $^ $@

%.asm: %.elf
	$(OBJDUMP) -S -d $^ > $@

%.sym: %.elf
	$(OBJDUMP) -t $^ > $@

${BUILD_DIR}/%.o: %.c
	$(CC) -c $^ $(CFLAGS) $(INCLUDE) -o $@
	
${BUILD_DIR}/%.o: %.cpp
	$(CC) -c $^ $(CFLAGS)  $(INCLUDE) -o $@

${BUILD_DIR}/%.o: %.S
	$(CC) -c $^ $(CFLAGS) -o $@ -D__ASSEMBLY__=1

clean:
	rm -f $(BUILD_DIR)/*.elf
	rm -f $(BUILD_DIR)/*.hex
	rm -f $(BUILD_DIR)/*.map
	rm -f $(BUILD_DIR)/*.v
	rm -f $(BUILD_DIR)/*.o
	rm -f $(BUILD_DIR)/*.asm
	find $(BUILD_DIR) -type f -name '*.d' -print0 | xargs -0 -r rm
	find $(BUILD_DIR) -type f -name '*.o' -print0 | xargs -0 -r rm
	rm -r $(BUILD_DIR)
