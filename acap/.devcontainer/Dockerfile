ARG ARCH
ARG VERSION=1.1
ARG UBUNTU_VERSION=20.04
ARG REPO=axisecp
ARG SDK=acap-native-sdk

FROM ${REPO}/${SDK}:${VERSION}-${ARCH}-ubuntu${UBUNTU_VERSION}

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    autoconf \
    libtool \
    cppcheck \
    libcjson-dev \
    libcurl4-openssl-dev \
    libssl-dev \
    pkg-config \
    automake && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*