#!/bin/bash

set +e
cd "$(dirname "$0")" || exit

dirIn="shdsrc"
dirOut="shaders"
ext=".spv"

echo "-- Shader Compilation"

start=$(date +%s.%N)

mkdir -p "$dirOut"

# Remove previous .spv files
find "$dirOut" -type f -delete

success=0
fail=0

mapfile -t files < <(find "$dirIn" -maxdepth 1 -type f | sort)
len=${#files[@]}

for ((i=0; i<len; i++)); do
    target="${files[$i]}"
    name=$(basename "$target")
    compiled="$dirOut/$name$ext"

    printf "\033[36m# [%d/%d] Compiling %s...\033[0m\n" "$((i+1))" "$len" "$name"

    if glslc "$target" -o "$compiled"; then
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
printf "\033[31mFail %d\033[0m | " "$fail"
printf "Total %d\n" "$len"