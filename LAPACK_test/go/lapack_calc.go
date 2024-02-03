package main

/*
#cgo LDFLAGS: -L. -llapacktest -L/source/lapack/lapack-3.12.0 -llapack -lrefblas -ldl -lm -lgfortran -lquadmath -static
#include "lapack_header.hpp"
*/
import "C"
import (
	"fmt"
	"time"
)

func main() {
	// fmt.Println("Hello, world!")

	var N C.int = 100
	var T int = 1000
	var i int

	start := time.Now()
	var A **C.double = C.alloc_matrix(N)
	var B **C.double = C.alloc_matrix(N)
	var C **C.double = C.alloc_matrix(N)

	defer C.free_matrix(A)
	defer C.free_matrix(B)
	defer C.free_matrix(C)

	for i = 0; i < T; i++ {
		C.dgemm_calc(A, B, C, N)
	}
	end := time.Now()
	elapsed := end.Sub(start)
	fmt.Printf("%v\n", elapsed)

}
