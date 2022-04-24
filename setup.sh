#!/bin/bash

export PROJECT_ROOT=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
export CC="clang"
export CXX="clang++"

generate_cmake() {
  cd ${PROJECT_ROOT}
  rm build -rf ; mkdir build ; cd build
  # temporary workaround for clang
  cmake .. -G"Unix Makefiles" -DCMAKE_C_FLAGS_INIT="--target=x86_64-pc-windows-gnu" -DCMAKE_CXX_FLAGS_INIT="--target=x86_64-pc-windows-gnu"
}

build_app() {
  pushd . &>/dev/null
  generate_cmake
  make
  popd &>/dev/null
}

run_tests() {
  pushd ${PROJECT_ROOT}/build/src/zipcxx/ut/ &>/dev/null
  readarray -d '' tests < <(/usr/bin/find . -name "Test*.exe" -print0)
  failedTests=()
  for test in "${tests[@]}"; do
    testName=$(basename $test .exe)
    testBinary=$(basename $test)
    testPath=$(dirname $test)
    echo "Running $testBinary"
    pushd "$testPath" &>/dev/null
    ./$testBinary
    if [ $? -ne 0 ]; then 
      failedTests+=("$testName")
    fi
    popd &>/dev/null
  done
  if [ "${#failedTests[@]}" -ne 0 ]; then
    echo "[-] The following test failed:"
    for testName in "${failedTests[@]}"; do
      echo "  $testName"
    done
  else
    echo "[+] All tests passed successfully"
  fi
  popd &>/dev/null
}

view() {
  ${PROJECT_ROOT}/build/src/tools/zipviewer/zipviewer* $@
}

registeredFunctions=("generate_cmake" "build_app" "run_tests" "view")
echo "Command available:"
for foo in "${registeredFunctions[@]}"; do
  echo "    $foo"
done
