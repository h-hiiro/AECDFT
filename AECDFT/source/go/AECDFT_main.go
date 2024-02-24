package main

import (
	"fmt"
	"log"
	"os"
	"path/filepath"
	"time"
)

const (
	InputDir         string = "./Input"
	OutputDir        string = "./Output"
	SimpleTimeFormat string = "2006-01-02_15-04-05"
)

func main() {
	// Time zone set
	var loc *time.Location
	var err error
	if loc, err = SetLocation(); err != nil {
		log.Fatal(err)
	}

	// Check arguments
	deleteAfterSuccess := LoadArgs_rm()

	// List input files
	inputFiles, err := FindInputFiles(InputDir)
	if err != nil {
		log.Fatal(err)
	}

	var calcSummaries []CalcSummary
	for i, inputFile := range inputFiles {
		fmt.Printf("%s is found\n", inputFile)
		calcSummaries = append(calcSummaries, CalcSummary{InputFile: inputFile})

		// Generate the data variable
		var calcData *CalcData
		if calcData, err = NewCalcData(); err != nil {
			calcSummaries[i].Error = err
			continue
		}
		calcData.Exec.Started = time.Now().In(loc)

		// Load the input file and validate it
		if err = calcData.AppendInput(filepath.Join(InputDir, inputFile)); err != nil {
			calcSummaries[i].Error = err
			continue
		}
		if err = calcData.Input.Validate(); err != nil {
			calcSummaries[i].Error = err
			continue
		}

		// Prepare the variables
		Grid := GenGrid(calcData.Input.Grid)

		fmt.Printf("Grid size: %v\n", Grid.Size)

		// Calculate the computation time
		calcData.Exec.Finished = time.Now().In(loc)
		calcData.Exec.Duration_sec = calcData.Exec.Finished.Sub(calcData.Exec.Started).Seconds()
		calcSummaries[i].Duration_sec = calcData.Exec.Duration_sec

		// Export the result
		outputFile := fmt.Sprintf("%s_%s.json", calcData.Exec.Started.Format(SimpleTimeFormat), calcData.Input.Title)
		calcSummaries[i].OutputFile = outputFile
		if err = calcData.Export(filepath.Join(OutputDir, outputFile)); err != nil {
			calcSummaries[i].Error = err
			continue
		}

		// Set nil to Error (success)
		calcSummaries[i].Error = nil

		// Remove the input files
		if deleteAfterSuccess {
			if err = os.Remove(filepath.Join(InputDir, inputFile)); err != nil {
				fmt.Printf("Error during removal: %v\n", err)
			} else {
				fmt.Printf("%s was removed\n", inputFile)
			}
		}

		// Free allocated vectors
		Grid.Free()
	}

	// Display the calculation summary
	fmt.Println("----------------")
	fmt.Println("Calculation Summary:")
	for _, calcSummary := range calcSummaries {
		fmt.Printf("%v\n", calcSummary)
	}
}
