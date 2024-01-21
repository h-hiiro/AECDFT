# builder
FROM ubuntu:latest AS builder

RUN apt-get -y update
RUN apt-get -y install wget emacs build-essential gfortran

# lapack
WORKDIR /source/lapack
RUN wget https://github.com/Reference-LAPACK/lapack/archive/refs/tags/v3.12.0.tar.gz && tar xzf v3.12.0.tar.gz
WORKDIR /source/lapack/lapack-3.12.0/
RUN cp make.inc.example make.inc && make

## install adpack 2.2 modified
# WORKDIR /source/adpack22mod
# COPY adpack2.2_mod /source/adpack22mod/source/
# WORKDIR /source/adpack22mod/source/
# RUN make

## install adpack 2.2 original
WORKDIR /source/adpack22
COPY adpack2.2 /source/adpack22/
WORKDIR /source/adpack22/source/
RUN make && make install
WORKDIR /source/adpack22/work