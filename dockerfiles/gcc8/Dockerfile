FROM ubuntu:18.04

RUN apt-get update ; apt-get upgrade -y ; apt-get install -y gcc-8 g++-8 && \
  update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 60 --slave /usr/bin/g++ g++ /usr/bin/g++-8 && \
  update-alternatives --config gcc

RUN apt-get install  -y make && apt-get install -y libboost-all-dev