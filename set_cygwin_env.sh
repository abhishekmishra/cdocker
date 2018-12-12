#!/bin/bash

export IUP_HOME=/cygdrive/e/IUP/iup-3.25_Win64_dllw6_lib
export CD_HOME=/cygdrive/e/IUP/cd-5.11.1_Win64_dllw6_lib
export IM_HOME=/cygdrive/e/IUP/im-3.12_Win64_dllw6_lib
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${IUP_HOME}:${CD_HOME}:${IM_HOME}
export PATH=${PATH}:${IUP_HOME}:${CD_HOME}:${IM_HOME}
