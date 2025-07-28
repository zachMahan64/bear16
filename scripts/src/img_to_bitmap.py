from PIL import Image

img = Image.open("../images/bear_clip_art.webp")

NAME = "bear_fs_image"
WIDTH, HEIGHT = 256, 192
POSITIVE_PIX = '0'
NEGATIVE_PIX = '1'

# resize
img = img.resize((WIDTH, HEIGHT), Image.NEAREST)

# Convert to grayscale and apply manual threshold
img = img.convert("L")
threshold = 128
img = img.point(lambda p: 255 if p > threshold else 0, mode='1')  # binarize

# Extract binary pixel values: 1 for white, 0 for black
pixels = list(img.getdata())

# convert flat pixel list into 2D row-wise matrix
binary_matrix = [pixels[i:i+WIDTH] for i in range(0, len(pixels), WIDTH)]

# emit .word-packed binary rows
print(f"{NAME}:")
for row in binary_matrix:
    print(".byte ", end="")
    for i in range(0, WIDTH, 8):
        bits = row[i:i+8]
        word = ''.join([POSITIVE_PIX if b else NEGATIVE_PIX for b in bits])
        print(f"0b{word} ", end="")
    print()
print(f"{NAME}_end:")
