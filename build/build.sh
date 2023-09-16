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

set +e

# Get project dir
SCRIPT_DIR=$(cd $(dirname $0); pwd)
SCRIPT_BASE_DIR=$(basename "$SCRIPT_DIR")

if [[ $SCRIPT_BASE_DIR == "build" ]]; then
    echo -e "\033[31m[!] Please exec \"build.sh\" at project root dir.\033[0m"
    exit 1
else
    PROJECT_DIR=$SCRIPT_DIR
    while [[ ! -f "${SCRIPT_DIR}/.gn" ]]; do
        SCRIPT_DIR="$(dirname "${SCRIPT_DIR}")"
        if [[ "${SCRIPT_DIR}" == "/" ]]; then
            echo -e "\033[31m[!] Cannot find project root dir from $(pwd)\033[0m"
            exit 1
        fi
    done
fi

if [[ "${PROJECT_DIR}x" == "x" ]]; then
  echo "\033[31m[!] project root dir cannot be empty.\033[0m"
  exit 1
fi

# builder third_party
if [ -d ${PROJECT_DIR}/third_party/ft_flutter ]; then
    cd ${PROJECT_DIR}/third_party/ft_flutter
    ./build.sh $*
else
    echo -e "\033[31m[!] The third_party/ft_flutter directory does not exist. Ensure that relevant dynamic libraries are available. Otherwise, run \"./build/prebuild.sh\" in the project root directory first.\033[0m"
fi

if [ -d ${PROJECT_DIR}/third_party/ft_multimedia ]; then
    cd ${PROJECT_DIR}/third_party/ft_multimedia
    ./build.sh $*
else
    echo -e "\033[31m[!] The third_party/ft_multimedia directory does not exist. Ensure that relevant dynamic libraries are available. Otherwise, run \"./build/prebuild.sh\" in the project root directory first.\033[0m"
fi

if [ -d ${PROJECT_DIR}/third_party/ft_mmi ]; then
    cd ${PROJECT_DIR}/third_party/ft_mmi
    ./build.sh $*
else
    echo -e "\033[31m[!] The third_party/ft_mmi directory does not exist. Ensure that relevant dynamic libraries are available. Otherwise, run \"./build/prebuild.sh\" in the project root directory first.\033[0m"
fi


cd ${PROJECT_DIR}

# Exec builder
python3 ${PROJECT_DIR}/build/builder.py --project-dir ${PROJECT_DIR} build $*

# Check builder result
if [[ "$?" -ne 0 ]]; then
    echo -e "\033[31m[!] Build failed.\033[0m"
    exit 1
fi
echo -e "\033[0;32m[-] Build done.\033[0m"
