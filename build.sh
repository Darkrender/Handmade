echo Building Handmade Hero

OSX_LD_FLAGS="-framework AppKit"

mkdir ./build
pushd ./build
clang -g $OSX_LD_FLAGS -o handmade ../code/osx_main.m
popd