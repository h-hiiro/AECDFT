package main

import "fmt"

func (calcData *CalcData) PrepareEigenstateArray() error {
	for nm1, occ_n := range calcData.Input.Atom.Occupancies {
		for l, occ_nl := range occ_n {
			if calcData.Input.DFT.EqType == "Dirac" && l > 0 {
				var occ_nl_jphalf float64
				var occ_nl_jmhalf float64
				var maxOcc_jphalf float64 = 2.0 * (float64(l) + 1.0)
				var maxOcc_jmhalf float64 = 2.0 * float64(l)
				var maxOcc_sum float64 = 2.0 * (2.0*float64(l) + 1.0)
				switch calcData.Input.Atom.OccupancyRule_Dirac {
				case "one-half":
					occ_nl_jphalf = occ_nl * 0.5
					occ_nl_jmhalf = occ_nl * 0.5
				case "proportional":
					occ_nl_jphalf = occ_nl * maxOcc_jphalf / maxOcc_sum
					occ_nl_jmhalf = occ_nl * maxOcc_jmhalf / maxOcc_sum
				case "l-1/2":
					if occ_nl < maxOcc_jmhalf {
						occ_nl_jphalf = 0.0
						occ_nl_jmhalf = occ_nl
					} else {
						occ_nl_jphalf = occ_nl - maxOcc_jmhalf
						occ_nl_jmhalf = maxOcc_jmhalf
					}
				case "l+1/2":
					if occ_nl < maxOcc_jphalf {
						occ_nl_jphalf = occ_nl
						occ_nl_jmhalf = 0.0
					} else {
						occ_nl_jphalf = maxOcc_jphalf
						occ_nl_jmhalf = occ_nl - maxOcc_jphalf
					}
				default:
					return fmt.Errorf("invalid occupancy rule")
				}

				es1 := Eigenstate{N: nm1 + 1, L: l, Kappa: -(l + 1), Occupancy: occ_nl_jphalf}
				es2 := Eigenstate{N: nm1 + 1, L: l, Kappa: l, Occupancy: occ_nl_jmhalf}
				calcData.Output.Eigenstates = append(calcData.Output.Eigenstates, es1)
				calcData.Output.Eigenstates = append(calcData.Output.Eigenstates, es2)
			} else {
				es1 := Eigenstate{N: nm1 + 1, L: l, Occupancy: occ_nl}
				calcData.Output.Eigenstates = append(calcData.Output.Eigenstates, es1)
			}
		}
	}
	return nil
}
