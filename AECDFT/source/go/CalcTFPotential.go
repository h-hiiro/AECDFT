package main

/*
#include "../cpp/LibAECDFT.hpp"
*/
import "C"
import (
	"fmt"
	"math"
)

// # Thomas-Fermi potential
func CalcTFPotential(Grid *Grid_value, Z float64, V *Potential, rho *DensityDistribution, TF TF_conf) error {
	mu := math.Pow(3.0*math.Pi/4.0, 2.0/3.0) / (2.0 * math.Pow(Z, 1.0/3.0))
	// fmt.Printf("[CalcTFPotential] mu = %f\n", mu)
	X := (*Axis)(C.scale_grid(
		C.int(Grid.Size),
		(*C.double)(Grid.R),
		C.double(1.0/mu)))

	// # Calculation of the TF potential
	VTF := (*Potential)(C.alloc_dvector(C.int(Grid.Size)))
	TF_result := bool(C.calc_TFPotential(
		C.int(Grid.Size),
		(*C.double)(X),
		(*C.double)(VTF),
		C.double(TF.InitialDiff_left),
		C.double(TF.InitialDiff_right),
		C.double(TF.Threshold)))
	if !TF_result {
		return fmt.Errorf("tf calculation failed")
	}

	C.calc_TFDensity(
		C.int(Grid.Size),
		(*C.double)(Grid.R),
		(*C.double)(VTF),
		C.double(Z),
		(*C.double)(V),
		(*C.double)(rho))

	// C.print_dvector2(C.int(Grid.Size), (*C.double)(Grid.R), (*C.double)(V), C.CString("%.6f"))
	// C.print_dvector2(C.int(Grid.Size), (*C.double)(Grid.R), (*C.double)(rho), C.CString("%.6f"))

	C.free_dvector((*C.double)(X))
	return nil
}
