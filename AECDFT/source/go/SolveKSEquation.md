# Differential equation 

## Schroedinger equation
If the relativistic effect is negligible, the differential equation to be solved is of the form of the Schroedinger equation.
Since the potential is isotropic, we can find eigenstates for both the Hamiltonian and the angular momentum $\mathbf{L}$.

The radial differential equation is 
$$\left[-\frac{1}{2}\frac{\partial^2}{\partial r^2}+\frac{l(l+1)}{2r^2}+v(r)\right]P(r)=\varepsilon P(r)$$
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
- replacing $k$ by $-\kappa\ (\kappa = -(l+1),\ l)$, and
- exchanging $F(r)$ and $G(r)$.

The final form is
$$\left(\frac{\partial}{\partial r}-\frac{\kappa}{r}\right) F(r) + \alpha(\varepsilon-V(r))G(r) = 0 $$
$$\left(\frac{\partial}{\partial r}+\frac{\kappa}{r}\right) G(r) -\alpha\left(\varepsilon-V+\frac{2}{\alpha^2}\right)F(r)=0.$$
Using the latter equation, we get
$$ F(r)=\frac{\alpha}{2(1+(\varepsilon-V(r))/\alpha^2)}\left(\frac{\partial}{\partial r}+\frac{\kappa}{r}\right)G(r).$$
Inserting it into the former equation, finally we get
$$\left[\frac{1}{2m}\left(\frac{\partial^2}{\partial r^2}+\frac{\alpha^2}{2m}\frac{\partial V}{\partial r}\left(\frac{\partial}{\partial r}+\frac{\kappa}{r}\right)-\frac{\kappa(\kappa+1)}{r^2}\right)+(\varepsilon-V(r))\right]G(r) = 0$$
$$m=1+\frac{\alpha^2(\varepsilon-V(r))}{2}.$$

# Transforming the ODEs for calculations
## Transformed Schroedinger equation

We apply the following transformation for the simpler representation:
$$P(r)=r^{l+1} L(x)$$
$$M(x)=\frac{\partial}{\partial x} L(x)$$
$$x=\log r.$$

Using them, we derive
$$\frac{\partial}{\partial r}P(r)=(l+1)r^l L(x)+r^{l+1} \frac{\partial}{\partial r}L(x)$$
$$\frac{\partial^2}{\partial r^2}P(r)=l(l+1)r^{l-1} L(x)+2(l+1)r^l \frac{\partial}{\partial r}L(x)+r^{l+1}\frac{\partial^2}{\partial r^2} L(x)$$
$$\frac{\partial}{\partial r} L(x)=\frac{1}{r}\frac{\partial}{\partial x}L(x)=e^{-x}\frac{\partial}{\partial x}L(x)$$
$$\Longrightarrow \frac{\partial}{\partial r}P(r)=r^l \left[(l+1)L(x)+\frac{\partial}{\partial x}L(x)\right]$$
$$\frac{\partial^2}{\partial r^2} L(x)=e^{-2x}\left[-\frac{\partial}{\partial x}L(x)+\frac{\partial^2}{\partial x^2}L(x)\right]$$
$$\Longrightarrow \frac{\partial^2}{\partial r^2}P(r)=r^{l-1}\left[l(l+1)L(x)+(2l+1)\frac{\partial}{\partial x}L(x)+\frac{\partial^2}{\partial x^2}L(x)\right].$$

Therefore, the Schroedinger equation becomes
$$\left[\frac{\partial^2}{\partial x^2}+(2l+1)\frac{\partial}{\partial x}-2r^2(v(r)-\varepsilon)\right]L(x)=0$$
$$\Longrightarrow \frac{\partial}{\partial x} L(x)=M(x),\ \ 
\frac{\partial}{\partial x} M(x)=-(2l+1)M(x)+2r^2(v(r)-\varepsilon)L(x).$$

## Transformed Dirac equation
In case of the Dirac equation, the similar transformation is applied to $G(r)$.
$$\left[\frac{\partial^2}{\partial x^2}+(2l+1)\frac{\partial}{\partial x} +l(l+1)-\kappa(\kappa+1)+\frac{r\alpha^2}{2m}\frac{\partial V}{\partial r}\left(l+1+\kappa+\frac{\partial}{\partial x}\right)-2mr^2(v(r)-\varepsilon)\right]L(x)=0$$
$$\Longrightarrow \frac{\partial}{\partial x} L(x)=M(x),\ \ 
\frac{\partial}{\partial x} M(x)=-\left(2l+1+\frac{r\alpha^2}{2m}\frac{\partial V}{\partial r}\right)M(x)+\frac{r\alpha^2}{2m}\frac{\partial V}{\partial r}(l+1+\kappa)L(x)+2mr^2(v(r)-\varepsilon)L(x).$$

In the above transformation, we used $\kappa(\kappa+1)=l(l+1)$ because $\kappa = -(l+1),\ l$.

## Transformed Scalar Dirac equation
In the case of the scalar relativistic Dirac equation, the $\kappa$ dependence is averaged.
Since $\langle\kappa\rangle=[-(l+1)(2l+2)+l\cdot 2l]/(4l+2)=-1$, the $l+1+\kappa$ part in the second term in the RHS is changed to $l$.

# Asymptotic forms
When we solve the Schroedinger or Dirac equations, we need to set the boundary correction at $r\rightarrow 0$ or $r\rightarrow \infty$.
The ODEs can be represented by
$$\left[ \frac{\partial^2}{\partial r^2}+C_1\frac{\partial}{\partial r}+C_0\right]P_{nl}=0,$$
$$\text{Schroedinger:}\ C_1 = 0,\ C_0 = -\frac{l(l+1)}{r^2}+2(\varepsilon-V(r)),$$
$$\text{Dirac:}\ C_1 = \frac{\alpha^2}{2m}\frac{\partial V}{\partial r},\ C_2 = \frac{\alpha^2}{2m} \frac{\partial V}{\partial r}\frac{\kappa}{r}-\frac{l(l+1)}{r^2}+2m(\varepsilon-V(r)).$$
The scalar Dirac equation can be obtained by replacing $\kappa$ by $-1$.

The potential $V(r)$ and the wave function $P_{nl}(r)$ is approximated to a polynomial:
$$V(r) = \sum_{\nu =0} A_\nu r^\nu,\ P_{nl}(r) = \sum_{\nu=0}B_\nu r^{\nu+l+1}\ \left(L(r)=\sum_{\nu = 0} B_\nu r^\nu \right).$$
Inserting them, we get
$$\sum_{\nu = 0}(\nu+l)(\nu+l+1)B_\nu r^{\nu+l-1}-l(l+1) \sum_{\nu = 0} B_\nu r^{\nu+l-1}+2\left[\varepsilon - \sum_{\nu=0}A_\nu r^\nu\right]\sum_{\nu = 0}B_\nu r^{\nu+l+1} =0$$
for the Schroedinger equation.
Comparing the power series, we get
$$r^{l-1}:\ l(l+1)B_0 - l(l+1)B_0 =0\ \therefore B_0=\text{arbitrary}$$
$$r^{l}:\ (l+1)(l+2)B_1 - l(l+1)B_1 =0\ \therefore B_1=0$$
$$r^{l+1}: (l+2)(l+3)B_2 - l(l+1) B_2 +2(\varepsilon-A_0)B_0= 0\ \therefore B_2 = B_0 \frac{A_0-\varepsilon}{2l+3}$$
$$r^{l+2}: (l+3)(l+4) B_3 -l(l+1) B_3 +2(\varepsilon-A_0)B_1-2A_1B_0 = 0\ \therefore B_3 = B_0\frac{A_1}{3l+6}$$
$$r^{l+3}: (l+4)(l+5)B_4 - l(l+1) B_4 + 2(\varepsilon-A_0)B_2 -2A_1 B_1 + 2A_2 B_0 = 0\ \therefore B_4 = \frac{(A_0-\varepsilon) B_2 +A_2B_0}{4l+10}.$$

The Dirac equation is transformed to 
$$\sum_{\nu = 0}(\nu+l)(\nu+l+1)B_\nu r^{\nu+l-1}$$
$$+\frac{\alpha^2}{2m}\sum_{\nu=0}(\nu+1)A_{\nu+1}r^\nu\sum_{\nu=0}(\nu+l+\kappa+1)B_\nu r^{\nu+l}$$
$$-l(l+1) \sum_{\nu = 0} B_\nu r^{\nu+l-1}+2m\left[\varepsilon - \sum_{\nu=0}A_\nu r^\nu\right]\sum_{\nu = 0}B_\nu r^{\nu+l+1} =0.$$
Therefore, the solution is

$$r^{l-1}:\ l(l+1)B_0 - l(l+1)B_0 =0\ \therefore B_0=\text{arbitrary}$$
$$r^{l}:\ (l+1)(l+2)B_1 + \frac{\alpha^2}{2m}A_1(l+\kappa+1)B_0 - l(l+1)B_1 =0\ \therefore B_1=-\frac{\alpha^2}{2m} \frac{A_1(l+\kappa+1)}{2l+2}$$
$$r^{l+1}: (l+2)(l+3)B_2 +\frac{\alpha^2}{2m}(2A_2(l+\kappa+1)+A_1(l+\kappa+2))- l(l+1) B_2 +2m(\varepsilon-A_0)B_0= 0\ \therefore B_2 = mB_0 \frac{A_0-\varepsilon}{2l+3}-\frac{\alpha^2}{2m}\frac{2A_2(l+\kappa+1)+A_1(l+\kappa+2)}{4l+6}$$
$$r^{l+2}: (l+3)(l+4) B_3 +\frac{\alpha^2}{2m} \sum_{\nu=1}^3 \nu A_\nu(l+\kappa+4-\nu)-l(l+1) B_3 +2m(\varepsilon-A_0)B_1-2mA_1B_0 = 0$$
$$\therefore B_3 = mB_0\frac{A_1}{3l+6}-\frac{\alpha^2}{2m}\frac{\sum_{\nu=1}^3 \nu A_\nu(l+\kappa+4-\nu)}{6l+12}$$
$$r^{l+3}: (l+4)(l+5)B_4 +\frac{\alpha^2}{2m}\sum_{\nu=1}^4 \nu A_\nu (l+\kappa+5-\nu)- l(l+1) B_4 + 2m(\varepsilon-A_0)B_2 -2mA_1 B_1 + 2mA_2 B_0 = 0$$
$$\therefore B_4 = m\frac{(A_0-\varepsilon) B_2 +A_2B_0}{4l+10}-\frac{\alpha^2}{2m}\frac{\sum_{\nu=1}^4 \nu A_\nu (l+\kappa+5-\nu)}{8l+20}.$$

Next is the asymptotic form at $r\rightarrow\infty$.
Since the $V(r)$ and $r^{-n}$ terms vanish, the asymptotic equation is
$$\left[\frac{\partial^2}{\partial r^2} +2m\varepsilon\right] P_{nl}(r)=0\ (m=1\ \text{for Schroedinger equation})$$
Therefore,
$$P_{nl}(r)\rightarrow \exp(-\sqrt{2m\varepsilon}\cdot r).$$

## Potential fitting
We need to determine the polynomial coefficient $A_i$.
Here we use $n-1$-th order polynomial to determine $A_i\ (i=0,\ \cdots, n-1)$.
See [Fit_potential.cpp](../cpp/Fit_potential.cpp) for the implementation.

Given the first $n$ points $(r_i, V_i)$ and the polynomial
$$V(r) = \sum_{\nu = 0}^{n-1} A_i r_i^\nu,$$
The polynomial coefficients $A_i$ satisfies
$$\begin{pmatrix}1 & r_0 & \cdots & r_0^{n-1} \\
1 & r_1 & \cdots & r_1^{n-1} \\
\vdots & \vdots & \ddots & \vdots \\
1 & r_{n-1} & \cdots & r_{n-1}^{n-1} \end{pmatrix}\begin{pmatrix}A_0 \\ A_1 \\ \vdots \\ A_{n-1}\end{pmatrix} = \begin{pmatrix}V_0 \\ V_1 \\ \vdots \\ V_{n-1} \end{pmatrix}.$$
This linear equation can be solved by the `dgesv` routine in LAPACK.