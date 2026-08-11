#!/bin/bash

set -e

path="$(cd "$(dirname "$0")" && pwd)"
cd "$path" || exit


printf "\033[36m=================== Package Installation ===================\033[0m\n"

sudo apt update
sudo apt install -y clang clang-tools build-essential make cmake libopencv-dev libvulkan-dev libglm-dev ninja-build libgtk-3-dev git wget xz-utils libglfw3-dev

if find /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu -name "libgmp.a" 2>/dev/null | grep -q .; then
    printf "\033[33mGMP already installed. Skipping GMP build\033[0m\n"
else
    if [[ ! -d gmp-6.3.0 ]]; then
       wget https://gmplib.org/download/gmp/gmp-6.3.0.tar.xz
        tar -xf gmp-6.3.0.tar.xz
    fi
    cd gmp-6.3.0 || exit
    rm -rf build
    mkdir build
    cd build || exit
    ../configure CFLAGS="-O3 -march=native"
    make -j"$(nproc)"
    sudo make install
    cd ~ || exit
    sudo ldconfig
    printf "\033[32mGMP is successfully installed.\033[0m"
fi


printf "\033[36m=================== RFF Installation ===================\033[0m\n"

url="https://github.com/Merutilm/RFF-2.0";
hash=$(git ls-remote "$url" HEAD | head -n 1 | awk '{print $1}')
if [[ -f version.config ]]; then
    prevHash=$(<version.config)
else
    prevHash=""
fi

if [[ "$prevHash" == "$hash" ]]; then
  read -n 1 -s -r -p "Application is up to date. Press any key to exit...\n"
  exit 0
fi

rm -rf RFF-2.0
git clone https://github.com/Merutilm/RFF-2.0
cd RFF-2.0 || exit
mapfile -t externRepos < <(grep -vE '^\s*(#|$)' "extern_sources")

printf "\033[32mRFF has been loaded. configuring dependencies\033[0m\n"

mkdir -p extern
cd extern || exit

for url in "${externRepos[@]}"; do
    git clone "$url"
done

printf "\033[32mDependencies have been loaded. Application is ready to build\033[0m\n"

cd ..
mkdir -p build

cmake -B build -G "Ninja" -S . \
-DCMAKE_C_COMPILER=clang \
-DCMAKE_CXX_COMPILER=clang++ \
-DCMAKE_BUILD_TYPE=Release

cmake --build build -j"$(nproc)"

rm -rf ../res ../bin ../shaders
mv res ../res
mv bin ../bin
mv shaders ../shaders
cd ..
rm -rf RFF-2.0

echo "$hash" > version.config
printf "\033[32mInstallation Finished\033[0m\n"
printf "\033[32mLocation: %s/bin/RFF\033[0m\n" "$path"
read -n 1 -r -p "Do you want to launch installed application now? [y/n] :" answer

if [[ "$answer" == "y" || "$answer" == "Y" ]]; then
  ./bin/RFF
fi



