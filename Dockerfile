# 使用 Ubuntu 作為基底映像
FROM ubuntu:20.04

# 設定 noninteractive 模式以避免提示互動
ENV DEBIAN_FRONTEND=noninteractive
ENV XDG_RUNTIME_DIR=/tmp

# 更新 apt 並安裝必要的工具與依賴項
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libsdl2-dev \
    libsdl2-mixer-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    x11-apps \
    tzdata \
    git \
    autoconf \
    libtool \
    pkg-config \
    python3 \
    python3-pip \
    python3-dev \
    && apt-get clean

# 安裝 xvfb 和 OpenGL 相關依賴
RUN apt-get update && apt-get install -y xvfb libgl1-mesa-glx libgl1-mesa-dri

# 設定時區，避免卡在 geographic area
RUN ln -fs /usr/share/zoneinfo/Asia/Taipei /etc/localtime && \
    dpkg-reconfigure --frontend noninteractive tzdata

# 安裝 SDL2 TTF 支援
RUN apt-get update && apt-get install -y libsdl2-ttf-dev

# 安裝 Protocol Buffers
RUN git clone --recurse-submodules -b v23.3 https://github.com/protocolbuffers/protobuf.git /opt/protobuf && \
    cd /opt/protobuf && \
    mkdir build && cd build && \
    cmake .. && \
    make -j$(nproc) && \
    make install && \
    ldconfig

# 安裝 gRPC
RUN git clone --recurse-submodules -b v1.59.0 https://github.com/grpc/grpc.git /opt/grpc && \
    cd /opt/grpc && \
    mkdir -p cmake/build && cd cmake/build && \
    cmake ../.. && \
    make -j$(nproc) && \
    make install && \
    ldconfig

# 安裝 Python gRPC 工具
RUN pip3 install grpcio grpcio-tools

# 清理安裝過程中下載的文件，減少映像大小
RUN rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*
