package main

/*
#include "../cpp/LibAECDFT.hpp"
*/
import "C"
import "fmt"

func CalcDiffMatrix(Grid *Grid_value, V *Potential, conf DFT_conf, l int, kappa int, epsilon float64, diffCoeffs *DifferentialCoefficients) error {
	switch conf.EqType {
	case "Sch":
		// ## Transformed Schroedinger equation
		C.calc_diffs_Sch(
			C.int(Grid.Size),
			(*C.double)(Grid.R),
			(*C.double)(V),
			C.int(l),
			C.double(epsilon),
			(*C.double)(diffCoeffs))
	case "SDirac":
		// ## Transformed scalar Dirac equation
		C.calc_diffs_SDirac(
			C.int(Grid.Size),
			(*C.double)(Grid.R),
			(*C.double)(Grid.X),
			C.double(Grid.DX),
			(*C.double)(V),
			C.int(l),
			C.double(epsilon),
			(*C.double)(diffCoeffs))
	case "Dirac":
		C.calc_diffs_Dirac(
			C.int(Grid.Size),
			(*C.double)(Grid.R),
			(*C.double)(Grid.X),
			C.double(Grid.DX),
			(*C.double)(V),
			C.int(l),
			C.int(kappa),
			C.double(epsilon),
			(*C.double)(diffCoeffs))

	default:
		return fmt.Errorf("invalid xc type")
	}
	return nil
}
