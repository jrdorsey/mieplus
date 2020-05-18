#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
using namespace std;

int mieInterp(int jtot, double *thresholds, double *qScatThresh, FILE* inScatFile);

int main (int argc, char** argv)
  {
  int mlines, clines, dlines, i, j, HitCount, HitIndex[20000], k;
  string line, token;
  vector <string> array;
  double diff, TestQScat1, TestQScat2, splineTargetQScat, splineControlDiameter, *ControlDiameter, *ControlQScat, *MappedDiameter, *MappedQScat, *ControlSplineQScat, *MappedSplineQScat, *ThreshDiameter; // QScatTolerance
  FILE *outfile, *details, *tfptr;
  if(argc < 5 || argc > 6)
    {
      cout << endl<< "Usage: ri_map controlfile mappingfile thresholdfile summaryfile [detailfile]" << endl << endl <<  "Where controlfile is typically water, mappingfile may be" << endl << "ash or other another aerosol type, and thresholdfile contains" << endl << "the diameter limits to be re-mapped" << endl << endl << "summaryfile (and optionally detailfile) are the names for the output files." << endl << endl;
      return -9;
    }
//  QScatTolerance = 5.0;
  splineTargetQScat = 0.0;
  clines = 0;
  mlines = 0;
  dlines = 0;
  outfile = fopen(argv[4],"w");
  details = NULL;
  if(argc > 5)
    {
      details = fopen(argv[5],"w");
    }
//  printf("Tolerance = %lf\n", tolerance);
  ifstream cx (argv[1]);
  if (cx.is_open())
    {
      while (!cx.eof())
      {
        getline(cx,line);
        clines++;
      }
    }
  else cout << "Unable to open file"; 
  cx.close();
  ControlDiameter = new double [clines];
  ControlQScat = new double [clines];
  cx.open(argv[1]);
  for(i=0;i<clines-1;i++)
    {
      getline(cx,line);
      if(i>0)
        {
          sscanf(line.c_str(), "%lf, %lf\n", &ControlDiameter[i], &ControlQScat[i]);
        }
    }
  cx.close();
  
//  printf("Lines in control file: %i\n", clines-2);
  FILE* controlInScatTable = fopen(argv[1], "r");
  ControlSplineQScat = new double [clines-2];
  mieInterp(clines-2, ControlDiameter, ControlSplineQScat, controlInScatTable);
  fclose(controlInScatTable);
  
  ifstream mx (argv[2]);
  if (mx.is_open())
    {
      while (!mx.eof())
      {
        getline(mx,line);
        mlines++;
      }
    }
  else cout << "Unable to open file"; 
  mx.close();
  MappedDiameter = new double [mlines];
  MappedQScat = new double [mlines];
  mx.open(argv[2]);
  for(i=0;i<mlines-1;i++)
    {
      getline(mx,line);
      if(i>1)
        {
          sscanf(line.c_str(), "%lf, %lf\n", &MappedDiameter[i], &MappedQScat[i]);
        }
    }
  mx.close();
  
//  printf("Lines in mapped file: %i\n", mlines);
  FILE* mappedInScatTable = fopen(argv[2], "r");
  MappedSplineQScat = new double [mlines-2];
  mieInterp(mlines-2, MappedDiameter, MappedSplineQScat, mappedInScatTable);
  fclose(mappedInScatTable);

  tfptr = fopen("output/polytest.csv", "w");
  fprintf(tfptr, "Cdia, CQs, CSQs, Mdia, MQs, MSQs\n");
  for (i = 0; i < mlines-3; i++)
  {
      fprintf(tfptr,"%lf, %lf, %lf, %lf, %lf, %lf\n", ControlDiameter[i], ControlQScat[i], ControlSplineQScat[i], MappedDiameter[i], MappedQScat[i], MappedSplineQScat[i]);
  }
  fclose(tfptr);
  
  ifstream dx (argv[3]);
  if (dx.is_open())
    {
      while (!dx.eof())
      {
        getline(dx,line);
        dlines++;
      }
    }
  else cout << "Unable to open file"; 
  dx.close();
  ThreshDiameter = new double [dlines-1];
  dx.open(argv[3]);
  for(i=0;i<dlines-1;i++)
    {
      getline(dx,line);
      sscanf(line.c_str(), "%lf\n", &ThreshDiameter[i]);
    }
  dx.close();
  if(argc > 5)
    {
      fprintf(details,"ThreshDiameter, LowControlDiameter, MedianControlDiameter, MaxControlDiameter, DHitCount, DMedianHitIndex, MedianQScatTarget, LowQScat, MedianQScat, MaxQScat, QHitCount, QMedianHitIndex, MinMappedDiameter, SplineMatchDiameter, MedianMappedDiameter, MaxMappedDiameter\n");
    }
  fprintf(outfile,"ThreshDiameter, MinMappedDiameter, SplineMatchDiameter, MedianMappedDiameter, MaxMappedDiameter\n");
  for(i=0;i<dlines-1;i++) // Run over all threshold values
    {
      for(j=0;j<20000;j++)
        {
        HitIndex[j] = 0;
        }
      HitCount = 0;
      for(k=0;k<mlines-1;k++) // Run over all mapped file values
        {
          diff = abs(ThreshDiameter[i] - ControlDiameter[k]);
          if(diff<=0.01)
            {
              HitIndex[HitCount] = k; // HitIndex is a list of control diameters within 10nm of the current threshold values
              HitCount++;
            }
          if(ControlDiameter[k] == ThreshDiameter[i])
          {
              splineTargetQScat = MappedSplineQScat[k];
          }
        }
      
      if(argc > 5)
        {
          fprintf(details,"%lf, %lf, %lf, %lf, %i, %i, %lf, ", ThreshDiameter[i], *std::min_element(&ControlDiameter[HitIndex[0]] , &ControlDiameter[HitIndex[HitCount-1]]), ControlDiameter[HitIndex[int((HitCount-1)/2)]], *std::max_element(&ControlDiameter[HitIndex[0]] , &ControlDiameter[HitIndex[HitCount-1]]), HitCount, HitIndex[int((HitCount-1)/2)], ControlQScat[HitIndex[int((HitCount-1)/2)]]);
        }
      TestQScat1 = *std::max_element(&MappedQScat[HitIndex[0]] , &MappedQScat[HitIndex[HitCount-1]]); // TestQScat1 is the upper mapped scattering cross section at the current threshold diameter
      TestQScat2 = *std::min_element(&MappedQScat[HitIndex[0]] , &MappedQScat[HitIndex[HitCount-1]]); // TestQScat1 is the lower mapped scattering cross section at the current threshold diameter
//      printf("%f    %f, %f\n",ThreshDiameter[i], TestQScat1, TestQScat2);
      HitCount = 0;
      for(j=0;j<20000;j++)
        {
          HitIndex[j] = 0;
        }
      for(k=1;k<mlines-2;k++)
        {
          if((TestQScat1 < ControlQScat[k+1] && TestQScat1 > ControlQScat[k-1]) || (TestQScat1 > ControlQScat[k+1] && TestQScat1 < ControlQScat[k-1]) || (TestQScat2 < ControlQScat[k+1] && TestQScat2 > ControlQScat[k-1]) || (TestQScat2 > ControlQScat[k+1] && TestQScat2 < ControlQScat[k-1]) || (TestQScat1 > ControlQScat[k] && ControlQScat[k]> TestQScat2)) // Test for crossing the water scattering curve at the current threshold's mapped scattering cross section
            {
              HitIndex[HitCount] = k;
              HitCount++;
            }
          if((splineTargetQScat < ControlQScat[k+1] && splineTargetQScat > ControlQScat[k-1]) || (splineTargetQScat > ControlQScat[k+1] && splineTargetQScat < ControlQScat[k-1])) // Test for crossing the water spline scattering curve at the current threshold's mapped spline scattering cross section
            {
              splineControlDiameter = ControlDiameter[k];
            }
        }
        if(argc > 5)
          {
            fprintf(details,"%lf, %lf, %lf, %lf, %i, %i, %lf, %lf, %lf\n", *std::min_element(&MappedQScat[HitIndex[0]], &MappedQScat[HitIndex[HitCount-1]]),splineControlDiameter,  MappedQScat[HitIndex[int((HitCount-1)/2)]], *std::max_element(&MappedQScat[HitIndex[0]] , &MappedQScat[HitIndex[HitCount-1]]),HitCount,HitIndex[int((HitCount-1)/2)], *std::min_element(&MappedDiameter[HitIndex[0]] , &MappedDiameter[HitIndex[HitCount-1]]), MappedDiameter[HitIndex[int((HitCount-1)/2)]], *std::max_element(&MappedDiameter[HitIndex[0]] , &MappedDiameter[HitIndex[HitCount-1]]));
          }
        fprintf(outfile, "%lf, %lf, %lf, %lf, %lf\n", ThreshDiameter[i], *std::min_element(&MappedDiameter[HitIndex[0]] , &MappedDiameter[HitIndex[HitCount-1]]), splineControlDiameter, MappedDiameter[HitIndex[int((HitCount-1)/2)]], *std::max_element(&MappedDiameter[HitIndex[0]] , &MappedDiameter[HitIndex[HitCount-1]]));
    }
  if(argc > 5)
    {
    fclose(details);
    }
  fclose(outfile);
  delete [] ControlDiameter;
  delete [] ControlQScat;
  delete [] MappedDiameter;
  delete [] MappedQScat;
  delete [] ThreshDiameter;
  return 0;
  }
