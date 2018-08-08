#include "fftw3.h"

#include <complex>



// http://www.fftw.org/fftw3_doc/
// http://www.fftw.org/fftw3_doc/Complex-DFTs.html#Complex-DFTs
// http://www.fftw.org/fftw3_doc/Guru-Complex-DFTs.html#Guru-Complex-DFTs



using namespace std;

using Complex = complex<double>;



int main() {

   const size_t FFT_SIZE  = 12;
   const size_t FFT_COUNT =  1;  // How many FFTs to perform together.



   // Allocate FFT buffer. We'll use the same buffer for input and output.
   // FFTW uses its own complex data type, but we want to use std::complex.
   // The memory layout is identical, so it's OK.

   auto buffer = (Complex *)fftw_malloc(FFT_SIZE * sizeof(Complex));



   // Setup parameters for guru interface. There's a simpler interface,
   // but we use the guru interface in the scanner.

   int       rank     = 1;            // 1D FFT.
   int       n[]      = {FFT_SIZE};   // This has an element per dimension. I don't get it, but we only need 1 dimension, so it's easy enough.
   int       howmany  = FFT_COUNT;
   fftw_complex * in  = reinterpret_cast<fftw_complex *>(&buffer[0]);    // Input buffer.
   fftw_complex * out = reinterpret_cast<fftw_complex *>(&buffer[0]);    // Output buffer.
   int *     inembed  = nullptr;      // Something.
   int *     onembed  = nullptr;
   int       istride  = 1;            // Our input data is contiguous, so stride is 1.
   int       ostride  = 1;            // We also want our output to be contiguous.
   int       idist    = FFT_SIZE;     // Distance between data sets in buffer. We only do 1 FFT, so it doesn't matter.
   int       odist    = FFT_SIZE;
   unsigned  flags    = FFTW_MEASURE | FFTW_DESTROY_INPUT; // Different flags to tweak performance.



   // Create plan for FFT.

   fftw_plan plan = fftw_plan_many_dft( rank, n, howmany
                                      , in , inembed, istride, idist
                                      , out, onembed, ostride, odist
                                      , FFTW_FORWARD, flags
                                      );


   // Create plan for IFFT.

   fftw_plan iplan = fftw_plan_many_dft( rank, n, howmany
                                       , out, onembed, ostride, odist
                                       , in , inembed, istride, idist
                                       , FFTW_BACKWARD, flags
                                       );



   // Put some data in the buffer.

   for (size_t k = 0; k < FFT_SIZE; ++k) {

      buffer[k] = Complex(k, k);
      printf("buffer[%i] = %f + i * %f\n", k, buffer[k].real(), buffer[k].imag());
   }



   // Do FFT using the forward plan.

   fftw_execute(plan);



   printf("\n");
   for (size_t k = 0; k < FFT_SIZE; ++k) {

     // buffer[k] = Complex(k, k);
      printf("buffer[%i] = %f + i * %f\n", k, buffer[k].real(), buffer[k].imag());
   }



   // Clean up.

   fftw_destroy_plan(plan);
   fftw_destroy_plan(iplan);
   fftw_free(buffer);



   return 0;
}