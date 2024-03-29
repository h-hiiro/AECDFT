# Main routine

- Sets the time zone. See [Setlocation.go](./SetLocation.go) for the details.
  - The datetime information will appear in the output file name.
  - The time zone can be specified by the ```AECDFT_TZ``` environment variable.
- Check the arguments. See [LoadArgs.go](./LoadArgs.go) for the details.
  - if ```--rm``` exists, the routine removes the input files with which the calculations successfully finished.
- Get a list of input files with extensions ```.json``` (JSON) or ```.jsonc``` (JSON with comments).
- For each input file, perform calculations.

# Calculation part

## Variables
- The variable ```calcSummaries``` lists the calculation summary. See [CalcSummary.go](./CalcSummary.go) for the structure definition.
- The variable ```calcData``` contains all the calculation conditions and results. See [CalcData.go](./CalcData.go) for the structure definition.
- The variable ```Grid``` (type ```*Grid_value```) contains the radial grid configuration. See ```GenGrid``` in [Initialize.go](./Initialize.go) for the details.
  - The program use the logarithmically separated grid. ```Grid.X``` is the log value (equally separated) and ```Grid.R``` is the normal value (logrithmically separated).
- The variables ```V_core```, ```V_hartree```, ```V_xc```, and ```V_total``` (type ```*Potential```) contain the potentials.
- The variable ```rho``` (type ```*DensityDistribution```) contains the density distribution $\rho(r)$.

## Procedure

- Load the input file and validate it. See ```NewCalcData```, ```AppendInput```, and ```Validate``` in [CalcData.go](./CalcData.go) for the details.
  - Some options have the default values, as specified in [input_default.jsonc](../../default/input_default.jsonc).
- Prepare the variables.
- Fill ```rho``` (and ```V_total```) by the initial guess.
  - The **Thomas-Fermi (TF) distribution** is used. See [CalcTFPotential.go](./CalcTFPotential.go) and [Calc_TF.cpp](../cpp/Calc_TF.cpp) for the details.
  - The TF potential is not used while the TF density is used.
- Perform iteration until the system satisfies the self consistency condition.
  - Prepare the potential from the resultant density distribution of the last iteration. See [CalcDFTPotential.go](./CalcDFTPotential.go) for the details.