#!/bin/sh
# Build the Lightning Piggy firmware inside Docker (no local Arduino toolchain).
#
#   ./docker-build.sh              # generate headers + compile firmware -> Main/build/
#   ./docker-build.sh shell        # interactive container with toolchain
#   ./docker-build.sh image        # (re)build the images first
#
set -e
IMAGE=lightning-piggy
TARGET=${1:-build}

if [ "$TARGET" = image ]; then
    docker build -f Containerfile --target builder -t "$IMAGE-builder" .
    docker build -f Containerfile -t "$IMAGE" .
    exit 0
fi

mkdir -p Main/build

# Generate the headers Config.ino and Time.ino include, mirroring the
# Containerfile pipeline so workspace and image builds stay consistent.
docker run --rm -v "$PWD":/work -w /work "$IMAGE""-builder" sh -c '
    cd timezones && python3 csv_to_h.py &&
    cd ../webconfig &&
    npx --yes tailwindcss@3.4.17 -c tailwind.config.js -i input.css -o tailwind_output.css --minify &&
    python3 inline_css.py &&
    python3 gzip_file_to_PROGMEM_header.py index_inlined.html index.html.gzip.h'

if [ "$TARGET" = shell ]; then
    exec docker run --rm -it -v "$PWD":/work -w /work "$IMAGE"
fi

exec docker run --rm \
    -v "$PWD":/work -w /work \
    "$IMAGE" \
    arduino-cli compile \
      -b "esp32:esp32:esp32:PartitionScheme=custom,FlashMode=dio" \
      --libraries Main/libraries/ \
      --export-binaries \
      --output-dir Main/build/esp32.esp32.esp32 \
      --warnings default \
      Main/
