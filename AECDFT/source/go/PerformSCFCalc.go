package main

/*
#include "../cpp/LibAECDFT.hpp"
*/
import "C"
import "fmt"

func (calcData *CalcData) PerformSCFCalc() error {
	// Prepare the variables
	Grid := calcData.Input.Grid.GenGrid()
	V_total := (*Potential)(C.alloc_dvector(C.int(Grid.Size)))
	V_core := (*Potential)(C.alloc_dvector(C.int(Grid.Size)))
	V_hartree := (*Potential)(C.alloc_dvector(C.int(Grid.Size)))
	V_xc := (*Potential)(C.alloc_dvector(C.int(Grid.Size)))
	rho := (*DensityDistribution)(C.alloc_dvector(C.int(Grid.Size)))
	diffCoeffs := (*DifferentialCoefficients)(C.alloc_dvector(C.int(Grid.Size * 4)))
	if err := calcData.PrepareEigenstateArray(); err != nil {
		return err
	}
	// Wavefunctions := calcData.PrepareOrbitals()
	// Free allocated vectors after the calculation
	defer Grid.Free()
	defer C.free_dvector((*C.double)(V_total))
	defer C.free_dvector((*C.double)(V_core))
	defer C.free_dvector((*C.double)(V_hartree))
	defer C.free_dvector((*C.double)(V_xc))
	defer C.free_dvector((*C.double)(rho))
	defer C.free_dvector((*C.double)(diffCoeffs))

	/// the name NormRD is the convention in ADPACK and OpenMX
	NormRD_history := C.alloc_dvector(C.int(calcData.Input.SCF.MaxIteration))
	var NormRD DeltaRhoNorm
	var IterationNumber int = 0
	var SCF_OK bool = false

	// Obtain the initial density distriubtion (Thomas-Fermi type)
	// rho is used while V is not used
	if err := CalcTFPotential(Grid, calcData.Input.Atom.Z, V_total, rho, calcData.Input.TF); err != nil {
		return err
	}
	// C.print_dvector2(C.int(Grid.Size), (*C.double)(Grid.R), (*C.double)(V_total), C.CString("%.6f"))

	// SCF loop
	for !SCF_OK && IterationNumber < calcData.Input.SCF.MaxIteration {
		IterationNumber++

		// Prepare potential
		CalcCore(Grid, calcData.Input.Atom.Z, V_core)
		CalcHartree(Grid, rho, V_hartree)
		if err := CalcXC(Grid, rho, calcData.Input.DFT, V_xc); err != nil {
			return err
		}
		CalcSum(Grid, V_core, V_hartree, V_xc, V_total)
		// C.print_dvector2(C.int(Grid.Size), (*C.double)(Grid.R), (*C.double)(V_total), C.CString("%.6f"))

		// for debug
		var l int = 1
		var kappa int = 1
		var epsilon float64 = -1
		if err := CalcDiffMatrix(Grid, V_total, calcData.Input.DFT, l, kappa, epsilon, diffCoeffs); err != nil {
			return err
		}
		// C.print_dvector(C.int(Grid.Size*4), (*C.double)(diffCoeffs), C.CString("%.6f"))

		// SCF check
		if NormRD < calcData.Input.SCF.Threshold {
			SCF_OK = true
		}
		C.set_value(NormRD_history, C.int(IterationNumber-1), C.double(NormRD))
		fmt.Printf("SCF Loop #%4d: NormRD (norm of density difference) is %15.12f\n", IterationNumber, NormRD)
	}

	calcData.Output.SCF.Achieved = SCF_OK
	if SCF_OK {
		fmt.Printf("SCF is achieved with loop #%d\n", IterationNumber)
	} else {
		fmt.Printf("SCF is NOT acheived with loop #%d\n", IterationNumber)
	}

	return nil
}
