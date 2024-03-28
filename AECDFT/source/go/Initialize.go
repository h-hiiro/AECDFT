package main

/*
#include "../cpp/LibAECDFT.hpp"
*/
import "C"
import "fmt"

func (grid_conf Grid_conf) GenGrid() *Grid_value {
	grid_value := new(Grid_value)
	grid_value.Size = grid_conf.Num
	grid_value.X = (*Axis)(C.alloc_dvector(C.int(grid_conf.Num)))
	grid_value.R = (*Axis)(C.alloc_dvector(C.int(grid_conf.Num)))
	grid_value.DX = (grid_conf.XMax - grid_conf.XMin) / float64(grid_conf.Num-1)
	C.fill_grid(C.int(grid_value.Size),
		(*C.double)(grid_value.X),
		(*C.double)(grid_value.R),
		C.double(grid_conf.XMin),
		C.double(grid_value.DX))
	return grid_value
}

func (grid *Grid_value) Free() {
	C.free_dvector((*C.double)(grid.X))
	C.free_dvector((*C.double)(grid.R))
}

func (calcData *CalcData) PrepareOrbitals() []*Wavefunction {
	calcData.Input.Atom.NumElectrons = 0.0
	calcData.Input.Atom.NumOrbitals = 0
	for nm1, occ_n := range calcData.Input.Atom.Occupancies {
		for l, occ_nl := range occ_n {
			calcData.Input.Atom.NumElectrons += occ_nl
			calcData.Input.Atom.NumOrbitals++
			calcData.Output.Eigenstates = append(
				calcData.Output.Eigenstates,
				Eigenstate{N: nm1 + 1, L: l, Occupancy: occ_nl, E: 0})
		}
	}
	fmt.Printf("Occupancies:\n")
	for _, es := range calcData.Output.Eigenstates {
		fmt.Printf("%s\n", es.String_nlo())
	}
	fmt.Printf("Total number of electrons: %5.1f\n", calcData.Input.Atom.NumElectrons)
	fmt.Printf("Total number of orbitals: %d\n", calcData.Input.Atom.NumOrbitals)

	var wfns []*Wavefunction
	for range calcData.Output.Eigenstates {
		wfns = append(wfns, (*Wavefunction)(C.alloc_dvector(C.int(calcData.Input.Grid.Num))))
	}
	return wfns
}
