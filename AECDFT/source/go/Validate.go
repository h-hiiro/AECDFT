package main

import (
	"fmt"
	"slices"
)

func (input *InputData) Validate() error {
	// Title: 1 <= length <= 128
	if !(1 <= len(input.Title) && len(input.Title) <= 128) {
		return fmt.Errorf("title length should be in [1, 128]")
	}

	// Grid: XMin<XMax
	if !(input.Grid.XMin < input.Grid.XMax) {
		return fmt.Errorf("invalid grid range")
	}

	// Atom: 1 <= Z <= 118
	if !(1 <= input.Atom.Z && input.Atom.Z <= 118) {
		return fmt.Errorf("invalid atomic number")
	}
	// Atom: LMax >= 0
	if !(0 <= input.Atom.LMax) {
		return fmt.Errorf("invalid maximum l")
	}
	// Atom: NMax >= 1
	if !(1 <= input.Atom.NMax) {
		return fmt.Errorf("invalid maximum n")
	}
	// Occupied: size check
	if !(len(input.Atom.Occupancies) == input.Atom.NMax) {
		return fmt.Errorf("invalid first dimension size of occupancies array")
	}
	for nm1, occ := range input.Atom.Occupancies {
		n := nm1 + 1
		size := min(n, input.Atom.LMax+1)
		if !(len(occ) <= size) {
			return fmt.Errorf("invalid second dimension size of occupancies array [%d]", nm1)
		}
	}
	// Occupancies: 2(l+1) >= [n-1][l] >= 0
	for nm1, occ_n := range input.Atom.Occupancies {
		for l, occ_nl := range occ_n {
			if !(-1e-5 <= occ_nl && occ_nl <= float64(2*(2*l+1))+1e-5) {
				return fmt.Errorf("invalid occupancy [%d][%d]", nm1, l)
			}
		}
	}
	// SCF: 1 <= MaxIteration
	if !(1 <= input.SCF.MaxIteration) {
		return fmt.Errorf("invalid maximum iteration")
	}

	// DFT: XCType
	XCTypes := []string{"Xa-Slater", "LDA-CA", "LDA-VWN", "GGA-PBE"}
	if !slices.Contains(XCTypes, input.DFT.XCType) {
		return fmt.Errorf("invalid xc type")
	}

	return nil
}
