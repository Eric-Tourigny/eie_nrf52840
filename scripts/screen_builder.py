import sys
from PIL import Image

NUM_SCREEN_ROWS = 4
NUM_SCREEN_COLS = 3
SCREEN_WIDTH = 21
SCREEN_HEIGHT = 16

TILE_WIDTH = 15
TILE_HEIGHT = 15

TILE_MAP: dict[tuple[int, ...], str] = {
    (101, 101, 101, 255): "SpriteRockTile",
    (59, 145, 56, 255): "SpriteGrassTile",
    (162, 93, 7, 255): "SpriteDirtTile"
}

SCREEN_STRING_FORMAT = """static const tile_info_t screen{row}{col}_tiles = {{
  {statements}
}}

screen_t SCREEN{row}{col} = {{
  .objects = screen{row}{col}_tiles,
  .num_objects = ARRAY_SIZE(screen{row}{col})
}}
"""

def process_image(source: str):
    screen_strings = []
    with Image.open(source) as image:
        for screen_row in range(NUM_SCREEN_ROWS):
            for screen_col in range(NUM_SCREEN_COLS):
                left = screen_col * (SCREEN_WIDTH - 1)
                upper = screen_row * SCREEN_HEIGHT
                screen = image.crop((left, upper, left + SCREEN_WIDTH, upper + SCREEN_HEIGHT))
                pixels = screen.load()
                statements = []
                if pixels is not None:
                    for row in range(SCREEN_WIDTH):
                        for col in range(SCREEN_HEIGHT):
                            px = pixels[row, col]
                            if px in TILE_MAP:
                                statements.append(f"  {{&{TILE_MAP[px]}, {row * TILE_WIDTH}, {col * TILE_HEIGHT}}}")

                screen_strings.append(SCREEN_STRING_FORMAT.format(row=screen_row, col=screen_col, statements=",\n".join(statements)))
    return ",\n\n".join(screen_strings)


if __name__ == "__main__":
    if len(sys.argv) > 1:
        print(process_image(sys.argv[1]))
    else:
        print("Requires path to image as command line argument")
