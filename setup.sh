#!/bin/bash

PROJECT_ROOT=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

build_app() {
  pushd ${PROJECT_ROOT} >/dev/null
  rm build -rf ; mkdir build ; cd build
  cmake .. -G"Unix Makefiles"
  make
  popd >/dev/null
}

run_tests() {
  pushd ${PROJECT_ROOT}/build/src/zipcxx/ut/ >/dev/null
  readarray -d '' tests < <(/usr/bin/find . -name "Test*" -print0)
  for test in "$tests"; do
    echo "Running $test"
    pushd $test >/dev/null
    "$test.exe"
    popd >/dev/null
  done
  popd >/dev/null
}

registeredFunctions=("build_app" "run_tests")
echo "Command available:"
for foo in "${registeredFunctions[@]}"; do
  echo "    $foo"
done
