../emsdk/emsdk activate latest
source ../emsdk/emsdk_env.sh
rm -rf build_web_debug
rm -rf SDK_debug
mkdir build_web_debug
cd build_web_debug
emcmake cmake .. -DWEB=1 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../SDK_debug
emmake make
emmake make install