# Satellite
The MRover Satellite image analysis tool

To use, simply grab some satellite imagery from Landsat.

Example:

    ./imgread -e "mul(div(1,255),%3,add(%1,%2))" -i LE07_L1TP_037033_20161125_20170116_01_T1_B1.TIF -i LE07_L1TP_037033_20161125_20170116_01_T1_B2.TIF -i LE07_L1TP_037033_20161125_20170116_01_T1_B3.TIF -o output.ppm


This will apply the formula `((%1+%2)*%3/255)` to the image.

Formulas available for use:
 * add
 * sub
 * mul
 * div
 * exp
 * pow
 * abs
 * lt
 * lte
 * gt
 * gte
 * eq
 * and
 * or
 * not
 * xor
 * if

And you can use the `-h` flag for help using the program.

## Building the software
Supported Platforms:
 * x86 Linux
 * Intel Mac

### Downloading the equation library
Once cloned, type

    git submodule update --init

To download the required equation library.

### Configuring the environment
The environment needs to be configured to add the equation library to the path, as well as libtiff.
If libtiff is not installed, it will need to be downloaded.

To configure the environment, simply run

    source configenv

which will configure the environment for you, including downloading and building a local copy of libtiff.


### Compilation
To build it, simply type `make`.

Once the software is built, you may view the help page using this command:

    ./imgread -h
