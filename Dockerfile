FROM ubuntu

RUN apt-get update && apt-get install -y \
    git \
    gcc \
    g++ \
    cmake \
    libssl-dev \
    llvm

RUN ldconfig

COPY . /app
COPY ./src/data /app/data

WORKDIR /app/build

RUN cmake ..
RUN make

WORKDIR /app

ENTRYPOINT ["./build/stalcraft-auction-parser"]