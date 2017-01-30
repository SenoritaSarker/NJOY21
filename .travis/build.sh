#!/bin/bash
set -x

if [ "$TRAVIS_OS_NAME" = "linux" ]; then
  if [ "$CXX" = "clang++" ]; then
      sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-3.8 90 \
	   --slave /usr/bin/clang++ clang++ /usr/bin/clang++-3.8 \
	   --slave /usr/bin/gfortran gfortran /usr/bin/gfortran-6
    sudo update-alternatives --config clang
    export PATH=/usr/bin:$PATH
    export appended_flags=$appended_flags" -isystem /usr/include/c++/v1/"
    export CUSTOM=("-D no_link_time_optimization=TRUE")
  else
#   sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-6 90 \
#        --slave /usr/bin/g++ g++ /usr/bin/g++-6 \
#        --slave /usr/bin/gfortran gfortran /usr/bin/gfortran-6 \
#        --slave /usr/bin/gcov gcov /usr/bin/gcov-6 \
#        --slave /usr/bin/gcc-ar ar /usr/bin/gcc-ar-6 \
#        --slave /usr/bin/gcc-nm nm /usr/bin/gcc-nm-6 \
#        --slave /usr/bin/gcc-ranlib ranlib /usr/bin/gcc-ranlib-6
#   sudo update-alternatives --config gcc
    export appended_flags=$appended_flags" -Wno-error=subobject-linkage -Wno-subobject-linkage"
#   export CUSTOM=('-D CMAKE_AR=/usr/bin/gcc-ar' '-D CMAKE_NM=/usr/bin/gcc-nm' '-D CMAKE_RANLIB=/usr/bin/gcc-ranlib')
    ln -s /usr/bin/gcc-6 /usr/local/bin/gcc
    ln -s /usr/bin/g++-6 /usr/local/bin/g++
    ln -s /usr/bin/gfortran /usr/local/bin/gfortran
    export CC=/usr/bin/gcc-6
    export CXX=/usr/bin/g++-6
    export FC=/usr/bin/gfortran
    gcc -v && g++ -v && gofrtran -v
  fi;
fi

mkdir build
cd build
cmake ${CUSTOM[@]}\
      -D build_type=$build_type \
      -D static_libraries=$static_libraries \
      -D appended_flags="$appended_flags" ..
make -j2
export COMPILATION_FAILURE=$?
if [ $COMPILATION_FAILURE -ne 0 ];
then
  exit 1
fi

ctest --output-on-failure -j2
export TEST_FAILURE=$?
if [ $TEST_FAILURE -ne 0 ];
then
    exit 1
fi
if [ "$build_type" = "coverage" ]
then
  pip install --user cpp-coveralls
  echo "loading coverage information"
  coveralls  --exclude-pattern "/usr/include/.*|.*/CMakeFiles/.*|.*subprojects.*|.*dependencies.*|.*test\.cpp" --root ".." --build-root "." --gcov-options '\-lp'
fi
