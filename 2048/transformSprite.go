package main

import (
	"bufio"
	"fmt"
	"image/png"
	"os"
)

func check(err error) {
	if err != nil {
		panic(err)
	}
}

func main() {
	imageFile, err := os.Open("sprite.png")
	check(err)
	defer imageFile.Close()

	img, err := png.Decode(imageFile)
	check(err)

	headerFile, err := os.Create("sprite.h")
	check(err)
	defer headerFile.Close()

	writer := bufio.NewWriter(headerFile)
	defer writer.Flush()

	_, err = fmt.Fprintf(writer, "#define TILE_BMP_HEIGHT   16\n")
	check(err)
	_, err = fmt.Fprintf(writer, "#define TILE_BMP_WIDTH    16\n")
	check(err)
	_, err = fmt.Fprintf(writer, "static const unsigned char PROGMEM tile_bmps[16][32] = {\n")
	check(err)

	for tileY := 0; tileY < 4; tileY++ {
		for tileX := 0; tileX < 4; tileX++ {
			tileIndex := tileX + tileY*4
			_, err = fmt.Fprintf(writer, "  { // index %d, tile bitmap for 2^%d (%d):\n", tileIndex, 1+tileIndex, 1<<(1+tileIndex))
			check(err)
			for rowIndex := 0; rowIndex < 16; rowIndex++ {
				_, err = fmt.Fprintf(writer, "    ")
				check(err)
				for byteNumber := 0; byteNumber < 2; byteNumber++ {
					_, err = fmt.Fprintf(writer, "B")
					check(err)
					for byteIndex := 0; byteIndex < 8; byteIndex++ {
						x := tileX*16 + byteNumber*8 + byteIndex
						y := tileY*16 + rowIndex
						r, _, _, _ := img.At(x, y).RGBA()
						bit := 0
						if r > 0 {
							bit = 1
						}
						_, err = fmt.Fprintf(writer, "%d", bit)
						check(err)
					}
					_, err = fmt.Fprintf(writer, ", ")
					check(err)
				}
				_, err = fmt.Fprintf(writer, "\n")
				check(err)
			}
			fmt.Fprintf(writer, "  },\n")
		}
	}

	fmt.Fprintf(writer, "};\n\n")

}
