package main

import "os"

func LoadArgs_rm() bool {
	for i, v := range os.Args {
		if i > 0 && v == "--rm" {
			return true
		}
	}
	return false
}
