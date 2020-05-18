#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
using namespace std;

int main (int argc, char** argv)
  {
  int mlines, clines, dlines, i, j, HitCount, HitIndex[20000], k;
  string line, token;
  vector <string> array;
  double diff, TestQScat, *ControlDiameter, *ControlQScat, *MappedDiameter, *MappedQScat, *ThreshDiameter; // QScatTolerance
  FILE *outfile, *details;
  if(argc < 5 || argc > 6)
    {
      cout << endl<< "Usage: ri_map controlfile mappingfile thresholdfile summaryfile [detailfile]" << endl << endl <<  "Where controlfile is typically water, mappingfile may be" << endl << "ash or other another aerosol type, and thresholdfile contains" << endl << "the diameter limits to be re-mapped" << endl << endl << "summaryfile (and optionally detailfile) are the names for the output files." << endl << endl;
      return -9;
    }
//  QScatTolerance = 5.0;
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
      fprintf(details,"ThreshDiameter, LowControlDiameter, MedianControlDiameter, MaxControlDiameter, DHitCount, DMedianHitIndex, MedianQScatTarget, LowQScat, MedianQScat, MaxQScat, QHitCount, QMedianHitIndex, MinMappedDiameter, MedianMappedDiameter, MaxMappedDiameter\n");
    }
  fprintf(outfile,"ThreshDiameter[i], MinMappedDiameter, MedianMappedDiameter, MaxMappedDiameter\n");
  for(i=0;i<dlines-1;i++) // Run over all threshold values
    {
      for(j=0;j<20;j++)
        {
        HitIndex[j] = 0;
        }
      HitCount = 0;
      for(k=0;k<mlines-1;k++) // Run over all mapped file values
        {
          diff = abs(ThreshDiameter[i] - ControlDiameter[k]);
          if(diff<0.01)
            {
              HitIndex[HitCount] = k; // HitIndex is a list of control diameters within 10nm of the current threshold values
              HitCount++;
            }
        }
      if(argc > 5)
        {
          fprintf(details,"%lf, %lf, %lf, %lf, %i, %i, %lf, ", ThreshDiameter[i], ControlDiameter[HitIndex[0]], ControlDiameter[HitIndex[int((HitCount-1)/2)]], ControlDiameter[HitIndex[HitCount-1]], HitCount, HitIndex[int((HitCount-1)/2)], ControlQScat[HitIndex[int((HitCount-1)/2)]]);
        }
      TestQScat = MappedQScat[HitIndex[int((HitCount)/2)]]; // TestQScat is the mapped scattering cross section at the current threshold diameter
      HitCount = 0;
      for(j=0;j<20;j++)
        {
          HitIndex[j] = 0;
        }
      for(k=1;k<mlines-2;k++)
        {
          if((TestQScat < ControlQScat[k+1] && TestQScat > ControlQScat[k-1]) || (TestQScat > ControlQScat[k+1] && TestQScat < ControlQScat[k-1])) // Test for crossing the water scattering curve at the current threshold's mapped scattering cross section
            {
              HitIndex[HitCount] = k;
              HitCount++;
            }
        }
        if(argc > 5)
          {
            fprintf(details,"%lf, %lf, %lf, %i, %i, %lf, %lf, %lf\n", MappedQScat[HitIndex[0]], MappedQScat[HitIndex[int((HitCount-1)/2)]], MappedQScat[HitIndex[HitCount-1]],HitCount,HitIndex[int((HitCount-1)/2)],MappedDiameter[HitIndex[0]], MappedDiameter[HitIndex[int((HitCount-1)/2)]], MappedDiameter[HitIndex[HitCount-1]]);
          }
        fprintf(outfile, "%lf, %lf, %lf, %lf\n", ThreshDiameter[i], MappedDiameter[HitIndex[0]], MappedDiameter[HitIndex[int((HitCount-1)/2)]], MappedDiameter[HitIndex[HitCount-1]]);
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
