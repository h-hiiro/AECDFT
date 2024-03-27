# Density functional theory (DFT)

The density functional theory (DFT) [[P. Hohenberg and W. Kohn, Phys. Rev. **136**, B864 (1964)](https://journals.aps.org/pr/abstract/10.1103/PhysRev.136.B864) and [W. Kohn and L. J. Sham, Phys. Rev. **140**, A1133 (1965)](https://journals.aps.org/pr/abstract/10.1103/PhysRev.140.A1133)] analyzes the electronic structure of an interacting system using another associated non-interacting system.
It based on the Hohenberg-Kohn lemma claiming that the electron density distribution $n(\mathbf{r})$ is a good physical quantity to **uniquely** determine the system (and the external potential).
Therefore, given the external potential, we try to obtain the density distribution associated with the ground state energy.

The Hohenberg-Kohn lemma tells us that the total energy should be a functional of the density distribution:
$$ F[n(\mathbf{r})]=T[n(\mathbf{r})]+\int V(\mathbf{r})n(\mathbf{r})\mathrm{d}^3\mathbf{r}+\frac{1}{2}\int \frac{n(\mathbf{r})n(\mathbf{r}^\prime)}{|\mathbf{r}-\mathbf{r}^\prime|}\mathrm{d}^3\mathbf{r}\mathrm{d}^3\mathbf{r}^\prime+E_\mathrm{xc}[n(\mathbf{r})].$$
The four terms in the right hand side correspond to the kinetic energy, the external potential, the electron-electron interaction (Hartree term), and the exchange correlation energy, respectively.
We apply the variation method to obtain the ground state, then we can construct the Kohn-Sham (KS) system where the external potential is 
$$V_\mathrm{KS}(\mathbf{r})=V(\mathbf{r})+\frac{\delta E_\mathrm{xc}}{\delta n}\equiv V(\mathbf{r})+V_\mathrm{xc}(\mathbf{r}).$$
Therefore, the Schr&ouml;dinger equation in the KS system is
$$\left[-\frac{1}{2}\Delta + V(\mathbf{r})+\int\frac{n(\mathbf{r}^\prime)}{|\mathbf{r}-\mathbf{r}^\prime|}\mathrm{d}^3\mathbf{r}^\prime+V_\mathrm{xc}(\mathbf{r})\right]\psi_i(\mathbf{r})=\varepsilon_i \psi_i(\mathbf{r}).$$
The density distribution can be calculated by 
$$n(\mathbf{r})=\sum_{i\in \mathrm{occupied}} |\psi_i(\mathbf{r})|^2.$$

We note that the sum of eigenvalues $\varepsilon_i$ does not equal to the total energy.
The correct formulation is 
$$ F[n(\mathbf{r})]=\sum_{i\in\mathrm{occupied}}\varepsilon_i - \frac{1}{2}\int\frac{n(\mathbf{r})n(\mathbf{r}^\prime)}{|\mathbf{r}-\mathbf{r}^\prime|}\mathrm{d}^3\mathbf{r}\mathrm{d}^3\mathbf{r}^\prime-\int V_\mathrm{xc}(\mathbf{r})n(\mathbf{r})\mathrm{d}^3\mathbf{r}+E_\mathrm{xc}[n(\mathbf{r})].$$


# DFT potential
See [Calc_potential.cpp](../cpp/Calc_potential.cpp) for the implementation.

## Core potential
Since this software handles a one-atom system, the external potential is simply $V_\mathrm{core}(r)=-\frac{Z}{r}$.

## Hartree potential
Assuming the isotropic density distribution $n(r)$, we calculate the Hartree potential.
$$V_\mathrm{hartree}(r)=\int\frac{n(\mathbf{r})}{|\mathbf{r}-\mathbf{r}^\prime|}\mathrm{d}^3\mathbf{r}^\prime$$
$$=2\pi \int_0^\infty \mathrm{d}r^\prime \int_0^\pi \mathrm{d}\theta \frac{n(r^\prime)r^{\prime 2}\sin\theta}{\sqrt{r^2+r^{\prime 2}-2rr^\prime\cos\theta}}$$
$$=2\pi \int_0^\infty \mathrm{d}r^\prime n(r^\prime)r^{\prime 2} \left[\frac{1}{rr^\prime}\sqrt{r^2+r^{\prime 2}-2rr^\prime\cos\theta}\right]^\pi_0$$
$$=\frac{4\pi}{r}\int_0^r \mathrm{d}r^\prime n(r^\prime)r^{\prime 2}+4\pi\int_r^\infty \mathrm{d}r^\prime n(r^\prime)r^\prime.$$
Since the software uses the logarithmically separated grid $r=\exp(x)$, the integral can be slightly modified to
$$V_\mathrm{hartree}=\frac{4\pi}{r}\int_0^r \mathrm{d}x\cdot n(r^\prime)r^{\prime 3}+4\pi\int_r^\infty \mathrm{d}x\cdot n(r^\prime)r^{\prime 2}.$$
This formula is used in the implementation.

## Exchange correlation potential
Although the Hohenberg-Kohn lemma gurantees that the other part (exchange correlation) is the funcional of the electron density, the explicit formula has not yet been derived.
Various formulations have been proposed so far.

### Xa by Slater
While Slater's $X\alpha$ method [[J. C. Slater, Phys. Rev. **81**, 385 (1951)]()] is not originally in the stream of DFT, now it can be interpreted as a method to represent the exchange correlation part.

They try to simplify the Hartree-Fock equation
$$\left[-\frac{1}{2}\Delta + V_\mathrm{ext}(\mathbf{r})\right]\psi_i(\mathbf{r})+\left[\sum_j \int \frac{\psi_j^*(\mathbf{r}^\prime)\psi_j(\mathbf{r}^\prime)}{|\mathbf{r}-\mathbf{r}^\prime|}\mathrm{d}^3\mathbf{r}^\prime\right]\psi_i(\mathbf{r})$$
$$-\left[\sum_j \int \frac{\psi_j^*(\mathbf{r}^\prime)\psi_i(\mathbf{r}^\prime)\psi_j(\mathbf{r})}{|\mathbf{r}-\mathbf{r}^\prime|}\mathrm{d}^3\mathbf{r}^\prime\right]=\varepsilon_i \psi_i(\mathbf{r}),$$
especially the last term.
Multiplying $\psi_i^*(\mathbf{r})\psi_i(\mathbf{r})$ to the denominator and the numerator, the last term becomes 
$$-\sum_j \int \frac{\psi_j^*(\mathbf{r}^\prime)\psi_i(\mathbf{r}^\prime)\psi_j(\mathbf{r})}{|\mathbf{r}-\mathbf{r}^\prime|}\mathrm{d}^3\mathbf{r}^\prime=-\left[\sum_j \int \frac{\psi_j^*(\mathbf{r}^\prime)\psi_i^*(\mathbf{r})\psi_i(\mathbf{r}^\prime)\psi_j(\mathbf{r})}{|\mathbf{r}-\mathbf{r}^\prime|\psi_i^*(\mathbf{r})\psi_i(\mathbf{r})}\mathrm{d}^3\mathbf{r}^\prime\right]\psi_i(\mathbf{r}).$$
Assuming that the term in the square brackets does not strongly depend on $i$, they use the weighted average as the approximated value.
The weight is $\psi_i^*(\mathbf{r})\psi_i(\mathbf{r})/\sum_k \psi_k^*(\mathbf{r})\psi_k(\mathbf{r})$.
Then the weighted average is 
$$-\int \frac{\sum_{ij}\psi_j^*(\mathbf{r}^\prime)\psi_i^*(\mathbf{r})\psi_i(\mathbf{r}^\prime)\psi_j(\mathbf{r})}{|\mathbf{r}-\mathbf{r}^\prime|\sum_k \psi_k^*(\mathbf{r})\psi_k(\mathbf{r})}\mathrm{d}^3\mathbf{r}^\prime.$$
In the case of the free-electron gas with a density $n$, we can perform the integration.
Finally, we get
$$V_\mathrm{xc}=-\frac{3 k_\mathrm{F}}{2\pi}=-\frac{3}{2}\left(\frac{3n}{\pi}\right)^{1/3}$$
$$2\times \frac{4}{3}\pi k_\mathrm{F}^3\times \frac{1}{(2\pi)^3} = n\ \Longleftrightarrow\ k_\mathrm{F}=(3\pi^2 n)^{1/3}.$$

The empirical parameter $\alpha$ is added.
Since Kohn and Sham [[Phys. Rev. **140**, A1133 (1965)](https://journals.aps.org/pr/abstract/10.1103/PhysRev.140.A1133)] have reported that the exchange potential derived from the variation w.r.t. the density is 2/3 times different than Slater's.
$$\varepsilon_\mathrm{xc}=-\frac{3}{4}\left(\frac{3n}{\pi}\right)^{1/3}\ \Longrightarrow\ \frac{\delta (n\varepsilon_\mathrm{xc})}{\delta n}=-\left(\frac{3n}{\pi}\right)^{1/3}$$
Usually, $\alpha$ is set between 2/3 and 1.

### LDA by Ceperley and Alder
The local density approximation (LDA) computes the exchange correlation energy by the following integration:
$$ E_\mathrm{xc}=\int \varepsilon_{xc}(n(\mathbf{r}))n(\mathbf{r})\mathrm{d}^3\mathbf{r},$$
where $\varepsilon_{xc}(n)$ represents the exchange correlation energy density in the uniform electron gas system with a density $n$.
The exchange part (Hartree energy) can be numerically computed:
$$\varepsilon_\mathrm{x} = -\frac{3}{4}\left(\frac{3n}{\pi}\right)^{1/3}= -\frac{3}{4}\left(\frac{9}{4\pi^2}\right)^{1/3} r_s^{-1} = -\frac{0.4582}{r_s}$$
$$ \frac{4}{3}\pi r_s^3 = \frac{1}{n}$$
$r_s$ is the length parameter associated with the density $n$.
Ceperley and Alder [[Phys. Rev. Lett. **45**, 566 (1980)](https://journals.aps.org/prl/abstract/10.1103/PhysRevLett.45.566)] calculated the correlation energy by a Monte Carlo method and obtained the following fitting curve:
$$\varepsilon_{\mathrm{c}}=\begin{cases}
-\dfrac{0.1423}{1+1.0529\sqrt{r_s}+0.3334 r_s} & r_s\le 1.0 \\
-0.0480 + 0.0311 \log(r_s)-0.0116 r_s + 0.0020 r_s \log(r_s) & r_s\ge 1.0.
\end{cases}$$
The fitting parameter can be obtained from [[J. P. Perdew and A. Zunger, Phys. Rev. B **23**, 5048 (1981)](https://journals.aps.org/prb/abstract/10.1103/PhysRevB.23.5048)].
Finally, the exchange correlation potential is
$$ V_\mathrm{xc}=\frac{\delta (n\varepsilon_{xc})}{\delta n}=\varepsilon_\mathrm{x}+\varepsilon_\mathrm{c}-\frac{r_s}{3}\frac{\mathrm{d}}{\mathrm{d}r_s}(\varepsilon_\mathrm{x}+\varepsilon_\mathrm{c})$$
$$\because n=\frac{3}{4\pi}r_s^{-3} \ \Longrightarrow\ \delta n = -\frac{3n}{r_s}\mathrm{d}r_s.$$

### LDA by Vosko, Wilk, and Nusair
The correlation function proposed by Vosko, Wilk, and Nusair [[S. H. Vosko *et al.*, Can. J. Phys. **58**, 1200 (1980)](https://doi.org/10.1139/p80-159)] is 
$$\varepsilon_\mathrm{c}=A\left[\log \frac{x^2}{X(x)}+\frac{2b}{Q}\mathrm{atan}\frac{Q}{2x+b}-\frac{bx_0}{X(x_0)}\left(\log\frac{(x-x_0)^2}{X(x)}+\frac{2(b+2x_0)}{Q}\mathrm{atan}\frac{Q}{2x+b}\right)\right]$$
$$x=\sqrt{r_s},\ X(x)=x^2+bx+c,\ Q=\sqrt{4c-b^2}$$
$$A=0.0310907,\ x_0=-0.10498,\ b=3.72744,\ c=12.9352.$$
The value of $A$ looks different twice due to the energy unit; the VWN paper seems to use the Rydberg atomic unit.
The derivative form is
$$\frac{\mathrm{d}\varepsilon_\mathrm{c}}{\mathrm{d}r_s}=\frac{A}{2x^2}\left[\frac{bx+2c}{X(x)}-\frac{4bx}{4x^2+4bx+b^2+Q^2}-\frac{bxx_0}{X(x_0)}\left(\frac{2xx_0+b(x+x_0)+2c}{(x-x_0)X(x)}-\frac{4(b+2x_0)}{4x^2+4bx+b^2+q^2}\right)\right]$$