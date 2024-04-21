# Differential equation 

## Schroedinger equation
If the relativistic effect is negligible, the differential equation to be solved is of the form of the Schroedinger equation.
Since the potential is isotropic, we can find eigenstates for both the Hamiltonian and the angular momentum $\mathbf{L}$.

The radial differential equation is 
$$\left[-\frac{1}{2}\frac{\partial^2}{\partial r}+\frac{l(l+1)}{2r^2}+v(r)\right]P(r)=\varepsilon P(r)$$
$$\psi(r)=\frac{P(r)}{r}Y_{lm}(\theta, \varphi).$$

## Dirac equation
The differential equation in the relativistic situation should be derived from the Dirac equation.
We follow the derivation in [L. I. Schiff, "Quantum Mechanics", translated to Japanese by K. Inoue](http://www.yoshiokasyoten.sakura.ne.jp/phys/ISBN4-8427-0158-7.html).

The Dirac Hamiltonian in the potential $V(r)$ is 
$$H=c\boldsymbol\alpha\cdot\mathbf{p}+\beta c^2+V(r)$$
$$\boldsymbol\alpha=\begin{pmatrix}0 & \boldsymbol\sigma \\ \boldsymbol\sigma & 0 \end{pmatrix},\ \beta=\begin{pmatrix}I & 0 \\ 0 & -I \end{pmatrix}$$
$$\boldsymbol\sigma = \begin{pmatrix}0 & 1 \\ 1 & 0 \end{pmatrix},\ \begin{pmatrix}0 & -\mathrm{i} \\ \mathrm{i} & 0 \end{pmatrix},\ \begin{pmatrix}1 & 0 \\ 0 & -1 \end{pmatrix}.$$
$\boldsymbol\sigma$ is called the Pauli matrices.
Then we introduce three new operators for the modification:
$$ p_r=\frac{1}{r}(\mathbf{r}\cdot\mathbf{p}-\mathrm{i})$$
$$\alpha_r=\frac{1}{r}\boldsymbol\alpha\cdot{\mathbf{r}}$$
$$k=\beta(\boldsymbol\sigma^\prime\cdot\mathbf{L}+1),\ \ (\boldsymbol\sigma^\prime = \begin{pmatrix}\boldsymbol\sigma & 0 \\ 0 & \boldsymbol\sigma\end{pmatrix}).$$
These operatiors satisfy
$$\boldsymbol\alpha\cdot\mathbf{p}=\alpha_r p_r +\frac{\mathrm{i}}{r}\alpha_r \beta k.$$
Therefore the Hamiltonian can be transformed as follows:
$$H=c\alpha_r p_r +\frac{\mathrm{i}c}{r}\alpha_r \beta k + \beta mc^2 +V(r).$$
Also it can be proved that $k$ commutes with the Hamiltonian.
Since $k$ is related to the combined angular momentum $\mathbf{J}=\mathbf{L}+\mathbf{S}$, we use the eigenstates for $\mathbf{L}$ with the eigenvalue $l$ ($2l+1$ degeneracy).
In this case, the eigenstates for $\mathbf{J}$ is $l+1/2$ ($2l+2$ degeneracy) or $l-1/2$ ($2l$ degeneracy) and the correponding $k$ values are $l+1$ or $-l$.

In the differential equation for the simultaneous eigenstates, we don't need $4\times4$ matrix.
$\beta=\begin{pmatrix}1 & 0 \\ 0 & -1\end{pmatrix},\ \alpha_r = \begin{pmatrix} 0 & -\mathrm{i} \\ \mathrm{i} & 0 \end{pmatrix}$ satisfy the required conditions.
The eigenstate has the two components, so we represent them like $\psi=\begin{pmatrix} F(r)/r \\ G(r)/r\end{pmatrix}.$
Then the Hamiltonian matrix is 
$$H=\begin{pmatrix} c^2+V & -\left(\dfrac{\mathrm{i}c}{r}\mathbf{r}\cdot\mathbf{p}+\dfrac{c}{r}\right)-\dfrac{ck}{r} \\ \dfrac{\mathrm{i}c}{r}\mathbf{r}\cdot\mathbf{p}+\dfrac{c}{r}-\dfrac{ck}{r} & -c^2+V\end{pmatrix} $$
and the differntial equation is 
$$ (E-c^2-V)F(r) + c\frac{\partial G}{\partial r} + \frac{ck}{r}G(r) = 0$$
$$ (E+c^2-V)F(r) - c\frac{\partial G}{\partial r} + \frac{ck}{r}G(r) = 0$$
These equations can be transfomed the the ones used in the ADPACK [(Link)](https://t-ozaki.issp.u-tokyo.ac.jp/mpcoms2021_lectures/Atomic_DFT-implementation.pdf) by 
- defining $\alpha=1/c$ (fine structure constant)
- replacing $E-c^2$ by $\varepsilon$ (energy without the mass term)
- replacing $k$ by $-\kappa = -(l+1),\ l$, and
- exchanging $F(r)$ and $G(r)$.
The final form is
$$\left(\frac{\partial}{\partial r}-\frac{\kappa}{r}\right) F(r) + \alpha(\varepsilon-V(r))G(r) = 0 $$
$$\left(\frac{\partial}{\partial r}+\frac{\kappa}{r}\right) G(r) -\alpha\left(\varepsilon-V+\frac{2}{\alpha^2}\right)F(r)=0.$$
Using the latter equation, we get
$$ F(r)=\frac{\alpha}{2(1+(\varepsilon-V(r))/\alpha^2)}\left(\frac{\partial}{\partial r}+\frac{\kappa}{r}\right)G(r).$$
Inserting it into the former equation, finally we get
$$\left[\frac{1}{2m}\left(\frac{\partial^2}{\partial r^2}+\frac{\alpha}{2m}\frac{\partial V}{\partial r}\left(\frac{\partial}{\partial r}+\frac{\kappa}{r}\right)-\frac{\kappa(\kappa+1)}{r^2}\right)+(\varepsilon-V(r))\right]G(r) = 0$$
$$m=1+\frac{\alpha^2(\varepsilon-V(r))}{2}.$$