#include <stdlib.h>
#include <stdio.h>
#include <math.h>
//#include <gsl/gsl_errno.h>
//#include <gsl/gsl_spline.h>
//#include <gsl/gsl_bspline.h>

bool polynomialfit(int obs, int degree, double *dx, double *dy, double *store);

int mieInterp(int jtot, double *thresholds, double *qScatThresh, FILE* inScatFile)
// Populates arrays of diameter and qscat with cubic spline interpolated versions of the Mie curve in inScatFile.
// Resolution is detemined by jtot and thresholds values.
{
int i, r, j;
int polyDegree = 6;
double dummy1, dummy2;
double *qScat, *diameter, *coeffs;
char buffer[1024];

if (jtot < 1)
    {
    printf("\nError\n\nNeed a set number of layers (%i)\n", jtot);
    return(-1);
    }

// Move to start of QScat file to count the lines
fseek(inScatFile, 0, SEEK_SET);
int number_of_lines = 0;
int ch;
while (EOF != (ch=getc(inScatFile)))
if ('\n' == ch)
    ++number_of_lines;
number_of_lines--;

// Allocate arrays for data
diameter = new double[(number_of_lines*sizeof(double))];
qScat = new double[(number_of_lines*sizeof(double))];

// Move back to start of QScat file to actually read this time
fseek(inScatFile, 0, SEEK_SET);
// This is in an IF statement to suppress unused result warning
if (fgets(buffer, 1024, inScatFile) == NULL)
  {
  printf("Empty input file\n");
  }

for(i=0; i < number_of_lines; i++)
  {
  r = fscanf(inScatFile,"%lf,%lf", &dummy1, &dummy2);
  if(r == 0)
    {
    printf("Error in Qscat file scanning\n");
    }
  diameter[i] = dummy1;
  qScat[i] = dummy2;
//  printf("%lf, %lf\n", dummy1, dummy2);
  }
//printf("Done reading in data\n");

// Do polynomial fit to Mie curve
coeffs = new double[(polyDegree*sizeof(double))];

polynomialfit(number_of_lines, polyDegree, diameter, qScat, coeffs);

// This is the loop in which interpolation is done
for (i = 0; i < jtot-1; i++)
  {
//  printf("Interpolation iter: %i, %lf\n", i, thresholds[i]);
  qScatThresh[i] = coeffs[0];
  for (j = 1; j < polyDegree-1; j++)
    {
      qScatThresh[i] += coeffs[j] * pow(diameter[i], j);
    }
  }

//printf("Output file: %s\n", outFile);
//fptr1=fopen(outFile,"w");
//fprintf(fptr1,"Diameter, Qscat\n");
//for(i = 0; i < jtot; i++)
//  {
//  fprintf(fptr1,"%8.6f, %8.6f\n", thresholds[i], qScatThresh[i]);
//  }  
//fclose(fptr1);

delete [] diameter;
delete [] qScat;
delete [] coeffs;
return(0);
}
