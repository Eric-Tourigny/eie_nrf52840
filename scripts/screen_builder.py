import sys
from PIL import Image

TILE_WIDTH = 15
TILE_HEIGHT = 15

TILE_MAP: dict[tuple[int, ...], str] = {
    (101, 101, 101, 255): "SpriteRockTile",
    (59, 145, 56, 255): "SpriteGrassTile",
    (162, 93, 7, 255): "SpriteDirtTile"
}

def process_image(source: str):
    rows = []
    with Image.open(source) as image:
        width, height = image.size
        pixels = image.load()
        if pixels is not None:
            for row in range(width):
                for col in range(height):
                    px = pixels[row, col]
                    if px in TILE_MAP:
                        rows.append(f"  {{&{TILE_MAP[px]}, {row * TILE_WIDTH}, {col * TILE_HEIGHT}}}")
    return ",\n".join(rows)


if __name__ == "__main__":
    if len(sys.argv) > 1:
        print(process_image(sys.argv[1]))
    else:
        print("Requires path to image as command line argument")
