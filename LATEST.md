Changes in v2.0.1
-----------------
- removed hardcoded rpath that broke Fedora RPM builds (#126; thanks to Joachim Frieben)
- link Fortran tests against libgiza explicitly (#124)
- fixed giza_qtextlen leaving Unicode text length uninitialised, which failed test-unicode on 32-bit (#125)
