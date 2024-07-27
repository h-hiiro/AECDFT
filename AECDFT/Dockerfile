# go & cpp builder
FROM golang:latest AS builder
RUN apt-get -y update && apt-get -y install gfortran emacs
WORKDIR /source/lapack
RUN wget https://github.com/Reference-LAPACK/lapack/archive/refs/tags/v3.12.0.tar.gz && tar xzf v3.12.0.tar.gz
WORKDIR /source/lapack/lapack-3.12.0/
RUN cp make.inc.example make.inc && make

COPY ./source/go/*.go /source/go/
COPY ./source/cpp /source/cpp
WORKDIR /source/cpp
RUN make
WORKDIR /source/go
RUN go mod init AECDFT && go get -u github.com/tidwall/jsonc && go build -tags timetzdata 

FROM busybox:latest
WORKDIR /work
COPY --from=builder /source/go/AECDFT AECDFT.o
COPY default/input_default.jsonc ./
ENV AECDFT_TZ=Asia/Tokyo