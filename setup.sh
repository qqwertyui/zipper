#!/bin/bash

PROJECT_ROOT=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

eval_at() {
  if [ $# -ne 2 ]; then
    return
  fi
  pushd $1 &> /dev/null
  eval $2
  popd &> /dev/null
} 

CMD_BUILD="rm build -rf; mkdir build; cd build; cmake .. -G\"Unix Makefiles\"; make"
CMD_BUILD_N_TEST="$CMD_BUILD && cd tests/ && ./zipper_tests.exe"

alias build_app="eval_at '$PROJECT_ROOT' '$CMD_BUILD'"
alias build_app_and_test="eval_at '$PROJECT_ROOT' '$CMD_BUILD_N_TEST'"