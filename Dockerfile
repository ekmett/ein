# syntax=docker/dockerfile:1.3

# Base Stage: Common dependencies
FROM ubuntu:24.10 AS base
#FROM ubuntu:22.04 AS base
WORKDIR /tmp/install
SHELL ["/bin/bash", "-c"]

RUN apt-get update \
 && apt-get install -y lsb-release software-properties-common wget unzip gnupg make sudo graphviz git \
 && rm -rf /usr/share/doc /usr/share/man

# CMake
FROM base AS cmake
WORKDIR /tmp/install
ENV CMAKE_VERSION=3.30.5
RUN set -x \
 && CMAKE_INSTALLER=cmake-${CMAKE_VERSION}-linux-x86_64.sh \
 && wget -q "https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/${CMAKE_INSTALLER}" \
 && sha256sum -c <(echo "83de8839f3fb0d9caf982a0435da3fa8c4fbe2c817dfec99def310dc7e6a8404  ${CMAKE_INSTALLER}") \
 && sh ${CMAKE_INSTALLER} --skip-license --prefix=/usr

RUN tar cfpPJ /tmp/cmake.tar.xz \
      /usr/bin/cmake /usr/bin/cpack /usr/bin/ctest \
      /usr/share/bash-completion/completions/cmake \
      /usr/share/bash-completion/completions/cpack \
      /usr/share/bash-completion/completions/ctest \
      /usr/share/cmake*

RUN rm -rf /tmp/install

# Misc.
FROM base AS misc
WORKDIR /tmp/install
ENV DOXYGEN_VERSION=1.12.0 \
    CCACHE_VERSION=4.10.2 \
    NINJA_VERSION=1.12.1

ENV DOXYGEN_ARCHIVE=doxygen-${DOXYGEN_VERSION}.linux.bin.tar.gz \
    NINJA_ARCHIVE=ninja-linux.zip \
    CCACHE_ARCHIVE=ccache-${CCACHE_VERSION}-linux-x86_64.tar.xz

RUN wget -q "https://www.doxygen.nl/files/${DOXYGEN_ARCHIVE}" \
 && sha256sum -c <( echo "3c42c3f3fb206732b503862d9c9c11978920a8214f223a3950bbf2520be5f647  ${DOXYGEN_ARCHIVE}")

RUN tar xfpz ${DOXYGEN_ARCHIVE} \
 && make -C doxygen-${DOXYGEN_VERSION} install

RUN wget -q "https://github.com/ninja-build/ninja/releases/download/v${NINJA_VERSION}/${NINJA_ARCHIVE}" \
 && sha256sum -c <(echo "6f98805688d19672bd699fbbfa2c2cf0fc054ac3df1f0e6a47664d963d530255  ${NINJA_ARCHIVE}")

RUN unzip ${NINJA_ARCHIVE} -d /usr/local/bin/ \
 && chmod +x /usr/local/bin/ninja

RUN wget -q "https://github.com/ccache/ccache/releases/download/v${CCACHE_VERSION}/${CCACHE_ARCHIVE}" \
 && sha256sum -c <(echo "80cab87bd510eca796467aee8e663c398239e0df1c4800a0b5dff11dca0b4f18  ${CCACHE_ARCHIVE}")

RUN tar xfJ ${CCACHE_ARCHIVE} -O ccache-${CCACHE_VERSION}-linux-x86_64/ccache > /usr/local/bin/ccache \
 && chmod +x /usr/local/bin/ccache

RUN tar cfpPJ /tmp/misc.tar.xz /usr/local/bin/doxygen /usr/local/bin/ccache /usr/local/bin/ninja
RUN rm -rf /var/lib/apt/lists/* /tmp/install

# LLVM
FROM base AS llvm
WORKDIR /tmp/install
ENV LLVM_VERSION=19
RUN LLVM_INSTALLER=llvm.sh \
 && wget -q "https://apt.llvm.org/${LLVM_INSTALLER}" \
 && sha256sum -c <(echo "3080a6f961db6559698ea7692f0d5efa5ad9fde9ac6cf0758cfab134509b5bd6  ${LLVM_INSTALLER}") \
 && bash ${LLVM_INSTALLER} 19 \
 && apt-get install -y --no-install-recommends clang-tools-${LLVM_VERSION} lld-${LLVM_VERSION} \
 && update-alternatives --install /usr/bin/clang clang /usr/bin/clang-${LLVM_VERSION} 200 \
 && update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-${LLVM_VERSION} 200 \
 && update-alternatives --install /usr/bin/ld ld /usr/bin/ld.lld-${LLVM_VERSION} 200
# && tar cfpPJ /tmp/llvm.tar.xz \
#        /usr/bin/clang* \
#        /usr/lib/llvm-${LLVM_VERSION} \
#        /usr/lib/clang \
#        /usr/share/bash-completion/completions/clang*

# Node Stage
#FROM base AS node
#WORKDIR /tmp/install
ENV NODE_VERSION=20
RUN NODE_INSTALLER=setup_${NODE_VERSION}.x \
 && wget -q "https://deb.nodesource.com/${NODE_INSTALLER}" \
 && sha256sum -c <(echo "dd3bc508520fcdfdc8c4360902eac90cba411a7e59189a80fb61fcbea8f4199c  ${NODE_INSTALLER}") \
 && bash ${NODE_INSTALLER} \
 && apt-get install -y nodejs

#FROM base AS gather
COPY --from=cmake /tmp/cmake.tar.xz /tmp/
#COPY --from=llvm  /tmp/llvm.tar.xz /tmp/
#COPY --from=node  /tmp/node.tar.xz /tmp/
COPY --from=misc  /tmp/misc.tar.xz /tmp/

#FROM base AS final
WORKDIR /tmp
#COPY --from=gather /tmp/*.tar.xz /tmp/
RUN ls *.tar.xz | xargs -I {} tar xfpJ {} -C / && rm -rf *.tar.xz
RUN apt-get clean \
 && rm -rf /var/lib/apt/lists/* \
 && rm -rf /usr/share/doc /usr/share/doc-base \
      /usr/share/man /usr/share/info /usr/share/lintian /usr/share/linda \
      /usr/share/locale /usr/share/locales /usr/lib/locale

LABEL org.opencontainers.image.description build container for https://github.com/ekmett/ein.git (ubuntu 24.10)
