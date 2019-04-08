#include <stdio.h>
#include "cs1300bmp.h"
#include <iostream>
#include <fstream>
#include "Filter.h"

using namespace std;

#include "rdtsc.h"

//
// Forward declare the functions
//
Filter * readFilter(string filename);
double applyFilter(Filter *filter, cs1300bmp *input, cs1300bmp *output);

int
main(int argc, char **argv)
{

  if ( argc < 2) {
    fprintf(stderr,"Usage: %s filter inputfile1 inputfile2 .... \n", argv[0]);
  }

  //
  // Convert to C++ strings to simplify manipulation
  //
  string filtername = argv[1];

  //
  // remove any ".filter" in the filtername
  //
  string filterOutputName = filtername;
  string::size_type loc = filterOutputName.find(".filter");
  if (loc != string::npos) {
    //
    // Remove the ".filter" name, which should occur on all the provided filters
    //
    filterOutputName = filtername.substr(0, loc);
  }

  Filter *filter = readFilter(filtername);

  double sum = 0.0;
  int samples = 0;

  for (int inNum = 2; inNum < argc; inNum++) {
    string inputFilename = argv[inNum];
    string outputFilename = "filtered-" + filterOutputName + "-" + inputFilename;
    struct cs1300bmp *input = new struct cs1300bmp;
    struct cs1300bmp *output = new struct cs1300bmp;
    int ok = cs1300bmp_readfile( (char *) inputFilename.c_str(), input);

    if ( ok ) {
      double sample = applyFilter(filter, input, output);
      sum += sample;
      samples++;
      cs1300bmp_writefile((char *) outputFilename.c_str(), output);
    }
    delete input;
    delete output;
  }
  fprintf(stdout, "Average cycles per sample is %f\n", sum / samples);

}

struct Filter *
readFilter(string filename)
{
  ifstream input(filename.c_str());

  if ( ! input.bad() ) {
    int size = 0;
    input >> size;
    Filter *filter = new Filter(size);
    int div;
    input >> div;
    filter -> setDivisor(div);
    for (int i=0; i < size; i++) {
      for (int j=0; j < size; j++) {
	int value;
	input >> value;
	filter -> set(i,j,value);
      }
    }
    return filter;
  } else {
    cerr << "Bad input in readFilter:" << filename << endl;
    exit(-1);
  }
}


double
applyFilter(struct Filter *filter, cs1300bmp *input, cs1300bmp *output)
{

    long long cycStart, cycStop;

    cycStart = rdtscll();

    // Local variables instead of dereferenced memory
    int inWidth = output -> width = input -> width;
    int inHeight = output -> height = input -> height;
    
    // Calculate sizes outside of loop
    int inputWidth = inWidth - 1;
    int inputHeight = inHeight - 1;
    
    // Moved getSize() call outside of loop
    int filterSize = filter -> getSize();
    
    // Moved getDivisor() call outside of loop
    int planeDivisor = filter -> getDivisor();
    
    // 2D array to store filter x, y values
    int xyFilterValues[filterSize][filterSize];
    
    // Fill in 2D array with filter values
    for (int i = 0; i < filterSize; i++) {
        for (int j = 0; j < filterSize; j++) {
            xyFilterValues[i][j] = filter -> get(i, j);
        }
    } 
  
    // Values for each plane in image
    int plane1Value, plane2Value, plane3Value, plane1Value2, plane2Value2, plane3Value2, plane1Value3, plane2Value3, plane3Value3;
    
    // Reduced the number of iterations for the row and col loops by a factor of 2
    for (int row = 1; row < inputWidth; row += 2) {
        for (int col = 1; col < inputHeight; col += 2) {
            // Set plane values to 0
            plane1Value = 0;
            plane2Value = 0;
            plane3Value = 0;
            plane1Value2 = 0;
            plane2Value2 = 0;
            plane3Value2 = 0;
            plane1Value3 = 0;
            plane2Value3 = 0;
            plane3Value3 = 0;      
       
            for (int i = 0; i < filterSize; i++) {
                // Initialize temp plane variables set to 0
                int tempPlaneValue1 = 0;
                int tempPlaneValue2 = 0;
                int tempPlaneValue3 = 0;
                int tempPlaneValue4 = 0;
                int tempPlaneValue5 = 0;
                int tempPlaneValue6 = 0;
                int tempPlaneValue7 = 0;
                int tempPlaneValue8 = 0;
                int tempPlaneValue9 = 0;

                // Initialize row variable outside column loop	
                int rowIndex = row + i - 1;
                int rowIndex2 = rowIndex + 1;

                for (int j = 0; j < filterSize; j++) {
                    // Initialize temporary variables to save calculation time
                    int xyValue = xyFilterValues[i][j];
                    int colIndex = col + j - 1;
                    int colIndex2 = colIndex + 1;
	  
                    // Process each plane individually instead of looping 
                    tempPlaneValue1 += input -> color[0][rowIndex][colIndex] * xyValue;
                    tempPlaneValue2 += input -> color[1][rowIndex][colIndex] * xyValue; 
                    tempPlaneValue3 += input -> color[2][rowIndex][colIndex] * xyValue;
                    tempPlaneValue4 += input -> color[0][rowIndex][colIndex2] * xyValue;
                    tempPlaneValue5 += input -> color[1][rowIndex][colIndex2] * xyValue; 
                    tempPlaneValue6 += input -> color[2][rowIndex][colIndex2] * xyValue;
                    tempPlaneValue7 += input -> color[0][rowIndex2][colIndex] * xyValue;
                    tempPlaneValue8 += input -> color[1][rowIndex2][colIndex] * xyValue; 
                    tempPlaneValue9 += input -> color[2][rowIndex2][colIndex] * xyValue;
                }

                // Add temp values to plane values
                plane1Value += tempPlaneValue1;
                plane2Value += tempPlaneValue2;
                plane3Value += tempPlaneValue3;
                plane1Value2 += tempPlaneValue4;
                plane2Value2 += tempPlaneValue5;
                plane3Value2 += tempPlaneValue6;
                plane1Value3 += tempPlaneValue7;
                plane2Value3 += tempPlaneValue8;
                plane3Value3 += tempPlaneValue9;
            }

            // Used divisor variable instead of making another function call
            // Does not calculate if divisor is 1
            if (planeDivisor != 1) {
                plane1Value /= planeDivisor;
                plane2Value /= planeDivisor;
                plane3Value /= planeDivisor;
                plane1Value2 /= planeDivisor;
                plane2Value2 /= planeDivisor;
                plane3Value2 /= planeDivisor;
                plane1Value3 /= planeDivisor;
                plane2Value3 /= planeDivisor;
                plane3Value3 /= planeDivisor;
            }

            // Used ternary operators instead of if statements
            // If plane value < 0 then set plane value to 0 else plane value > 255 set plane value to 255 else set to plane value
            plane1Value = plane1Value < 0 ? 0 : plane1Value > 255 ? 255 : plane1Value;
            plane2Value = plane2Value < 0 ? 0 : plane2Value > 255 ? 255 : plane2Value;
            plane3Value = plane3Value < 0 ? 0 : plane3Value > 255 ? 255 : plane3Value;
            plane1Value2 = plane1Value2 < 0 ? 0 : plane1Value2 > 255 ? 255 : plane1Value2;
            plane2Value2 = plane2Value2 < 0 ? 0 : plane2Value2 > 255 ? 255 : plane2Value2;
            plane3Value2 = plane3Value2 < 0 ? 0 : plane3Value2 > 255 ? 255 : plane3Value2;
            plane1Value3 = plane1Value3 < 0 ? 0 : plane1Value3 > 255 ? 255 : plane1Value3;
            plane2Value3 = plane2Value3 < 0 ? 0 : plane2Value3 > 255 ? 255 : plane2Value3;
            plane3Value3 = plane3Value3 < 0 ? 0 : plane3Value3 > 255 ? 255 : plane3Value3;

            // Set output values to plane values
            output -> color[0][row][col] = plane1Value;
            output -> color[1][row][col] = plane2Value;
            output -> color[2][row][col] = plane3Value;
            output -> color[0][row][col + 1] = plane1Value2;
            output -> color[1][row][col + 1] = plane2Value2;
            output -> color[2][row][col + 1] = plane3Value2;
            output -> color[0][row + 1][col] = plane1Value3;
            output -> color[1][row + 1][col] = plane2Value3;
            output -> color[2][row + 1][col] = plane3Value3;
        }
    }

    cycStop = rdtscll();
    double diff = cycStop - cycStart;
    double diffPerPixel = diff / (output -> width * output -> height);
    fprintf(stderr, "Took %f cycles to process, or %f cycles per pixel\n", diff, diff / (output -> width * output -> height));
    return diffPerPixel;
}
