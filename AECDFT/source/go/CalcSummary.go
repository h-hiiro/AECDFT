package main

import "fmt"

type CalcSummary struct {
	InputFile    string
	OutputFile   string
	Duration_sec float64
	Error        error
}

func (calcSummary CalcSummary) String() string {
	var result string
	if calcSummary.Error != nil {
		result = "Fail"
	} else {
		result = "Success"
	}
	str := fmt.Sprintf("[%-7s] %s -> %s (%.3f sec)",
		result,
		calcSummary.InputFile,
		calcSummary.OutputFile,
		calcSummary.Duration_sec)
	if calcSummary.Error != nil {
		str = fmt.Sprintf("%s: %v", str, calcSummary.Error)
	}
	return str
}
