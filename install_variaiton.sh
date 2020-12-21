#!/usr/bin/env bash
UNAME=$( command -v uname)

case $( "${UNAME}" | tr '[:upper:]' '[:lower:]') in
  linux*)
    sh buildScript.sh
    cp ./build/libroad-generation.so ./variation/variation/resources/libroad-generation.so
    cd variation
    pip3 install wheel
    python3 setup.py bdist_wheel
    cd dist 
    pip3 install roadvariation-0.1-py3-none-any.whl
    ;;
  msys*|cygwin*|mingw*|nt|win*)
    printf 'windows\n'
    ./build_xercesc_win64.sh
    ./buildScript.sh
    cp ./Release/road-generation_py.dll ./variation/variation/resources/road-generation_py.dll
    cd variation
    python -m pip install wheel
    python -m setup.py bdist_wheel
    cd dist 
    pip3 install roadvariation-0.1-py3-none-any.whl
    ;;
  *)
    printf 'unknown\n'
    ;;

esac

sleep 50