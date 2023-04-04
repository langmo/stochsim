if not exist "build" mkdir build
cd build
cmake -G "Visual Studio 16 2019" -DBUILD_MATLAB=On -DCMAKE_BUILD_TYPE=Release ..
cmake --build .