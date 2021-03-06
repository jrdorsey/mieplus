// Calling wrapper for Warren Wiscombe's Mie scattering code
// James Dorsey, University of Manchester, 2010

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <complex>
#include <cmath>
#include <cstring>
#include <iomanip>
using namespace std;

   extern "C"
   {
   void miev0_(float *, complex<float> *, int *, float *, int *, int *, float *, int *, int *, int *, int *, float *, float *, float *, float *, complex<float> *, complex<float> *, complex<float> *, complex<float> *, complex<float> *, complex<float> *, float *);
   }

   int main(int argc, char* argv[])
   {
   char *probetype;
   complex<float> SFORW, SBACK, TFORW[2], TBACK[2];
   int NUMANG, NMOM, IPOLZN, MOMDIM, i, j, PRNT[2], PERFCT, ANYANG, twinrange, NUMDIM;
   float XX, MIMCUT, QEXT, QSCA, GQSC, PMOM[2][2], SPIKE;
   float DMIN, DMAX, DSTEP, NR, NC, anglestep, maxoutangle, minoutangle, rmaxoutangle, rminoutangle, nang, ndim;
   float theta, XMIN, XMAX, XSTEP, XSAVE, I1, I2, lambda, Qscat, pi;
   complex<float> *S1, *S2;
   float *XMU, *I_unpol, *QscatTot;
   pi=4.0*atan(1.0);
   twinrange = 0;
   rminoutangle = 0.0;
   rmaxoutangle = 0.0;
   if(argc == 2)
      {
      if(!strcmp(argv[1],"--help")||!strcmp(argv[1],"-h"))
         {
         cout << endl << "This program is not debugged until the last user is dead." << endl << endl << "Copyright James Dorsey - University of Manchester May 2010" << endl << endl;
         return -7;
         }
      }
   if((argc<7||argc>=9))
      {
      cout << endl << "Usage: mie++ probe dmin dmax dstep nr nc [astep]" << endl << endl << "  probe - DMT probe type" << endl;
      cout << "  dmin  - Minimum particle diameter" << endl << "  dmax  - Maximum particle diameter" << endl;
      cout << "  dstep - Particle diameter resolution" << endl << "  nr    - Real component of refractive index" << endl;
      cout <<"  nc    - Imaginary component of refractive index" << endl << "  astep - Angular resolution to integrate over in degrees";
      cout << endl << endl << "i.e. There MUST be one text and five or six numeric arguments" << endl << endl;
      return -8;
      }
   probetype = argv[1];
   if(!strcmp(probetype,"CAS")||!strcmp(probetype,"CAS-F"))
      {
      minoutangle = 4.0;
      maxoutangle = 12.0;
      lambda = 0.680;
      }
   else if(!strcmp(probetype,"CAS-B"))
      {
      minoutangle = 168;
      maxoutangle = 176;
      lambda = 0.680;
      }
   else if(!strcmp(probetype,"PCASP"))
      {
      minoutangle = 35.0;
      maxoutangle = 120.0;
      rminoutangle = 60;
      rmaxoutangle = 145;
      lambda = 0.6328;
      twinrange = 1;
      }
   else if(!strcmp(probetype,"GRIMM"))
      {
      minoutangle = 30.0;
      maxoutangle = 150.0;
      rminoutangle = 81.0;
      rmaxoutangle = 99.0;
      lambda = 0.655;
      twinrange = 1;
      }
   else if(!strcmp(probetype,"UHSAS"))
      {
      minoutangle = 36.0;
      maxoutangle = 145.0;
      lambda = 1.054;
      }
   else if(!strcmp(probetype,"CDP"))
      {
      minoutangle = 4.0;
      maxoutangle = 12.0;
      lambda = 0.658;
      }
   else if(!strcmp(probetype,"FSSP"))
      {
      minoutangle = 4.0;
      maxoutangle = 12.0; 
      lambda = 0.658;
      }
   else if(!strcmp(probetype,"WIBS"))
      {
      minoutangle = 3.5;
      maxoutangle = 12.6;
      lambda = 0.635;
      }
   else if(!strcmp(probetype,"BCP"))
      {
      minoutangle = 144.0;
      maxoutangle = 156.0;
      lambda = 0.658;
      }
   else if(!strcmp(probetype,"APSD")||!strcmp(probetype,"APSD-S"))
      {
      cout << "This is currently set to GRIMM optical arrangement" << endl;
      minoutangle = 31.0;
      maxoutangle = 149.0;
      lambda = 0.658;
      }
   else if(!strcmp(probetype,"APSD-B"))
      {
      minoutangle = 147.0;
      maxoutangle = 173.0;
      lambda = 0.658;
      }
   else if(!strcmp(probetype,"BLUE_BCP"))
      {
      minoutangle = 144.0;
      maxoutangle = 156.0;
      lambda = 0.450;
      }
   else if(!strcmp(probetype,"LIDAR"))
      {
      minoutangle = 179.7;
      maxoutangle = 180.0;
      lambda = 0.355;
      }
   else if(!strcmp(probetype,"TOTAL"))
      {
      minoutangle = 0.0;
      maxoutangle = 180.0;
      lambda = 0.355;
      }
   else
      {
      cerr << endl << "You MUST specify a probe type. Only the following are currently supported:" << endl << endl << "  APSD" << endl << "  APSD-B" << endl << "  APSD-S" << endl << "  BCP" << endl << "  BLUE_BCP" << endl << "  CAS" << endl << "  CAS-B" << endl << "  CAS-F" << endl << "  CDP" << endl << "  FSSP" << endl << "  GRIMM (1.10x series)" << endl << "  PCASP" << endl << "  UHSAS" << endl << "  WIBS" << endl << endl << "Note that probe type is case sensitive in this version of the software" << endl << "CAS is equivalent to CAS-F (i.e. forward scatter) while CAS-B is backscatter" << endl << "Same convention for APSD with APSD and APSD-S being sidescatter" << endl << endl;
      return -9;
      }
   if(EOF == sscanf(argv[2], "%f", &DMIN))
      {
      cerr << endl << endl << "Problem reading DMIN" << endl << endl;
      return -10;
      }
   if(EOF == sscanf(argv[3], "%f", &DMAX))
      {
      cerr << endl << endl << "Problem reading DMAX" << endl << endl;
      return -11;
      }
   if(EOF == sscanf(argv[4], "%f", &DSTEP))
      {
      cerr << endl << endl << "Problem reading DSTEP" << endl << endl;
      return -12;
      }
   if(EOF == sscanf(argv[5], "%f", &NR))
      {
      cerr << endl << endl << "Problem reading NR" << endl << endl;
      return -13;
      }
   if(EOF == sscanf(argv[6], "%f", &NC))
      {
      cerr << endl << endl << "Problem reading NC" << endl << endl;
      return -14;
      }
   anglestep = 0.1;
   if(argc>=8)
      {
      if(EOF == sscanf(argv[7], "%f", &anglestep))
            {
            cerr << endl << endl << "Problem reading ANGLESTEP" << endl << endl;
            return -15;
            }
      }
   if(anglestep<0.009||anglestep>1.0)
      {
      cerr << endl << endl << "Error: Invalid angle step. Must be 0.01 to 1.0" << endl << endl;
      return -22;
      }
   complex<float> CREFIN = complex<float>(NR, NC);
   PERFCT = false;
   MIMCUT = 0.0000000001;
   ANYANG = 1;
   PRNT[0] = 0;
   PRNT[1] = 0;
   NMOM = 0;
   IPOLZN = 0;
   MOMDIM = 1;
   XMIN = pi * DMIN / lambda;
   XMAX = pi * DMAX / lambda;
   XSTEP = pi * DSTEP / lambda;
   ndim = (XMAX - XMIN) / XSTEP;
   NUMDIM = int(ndim+0.5)+1;
   if(NUMDIM<2)
      {
      cerr << endl << endl << "Error: Diameter step too big for range selected." << endl << endl;
      }
   QscatTot = new float [NUMDIM];
   for(j=0;j<NUMDIM;j++)
      {
      QscatTot[j] = 0.0;
      }
// Start of geometry specific stuff - this all needs to be re-run for the second scattering range
   nang = (maxoutangle - minoutangle) / anglestep;
   NUMANG = int(nang+0.5);
   if(NUMANG<1)
      {
      cerr << endl << endl << "Error: Angle step too big for first range selected." << endl << endl;
      }
   S1 = new complex<float> [NUMANG];
   S2 = new complex<float> [NUMANG];
   XMU = new float [NUMANG];
   I_unpol = new float [NUMANG];
   theta = minoutangle;
   for(i=0;i<NUMANG;i++)
      {
      XMU[i] = cos(theta * pi / 180.0);
      theta = theta + anglestep;
//      printf("%f    %f    %f\n", theta, XMU[i], cos(theta * pi / 180.0));
      }
   XX = XMIN;
   cout << argv[1] << " Diameter, Qscat (" << real(CREFIN) << " + " << imag(CREFIN) << ")" << endl;
   for(j=0;j<NUMDIM;j++)
      {
      XSAVE = XX;
      miev0_(&XX,&CREFIN,&PERFCT,&MIMCUT,&ANYANG,&NUMANG,XMU,&NMOM,&IPOLZN,&MOMDIM,PRNT,&QEXT,&QSCA,&GQSC,*PMOM,&SFORW,&SBACK,S1,S2,TFORW,TBACK,&SPIKE);
      XX = XSAVE;
      for(i=0;i<NUMANG;i++)
         {
         I1 = pow(real(S1[i]),2) + pow(imag(S1[i]),2);
         I2 = pow(real(S2[i]),2) + pow(imag(S2[i]),2);
         I_unpol[i] = I1+I2;
         }
      Qscat = 0.0;
      for(i=0;i<NUMANG-1;i++)
         {
         Qscat = Qscat + (0.5 * (anglestep * (pi/180)) * (I_unpol[i] * sin(acos(XMU[i])) + I_unpol[i+1] * sin(acos(XMU[i+1]))));
         }
      Qscat = Qscat * pow(lambda,2) / (4 * pi);
      QscatTot[j] += Qscat;
      XX = XX + XSTEP;
      }
   delete [] S1;
   delete [] S2;
   delete [] XMU;
   delete [] I_unpol;

   if(twinrange == 1)
      {
      nang = (rmaxoutangle - rminoutangle) / anglestep;
      NUMANG = int(nang+0.5);
      if(NUMDIM<2)
         {
         cerr << endl << endl << "Error: Angle step too big for second range selected." << endl << endl;
         }
      S1 = new complex<float> [NUMANG];
      S2 = new complex<float> [NUMANG];
      XMU = new float [NUMANG];
      I_unpol = new float [NUMANG];
      theta = rminoutangle;
      for(i=0;i<NUMANG;i++)
         {
         XMU[i] = cos(theta * pi / 180.0);
         theta = theta + anglestep;
         }
      XX = XMIN;
      for(j=0;j<NUMDIM;j++)
         {
         XSAVE = XX;
         miev0_(&XX,&CREFIN,&PERFCT,&MIMCUT,&ANYANG,&NUMANG,XMU,&NMOM,&IPOLZN,&MOMDIM,PRNT,&QEXT,&QSCA,&GQSC,*PMOM,&SFORW,&SBACK,S1,S2,TFORW,TBACK,&SPIKE);
         XX = XSAVE;
         for(i=0;i<NUMANG;i++)
            {
            I1 = pow(real(S1[i]),2) + pow(imag(S1[i]),2);
            I2 = pow(real(S2[i]),2) + pow(imag(S2[i]),2);
            I_unpol[i] = I1+I2;
            }
         Qscat = 0.0;
         for(i=0;i<NUMANG-1;i++)
            {
            Qscat = Qscat + (0.5 * (anglestep * (pi/180)) * (I_unpol[i] * sin(acos(XMU[i])) + I_unpol[i+1] * sin(acos(XMU[i+1]))));
            }
         Qscat = Qscat * pow(lambda,2) / (4 * pi);
         QscatTot[j] += Qscat;
         XX = XX + XSTEP;
         }
      delete [] S1;
      delete [] S2;
      delete [] XMU;
      delete [] I_unpol;
      }

   if(!strcmp(argv[1],"LIDAR"))
      {
      for(j=0;j<NUMDIM;j++)
         {
         cout.width(7);
         cout << setprecision(6) << DMIN + j * DSTEP;
         cout.width(2);
         cout << ", ";
         cout.width(13);
         cout << setprecision(12) << QEXT / QscatTot[j];
         cout.width(2);
         cout << ", ";
         cout.width(13);
         cout << setprecision(12) << QEXT;
         cout.width(2);
         cout << ", ";
         cout.width(13);
         cout << setprecision(12) << QSCA;
         cout.width(2);
         cout << ", ";
         cout.width(13);
         cout << setprecision(12) << *TBACK << endl;
         }
      }
   else
      {
      for(j=0;j<NUMDIM;j++)
         {
         cout.width(7);
         cout << setprecision(6) << DMIN + j * DSTEP;
         cout.width(2);
         cout << ", ";
         cout.width(13);
         cout << setprecision(12) << QscatTot[j] << endl;
         }
      }
   delete [] QscatTot;
   return 0;
   }
