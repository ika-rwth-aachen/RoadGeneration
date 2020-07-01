# update or create build directory
echo "bulding"
#if [[ "$OSTYPE" == "linux-gnu" ]]; then
    echo "Linux"

    
    
    echo "installed libs" 
    mkdir -p build
    cd build
    rm -rf *

    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../bin ..

    cmake --build .
    echo "testttt"
   
#else
#    echo "Windows?"
#    rm -r build 
#    mkdir build
#    cd build
#    # configure cmake
#    cmake -G "Visual Studio 16 2019" -DCMAKE_INSTALL_PREFIX=../bin -DXercesC_INCLUDE_DIR="../libs/xerces-c-3.2.3/bin/include" -DXercesC_LIBRARY="../libs/xerces-c-3.2.3/bin/lib/xerces-c_3" ..
#    # make
#    cmake --build . --config Release
#fi 
