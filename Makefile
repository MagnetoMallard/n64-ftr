BUILD_DIR=build
include $(N64_INST)/include/n64.mk
include $(T3D_INST)/t3d.mk

N64_CFLAGS += -std=gnu2x -O2 -ffast-math
VPATH = libs:src:src/lights:src/actors:src/camera

src = main.c camera.c light.c stage.c actor.c actor_animation.c audio/audio_fx.c audio/audio_playback.c

assets_png = $(wildcard assets/*.png)
assets_gltf = $(wildcard assets/*.glb)
assets_xm1 = $(wildcard assets/*.xm)
assets_xm2 = $(wildcard assets/*.XM)
assets_ttf = $(wildcard assets/*.ttf)
assets_conv = $(addprefix filesystem/,$(notdir $(assets_png:%.png=%.sprite))) \
			  $(addprefix filesystem/,$(notdir $(assets_ttf:%.ttf=%.font64))) \
			  $(addprefix filesystem/,$(notdir $(assets_gltf:%.glb=%.t3dm))) \
			  $(addprefix filesystem/,$(notdir $(assets_xm1:%.xm=%.xm64))) \
			  $(addprefix filesystem/,$(notdir $(assets_xm2:%.XM=%.xm64)))

AUDIOCONV_FLAGS ?=

all: ftrCart.z64

#Sizes for fonts are FTR=8 and Dot=10, change only in values of x2 for pixel perfection
filesystem/%.font64: assets/%.ttf
	@mkdir -p $(dir $@)
	@echo "    [FONT] $@"
	$(N64_MKFONT) --monochrome --outline 1 --size 8 $(MKFONT_FLAGS) -o filesystem "$<"
filesystem/%.xm64: assets/%.xm
	@mkdir -p $(dir $@)
	@echo "    [AUDIO] $@"
	@$(N64_AUDIOCONV) $(AUDIOCONV_FLAGS) -o filesystem "$<"
filesystem/%.xm64: assets/%.XM
	@mkdir -p $(dir $@)
	@echo "    [AUDIO] $@"
	@$(N64_AUDIOCONV) $(AUDIOCONV_FLAGS) -o filesystem "$<"

filesystem/%.sprite: assets/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	$(N64_MKSPRITE) $(MKSPRITE_FLAGS) -c 2 -o filesystem "$<"

filesystem/%.t3dm:  assets/%.glb
	@mkdir -p $(dir $@)
	@echo "    [T3D-MODEL] $@"
	$(T3D_GLTF_TO_3D) $(GLTF_FLAGS) "$<" $@
	$(N64_BINDIR)/mkasset -c 2 -o filesystem $@


$(BUILD_DIR)/ftrCart.dfs: $(assets_conv)
$(BUILD_DIR)/ftrCart.elf: $(src:%.c=$(BUILD_DIR)/%.o)

ftrCart.z64: N64_ROM_TITLE="Tiny3D - Model"
ftrCart.z64: $(BUILD_DIR)/ftrCart.dfs

# dummy entry for clion, which treats makefiles strangely
clionAres:

ares:
	$(ARES_DIR)/ares.exe ./ftrCart.z64

unfload:
	./UNFLoader.exe -d -r ./ftrCart.z64

sc64:
	./sc64deployer.exe reset
	./sc64deployer.exe upload ./ftrCart.z64
clean:
	rm -rf $(BUILD_DIR) *.z64
	rm -rf filesystem

build_lib:
	rm -rf $(BUILD_DIR) *.z64
	make -C $(T3D_INST)
	make all

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
