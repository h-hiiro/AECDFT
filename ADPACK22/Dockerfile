# builder
FROM intel/oneapi-basekit:latest AS builder

RUN apt-get -y update
RUN apt-get -y install wget emacs

## install adpack 2.2
WORKDIR /source/adpack22
COPY adpack2.2 /source/adpack22/
WORKDIR /source/adpack22/adpack22/
# RUN make