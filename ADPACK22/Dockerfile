# builder
FROM intel/oneapi-basekit:latest AS builder

RUN apt-get -y update
RUN apt-get -y install wget emacs

## install adpack 2.2
WORKDIR /source/adpack22
RUN wget https://www.openmx-square.org/adpack2.2.tar.gz && tar xzf adpack2.2.tar.gz
COPY Makefile_adpack /source/adpack22/adpack2.2/source/makefile
COPY FEMHF_All_Electron.c /source/adpack22/adpack2.2/source/
WORKDIR /source/adpack22/adpack2.2/source/
# RUN make