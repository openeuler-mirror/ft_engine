# Copyright (c) 2023 Huawei Technologies Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set -e

# =============================================================================
# Python
# =============================================================================
#
# check python3 in current system.
PYTHON_REQUIRED_VERSION="3.9.2"

echo -e "\e[36m[-] Prepare python3 packages...\e[0m"

# Check if python3 is installed
if ! command -v python3 &> /dev/null; then
    echo "python3 is not installed"
    exit 1
fi
if ! command -v pip3 &> /dev/null; then
    echo "pip3 is not installed"
    exit 1
fi

# Check python version
PYTHON_VERSION=$(python3 --version 2>&1 | awk '{print $2}')

# Compare the versions
if [ "$(printf '%s\n' "$PYTHON_REQUIRED_VERSION" "$PYTHON_VERSION" | sort -V | head -n1)" = "$PYTHON_REQUIRED_VERSION" ]; then
    echo "The python3 version is $PYTHON_VERSION"
else
    echo "The python3 version is less than $PYTHON_REQUIRED_VERSION"
fi

# Install python packages
SCRIPT_DIR=$(cd $(dirname $0);pwd)
PROJECT_DIR=$(dirname ${SCRIPT_DIR})

sudo pip3 install -r ${SCRIPT_DIR}/configs/requirements.txt

# Remove out dir
# rm -rf ${PROJECT_DIR}/out

# =============================================================================
# System Packages
# =============================================================================
#
# check system packages in current system by calling builder.py

echo -e "\e[36m[-] Prepare system packages...\e[0m"

# Check & Install required system packages
python3 ${PROJECT_DIR}/build/builder.py check --install-packages

# =============================================================================
# Prebuild
# =============================================================================
#
# download prebuild files
cd $home
PREBUILD_DIR="ft_prebuild"
if [ ! -d ${PREBUILD_DIR} ]; then
mkdir ${PREBUILD_DIR}
fi
cd ${PREBUILD_DIR}
FT_PREBUILD_DIR=$(pwd)

# install prebuild library
if [ ! -d ${FT_PREBUILD_DIR}/libs ]; then
git clone https://gitee.com/yanansong/ft_engine_prebuild.git -b rpms ${FT_PREBUILD_DIR}/libs
fi

ARCHNAME=`uname -m`

cd ${FT_PREBUILD_DIR}/libs/rpms/${ARCHNAME}
sudo ./installRPM

# install prebuild include.
if [ ! -d ${FT_PREBUILD_DIR}/inc ]; then
git clone https://gitee.com/yanansong/devel_inc.git ${FT_PREBUILD_DIR}/inc
fi

# copy include files to /usr/include.
cd ${FT_PREBUILD_DIR}/inc
sudo cp -fr * /usr/local/include

# install mesa_fangtian
if [ ! -d ${FT_PREBUILD_DIR}/rpm/binary ]; then
    git clone https://gitee.com/ShaoboFeng/rpm-fangtian.git ${FT_PREBUILD_DIR}/rpm/binary
fi
cd ${FT_PREBUILD_DIR}/rpm/binary
./install.sh
cd ${PROJECT_DIR}

# copy FT sa file to /usr/local/share/ft/
sudo mkdir -p /usr/local/share/ft
sudo cp -fr ${PROJECT_DIR}/etc/ft.xml /usr/local/share/ft/
sudo cp -fr ${PROJECT_DIR}/etc/icon   /usr/local/share/ft/
sudo cp -fr ${PROJECT_DIR}/etc/desktop /usr/local/share/ft/

# copy config files to /usr/local/share/ft/window_manager
sudo mkdir -p /usr/local/share/ft/window_manager
sudo cp ${PROJECT_DIR}/window_manager/resources/config/other/display_manager_config.xml /usr/local/share/ft/window_manager
sudo cp ${PROJECT_DIR}/window_manager/resources/config/other/window_manager_config.xml /usr/local/share/ft/window_manager

# =============================================================================
# download ft third party files
# =============================================================================

if [ ! -d ${PROJECT_DIR}/third_party/ft_flutter ]; then
git clone https://gitee.com/openeuler/ft_flutter.git ${PROJECT_DIR}/third_party/ft_flutter
fi
cd ${PROJECT_DIR}/third_party/ft_flutter
./project_build/prebuild.sh
./build.sh $*

if [ ! -d ${PROJECT_DIR}/third_party/ft_surface ]; then
git clone https://gitee.com/openeuler/ft_surface ${PROJECT_DIR}/third_party/ft_surface
fi
cd ${PROJECT_DIR}/third_party/ft_surface/ft_surface_wrapper
cmake .
make
sudo cp -fr libft_surface_wrapper.so /usr/lib64
sudo mkdir -p /usr/include/ftsurface
sudo cp -fr include/buffer_handle.h /usr/include/ftsurface
sudo cp -fr include/display_type.h /usr/include/ftsurface
sudo cp -fr include/external_window.h /usr/include/ftsurface
sudo cp -fr include/native_window_wrapper.h /usr/include/ftsurface
sudo cp -fr include/window.h /usr/include/ftsurface

if [ ! -d ${PROJECT_DIR}/third_party/ft_multimedia ]; then
git clone https://gitee.com/openeuler/ft_multimedia.git ${PROJECT_DIR}/third_party/ft_multimedia
fi
cd ${PROJECT_DIR}/third_party/ft_multimedia
./build/prebuild.sh $*
./build.sh $*

if [ ! -d ${PROJECT_DIR}/third_party/ft_mmi ]; then
git clone https://gitee.com/openeuler/ft_mmi.git ${PROJECT_DIR}/third_party/ft_mmi
fi
cd ${PROJECT_DIR}/third_party/ft_mmi
./build/prebuild.sh
./build.sh $*

cd ${PROJECT_DIR}
echo -e "\033[32m[*] Pre-build Done. You need exec 'build.sh'.\033[0m"
