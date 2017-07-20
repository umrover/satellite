#include <equations.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <tiffio.h>
#include <getopt.h>

typedef struct _TIFF_Image {
	char *filename;
	TIFF *tiff;
	tdata_t buf;
	uint32 width;
	uint32 height;
	uint32 sampleformat;
	uint16 samplesperpixel;
	uint16 bitspersample;
} timg_t;

int main(int argc, char **argv) {
	// Process args
	int opt;

	char *equation_string = 0;

	char *requation = 0;
	char *gequation = 0;
	char *bequation = 0;

	size_t image_count = 0;
	size_t images_allocated = 4;
	timg_t *images = malloc(sizeof(timg_t) * images_allocated);

	char *output = 0;

	while ((opt = getopt(argc, argv, "hr:g:b:e:i:o:")) != -1)
	{
		switch (opt)
		{
			case 'e':
				if (requation || gequation || bequation)
				{
					fprintf(stderr, "Cannot specify a color equation and a brightness equation\n");
					return 1;
				}

				equation_string = malloc(strlen(optarg)+1);
				strcpy(equation_string, optarg);
				break;
			case 'r':
				if (equation_string)
				{
					fprintf(stderr, "Cannot specify a color equation and a brightness equation\n");
					return 1;
				}

				requation = malloc(strlen(optarg)+1);
				strcpy(requation, optarg);
				break;
			case 'g':
				if (equation_string)
				{
					fprintf(stderr, "Cannot specify a color equation and a brightness equation\n");
					return 1;
				}

				gequation = malloc(strlen(optarg)+1);
				strcpy(gequation, optarg);
				break;
			case 'b':
				if (equation_string)
				{
					fprintf(stderr, "Cannot specify a color equation and a brightness equation\n");
					return 1;
				}

				bequation = malloc(strlen(optarg)+1);
				strcpy(bequation, optarg);
				break;
			case 'i':
				// Reallocate images by a factor of two.
				if (images_allocated == image_count)
				{
					images = realloc(images, sizeof(timg_t) * (images_allocated *= 2));
				}

				images[image_count].filename = malloc(strlen(optarg)+1);
				strcpy(images[image_count].filename, optarg);

				images[image_count].tiff = 0;
				images[image_count].width = 0;
				images[image_count].height = 0;
				images[image_count].sampleformat = 0;
				images[image_count].bitspersample = 0;
				images[image_count].samplesperpixel = 0;

				image_count++;
				break;
			case 'o':
				output = malloc(strlen(optarg)+1);
				strcpy(output, optarg);
				break;
			case 'h':
				printf("     ____    ____  _____\n");
				printf("    |_   \\  /   _||  __ \\\n");
				printf("      |   \\/   |  | |__) | ___ __   __ ___  _ __\n");
				printf("      | |\\  /| |  |  _  / / _ \\\\ \\ / // _ \\| '__|\n");
				printf("     _| |_\\/_| |_ | | \\ \\| (_) |\\ V /|  __/| |\n");
				printf("    |_____||_____||_|  \\_\\\\___/  \\_/  \\___||_|\n");
				printf("=====================================================\n");
				printf("             | Image Analysis Toolkit |\n");
				printf("             +------------------------+\n\n");

				printf("Arguments:\n");
				printf("    -e <equation>   Specifies the equation to use to process the image(s)\n");
				printf("    -r <equation>   Specifies the equation to use to process the image(s). Determines the red channel of the output.\n");
				printf("    -g <equation>   Specifies the equation to use to process the image(s). Determines the red channel of the output.\n");
				printf("    -b <equation>   Specifies the equation to use to process the image(s). Determines the red channel of the output.\n");
				printf("    -i <image>      The image(s) to use. Specify multiple times for multiple images.\n");
				printf("    -o <image>      The location to save the resulting PPM image.\n");
				printf("    -h              Displays this help menu\n\n");

				printf("Examples:\n");
				printf("    %s -e 'mul(%%1,2)' -i image.tiff -o result.ppm\n", argv[0]);
				printf("        + This will multiply the data in image.tiff by two, and\n");
				printf("              write the resulting image to result.ppm.\n");

				return 0;
				break;
		}
	}

	Equation *eq = 0;
	Equation *req = 0, *geq = 0, *beq = 0;
	if (equation_string)
	{
		eq = parseEquation(equation_string);

		if (!eq) {
			fprintf(stderr, "An error occurred while parsing the equation\n");
			return 1;
		}
	}
	else if (requation && gequation && bequation)
	{
		req = parseEquation(requation);
		geq = parseEquation(gequation);
		beq = parseEquation(bequation);

		if (!req || !geq || !beq)
		{
			fprintf(stderr, "Invalid color equation\n");
			return 1;
		}
	}
	else if (requation || gequation || bequation)
	{
		fprintf(stderr, "Color mode requres all of -r, -g, -b to be specified.\n");
		return 1;
	}
	else
	{
		fprintf(stderr, "Equation required.\n");
		return 1;
	}

	if (image_count == 0)
	{
		fprintf(stderr, "Image required.\n");
		return 1;
	}

	for (size_t i = 0; i < image_count; i++)
	{
		images[i].tiff = TIFFOpen(images[i].filename, "r");

		if (images[i].tiff)
		{
			// Note: uint32 is *not* uint32_t. uint32 is defined as unsigned long.
			TIFFGetField(images[i].tiff, TIFFTAG_SAMPLEFORMAT, &images[i].sampleformat);

			TIFFGetField(images[i].tiff, TIFFTAG_BITSPERSAMPLE, &images[i].bitspersample);
			TIFFGetField(images[i].tiff, TIFFTAG_SAMPLESPERPIXEL, &images[i].samplesperpixel);

			// Default to uint8 for the format (Conformant with TIFFv5/v6)
			if (images[i].sampleformat == 0) images[i].sampleformat = 1;

			if (images[i].sampleformat > 3 || images[i].sampleformat < 1)
			{
				fprintf(stderr, "Unsupported tiff sample format %u\n", images[i].sampleformat);
				return 1;
			}

			TIFFGetField(images[i].tiff, TIFFTAG_IMAGELENGTH, &images[i].height);
			TIFFGetField(images[i].tiff, TIFFTAG_IMAGEWIDTH, &images[i].width);
			images[i].buf = _TIFFmalloc(TIFFScanlineSize(images[i].tiff));
		}
		else
		{
			fprintf(stderr, "Failed to open %s\n", images[i].filename);
			return 1;
		}

		if ((images[i].height != images[0].height) || (images[i].width != images[0].width))
		{
			fprintf(stderr, "Image size mismatch. '%s' does not match '%s'\n", images[i].filename, images[0].filename);
		}

		if ((images[i].sampleformat != images[0].sampleformat) ||
			(images[i].samplesperpixel != images[0].samplesperpixel) ||
			(images[i].bitspersample != images[0].bitspersample))
		{
			fprintf(stderr, "Image format mismatch. '%s' does not match '%s'\n", images[i].filename, images[0].filename);
		}
	}

	double *values = calloc(sizeof(double), image_count * images[0].samplesperpixel);

	FILE *outfile = fopen(output, "w+");
	if (!outfile)
	{
		fprintf(stderr, "Could not open file '%s' for writing.\n", output);
		return 1;
	}

	// Write format
	fprintf(outfile, "P6\n%u %u\n255\n", images[0].width, images[0].height);

	printf("\n");

	uint32 last_progress = 0;

	for (uint32 row = 0; row < images[0].height; row++)
	{
		if ((row * 1000)/images[0].height > last_progress)
		{
			last_progress = (row * 1000)/images[0].height;
			size_t progress = 0;
			printf("\r|");
			for (; progress < 40 * row / images[0].height; progress++)
			{
				putchar('+');
			}

			for (; progress < 40; progress++)
			{
				putchar(' ');
			}

			printf("| %3.1f%%", ((float) row * 100)/images[0].height);
			fflush(stdout);
		}

		for (size_t img = 0; img < image_count; img++)
		{
			TIFFReadScanline(images[img].tiff, images[img].buf, row, 0);
		}

		for (uint32 i = 0; i < images[0].width; i++)
		{
			if (images[0].samplesperpixel == 4)
			{
				uint32 val = ((uint32 *) images[0].buf)[i];
				double r = (double) TIFFGetR(val);
				double g = (double) TIFFGetG(val);
				double b = (double) TIFFGetB(val);

				int ir = (int) executeEquation(eq, &r, 1);
				int ig = (int) executeEquation(eq, &g, 1);
				int ib = (int) executeEquation(eq, &b, 1);

				if (ir < 0) ir = 0;
				if (ir > 255) ir = 255;

				if (ig < 0) ig = 0;
				if (ig > 255) ig = 255;

				if (ib < 0) ib = 0;
				if (ib > 255) ib = 255;

				val = (((int) ir) & 0xff) | ((((int) ig) & 0xff)<<8) | ((((int) ib) & 0xff)<<16);

				// Ignore alpha component
				fwrite((char *) &val, sizeof(unsigned int) - 1, 1, outfile);
			}
			else if (images[0].samplesperpixel == 1)
			{
				for (size_t img = 0; img < image_count; img++)
				{
					values[img] = (double) ((uint8 *) images[img].buf)[i];
				}

				uint8 r = 0, g = 0, b = 0;

				if (eq)
				{
					int result = (int) executeEquation(eq, values, image_count);
					if (result < 0) result = 0;
					if (result > 255) result = 255;

					r = g = b = result;
				}
				else
				{
					int result = (int) executeEquation(req, values, image_count);
					if (result < 0) result = 0;
					if (result > 255) result = 255;
					r = result;

					result = (int) executeEquation(geq, values, image_count);
					if (result < 0) result = 0;
					if (result > 255) result = 255;
					g = result;

					result = (int) executeEquation(beq, values, image_count);
					if (result < 0) result = 0;
					if (result > 255) result = 255;
					b = result;
				}

				fwrite((char *) &r, 1, 1, outfile); // Red
				fwrite((char *) &g, 1, 1, outfile); // Green
				fwrite((char *) &b, 1, 1, outfile); // Blue
			}
		}
	}

	printf("\n");

	for (size_t i = 0; i < image_count; i++)
	{
		_TIFFfree(images[i].buf);
		TIFFClose(images[i].tiff);
	}

	free(values);
	if (eq) freeEquation(eq);
	if (req) freeEquation(req);
	if (geq) freeEquation(geq);
	if (beq) freeEquation(beq);

	return 0;
}
