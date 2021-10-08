FROM ubuntu:20.04

RUN apt-get update && apt-get upgrade -y && DEBIAN_FRONTEND=noninteractive apt-get install -y build-essential g++-10 libomp-dev cmake git libgoogle-perftools-dev  
RUN apt-get install wget g++ m4 zlib1g-dev make p7zip libboost-all-dev -y && wget https://gmplib.org/download/gmp/gmp-6.1.2.tar.xz && tar -xvf gmp-6.1.2.tar.xz && cd gmp-6.1.2 && ./configure --enable-cxx && make && make install
RUN apt-get install -y python3 python3-pip && pip3 install numpy keras tensorflow scikit-learn more_itertools
ADD . /app
RUN sh /app/docker/build.sh
