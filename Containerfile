# Lightning Piggy firmware build container.
#
# Builder stage: compiles the firmware with arduino-cli + esp32 core 3.2.0
# Final stage: same tools plus esptool/mklittlefs for flashing and QEMU emulation
#
# Build the compile step only:
#   docker buildx build --target builder -t lightning-piggy-builder .
# Full image (also has flash/emulation tools):
#   docker build -t lightning-piggy .
# Shell with everything mounted:
#   docker run --rm -it -v "$PWD":/work -w /work lightning-piggy bash
ARG ESP32_CORE_VERSION=3.2.0
FROM debian:bookworm-slim AS builder

ARG ESP32_CORE_VERSION
ARG ARDUINO_CLI_VERSION=1.1.1
ARG TARGETARCH

RUN apt-get update && apt-get install -y --no-install-recommends \
        ca-certificates curl git python3 xz-utils \
    && rm -rf /var/lib/apt/lists/*

# Some ESP-IDF build steps expect `python` to exist
RUN ln -s /usr/bin/python3 /usr/local/bin/python

# Install arduino-cli from the official install script
# Node is needed to regenerate the compressed web config portal page
RUN curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh \
        | BINDIR=/usr/local/bin sh -s -- 1.1.1 \
    && apt-get update && apt-get install -y --no-install-recommends nodejs npm \
    && rm -rf /var/lib/apt/lists/*

# Verify and configure: default profile so no per-user setup is needed
RUN arduino-cli version \
    && arduino-cli config init --overwrite \
    && arduino-cli config set board_manager.additional_urls \
        https://espressif.github.io/arduino-esp32/package_esp32_index.json \
    && arduino-cli core update-index \
    && arduino-cli core install esp32:esp32@${ESP32_CORE_VERSION} \
    && arduino-cli core list \
    && arduino-cli lib list

# Compile the firmware and keep the exported binaries in the standard
# Main/build path so the wrapper scripts find them.
WORKDIR /work
COPY Main /work/Main
COPY webconfig/input.css webconfig/index.html webconfig/inline_css.py webconfig/gzip_file_to_PROGMEM_header.py webconfig/tailwind.config.js /work/webconfig/
COPY timezones /work/timezones

# Generate the timezone header and the gzipped portal page, then compile
RUN cd /work/timezones && python3 csv_to_h.py \
    && cd /work/webconfig \
    && npx --yes tailwindcss@3.4.17 -c tailwind.config.js -i input.css -o tailwind_output.css --minify \
    && python3 inline_css.py \
    && python3 gzip_file_to_PROGMEM_header.py index_inlined.html index.html.gzip.h

RUN --mount=type=cache,target=/root/.cache/arduino \
    cd /work && arduino-cli compile \
        --fqbn "esp32:esp32:esp32:PartitionScheme=custom,FlashMode=dio" \
        --libraries Main/libraries/ \
        --export-binaries \
        --output-dir Main/build/esp32.esp32.esp32 \
        --warnings default \
        Main/

# Final stage: builder toolchain + esptool + mklittlefs for flash/merge/emulate
FROM builder AS final

RUN apt-get update && apt-get install -y --no-install-recommends pipx build-essential make cmake \
    && rm -rf /var/lib/apt/lists/* \
    && PIPX_HOME=/opt/pipx PIPX_BIN_DIR=/usr/local/bin pipx install esptool \
    && git clone --depth 1 --recursive --shallow-submodules https://github.com/earlephilhower/mklittlefs.git /tmp/mklittlefs \
    && cd /tmp/mklittlefs && make && cp mklittlefs /usr/local/bin/ \
    && rm -rf /tmp/mklittlefs \
    && cd /tmp && esptool.py version \
    && mklittlefs 2>&1 | head -1

ENV ARDUINO_DIRECTORIES_USER=/root/.arduino15
WORKDIR /work
CMD ["/bin/bash"]
