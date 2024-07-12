@echo off
@rem Please use ReactOS Build Environment (RosBE).
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug .
ninja
