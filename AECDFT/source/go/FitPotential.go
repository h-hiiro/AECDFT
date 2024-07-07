package main

/*
#include "../cpp/LibAECDFT.hpp"
*/
import "C"
import "fmt"

func FitPotential(Grid *Grid_value, V *Potential, order int, V_fit_coeffs *C.double, V_fit *Potential) error {
	Fit_result := bool(C.fit_potential(C.int(Grid.Size), (*C.double)(Grid.R), (*C.double)(V), C.int(order), V_fit_coeffs, (*C.double)(V_fit)))
	if !Fit_result {
		return fmt.Errorf("error in fitting")
	}

	C.print_dvector3(C.int(Grid.Size), (*C.double)(Grid.R), (*C.double)(V), (*C.double)(V_fit), C.CString("%.6f"))

	return nil
}
