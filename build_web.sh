../../emsdk/emsdk activate latest
source ../../emsdk/emsdk_env.sh
# rm -rf build_web
# rm -rf SDK
# mkdir build_web
cd build_web
emcmake cmake .. -DWEB=1 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../SDK
emmake make
emmake make install