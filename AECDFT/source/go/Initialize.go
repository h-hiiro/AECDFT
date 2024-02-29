package main

/*
#include "../cpp/LibAECDFT.hpp"
*/
import "C"

func GenGrid(grid_conf Grid_conf) *Grid_value {
	grid_value := new(Grid_value)
	grid_value.Size = grid_conf.Num
	grid_value.X = (*Axis)(C.alloc_dvector(C.int(grid_conf.Num)))
	grid_value.R = (*Axis)(C.alloc_dvector(C.int(grid_conf.Num)))
	grid_value.DX = (grid_conf.XMax - grid_conf.XMin) / float64(grid_conf.Num-1)
	C.fill_grid(C.int(grid_value.Size),
		(*C.double)(grid_value.X),
		(*C.double)(grid_value.R),
		C.double(grid_conf.XMin),
		C.double(grid_value.DX))
	return grid_value
}

func (grid *Grid_value) Free() {
	C.free_dvector((*C.double)(grid.X))
	C.free_dvector((*C.double)(grid.R))
}
