# download dependencies
brew install gnuplot

mkdir build
cd build
cmake -DBUILD_PYTHON_DLL=OFF ..
make -j4