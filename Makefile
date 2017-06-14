TARGET   	= 	mic
TITLE_ID 	= 	VITAMICIN
TITLE    	=  	mic sample

RES_DIR 	= resources
SRC_DIR  	= source
OUT_DIR 	= build

SOURCES  	= $(shell find $(SRC_DIR) -name '*.c')
OBJS     	= $(SOURCES:%.c=%.o)

PREFIX  	= arm-vita-eabi
CC      	= $(PREFIX)-gcc
CFLAGS  	= -Wl,-q -Wall -O3 -Wno-unused-variable
ASFLAGS 	= $(CFLAGS)
PSVITAIP 	= 10.0.63

LIBS 		= -lSceLibKernel_stub \
			  -lvita2d -lSceGxm_stub -lSceDisplay_stub -lSceSysmodule_stub \
			  -lSceCommonDialog_stub -lSceIme_stub -lScePower_stub \
			  -lScePgf_stub -lpng -ljpeg -lfreetype -lz -lm -lc \
			  -lSceCtrl_stub -lSceTouch_stub \
			  -lScePower_stub \
			  -ldebugnet \
			  -lSceNetCtl_stub -lSceNet_stub \
			  -lSceAudio_stub -lSceAudiodec_stub

all: $(OUT_DIR)/$(TARGET).vpk

%.vpk: $(OUT_DIR)/eboot.bin
	vita-mksfoex -s TITLE_ID=$(TITLE_ID) "$(TITLE)" $(OUT_DIR)/param.sfo
	vita-pack-vpk -s $(OUT_DIR)/param.sfo -b $(OUT_DIR)/eboot.bin \
		--add sce_sys/icon0.png=sce_sys/icon0.png \
		--add sce_sys/livearea/contents/bg.png=sce_sys/livearea/contents/bg.png \
		--add sce_sys/livearea/contents/startup.png=sce_sys/livearea/contents/startup.png \
		--add sce_sys/livearea/contents/template.xml=sce_sys/livearea/contents/template.xml \
	$(OUT_DIR)/$(TARGET).vpk

$(OUT_DIR)/eboot.bin: $(OUT_DIR)/$(TARGET).velf
	vita-make-fself -s $< $@

%.velf: %.elf
	vita-elf-create $< $@

$(OUT_DIR)/$(TARGET).elf: binfolder $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@

%.o: %.bmp
	$(PREFIX)-ld -r -b binary -o $@ $^
	
%.o: %.png
	$(PREFIX)-ld -r -b binary -o $@ $^
	
%.o: %.jpg
	$(PREFIX)-ld -r -b binary -o $@ $^
	
%.o: %.jpeg
	$(PREFIX)-ld -r -b binary -o $@ $^

clean:
	@rm -rf $(OUT_DIR) $(OBJS)

vpksend: $(OUT_DIR)/$(TARGET).vpk
	curl -T $(OUT_DIR)/$(TARGET).vpk ftp://$(PSVITAIP):1337/ux0:/
	@echo "Sent."

send: $(OUT_DIR)/eboot.bin
	curl -T $(OUT_DIR)/eboot.bin ftp://$(PSVITAIP):1337/ux0:/app/$(TITLE_ID)/
	@echo "Sent."

binfolder:
	@mkdir $(OUT_DIR) || true

