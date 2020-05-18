#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>

int main(int argc, char* argv[])
{
FILE *fptr1, *fptr2, *fptr3;
int i, r, jtot;
double dx, xi, cum_lai, lai, ht, cum_ht;
double dummy1, dummy2, dummy3, dummy4, dummy5;
double *lai_z, *pad, *ht_midpt, *lai_freq;

if (argc == 2)
{
  sscanf(argv[argc-1], "%d", &jtot);
}
else
{
printf("\nError\n\nNeed to enter number of layers on command line\ni.e. ./laiprofile n\n\n");
return(-1);
}

lai_z = new double[(jtot*sizeof(double))];
pad = new double[(jtot*sizeof(double))];
ht_midpt = new double[(10*sizeof(double))];
lai_freq = new double[(10*sizeof(double))];

fptr3=fopen("input/laiprofile.dat","r");
r = fscanf(fptr3,"%lf, %lf, %lf, %lf, %lf", &dummy1, &dummy2, &dummy3, &dummy4, &dummy5);
if(r == 0)
  {
  printf("Error in LAI file scanning\n");
  }
ht = dummy1;
lai = dummy2;
r = fscanf(fptr3," ");
for(i=0; i < 10; i++)
  {
  r = fscanf(fptr3,"%lf,%lf,", &dummy1, &dummy2);
  if(r == 0)
    {
    printf("Error in LAI file scanning\n");
    }
  ht_midpt[i] = dummy1;
  lai_freq[i] = dummy2;
//  printf("%f %f\n", ht_midpt[i], lai_freq[i]);
  }
fclose(fptr3);
//printf("\n%f %f\n\n", ht, lai);
dx = ht / jtot;
//printf("Height = %f    dx = %f    jtot = %i\n", ht, dx, jtot);
gsl_interp_accel *acc = gsl_interp_accel_alloc();
gsl_spline *spline = gsl_spline_alloc(gsl_interp_cspline, 10);
gsl_spline_init(spline, ht_midpt, lai_freq, 10);
xi = 0.0;
for (i = 0; i < jtot; i++)
  {
  xi = xi + (dx / ht);
//  printf("xi: %f\n",xi);
  if (xi < ht_midpt[0])
    {
    lai_z[i] = lai_freq[0];
//    printf("LAI_Z[%i] (%f) = %f (Down interp)\n", i, xi, lai_z[i]);
    }
  else if (xi > ht_midpt[9])
    {
    lai_z[i] = lai_freq[9]+((xi - ht_midpt[9]) * ((ht_midpt[9]-1)/lai_freq[9]-0.0));
    if (lai_z[i] < 0)
      {
      lai_z[i] = 0;
      }
//    printf("slope: %f\n", ((ht_midpt[9]-1)/lai_freq[9]-0.0)); //((xi-(ht_midpt[9]-xi))/(1-ht_midpt[9])));
//    printf(">> xi = %f\n", xi);
//    printf("LAI_Z[%i] (%f) = %f (Up interp) - %f\n", i, xi, lai_z[i], ht_midpt[9]);
//    printf("details: %f * ((%f - (%f-%f))/(1-%f))\n", lai_freq[9], xi, ht_midpt[9], xi, ht_midpt[9]);
    }
  else
    {
    lai_z[i] = gsl_spline_eval(spline, xi, acc);
//    printf("LAI_Z[%i] (%f) = %f\n", i, xi, lai_z[i]);
    }
  if(lai_z[i] < 0)
    {
    printf("Below zero error - i = %i, i/jt = %f\n\n", i, (float(i) / float(jtot)));
    lai_z[i] = lai_z[i-1] - (lai_z[i-1] * (float(i) / float(jtot)));
    if (i == 0)
      {
      lai_z[0] = 0.0;
      }
    }
//    xi = xi + (dx / ht);
  }
gsl_spline_free(spline);
gsl_interp_accel_free(acc);

cum_lai = 0.0;
for (i = 0; i < jtot; i++)
  {
  cum_lai += lai_z[i];
  }

for (i = 0; i < jtot; i++)
  {
  lai_z[i] = lai * lai_z[i] / cum_lai;
  }

fptr1=fopen("output/laiprofile.csv","w");
fptr2=fopen("tmp/laiprofile.acasa","w");
fprintf(fptr1,"Index, LAD, Sum\n");

cum_ht = 0.0;
cum_lai =0.0;
for(i = 0; i < jtot; i++)
  {
  cum_ht += dx;
  cum_lai += lai_z[i];
  pad[i]=lai_z[i]/dx;
//  printf("%i, %8.6f, %8.6f\n", i, lai_z[i], cum_lai);
  fprintf(fptr1,"%i, %8.6f, %8.6f\n", i, lai_z[i], cum_lai);
  fprintf(fptr2,"%9.6f\n", lai_z[i] / lai);
  }
fprintf(fptr2,"\n%9.6f\n", lai);
fclose(fptr1);
fclose(fptr2);
delete [] lai_z;
delete [] pad;
delete [] ht_midpt;
delete [] lai_freq;
return(0);
}
