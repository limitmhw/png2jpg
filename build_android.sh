#!/bin/bash
export ANDROID_NDK_HOME="/home/l/android-ndk-r27d"
TOOLCHAIN_PATH="$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake"
cmake -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_PATH"   -DANDROID_ABI=arm64-v8a  -DANDROID_PLATFORM=android-21   ..
