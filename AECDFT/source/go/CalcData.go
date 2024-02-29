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
}

type Grid_conf struct {
	XMin float64
	XMax float64
	Num  int
}

type Atom_conf struct {
	Z int
}

type TF_conf struct {
	InitialDiff_left  float64
	InitialDiff_right float64
	Threshold         float64
}

type OutputData struct {
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

type ExecData struct {
	Started      time.Time
	Finished     time.Time
	Duration_sec float64
}

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

	return nil
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
