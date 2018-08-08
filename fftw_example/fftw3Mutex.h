#include "PhylisEnvironmentInfoProviderWrapper.h" 
/*
This mutex shall be used to make all FFTW3 functions, except fftw(f)_execute(), threadsafe. Especially the 
planner will crash with multihtreaded calls. 

All wisdom creation and buffer allocation should be done locked.

When a named mutex is used, then for the time CFftw3Mutex is aquired, no windows API (e.g. GetModuleFilename() must be called, 
as Lock aquisition and Windows API may use the same internal lock (e.g. the loader lock) what yields to a race condition and 
a deadlock of the system.

Therefore the mutes schall be aquited only during the FFTW function calls. 
Example:
  //do some windows API here (reading wisdom files)
  {
      CFftw3Mutex       Fftw3Mutex;

      fftwf_forget_wisdom();
      
      fftwf_import_wisdom_from_string(pWisdomString);

      fftwf_malloc(sizeof(fftwf_complex) * N_FFT8192);

      fftwf_plan_dft_1d(N_FFT24,   m_pBuff, m_pBuff, FFTW_FORWARD,  iFFTWPlanControl);

      fftwf_export_wisdom(AppendCharToWisdomStringTree, &WisdomStringTree);
   }
   //do some windows API here (writing wisdom files)

Furthermore we use unnames windows mutex provided by the CEnvironmentInfoProviderWrapper class. But as this mutexes are 
implemented in an external DLL we should use the mutex as recommended above.
*/
#define MUTEX_NAME      "FFTW3_Mutex"

class CFftw3Mutex
{
public:
   CFftw3Mutex()
   {
      m_EnvironmentInfoProviderWrapper.CreateProcessMutex(MUTEX_NAME);
      Lock();
   }
   ~CFftw3Mutex() 
   {
      Unlock();
   }

   void                 Lock() 
   {
      m_EnvironmentInfoProviderWrapper.LockProcessMutex(MUTEX_NAME);
   }
   void                 Unlock() 
   {
      m_EnvironmentInfoProviderWrapper.UnlockProcessMutex(MUTEX_NAME);
   }
private:
   CEnvironmentInfoProviderWrapper
                        m_EnvironmentInfoProviderWrapper;
};