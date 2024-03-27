package main

/*
#include "../cpp/LibAECDFT.hpp"
*/
import "C"
import "fmt"

// # Density functional theory
// ## Core potential
func CalcCore(Grid *Grid_value, Z float64, V *Potential) {
	C.calc_core(
		C.int(Grid.Size),
		(*C.double)(Grid.R),
		C.double(Z),
		(*C.double)(V))
}

// ## Hartree potential
func CalcHartree(Grid *Grid_value, rho *DensityDistribution, V *Potential) {
	C.calc_hartree(
		C.int(Grid.Size),
		(*C.double)(Grid.R),
		C.double(Grid.DX),
		(*C.double)(rho),
		(*C.double)(V))
}

// ## Exchange correlation potential
func CalcXC(Grid *Grid_value, rho *DensityDistribution, conf DFT_conf, V *Potential) error {
	switch conf.XCType {
	case "Xa-Slater":
		// ### Xa by Slater
		C.calc_xc_Xa(
			C.int(Grid.Size),
			C.double(conf.Xa_alpha),
			(*C.double)(rho),
			(*C.double)(V))
	case "LDA-CA":
		// ### LDA by Ceperley and Alder
		C.calc_xc_LDA_CA(
			C.int(Grid.Size),
			(*C.double)(rho),
			(*C.double)(V))
	case "LDA-VWN":
		// ### LDA by Vosko, Wilk, and Nusair
	case "GGA-PBE":
		// ### GGA by Perdew, Burke, and Ernzerhof
	default:
		return fmt.Errorf("invalid xc type")
	}
	return nil
}

func CalcSum(Grid *Grid_value, V_core, V_hartree, V_xc, V_total *Potential) {
	C.calc_sum(
		C.int(Grid.Size),
		(*C.double)(V_core),
		(*C.double)(V_hartree),
		(*C.double)(V_xc),
		(*C.double)(V_total))
}
