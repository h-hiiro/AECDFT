package main

import (
	"encoding/json"
	"fmt"
	"os"
	"time"
)

const (
	InputData_default string = "./input_default.json"
	Indent            string = "  "
)

type CalcData struct {
	Input  InputData
	Output OutputData
	Exec   ExecData
}

type InputData struct {
	Title string
}

type OutputData struct {
}

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
	err = json.Unmarshal(inputFileData, &(calcData.Input))
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
