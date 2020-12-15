package main

import (
	"bufio"
	"flag"
	"fmt"
	"image/png"
	"os"
	"path/filepath"
	"strings"
)

func check(err error) {
	if err != nil {
		panic(err)
	}
}

func main() {
	var invert bool
	flag.BoolVar(&invert, "invert", false, "flip white and black")
	flag.Parse()

	inputFilepath := flag.Args()[0]

	imageFile, err := os.Open(inputFilepath)
	check(err)
	defer imageFile.Close()

	img, err := png.Decode(imageFile)
	check(err)

	inputExtension := filepath.Ext(inputFilepath)
	inputFilepathWithoutExtension := inputFilepath[0 : len(inputFilepath)-len(inputExtension)]
	outputFilepath := inputFilepathWithoutExtension + ".h"
	outputName := filepath.Base(inputFilepathWithoutExtension)

	outputRowCount := img.Bounds().Max.Y
	outputByteCount := img.Bounds().Max.X / 8

	headerFile, err := os.Create(outputFilepath)
	check(err)
	defer headerFile.Close()

	writer := bufio.NewWriter(headerFile)
	defer writer.Flush()

	_, err = fmt.Fprintf(writer, "#define %s_BMP_HEIGHT % 4d\n", strings.ToUpper(outputName), outputRowCount)
	check(err)
	_, err = fmt.Fprintf(writer, "#define %s_BMP_WIDTH  % 4d\n", strings.ToUpper(outputName), img.Bounds().Max.X)
	check(err)
	_, err = fmt.Fprintf(writer, "static const unsigned char PROGMEM %s_bmp[%d] = {\n", outputName, outputRowCount*outputByteCount)
	check(err)

	for rowIndex := 0; rowIndex < outputRowCount; rowIndex++ {
		_, err = fmt.Fprintf(writer, "  ")
		check(err)
		for byteNumber := 0; byteNumber < outputByteCount; byteNumber++ {
			_, err = fmt.Fprintf(writer, "B")
			check(err)
			for byteIndex := 0; byteIndex < 8; byteIndex++ {
				x := byteNumber*8 + byteIndex
				y := rowIndex
				r, _, _, _ := img.At(x, y).RGBA()
				bit := 1
				if r > 32768 != invert {
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
