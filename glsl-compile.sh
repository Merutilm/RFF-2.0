#!/bin/bash

set +e
cd "$(dirname "$0")" || exit

dirInclude="shaders/include"
dirIn="shaders/src"
dirOut="shaders/out"
dirCache="shaders/cache"
ext=".spv"

echo "-- Shader Compilation"

start=$(date +%s.%N)

mkdir -p "$dirOut"

success=0
skip=0
fail=0

mapfile -t files < <(find "$dirIn" -maxdepth 1 -type f | sort)
mapfile -t fileIncludes < <(find "$dirInclude" -maxdepth 1 -type f | sort)

len=${#files[@]}
lenInclude=${#fileIncludes[@]}

for ((file=0; file<lenInclude; file++)); do
    target="${fileIncludes[$file]}"
    name=$(basename "$target")
    cache="$dirCache/include/$name"

    if ! test -f "$cache" || [ "$(diff "$cache" "$target" | wc -l)" -gt 0 ]; then
        printf "\033[33m# Include %s changed. Invalidating caches...\033[0m\n" "$name"
        for file in "$dirCache"/src/*; do rm "$file"; done
        break;
    fi;
done


for ((file=0; file<len; file++)); do
    target="${files[$file]}"
    name=$(basename "$target")
    cache="$dirCache/src/$name"
    compiled="$dirOut/$name$ext"
    if test -f "$cache" && [ "$(diff "$cache" "$target" | wc -l)" -eq 0 ]; then
        printf "\033[33m# [%d/%d] %s Already Compiled. Skipping...\033[0m\n" "$((file+1))" "$len" "$name"
        ((skip++))
    elif glslc -O "$target" -I "$dirInclude" -o "$compiled"; then
        printf "\033[36m# [%d/%d] Compiling %s...\033[0m\n" "$((file+1))" "$len" "$name"
        ((success++))
    else
        printf "\033[31m-- Failed to compile %s!\033[0m\n" "$name"
        ((fail++))
    fi
done

end=$(date +%s.%N)
elapsed=$(awk "BEGIN {printf \"%.2f\", $end - $start}")

printf -- "-- Shader Compilation Done (%ss), " "$elapsed"
printf "\033[36mSuccess %d\033[0m | " "$success"
printf "\033[33mSkip %d\033[0m | " "$skip"
printf "\033[31mFail %d\033[0m | " "$fail"
printf "Total %d\n" "$len"


mkdir -p "$dirCache/include"
mkdir -p "$dirCache/src"

for file in "$dirInclude"/*; do cp "$file" "$dirCache/include/$(basename "$file")"; done
for file in "$dirIn"/*; do cp "$file" "$dirCache/src/$(basename "$file")"; done
