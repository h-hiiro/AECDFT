# go & cpp builder
FROM golang:latest AS builder
RUN apt-get -y update
RUN apt-get -y install gfortran emacs
WORKDIR /source/lapack
RUN wget https://github.com/Reference-LAPACK/lapack/archive/refs/tags/v3.12.0.tar.gz && tar xzf v3.12.0.tar.gz
WORKDIR /source/lapack/lapack-3.12.0/
RUN cp make.inc.example make.inc && make

WORKDIR /source/cpp
COPY cpp/lapack_test.cpp ./
RUN g++ lapack_test.cpp -L/source/lapack/lapack-3.12.0 -llapack -lrefblas -ldl -lm -lgfortran -lquadmath -static -mcmodel=medium -o cpp_lapack_test.o

WORKDIR /source/go
RUN go mod init go_lapack_test
COPY go/*.* ./
RUN g++ lapack_lib.cpp -c && ar rv liblapacktest.a lapack_lib.o
RUN go build

FROM busybox:latest
WORKDIR /work
COPY --from=builder /source/cpp/cpp_lapack_test.o ./
COPY --from=builder /source/go/go_lapack_test go_lapack_test.o