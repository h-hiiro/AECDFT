# builder
FROM intel/oneapi-basekit:latest AS builder

RUN apt-get -y update
RUN apt-get -y install wget build-essential

## lapack


## install adpack 2.2
WORKDIR /source/adpack22
RUN wget https://www.openmx-square.org/adpack2.2.tar.gz && tar xzf adpack2.2.tar.gz
COPY Makefile_adpack /source/adpack22/adpack2.2/source/makefile