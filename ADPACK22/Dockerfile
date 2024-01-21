# builder
FROM intel/oneapi-basekit:latest AS builder

RUN apt-get -y update
RUN apt-get -y install wget emacs

## install adpack 2.2
WORKDIR /source/adpack22
COPY adpack2.2_mod /source/adpack22/source/
WORKDIR /source/adpack22/source/
# RUN make