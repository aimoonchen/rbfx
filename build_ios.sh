rm -rf build
mkdir build
cd build
cmake .. -G Xcode -DVULKAN_SDK=/Users/simon/Downloads/MoltenVK -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchains/ios.toolchain.cmake -DPLATFORM=OS64 -DENABLE_BITCODE=FALSE -DENABLE_ARC=FALSE -DBUILD_SHARED_LIBS=OFF -DURHO3D_SAMPLES=OFF -DURHO3D_PLAYER=ON -DIOS=1