rm -rf build
mkdir build
cd build
cmake ..
make
cd ..
cp build/*.vpk ./
