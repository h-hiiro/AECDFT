package main

import (
	"os"
	"path/filepath"
)

func FindInputFiles(dir string) ([]string, error) {
	var JsonFiles []string
	AllInputFiles, err := os.ReadDir(dir)
	if err != nil {
		return nil, err
	}
	for _, inputFile := range AllInputFiles {
		if inputFile.IsDir() {
			continue
		}
		inputFileName := inputFile.Name()
		inputFileExt := filepath.Ext(inputFileName)
		if inputFileExt == ".json" {
			JsonFiles = append(JsonFiles, inputFileName)
		}
	}
	return JsonFiles, nil
}
