# Satellite
The MRover Satellite image analysis tool

To use, simply grab some satellite imagery from Landsat.

Example:

    LD_LIBRARY_PATH=. ./imgread -e "mul(div(1,255),%3,add(%1,%2))" -i LE07_L1TP_037033_20161125_20170116_01_T1_B1.TIF -i LE07_L1TP_037033_20161125_20170116_01_T1_B2.TIF -i LE07_L1TP_037033_20161125_20170116_01_T1_B3.TIF -o output.ppm


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

To build it, simply type `make`.
