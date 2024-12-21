# Change directory to .sh file location
cd $(dirname $0)

# Configure project
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Build project
cmake --build build -j

cp -v build/src/Reload/ImageLibraryV2-Reload build/src/Server
cp -v build/src/Compressed/ImageLibraryV2-Compressed build/src/Server
