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
    wget \
    && apt-get clean

# 安裝 xvfb 和 OpenGL 相關依賴
RUN apt-get update &&apt-get install -y xvfb libgl1-mesa-glx libgl1-mesa-dri

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

# 安裝適用於 ARM 架構的 Miniconda
RUN wget https://github.com/conda-forge/miniforge/releases/latest/download/Miniforge3-Linux-aarch64.sh -O ~/miniconda.sh && \
    bash ~/miniconda.sh -b -p /opt/conda && \
    rm ~/miniconda.sh

# 更新 PATH 環境變數以使用 conda
ENV PATH="/opt/conda/bin:$PATH"

# 創建 Conda 環境 'rl'
RUN conda create -n rl python=3.8 -y

# 將激活環境命令添加到 .bashrc
RUN echo "source /opt/conda/etc/profile.d/conda.sh && conda activate rl" >> ~/.bashrc

# 克隆 SnakeAgentCpp 專案並切換到該目錄
RUN git clone https://github.com/ChiaCheHo/SnakeAgentCpp.git /home/SnakeAgentCpp
WORKDIR /home/SnakeAgentCpp

# 安裝項目依賴
RUN conda run -n rl pip install --upgrade pip setuptools wheel && \
    conda run -n rl pip install -r requirements.txt && \
    conda run -n rl pip install Cython

# 清理安裝過程中下載的文件，減少映像大小
RUN rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# 切換到最終工作目錄
WORKDIR /home/RL/SnakeAgentCpp

# 設定容器啟動後進入 'rl' 環境
CMD ["bash", "-i"]
