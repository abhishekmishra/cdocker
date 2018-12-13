#!/bin/bash

export IUP_HOME=e:/IUP/iup-3.25_Win64_dllw6_lib
export CD_HOME=e:/IUP/cd-5.11.1_Win64_dllw6_lib
export IM_HOME=e:/IUP/im-3.12_Win64_dllw6_lib
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${IUP_HOME}:${CD_HOME}:${IM_HOME}
export PATH=${PATH}:${IUP_HOME}:${CD_HOME}:${IM_HOME}
export PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:/usr/lib/pkgconfig:/ming64/lib/pkgconfig:/ming64/share/lib/pkgconfig
