#! /bin/bash

cmake .
make

export CYG_PROFILE_ENABLE=1
export CYG_PROFILE_FUNC_FILTER=example
./example
