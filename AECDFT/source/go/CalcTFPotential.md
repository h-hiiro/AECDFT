# Thomas-Fermi potential

The Thomas-Fermi (TF) potential is the screened potential derived using the TF approximation [[N. H. March, Advances in Physics **6**, 1 (1957)](https://doi.org/10.1080/00018735700101156)].

We consider the case where the atomic nucleus with charge $Z$ is put at the origin and $Z$ electrons are around the nucleus.
$V(r)$ and $\rho(r)$ represent the potential and electron density, respectively.
Also, we suppose the similar relationship as the electron gas model holds at any $r$.
Considering the spin, the electron density $\rho(r)$ and the Fermi wavevector $k_\mathrm{F}$ can be associated with
$$\rho(r)=2\times\frac{4}{3}\pi k_\mathrm{F}^3\times\frac{1}{(2\pi)^3}=\frac{k_\mathrm{F}(r)^3}{3\pi^2}.$$

The Fermi level ($E_\mathrm{F}$) should be isotropically uniform, so the relationship
$$ E_\mathrm{F} = \frac{1}{2}k_\mathrm{F}(r)+V(r)=\textrm{const.} =0 $$
holds.
Representing $k_\mathrm{F}$ as a function of $\rho(r)$, and using $V(r)=-\phi(r)$ where $\phi(r)$ is the electric field, we get
$$ E_\mathrm{F}=\frac{1}{2}(3\pi^2\rho(r))^{2/3}-\phi(r).$$
Furthermore, combining it with the Poisson equation
$$\Delta\phi(r) =4\pi\rho(r),$$
we get
$$\frac{1}{r^2}\frac{\mathrm{d}}{\mathrm{d}r} r^2 \frac{\mathrm{d}}{\mathrm{d}r} (\phi(r)+E_\mathrm{F})=\frac{4}{3\pi}\Bigl(2(\phi(r)+E_\mathrm{F})\Bigr)^{3/2}.$$

We define another function $f(r)$ by
$$f(r)=\frac{r}{Z}(\phi(r)+E_\mathrm{F}).$$
In the limit $r\rightarrow 0$, $\phi(r)\rightarrow Z/r$ should be dominant, so $f(r)\rightarrow 1$.
In the limit $r\rightarrow \infty$, $f(r)\rightarrow 0$ should hold.
Using $f(r)$, we get
$$\frac{1}{r}\frac{\mathrm{d}^2}{\mathrm{d} r^2} f(r)=\frac{4}{3\pi} r^{-3/2} Z^{1/2} (2f(r))^{3/2}$$
$$\Longleftrightarrow\ \frac{\mathrm{d}^2}{\mathrm{d} r^2} f(r)=\frac{2^{7/2} Z^{1/2}}{3\pi}\frac{1}{r^{1/2}} f(r)^{3/2}.$$
To simplify the coefficients by the scaling $r=\mu x$, using $g(x)=f(r)=f(\mu x)$, we get
$$\frac{1}{\mu^2}\frac{\mathrm{d}^2}{\mathrm{d}x^2}g(x)=\frac{2^{7/2} Z{1/2}}{3\pi}\frac{1}{(\mu x)^{1/2}}g(x)^{3/2}$$
$$\therefore \mu = \left(\frac{3\pi}{2^{7/2} Z^{1/2}}\right)^{2/3} = \frac{1}{2Z^{1/3}}\left(\frac{3\pi}{4}\right)^{2/3}.$$
Using this scaling, the equation to be solved becomes
$$\frac{\mathrm{d}^2}{\mathrm{d} x^2} g(x) = \frac{g(x)^{3/2}}{\sqrt{x}},\ g(x)\rightarrow 1,0\ (x\rightarrow 0, \infty)$$
and the TF potential and density are 
$$V(r)=-\frac{Z}{r}g(r/\mu)$$
$$\rho(r)=\frac{k_\mathrm{F}(r)^3}{3\pi^2},\ k_\mathrm{F}=\sqrt{-2V(r)}.$$

# Calculation of the TF potential
The screened TF function $g(x)$ can be obtained by the **Euler method**.
Since $g(0)=1$ is given but $g^\prime(0)$ is not, we use the **bisection method** to result in $g(x)\rightarrow 0\ (x\rightarrow \infty)$.
See ```calc_TFPotential``` and ```TF_evolution``` in [Calc_TF.cpp](../cpp/Calc_TF.cpp) for the details of the algorithm.

After obtaining $g(x)$, we compute the TF potential $V(r)$ and density $\rho(r)$.
See ```calc_TFDensity``` in [Calc_TF.cpp](../cpp/Calc_TF.cpp) for the details.
