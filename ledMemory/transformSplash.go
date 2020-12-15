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
	imageFile, err := os.Open("splash.png")
	check(err)
	defer imageFile.Close()

	img, err := png.Decode(imageFile)
	check(err)

	headerFile, err := os.Create("splash.h")
	check(err)
	defer headerFile.Close()

	writer := bufio.NewWriter(headerFile)
	defer writer.Flush()

	_, err = fmt.Fprintf(writer, "#define SPLASH_BMP_HEIGHT   48\n")
	check(err)
	_, err = fmt.Fprintf(writer, "#define SPLASH_BMP_WIDTH    96\n")
	check(err)
	_, err = fmt.Fprintf(writer, "static const unsigned char PROGMEM splash_bmp[576] = {\n")
	check(err)

	for rowIndex := 0; rowIndex < 48; rowIndex++ {
		_, err = fmt.Fprintf(writer, "  ")
		check(err)
		for byteNumber := 0; byteNumber < 12; byteNumber++ {
			_, err = fmt.Fprintf(writer, "B")
			check(err)
			for byteIndex := 0; byteIndex < 8; byteIndex++ {
				x := byteNumber*8 + byteIndex
				y := rowIndex
				r, _, _, _ := img.At(x, y).RGBA()
				bit := 1
				if r > 0 {
					bit = 0
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

	fmt.Fprintf(writer, "};\n\n")

}
