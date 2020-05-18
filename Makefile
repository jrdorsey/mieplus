# GNU Makefile for Mie scattering code
# James Dorsey, UC Berkeley 2005 / Manchester 2010

CC = g++
LNK = g++
WINCC = i586-mingw32msvc-g++
CFLAGS = -Wall -pedantic -O3
CMFLAGS = -Wall -O3
FC = f95
WINFC = i586-mingw32msvc-gfortran
WINLNK = i586-mingw32msvc-g++
FFLAGS = -O3
WINFFLAGS = -ff2c -O3
LFLAGS = -Wall -pedantic -O3 -g -lgfortran
WINLFLAGS = -Wall -pedantic -O3 -lgfortran
GSLFLAGS = -lgsl -lgslcblas 

ifeq (${CC},g++)
  FORTSRC = src/miev0.f
  ERRSRC = src/ErrPack.f
  MCSRC = src/mie++.cpp
  RCSRC = src/FindDiameters.cpp
  SPLNSRC = src/mieCurveSplineExpand.cpp
  POLYSRC = src/polyfitgsl.cpp
  else
    abort Unsupported CC=${CC}
endif

main: mie++ ri_map

windows: Win_ri_map Win_mie++

mie++: mie++.o miev0.o errpack.o
	${CC} -o mie++ errpack.o miev0.o mie++.o ${LFLAGS}

strip: main
	strip mie++
	strip ri_map

winstrip: windows
	i586-mingw32msvc-strip mie++.exe
	i586-mingw32msvc-strip ri_map.exe

miev0.o: ${FORTSRC}
	${FC} ${FFLAGS} -o miev0.o -c ${FORTSRC}

errpack.o: ${ERRSRC}
	${FC} ${FFLAGS} -o errpack.o -c ${ERRSRC}

mie++.o: ${MCSRC}
	${CC} ${CFLAGS} -o mie++.o -c ${MCSRC}

polyfit.o: ${POLYSRC}
	${CC} ${CFLAGS} -o polyfit.o -c ${POLYSRC} ${GSLFLAGS}

rispline.o: $(SPLNSRC) polyfit.o
	${CC} ${CFLAGS} -o rispline.o -c ${SPLNSRC} ${GSLFLAGS}

ri_map.o: ${RCSRC}
	${CC} ${CMFLAGS} -o ri_map.o -c ${RCSRC}

ri_map: rispline.o ri_map.o
	${LNK} ${CFLAGS} -o ri_map polyfit.o rispline.o ri_map.o ${GSLFLAGS}

Win_mie++: ${MCSRC} Winerrpack.o Winmiev0.o Winmie++.o
	${WINLNK} -o mie++.exe Winerrpack.o Winmiev0.o Winmie++.o ${WINLFLAGS}

WinMIEV0.o: ${FORTSRC}
	${WINFC} ${FFLAGS} -o Winmiev0.o -c ${FORTSRC}

Winerrpack.o: ${ERRSRC}
	${WINFC} ${FFLAGS} -o Winerrpack.o -c ${ERRSRC}

Winmie++.o: ${MCSRC}
	${WINCC} ${CFLAGS} -o Winmie++.o -c ${MCSRC}

Win_ri_map: $(RCSRC)
	${WINLNK} -o ri_map.exe Winpolyfit.o Winrispline.o Winri_map.o ${WINLFLAGS} ${GSLFLAGS}

Winpolyfit.o: ${POLYSRC}
	${WINCC} ${CFLAGS} -o Winpolyfit.o -c ${POLYSRC} ${GSLFLAGS}

Winrispline.o: $(SPLNSRC) Winpolyfit.o
	${WINCC} ${CFLAGS} -o Winrispline.o -c ${SPLNSRC} ${GSLFLAGS}

Win_ri_map.o: ${RCSRC}
	${WINCC} ${CMFLAGS} -o Winri_map.o -c ${RCSRC}

tidy: mie++ ri_map
	rm -f *.o

wintidy: mie++.exe ri_map.exe
	rm -f *.o 

clean:
	rm -rf *.o
	rm -f mie++
	rm -f ri_map
	rm -f mie++.exe
	rm -f ri_map.exe
#	rm -rf output/*
