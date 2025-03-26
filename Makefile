BUILD_DIR=build
T3D_INST=$(shell realpath /mnt/o/src/n64/tiny3d)
N64_INST=$(shell realpath /opt/libdragon/)
ARES_DIR=$(shell realpath /mnt/o/src/n64/ares-v141/)
D=1
include $(N64_INST)/n64.mk
include $(T3D_INST)/t3d.mk

N64_CFLAGS += -std=gnu2x

src = main.c actor.c dragon.c camera.c lights.c stage.c

assets_png = $(wildcard assets/*.png)
assets_gltf = $(wildcard assets/*.glb)
assets_xm1 = $(wildcard assets/*.xm)
assets_xm2 = $(wildcard assets/*.XM)
assets_conv = $(addprefix filesystem/,$(notdir $(assets_png:%.png=%.sprite))) \
			  $(addprefix filesystem/,$(notdir $(assets_ttf:%.ttf=%.font64))) \
			  $(addprefix filesystem/,$(notdir $(assets_gltf:%.glb=%.t3dm))) \
			  $(addprefix filesystem/,$(notdir $(assets_xm1:%.xm=%.xm64))) \
			  $(addprefix filesystem/,$(notdir $(assets_xm2:%.XM=%.xm64)))

AUDIOCONV_FLAGS ?=

all: ftrCart.z64

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
	$(N64_MKSPRITE) $(MKSPRITE_FLAGS) -o filesystem "$<"

filesystem/%.t3dm: assets/%.glb
	@mkdir -p $(dir $@)
	@echo "    [T3D-MODEL] $@"
	$(T3D_GLTF_TO_3D) "$<" $@
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
