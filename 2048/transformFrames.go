package main

import (
	"bufio"
	"fmt"
	"image/png"
	"os"

	"github.com/nfnt/resize"
)

func check(err error) {
	if err != nil {
		panic(err)
	}
}

func main() {
	firstFrame := 6
	lastFrame := 17

	headerFile, err := os.Create("animation.h")
	check(err)
	defer headerFile.Close()

	writer := bufio.NewWriter(headerFile)
	defer writer.Flush()

	_, err = fmt.Fprintf(writer, "#define ANIMATION_BMP_COUNT    % 4d\n", lastFrame-firstFrame+1)
	check(err)
	_, err = fmt.Fprintf(writer, "#define ANIMATION_BMP_HEIGHT   % 4d\n", 64)
	check(err)
	_, err = fmt.Fprintf(writer, "#define ANIMATION_BMP_WIDTH    % 4d\n", 128)
	check(err)
	_, err = fmt.Fprintf(writer, "static const unsigned char PROGMEM animation_bmps[%d][%d] = {\n", lastFrame-firstFrame+1, 64*16)
	check(err)

	for frame := firstFrame; frame <= lastFrame; frame++ {
		frameIndex := frame - firstFrame

		imageFile, err := os.Open(fmt.Sprintf("frames/%04d.png", frame))
		check(err)
		defer imageFile.Close()

		img, err := png.Decode(imageFile)
		check(err)

		scaledImage := resize.Resize(128, 0, img, resize.Lanczos3)

		_, err = fmt.Fprintf(writer, "  { // frame/index %d\n", frameIndex)
		check(err)
		for rowIndex := 0; rowIndex < 64; rowIndex++ {
			_, err = fmt.Fprintf(writer, "    ")
			check(err)
			for byteNumber := 0; byteNumber < 16; byteNumber++ {
				_, err = fmt.Fprintf(writer, "B")
				check(err)
				for byteIndex := 0; byteIndex < 8; byteIndex++ {
					x := byteNumber*8 + byteIndex
					y := rowIndex
					r, _, _, _ := scaledImage.At(x, y).RGBA()
					bit := 0
					// fmt.Printf("%d ", r)
					if r > 32768 {
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

	fmt.Fprintf(writer, "};\n\n")

}
