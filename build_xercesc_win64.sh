# Download Xerces-C++ 3.2.3 from: http://xerces.apache.org/xerces-c/download.cgi
# Extract to ./libs/

cd libs/xerces-c-3.2.3
mkdir build
cd build
cmake -G "Visual Studio 16 2019" -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=../bin ..
cmake --build . --config Release --target install