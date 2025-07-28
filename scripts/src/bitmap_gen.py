# Zach Mahan
# Bear16 asm blank tile map script

NAME_FOR_LABELS = "ttt_tset_map".title()

DATA_DIR = ".byte"

NUM_ROWS = 8
TILE_HEIGHT_BYTES = 8
TILES_PER_ROW = 1

print(f"{NAME_FOR_LABELS}_start:")
for i in range(NUM_ROWS):
    print(f"{NAME_FOR_LABELS}_{i}:")
    for j in range(TILE_HEIGHT_BYTES):
        print(f"{DATA_DIR}", end=" ")
        for k in range(TILES_PER_ROW):
            print(f"0b00000000", end=" ")
        print()
print(f"{NAME_FOR_LABELS}_end:")
