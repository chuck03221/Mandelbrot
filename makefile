CFLAG = -Wall
XFLAG = -lpthread -O3
CXX = g++
.o.c:
	$(CXX) -c $< $(CFLAG)

Mandelbrot:Mandelbrot_main.o
	$(CXX) -o $@ $^ $(XFLAG)

.PHNOY:
clean:
	rm *.o Mandelbrot
