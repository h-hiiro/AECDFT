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
	fmt.Println("Hello, world!")
	loc := time.FixedZone(Location, TimeDiff)
	deleteAfterSuccess := LoadArgs_rm()

	inputFiles, err := FindInputFiles(InputDir)
	if err != nil {
		log.Fatal(err)
	}

	var calcSummaries []CalcSummary

	for i, inputFile := range inputFiles {
		fmt.Printf("%s is found\n", inputFile)
		calcSummaries = append(calcSummaries, CalcSummary{InputFile: inputFile})

		calcData, err := NewCalcData()
		if err != nil {
			calcSummaries[i].Error = err
			continue
		}
		calcData.Exec.Started = time.Now().In(loc)
		err = calcData.AppendInput(filepath.Join(InputDir, inputFile))
		if err != nil {
			calcSummaries[i].Error = err
			continue
		}

		fmt.Printf("Title: \"%s\"\n", calcData.Input.Title)
		err = calcData.Input.Validate()
		if err != nil {
			calcSummaries[i].Error = err
			continue
		}

		calcData.Exec.Finished = time.Now().In(loc)
		calcData.Exec.Duration_sec = calcData.Exec.Finished.Sub(calcData.Exec.Started).Seconds()
		calcSummaries[i].Duration_sec = calcData.Exec.Duration_sec

		outputFile := fmt.Sprintf("%s_%s.json", calcData.Exec.Started.Format(SimpleTimeFormat), calcData.Input.Title)
		calcSummaries[i].OutputFile = outputFile

		err = calcData.Export(filepath.Join(OutputDir, outputFile))
		if err != nil {
			calcSummaries[i].Error = err
			continue
		}

		calcSummaries[i].Error = nil
		if deleteAfterSuccess {
			err = os.Remove(filepath.Join(InputDir, inputFile))
			if err != nil {
				fmt.Printf("Error during removal: %v\n", err)
			} else {
				fmt.Printf("%s was removed\n", inputFile)
			}

		}
	}

	fmt.Println("----------------")
	fmt.Println("Calculation Summary:")
	for _, calcSummary := range calcSummaries {
		fmt.Printf("%v\n", calcSummary)
	}
}
