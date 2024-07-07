package main

import "C"
import (
	"encoding/json"
	"fmt"
	"os"
	"time"

	"github.com/tidwall/jsonc"
)

const (
	InputData_default string = "./input_default.jsonc"
	Indent            string = "  "
)

type CalcData struct {
	Input  InputData
	Output OutputData
	Exec   ExecData
}

type InputData struct {
	Title string
	Grid  Grid_conf
	Atom  Atom_conf
	TF    TF_conf
	SCF   SCF_conf
	DFT   DFT_conf
}

type Grid_conf struct {
	XMin float64
	XMax float64
	Num  int
}

type Atom_conf struct {
	Z                   float64
	LMax                int
	NMax                int
	Occupancies         [][]float64
	NumElectrons        float64
	NumOrbitals         int
	OccupancyRule_Dirac string
}

type TF_conf struct {
	InitialDiff_left  float64
	InitialDiff_right float64
	Threshold         float64
}

type SCF_conf struct {
	MaxIteration int
	Threshold    DeltaRhoNorm
}

type DFT_conf struct {
	XCType                string
	Xa_alpha              float64
	EqType                string
	PotentialFittingOrder int
}

type OutputData struct {
	SCF         SCF_result
	Eigenstates []Eigenstate
}
type Eigenstate struct {
	N         int
	L         int
	Kappa     int
	Occupancy float64
	E         float64
}

func (e Eigenstate) String_nlo() string {
	return fmt.Sprintf("N=%d, L=%d, Occupancy=%5.1f", e.N, e.L, e.Occupancy)
}
func (e Eigenstate) String() string {
	return fmt.Sprintf("N=%d, L=%d, Occupancy=%5.1f, Eigenvalue=%18.10f",
		e.N, e.L, e.Occupancy, e.E)
}

type Grid_value struct {
	X    *Axis
	R    *Axis
	Size int
	DX   float64
}

type Axis C.double
type Potential C.double
type DensityDistribution C.double
type DifferentialCoefficients C.double
type Wavefunction C.double

type DeltaRhoNorm float64

type SCF_result struct {
	Achieved bool
}

type ExecData struct {
	Started      time.Time
	Finished     time.Time
	Duration_sec float64
}

func NewCalcData() (*CalcData, error) {
	calcData := new(CalcData)
	err := calcData.AppendInput(InputData_default)
	if err != nil {
		return nil, err
	}
	return calcData, nil
}

func (calcData *CalcData) AppendInput(filepath string) error {
	inputFileData, err := os.ReadFile(filepath)
	if err != nil {
		return err
	}
	err = json.Unmarshal(jsonc.ToJSON(inputFileData), &(calcData.Input))
	if err != nil {
		return err
	}
	return nil
}

func (calcData *CalcData) Export(filepath string) error {
	outputFileData, err := json.MarshalIndent(calcData, "", Indent)
	if err != nil {
		return err
	}
	os.WriteFile(filepath, outputFileData, 0755)
	return nil
}
