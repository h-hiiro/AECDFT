package main

/*
#cgo LDFLAGS: -L../cpp -laecdft -lstdc++ -lm -static
#include "../cpp/LibAECDFT.hpp"
*/
import "C"
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

// # Main routine
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

	// # Calculation part
	var calcSummaries []CalcSummary
	for i, inputFile := range inputFiles {
		fmt.Printf("%s is found\n", inputFile)
		calcSummaries = append(calcSummaries, CalcSummary{InputFile: inputFile})

		// Generate the data variables
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
		Grid := calcData.Input.Grid.GenGrid()
		V_total := (*Potential)(C.alloc_dvector(C.int(Grid.Size)))
		V_core := (*Potential)(C.alloc_dvector(C.int(Grid.Size)))
		V_hartree := (*Potential)(C.alloc_dvector(C.int(Grid.Size)))
		V_xc := (*Potential)(C.alloc_dvector(C.int(Grid.Size)))
		rho := (*DensityDistribution)(C.alloc_dvector(C.int(Grid.Size)))
		// Wavefunctions := calcData.PrepareOrbitals()

		/// the name NormRD is the convention in ADPACK and OpenMX
		NormRD_history := C.alloc_dvector(C.int(calcData.Input.SCF.MaxIteration))
		var NormRD DeltaRhoNorm
		var IterationNumber int = 0
		var SCF_OK bool = false

		// Obtain the initial density distriubtion (Thomas-Fermi type)
		// rho is used while V is not used
		err = CalcTFPotential(Grid, calcData.Input.Atom.Z, V_total, rho, calcData.Input.TF)
		if err != nil {
			calcSummaries[i].Error = err
			continue
		}
		// C.print_dvector2(C.int(Grid.Size), (*C.double)(Grid.R), (*C.double)(V_total), C.CString("%.6f"))

		// SCF loop
		for !SCF_OK && IterationNumber < calcData.Input.SCF.MaxIteration {
			IterationNumber++

			// Prepare potential
			CalcCore(Grid, calcData.Input.Atom.Z, V_core)
			CalcHartree(Grid, rho, V_hartree)
			err = CalcXC(Grid, rho, calcData.Input.DFT, V_xc)
			if err != nil {
				calcSummaries[i].Error = err
				break
			}
			CalcSum(Grid, V_core, V_hartree, V_xc, V_total)
			// C.print_dvector2(C.int(Grid.Size), (*C.double)(Grid.R), (*C.double)(V_total), C.CString("%.6f"))

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
		C.free_dvector((*C.double)(V_total))
		C.free_dvector((*C.double)(V_core))
		C.free_dvector((*C.double)(V_hartree))
		C.free_dvector((*C.double)(V_xc))
		C.free_dvector((*C.double)(rho))

	}

	// Display the calculation summary
	fmt.Println("----------------")
	fmt.Println("Calculation Summary:")
	for _, calcSummary := range calcSummaries {
		fmt.Printf("%v\n", calcSummary)
	}
}
