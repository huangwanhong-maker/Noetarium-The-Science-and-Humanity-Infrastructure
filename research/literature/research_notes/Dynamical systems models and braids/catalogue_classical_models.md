# Catalogue of Canonical Deterministic Dynamical-System Models with Checkable Reference Values (state of knowledge: November 2026)

Provenance marks used on every line of the findings:

- **[V]** — the number or statement was read in this session (2026-11-12) at the linked page.
- **[M]** — recalled from the standard literature; the primary source is named and linked (DOI links written from memory, not resolved in this session). Treat as "to be re-checked against the primary source before it becomes a fixture".
- **[D]** — derived here by elementary arithmetic from the equations (always placed under *Inferences*).

Access notes for this session: Scholarpedia could not be read (its TLS certificate had expired on 2026-11-12; web.archive.org is not reachable from the fetch tool), so no value below rests on Scholarpedia. Wolf et al. (1985), Ghia et al. (1982), Chandrasekhar (1961) and Sprott's book appendix were not readable in the original; where their numbers appear they are marked [M] or come through a named secondary transcription.

All Lyapunov exponents are in base e (nats per iteration or per time unit) unless "bits" is written.

## 1. Low-dimensional maps

### Takeaway
The maps have exact or very well converged reference values (Feigenbaum constants, ln 2 exponents, Hénon 0.41922/−1.62319, Greene's K_c = 0.971635…), and nearly all of them admit an exact internal check: the sum of the exponents equals the logarithm of the Jacobian determinant. The form of the map (Hénon's 1 − a x² + y against a − x² + b y; standard map mod 2π against mod 1) must be fixed before any number is compared.

### Cited Findings

**Logistic map** x' = r x (1 − x)

- Period-doubling points r_n (period 2ⁿ born): r₁ = 3, r₂ = 1 + √6 = 3.4494897, r₃ = 3.5440903, r₄ = 3.5644073, r₅ = 3.5687594, r₆ = 3.5696916, r₇ = 3.5698913, r₈ = 3.5699340 (Alligood, Sauer and Yorke 1996, p. 504) [V] — [Wikipedia, Feigenbaum constants](https://en.wikipedia.org/wiki/Feigenbaum_constants)
- Same cascade for the quadratic map x' = a − x²: a = 0.75, 1.25, 1.3680989, 1.3940462, 1.3996312, 1.4008286, 1.4010853, 1.4011402; accumulation ("Feigenbaum point") at c = −1.4011551890… for z² + c (Alligood et al., p. 503) [V] — [Wikipedia, Feigenbaum constants](https://en.wikipedia.org/wiki/Feigenbaum_constants)
- Feigenbaum constants δ = 4.669201609102990671853203820466…, α = 2.502907875095892822283902873218… (OEIS A006890, A006891; Briggs 1991; Broadhurst 1999) [V] — [Wikipedia, Feigenbaum constants](https://en.wikipedia.org/wiki/Feigenbaum_constants)
- Accumulation point of the logistic map r∞ ≈ 3.56995 (the page gives 3.56994… and 3.56995); period-3 window opens at r = 1 + √8 = 3.828427…; fractal dimension of the Feigenbaum attractor "approximately 0.54" [V] — [Wikipedia, Logistic map](https://en.wikipedia.org/wiki/Logistic_map)
- r∞ = 3.5699456718…; Hausdorff dimension of the Feigenbaum attractor 0.538… (Grassberger 1981); superstable parameters R₁ = 2, R₂ = 1 + √5 = 3.236068, R₃ = 3.498562, R₄ = 3.554641 [M] — [Feigenbaum 1978, J. Stat. Phys. 19, 25](https://doi.org/10.1007/BF01020332)
- At r = 4: Lyapunov exponent λ = ln 2 = 0.693147181…; Kaplan–Yorke dimension 1.0 exactly; correlation dimension 1.0 exactly, "converges slowly" [V] — [Sprott, Common chaotic systems](https://sprott.physics.wisc.edu/chaos/comchaos.htm)
- At r = 4: invariant density ρ(x) = 1/(π √(x(1 − x))); the map is a nonlinear transformation of the bit shift and of the tent map at μ = 2 [V] — [Wikipedia, Logistic map](https://en.wikipedia.org/wiki/Logistic_map)
- At r = 4: exact solution x_n = sin²(2ⁿ θ₀), x₀ = sin² θ₀; topological entropy ln 2. Kneading theory: the order of the periodic windows is the universal (MSS) sequence, the same for every unimodal map [M] — [Metropolis, Stein and Stein 1973, J. Combin. Theory A 15, 25](https://doi.org/10.1016/0097-3165(73)90033-2)

**Tent map** x' = μ min(x, 1 − x)

- λ = ln μ for every orbit not hitting the corner, topological entropy ln μ for 1 < μ ≤ 2; at μ = 2 conjugate to the logistic map at r = 4. In binary floating point the μ = 2 map collapses every orbit to 0 after about 53 iterations, so the check must be made at μ slightly below 2 or in exact arithmetic [M] — [Wikipedia, Tent map](https://en.wikipedia.org/wiki/Tent_map)
- The beta transformation x' = βx mod 1 has λ = ln β (docstring citing Ott 2002) [V] — [PredefinedDynamicalSystems.jl documentation](https://juliadynamics.github.io/PredefinedDynamicalSystems.jl/dev/)

**Hénon map** x' = 1 − a x² + y, y' = b x; a = 1.4, b = 0.3 (Hénon 1976)

- Lyapunov exponents 0.41922, −1.62319; D_KY = 1.25827; correlation dimension 1.220 ± 0.036 [V] — [Sprott, Common chaotic systems](https://sprott.physics.wisc.edu/chaos/comchaos.htm)
- Independent computation by Wolf's algorithm ported by Sprott: 0.419217, −1.623190; D_KY = 1.258267 [V] — [Sprott, Lyapunov Exponent Spectrum Software](https://sprott.physics.wisc.edu/chaos/lespec.htm)
- Jacobian determinant −b; fixed points (0.631, 0.189) (saddle) and (−1.131, −0.339); correlation dimension 1.21 ± 0.01 (Grassberger and Procaccia 1983); box-counting dimension 1.261 ± 0.003 (Russell, Hanson and Ott 1980) [V] — [Wikipedia, Hénon map](https://en.wikipedia.org/wiki/H%C3%A9non_map)
- Wolf et al. (1985, Table I) quote the same map in bits per iteration: λ₁ = 0.603, λ₂ = −2.34, Lyapunov dimension 1.26 [M] — [Wolf, Swift, Swinney and Vastano 1985, Physica D 16, 285](https://doi.org/10.1016/0167-2789(85)90011-9)
- For b = 0.3 the cascade of period doublings accumulates near a ≈ 1.06 (Hénon 1976); a strange attractor is proved for a positive-measure set of parameters with small b (Benedicks and Carleson 1991), not for (1.4, 0.3) [M] — [Hénon 1976, Commun. Math. Phys. 50, 69](https://doi.org/10.1007/BF01608556)

**Lozi map** x' = 1 − a|x| + y, y' = b x; a = 1.7, b = 0.5 (Lozi 1978)

- Lyapunov exponents 0.47023, −1.16338; D_KY = 1.40419 (Sprott 2003, Appendix A); a hyperbolic strange attractor is proved (Misiurewicz 1980) [M] — [Sprott, Chaos and Time-Series Analysis (book site)](https://sprott.physics.wisc.edu/chaostsa/)

**Ikeda map** x' = γ + μ(x cos φ − y sin φ), y' = μ(x sin φ + y cos φ), φ = β − α/(1 + x² + y²); α = 6, β = 0.4, γ = 1, μ = 0.9

- Lyapunov exponents 0.50760, −0.71832; D_KY = 1.70665 (Sprott 2003, Appendix A) [M] — [Sprott, Chaos and Time-Series Analysis (book site)](https://sprott.physics.wisc.edu/chaostsa/)
- dysts ships the Ikeda map with the single parameter u = 0.9 and no reference exponents [V] — [dysts, discrete_maps.json](https://raw.githubusercontent.com/williamgilpin/dysts/master/dysts/data/discrete_maps.json)

**Standard (Chirikov) map** p' = p + K sin θ, θ' = θ + p', both mod 2π

- Form of the map and of the circle map θ' = θ + Ω − K sin θ [V] — [Wikipedia, Standard map](https://en.wikipedia.org/wiki/Standard_map)
- At K = 1 (chaotic sea): Lyapunov exponents 0.10497, −0.10497; D_KY = 2.0 exactly; correlation dimension 1.954 ± 0.077 [V] — [Sprott, Common chaotic systems](https://sprott.physics.wisc.edu/chaos/comchaos.htm)
- The last (golden-mean) invariant circle breaks at K_c = 0.971635 (Greene 1979); this value is the default parameter of the shipped system [V] — [PredefinedDynamicalSystems.jl documentation](https://juliadynamics.github.io/PredefinedDynamicalSystems.jl/dev/)
- K_c = 0.971635406…; for large K the exponent of the chaotic sea is ≈ ln(K/2) (Chirikov 1979) [M] — [Greene 1979, J. Math. Phys. 20, 1183](https://doi.org/10.1063/1.524170)

**Arnold cat map** (x, y) ↦ (2x + y, x + y) mod 1

- Matrix [[2, 1], [1, 1]]; number of points of period n is |λ₁ⁿ + λ₂ⁿ − 2| = 1, 5, 16, 45, 121, 320, 841, 2205, … (OEIS A004146) [V] — [Wikipedia, Arnold's cat map](https://en.wikipedia.org/wiki/Arnold%27s_cat_map)

**Circle map** θ' = θ + Ω − (K/2π) sin 2πθ (mod 1)

- Critical line K = 1: mode-locked intervals have full measure and their complement has fractal dimension D ≈ 0.87 (Jensen, Bak and Bohr 1984); golden-mean scaling constants at K = 1 are δ = −2.83361…, α = −1.28857… (Shenker 1982) [M] — [Jensen, Bak and Bohr 1984, Phys. Rev. A 30, 1960](https://doi.org/10.1103/PhysRevA.30.1960)

### Inferences
- [D] Exact internal checks, independent of any published table: Σλ = ln|det J|. Hénon: 0.41922 − 1.62319 = −1.20397 = ln 0.3. Lozi: 0.47023 − 1.16338 = −0.69315 = ln 0.5. Ikeda: 0.50760 − 0.71832 = −0.21072 = 2 ln 0.9. Standard and cat maps: 0. The [M] values for Lozi and Ikeda pass this check to five decimals, which supports the recalled figures though it does not replace reading the appendix.
- [D] Cat map: eigenvalues (3 ± √5)/2, so λ = ± ln((3 + √5)/2) = ± 0.962424, equal to its topological entropy; the baker map (x, y) ↦ (2x mod 1, (y + ⌊2x⌋)/2) has λ = ± ln 2. Hénon fixed point is stable for a < 3(1 − b)²/4 = 0.3675 at b = 0.3 and exists for a > −(1 − b)²/4 = −0.1225.
- The best template checks for maps are of three kinds: exact (ln 2, ln μ, ln β, periodic-point counts of the cat map, determinant sums), high-precision constants (δ, α, r_n, K_c), and estimated (correlation dimensions, which converge slowly and carry ±0.04 to ±0.08 in Sprott's own table).
- The standard map's exponent at K = 1 is a property of one chaotic component, not of the map: an implementation check must fix the initial condition or average over the connected chaotic sea.

### Gaps
- Sprott's Appendix A (62 cases) was not read directly; the Lozi and Ikeda values are from memory. Wolf et al.'s Table I was not readable (the hosted PDF redirected).
- Greene's K_c beyond six digits, the large-K entropy formula, and the cat map's exponent are not confirmed on a page read in this session (Scholarpedia's Chirikov article was unreachable).
- Kneading data: no numeric kneading invariants were verified; only the existence and universality of the MSS ordering is recorded.
- Topological entropy of the Hénon map at (1.4, 0.3) (≈ 0.4651 in the literature, Newhouse and Pignataro; Galias) was not verified.

## 2. Low-dimensional flows

### Takeaway
Lorenz 63 and Rössler have reference spectra that agree across independent sources to three or four digits when the parameters match (Lorenz 0.9056, 0, −14.5723 at σ = 10, r = 28, b = 8/3; Rössler 0.0714, 0, −5.3943 at a = b = 0.2, c = 5.7), and the bifurcation values of Lorenz (1, 13.926, 24.06, 24.74) and the Kuramoto threshold K_c = 2/(π g(0)) are firm. For the conservative systems (Hénon–Heiles, the double pendulum, the three-body problem) the checkable quantities are integrals of motion and special orbits, not spectra.

### Cited Findings

**Lorenz 63** ẋ = σ(y − x), ẏ = x(r − z) − y, ż = xy − bz; σ = 10, r = 28, b = 8/3

- Lyapunov exponents 0.9056, 0, −14.5723; D_KY = 2.06215; correlation dimension 2.068 ± 0.086; "the least significant digit is only a best estimate" [V] — [Sprott, Common chaotic systems](https://sprott.physics.wisc.edu/chaos/comchaos.htm)
- Second computation (Wolf's algorithm): 0.90563, 0, −14.57219; D_KY = 2.06215 [V] — [Sprott, Lyapunov Exponent Spectrum Software](https://sprott.physics.wisc.edu/chaos/lespec.htm)
- dysts stores, with β written as 2.667 and not 8/3: [0.90504, −0.00045, −14.57160], D_KY = 2.06208, correlation dimension 1.994, period 1.5008 [V] — [dysts, chaotic_attractors.json](https://raw.githubusercontent.com/williamgilpin/dysts/master/dysts/data/chaotic_attractors.json)
- Pitchfork at r = 1; the two outer equilibria lose stability in a subcritical Hopf bifurcation at r = σ(σ + b + 3)/(σ − b − 1) ≈ 24.74 (Hirsch, Smale and Devaney); correlation dimension 2.05 ± 0.01 (Grassberger and Procaccia 1983); Lyapunov dimension 2.06 ± 0.01; the attractor's existence proved by Tucker (2002) [V] — [Wikipedia, Lorenz system](https://en.wikipedia.org/wiki/Lorenz_system)
- Homoclinic bifurcation at r = 13.926, where a strange invariant set (not yet an attractor) is born [V] — [Symbolic dynamics analysis of the Lorenz equations, arXiv chao-dyn/9710014](https://arxiv.org/pdf/chao-dyn/9710014)
- The chaotic set becomes an attractor at r_A ≈ 24.06 (boundary crisis); the two symmetric subcritical Hopf bifurcations follow at 24.74 [V] — [Resonances in a Chaotic Attractor Crisis of the Lorenz Flow, arXiv 1705.08178](https://arxiv.org/pdf/1705.08178)
- Wolf et al. used σ = 16, r = 45.92, b = 4 and logarithms to base 2, not the standard parameters [V] — [Sprott, Lyapunov Exponent Spectrum Software](https://sprott.physics.wisc.edu/chaos/lespec.htm)
- Wolf et al.'s values at those parameters: 2.16, 0, −32.4 bits/s; Lyapunov dimension 2.07 [M] — [Wolf et al. 1985](https://doi.org/10.1016/0167-2789(85)90011-9)

**Rössler** ẋ = −y − z, ẏ = x + a y, ż = b + z(x − c)

- a = b = 0.2, c = 5.7: Lyapunov exponents 0.0714, 0, −5.3943; D_KY = 2.0132; correlation dimension 1.991 ± 0.065 ("converges slowly") [V] — [Sprott, Common chaotic systems](https://sprott.physics.wisc.edu/chaos/comchaos.htm)
- dysts at the same parameters: [0.07120, −0.0000154, −5.39373], D_KY = 2.01320, correlation dimension 1.797 [V] — [dysts, chaotic_attractors.json](https://raw.githubusercontent.com/williamgilpin/dysts/master/dysts/data/chaotic_attractors.json)
- Two parameter sets are in common use: Rössler's (0.2, 0.2, 5.7) and (0.1, 0.1, 14). Eigenvalues at (0.2, 0.2, 5.7): central point 0.0971 ± 0.9958i, −5.687; outer point −0.0000046 ± 5.428i, 0.1930. With a = b = 0.1: c = 4 period 1, c = 6 period 2, c = 8.5 period 4, c = 8.7 period 8, c = 9 chaos, c = 12 period 3, c = 12.6 period 6, c = 13 chaos, c = 15.4 period 5, c = 18 chaos [V] — [Wikipedia, Rössler attractor](https://en.wikipedia.org/wiki/R%C3%B6ssler_attractor)
- Wolf et al. used a third set, a = 0.15, b = 0.20, c = 10.0, and report 0.13, 0, −14.1 bits/s (0.090, 0, −9.8 in nats), Lyapunov dimension 2.01; for Rössler's hyperchaos (1979) they report 0.16, 0.03, 0, −39.0 bits/s, dimension 3.005 [M] — [Wolf et al. 1985](https://doi.org/10.1016/0167-2789(85)90011-9)

**Chua's circuit** ẋ = α(y − x − f(x)), ẏ = x − y + z, ż = −β y, f(x) = m₁x + ½(m₀ − m₁)(|x + 1| − |x − 1|)

- dysts: α = 15.6, β = 28, m₀ = −1.142857, m₁ = −0.71429; estimated spectrum [0.42938, −0.00062, −4.23150], D_KY = 2.10133, correlation dimension 1.799 [V] — [dysts, chaotic_attractors.json](https://raw.githubusercontent.com/williamgilpin/dysts/master/dysts/data/chaotic_attractors.json)
- DynamicalSystems.jl: a = 15.6, m₀ = −8/7, m₁ = −5/7, with period doubling as b runs from 25 to 51 (Chua 1992, 2007) [V] — [PredefinedDynamicalSystems.jl documentation](https://juliadynamics.github.io/PredefinedDynamicalSystems.jl/dev/)
- The double scroll of Matsumoto, Chua and Komuro (1985) is at α = 9, β = 100/7, m₀ = −8/7, m₁ = −5/7, with exponents about 0.23, 0, −1.78 and Lyapunov dimension 2.13 [M] — [Matsumoto, Chua and Komuro 1985, IEEE Trans. CAS 32, 797](https://doi.org/10.1109/TCS.1985.1085791)

**Duffing and Ueda** ẍ + kẋ + x³ = B cos t

- Ueda's attractor at k = 0.05, B = 7.5: Lyapunov exponents 0.1034, 0, −0.1534; D_KY = 2.6741; correlation dimension 2.675 ± 0.132 [V] — [Sprott, Common chaotic systems](https://sprott.physics.wisc.edu/chaos/comchaos.htm)
- k = 0.1, B = 12 gives the "broken-egg" attractor (docstring citing Ruelle 1980) [V] — [PredefinedDynamicalSystems.jl documentation](https://juliadynamics.github.io/PredefinedDynamicalSystems.jl/dev/)
- Double-well Duffing ẍ = x − x³ + ε(γ cos ωt − δẋ): the stable and unstable manifolds of the saddle intersect for γ/δ > 4 cosh(πω/2)/(3√2 πω) (Melnikov's criterion) [M] — [Guckenheimer and Holmes 1983, §4.5](https://doi.org/10.1007/978-1-4612-1140-2)

**Sprott's simplest flows**

- Simplest quadratic jerk flow x⃛ = −A ẍ + ẋ² − x at A = 2.017: 0.0551, 0, −2.0721; D_KY = 2.0266. Simplest piecewise-linear x⃛ = −A ẍ − ẋ − |x| + 1 at A = 0.6: 0.0362, 0, −0.6362; D_KY = 2.0569 [V] — [Sprott, Common chaotic systems](https://sprott.physics.wisc.edu/chaos/comchaos.htm)

**van der Pol** ẍ − μ(1 − x²)ẋ + x = 0

- Limit cycle of amplitude ≈ 2 for small μ with period 2π(1 + μ²/16 + …); for large μ the period is ≈ μ(3 − 2 ln 2) + 7.014 μ^(−1/3) [M] — [Wikipedia, Van der Pol oscillator](https://en.wikipedia.org/wiki/Van_der_Pol_oscillator)

**Lotka–Volterra** ẋ = αx − βxy, ẏ = δxy − γy

- First integral V = δx − γ ln x + βy − α ln y; period of small oscillations 2π/√(αγ) [M] — [Wikipedia, Lotka–Volterra equations](https://en.wikipedia.org/wiki/Lotka%E2%80%93Volterra_equations)

**Brusselator** Ẋ = A + X²Y − (B + 1)X, Ẏ = BX − X²Y

- Fixed point (A, B/A); it loses stability (Hopf) at B = 1 + A² (Prigogine and Lefever 1968) [V] — [Wikipedia, Brusselator](https://en.wikipedia.org/wiki/Brusselator)

**Lorenz 96** ẋ_j = (x_{j+1} − x_{j−2}) x_{j−1} − x_j + F, cyclic in j

- Form, N ≥ 4, F = 8 "a common value known to cause chaotic behavior" [V] — [Wikipedia, Lorenz 96 model](https://en.wikipedia.org/wiki/Lorenz_96_model)
- Primary values for J = 40, F = 8 (time unit 5 days, step 0.05 = 6 h, fourth-order Runge–Kutta): the steady state X_j = F is unstable for F > 8/9, first to waves of length 2π/cos⁻¹(1/4) = 4.77 zones (wavenumber 8 at J = 40); mean X = 2.3, standard deviation 3.6; the leading exponent corresponds to a doubling time of 0.42 units (2.1 days); 13 positive exponents; Kaplan–Yorke dimension about 27.1. At F = 10: doubling time 1.5 days, 14 positive exponents, dimension about 29.4 [V] — [Lorenz and Emanuel 1998, J. Atmos. Sci. 55, 399 (PDF)](https://texmex.mit.edu/ftp/pub/emanuel/PAPERS/lorenz98.pdf)
- dysts ships a 4-variable version at F = 20, spectrum [1.20836, −0.00011, −1.58404, −3.62421], D_KY = 2.7628, described as "containing … multiscale coupling of fast and slow variables", which is the two-scale model, not this one [V] — [dysts, chaotic_attractors.json](https://raw.githubusercontent.com/williamgilpin/dysts/master/dysts/data/chaotic_attractors.json)

**Hénon–Heiles** H = ½(p_x² + p_y²) + ½(x² + y²) + λ(x²y − y³/3), λ = 1

- Equations ẋ = p_x, ṗ_x = −x − 2λxy, ẏ = p_y, ṗ_y = −y − λ(x² − y²) [V] — [Wikipedia, Hénon–Heiles system](https://en.wikipedia.org/wiki/H%C3%A9non%E2%80%93Heiles_system)
- Escape energy E = 1/6; orbits essentially all regular at E = 1/12, mixed at E = 1/8, mostly chaotic at E = 1/6 [M] — [Hénon and Heiles 1964, Astron. J. 69, 73](https://doi.org/10.1086/109234)
- dysts spectrum [0.11497, 0.00096, −0.00086, −0.11506] at an unstated energy [V] — [dysts, chaotic_attractors.json](https://raw.githubusercontent.com/williamgilpin/dysts/master/dysts/data/chaotic_attractors.json)

**Restricted three-body problem** (rotating frame) ẍ − 2ẏ = Ω_x, ÿ + 2ẋ = Ω_y, Ω = ½(x² + y²) + (1 − μ)/r₁ + μ/r₂

- Jacobi integral C = 2Ω − ẋ² − ẏ²; the triangular points are linearly stable for μ < μ_Routh = ½(1 − √69/9) = 0.0385209. The Arenstorf orbit is a standard integrator test: μ = 0.012277471, x(0) = 0.994, ẏ(0) = −2.00158510637908252240537862224, period 17.0652165601579625588917206249 [M] — [Hairer, Nørsett and Wanner, Solving Ordinary Differential Equations I, §II.0](https://doi.org/10.1007/978-3-540-78862-1)

**Kuramoto model** θ̇_i = ω_i + (K/N) Σ_j sin(θ_j − θ_i), r e^{iψ} = (1/N) Σ_j e^{iθ_j}

- Critical coupling for unimodal symmetric g(ω): K_c = 2/(π g(0)); r = 0 for K < K_c in the limit N → ∞ (Kuramoto 1975, 1984; Strogatz 2000) [V] — [Wikipedia, Kuramoto model](https://en.wikipedia.org/wiki/Kuramoto_model)
- Lorentzian g of half-width Δ: K_c = 2Δ, and beyond a pitchfork R = √(1 − 2Δ/K) is the stable branch (from the Ott–Antonsen equation) [V] — [Bick, Goodfellow, Laing and Martens 2020, review, arXiv 1902.05307](https://arxiv.org/pdf/1902.05307)
- The Lorentzian case has an exact closed-form time evolution, Ṙ = −ΔR + (K/2)R(1 − R²) [M] — [Ott and Antonsen 2008, Chaos 18, 037113](https://doi.org/10.1063/1.2930766)

**Double pendulum**

- Shipped in both dysts and DynamicalSystems.jl with no reference values [V] — [PredefinedDynamicalSystems.jl documentation](https://juliadynamics.github.io/PredefinedDynamicalSystems.jl/dev/)

### Inferences
- [D] Σλ equals the mean divergence, which is constant for several systems and gives an exact check: Lorenz −(σ + 1 + b) = −13.6667 (0.9056 − 14.5723 = −13.6667); Ueda −k = −0.05 (0.1034 − 0.1534); the jerk flows −A (−2.0170, −0.6000); Lorenz 96 −N exactly (dysts' four-variable spectrum sums to −4.0000; for J = 40 the forty exponents must sum to −40).
- [D] Lorenz Hopf value: σ(σ + b + 3)/(σ − b − 1) = 470/19 = 24.7368 at σ = 10, b = 8/3. Lorenz 96 leading exponent from the doubling time: ln 2/0.42 ≈ 1.65 per unit (the paper gives the doubling time, rounded to two digits, not λ₁; the value 1.68 quoted in later literature is not verified here).
- [D] Kuramoto with Gaussian g of standard deviation σ: K_c = 2/(π g(0)) = σ√(8/π) ≈ 1.5958 σ.
- dysts' Lorenz differs from Sprott's in the fourth digit (0.9050 against 0.9056) and uses β = 2.667; its correlation dimensions (1.99 Lorenz, 1.80 Rössler) are below the accepted values (2.05, 1.99). Its spectra are usable as rough sanity values at its own parameters, not as fixtures.
- Three Rössler parameter sets circulate — (0.2, 0.2, 5.7), (0.1, 0.1, 14), (0.15, 0.2, 10) — and that, with bits against nats, accounts for the "different Rössler exponents" (0.0714, 0.090, 0.13).
- For Hamiltonian templates the checks should be: conservation of H (and of C for the three-body problem) to rounding under a symplectic integrator, the ± pairing of exponents, closure of the Arenstorf orbit, and the classification of sections at E = 1/12, 1/8, 1/6.

### Gaps
- Wolf et al.'s Table I and Matsumoto–Chua–Komuro's exponents are from memory.
- No authoritative Lyapunov spectrum for Chua at (15.6, 28) was found beyond dysts' automatic estimate.
- Kuramoto finite-N corrections and the near-threshold square-root law with its coefficient (Strogatz 2000) were not read; Wikipedia's rendering of it was garbled.
- Double pendulum: no canonical Lyapunov value exists (it depends on energy and on the mass and length ratios); only the small-angle normal modes and energy conservation are available as checks, and these were not sourced in this session.
- The homoclinic value for Rössler (Shilnikov) and Hopf values for Chua were not collected.
- van der Pol, Lotka–Volterra and three-body values are [M].

## 3. Partial differential equations

### Takeaway
The integrable and linear equations give exact solutions to check against (solitons, kinks, Cole–Hopf, the heat kernel); the pattern-forming ones give exact linear thresholds (most unstable wavenumbers, Turing and Hopf conditions, front speeds); and for chaos in a PDE the one well-tabulated case is Kuramoto–Sivashinsky, whose spectrum and dimension as functions of the domain length are published with tables (D_KY ≈ 0.226 L − 0.160 for periodic boundaries).

### Cited Findings

**Kuramoto–Sivashinsky** u_t + u_xx + u_xxxx + u u_x = 0 on [0, L]

- Forms: u_t + u_xx + u_xxxx + ½u_x² = 0 (integral) and v_t + v_xx + v_xxxx + v v_x = 0 (derivative, v = u_x); chaos reached through period doublings as L grows (Papageorgiou and Smyrlis 1991); a relative periodic orbit at L = 35 has three unstable and three marginal directions [V] — [Wikipedia, Kuramoto–Sivashinsky equation](https://en.wikipedia.org/wiki/Kuramoto%E2%80%93Sivashinsky_equation)
- Edson, Bunder, Mattner and Roberts (2019), derivative form, periodic boundaries (no zero-mean condition imposed, so Galilean and translation invariance give two zero exponents) and odd-periodic boundaries (u = u_xx = 0 at the ends): travelling wave for L ≲ 13, spatio-temporal chaos by L ≈ 100; all exponents bounded above by about 0.1 for both boundary conditions, as Manneville (1985) and Yang et al. (2009) found for rigid boundaries [V] — [Edson et al., arXiv 1902.09651](https://arxiv.org/abs/1902.09651)
- Fit for the i-th exponent, periodic case: λ_i(L) ≈ 0.093 − 0.94(i − 0.39)/L (their eq. 5) [V] — [Edson et al., arXiv 1902.09651](https://arxiv.org/abs/1902.09651)
- Their Table 1, periodic boundaries, twelve largest exponents and D_KY [V] — [Edson et al., arXiv 1902.09651](https://arxiv.org/abs/1902.09651):

| i | L = 12 | L = 13.5 | L = 22 | L = 36 | L = 60 | L = 100 |
|---|---|---|---|---|---|---|
| 1 | 0.003 | 0.059 | 0.043 | 0.080 | 0.089 | 0.088 |
| 2 | −0.005 | 0.004 | 0.003 | 0.056 | 0.067 | 0.082 |
| 3 | −0.088 | −0.004 | 0.002 | 0.014 | 0.055 | 0.070 |
| 4 | −0.089 | −0.227 | −0.004 | 0.003 | 0.041 | 0.061 |
| 5 | −0.186 | −0.730 | −0.008 | −0.003 | 0.030 | 0.048 |
| 6 | −3.524 | −1.467 | −0.185 | −0.004 | 0.005 | 0.041 |
| 7 | −3.525 | −1.529 | −0.253 | −0.021 | 0.003 | 0.033 |
| 8 | −9.835 | −6.956 | −0.296 | −0.088 | 0.000 | 0.028 |
| 9 | −9.849 | −6.963 | −0.309 | −0.160 | −0.004 | 0.018 |
| 10 | −9.959 | −7.977 | −1.965 | −0.224 | −0.009 | 0.012 |
| 11 | −10.01 | −7.993 | −1.967 | −0.309 | −0.029 | 0.005 |
| 12 | −10.12 | −9.199 | −5.599 | −0.373 | −0.066 | 0.003 |
| D_KY | 1.663 | 3.259 | 5.198 | 8.229 | 13.56 | 22.44 |

- D_KY ≈ 0.226 L − 0.160 (periodic) and 0.226 L − 2.106 (odd-periodic), most accurate for L ≳ 80; Manneville (1985) and Tajima and Greenside (2002) found 0.230 L for rigid boundaries, 50 < L < 400; odd-periodic D_KY: 0.000 (L = 17.5), 1.081 (18.1), 2.482 (18.2), 7.056 (41), 11.35 (60), 20.75 (100); the L = 22 exponents agree with Cvitanović, Davidchack and Siminos (2010) "to a difference of about 0.002" [V] — [Edson et al., arXiv 1902.09651](https://arxiv.org/abs/1902.09651)
- The paper appeared in ANZIAM Journal 61 (2019) 270–285 [M] — [Edson et al., arXiv 1902.09651](https://arxiv.org/abs/1902.09651)

**Gray–Scott** U_t = D_u ∇²U − UV² + F(1 − U), V_t = D_v ∇²V + UV² − (F + k)V

- Pearson's set-up: D_u = 2 × 10⁻⁵, D_v = 10⁻⁵, domain 2.5 × 2.5, periodic, 256 × 256 points, time step 1 (forward Euler; spot checks to 1024² and step 0.01); start from the trivial state U = 1, V = 0 (linearly stable for all F, k) with a central 20 × 20 block set to U = 1/2, V = 1/4 and ±1% noise; the saddle-node and Hopf curves meet at F = k = 0.0625 = 1/16, Turing patterns only in a narrow region near that point; the Hopf-born periodic orbit is stable for k < 0.035 and unstable above; pattern ε at F = 0.02, k = 0.059 [V] — [Pearson 1993, Science 261, 189 (arXiv patt-sol/9304003)](https://ar5iv.labs.arxiv.org/html/patt-sol/9304003)

**Brusselator with diffusion**, **Swift–Hohenberg**, **complex Ginzburg–Landau**

- Swift–Hohenberg u_t = r u − (1 + ∇²)²u − u³: growth rate σ(k) = r − (1 − k²)², onset at r = 0 with k_c = 1, a gradient flow; the Brusselator's Turing threshold is B_c = (1 + A√(D_X/D_Y))² with k_c² = A/√(D_X D_Y), against its Hopf threshold 1 + A² [M] — [Cross and Hohenberg 1993, Rev. Mod. Phys. 65, 851](https://doi.org/10.1103/RevModPhys.65.851)
- CGLE in the convention A_t = A + (1 + ib)∇²A − (1 + ic)|A|²A: plane waves A = √(1 − Q²) e^{i(Qx − ωt)} with ω = c + (b − c)Q²; all are unstable when 1 + bc < 0 (Benjamin–Feir–Newell); in Chaté's convention with −(1 − ic₃) the same line reads c₁c₃ = 1 [M] — [Aranson and Kramer 2002, Rev. Mod. Phys. 74, 99](https://doi.org/10.1103/RevModPhys.74.99)

**Burgers** u_t + u u_x = ν u_xx

- Linearised by Cole–Hopf, u = −2ν φ_x/φ with φ_t = ν φ_xx. Benchmark of Basdevant et al. (1986): u₀ = −sin πx on [−1, 1], ν = 0.01/π; max |u_x| at x = 0 is 152.00516, reached at πt = 1.6037 [M] — [Basdevant et al. 1986, Computers and Fluids 14, 23](https://doi.org/10.1016/0045-7930(86)90036-8)

**KdV** u_t + 6u u_x + u_xxx = 0

- Soliton u = (c/2) sech²(½√c (x − ct − x₀)); u₀ = N(N + 1) sech²x splits into N solitons; Zabusky and Kruskal's experiment: u_t + u u_x + δ²u_xxx = 0, δ = 0.022, u₀ = cos πx on [0, 2] periodic, breakdown time t_B = 1/π, recurrence near 30.4 t_B [M] — [Zabusky and Kruskal 1965, Phys. Rev. Lett. 15, 240](https://doi.org/10.1103/PhysRevLett.15.240)

**Nonlinear Schrödinger** iψ_t + ½ψ_xx + |ψ|²ψ = 0 (focusing)

- Soliton ψ = a sech(a(x − vt)) exp(i(vx + ½(a² − v²)t)); Peregrine solution ψ = [1 − 4(1 + 2it)/(1 + 4x² + 4t²)] e^{it}, peak three times the background; a plane wave of amplitude A is modulationally unstable for |k| < 2A with growth rate |k|√(A² − k²/4) [M] — [Wikipedia, Nonlinear Schrödinger equation](https://en.wikipedia.org/wiki/Nonlinear_Schr%C3%B6dinger_equation)

**sine-Gordon** φ_tt − φ_xx + sin φ = 0

- Kink φ = 4 arctan exp(±(x − vt)/√(1 − v²)) of energy 8/√(1 − v²); breather φ = 4 arctan[(√(1 − ω²)/ω) sin ωt / cosh(√(1 − ω²) x)]; linear dispersion ω² = 1 + k² [M] — [Wikipedia, Sine-Gordon equation](https://en.wikipedia.org/wiki/Sine-Gordon_equation)

**Fisher–KPP** u_t = D u_xx + r u(1 − u)

- Fronts from compact data travel at the minimal speed c* = 2√(rD); exact solution at c = 5/√6 (r = D = 1): u = [1 + C exp((x − 5t/√6)/√6)]⁻² (Ablowitz and Zeppetella 1979); the front lags 2t by (3/2) ln t (Bramson) [M] — [Ablowitz and Zeppetella 1979, Bull. Math. Biol. 41, 835](https://doi.org/10.1007/BF02462380)

**Allen–Cahn and Cahn–Hilliard**

- Allen–Cahn u_t = ε²u_xx + u − u³ has the kink tanh(x/(√2 ε)); the bistable equation u_t = u_xx + u(1 − u)(u − a) has the front speed (1 − 2a)/√2. Cahn–Hilliard c_t = ∇²(c³ − c − γ∇²c): growth rate about c = 0 is k²(1 − γk²), fastest at k = 1/√(2γ); mass is conserved, the free energy decreases, domains coarsen as t^{1/3} [M] — [Wikipedia, Cahn–Hilliard equation](https://en.wikipedia.org/wiki/Cahn%E2%80%93Hilliard_equation)

**FitzHugh–Nagumo** v̇ = v − v³/3 − w + I, ẇ = ε(v + a − b w) (+ D∇²v)

- FitzHugh's parameters a = 0.7, b = 0.8, ε = 0.08 [M] — [FitzHugh 1961, Biophys. J. 1, 445](https://doi.org/10.1016/S0006-3495(61)86902-6)

### Inferences
- [D] KS linear theory about u = 0: σ(k) = k² − k⁴, unstable band 0 < k < 1, fastest growth at k = 1/√2 with σ = 1/4; on a periodic domain the first mode becomes unstable at L = 2π. These are exact checks of a spectral implementation before any chaos is computed.
- [D] From Edson's fit, D_KY per unit length is 0.226 (periodic or odd-periodic) against 0.230 (rigid), a 2% difference; the positive exponents number roughly 0.1 L at L = 100 (twelve or more at L = 100 in Table 1). A template "KS at L = 22, periodic" should expect λ₁ ≈ 0.043–0.048, two zero exponents if the mean is free (one if the mean is fixed), and D_KY ≈ 5.2.
- [D] Gray–Scott steady states other than (1, 0): U = ½(1 ± √(1 − 4(F + k)²/F)), V = F(1 − U)/(F + k), existing when F ≥ 4(F + k)²; the saddle-node curve passes through F = k = 1/16, consistent with Pearson's statement.
- [D] Heat and wave equations supply exact modal checks: a Fourier mode of the heat equation decays as exp(−Dk²t); the wave equation's d'Alembert solution and ω = ck.
- The convention problem is acute for PDEs: KdV (coefficient 6 or 1, sign), NLS (½ψ_xx + |ψ|²ψ against ψ_xx + 2|ψ|²ψ), CGLE (signs of the imaginary coefficients), KS (integral or derivative form, L or L/2π as the parameter, mean fixed or free), Gray–Scott (k against F + k). A template must store the form with its values.

### Gaps
- Manneville (1985), Tajima and Greenside (2002) and Cvitanović et al. (2010) were not read; their numbers come through Edson et al. The Kolmogorov–Sinai entropy density of KS was not collected.
- Every exact solution in this section other than KS and Gray–Scott is [M] (textbook formulas, sources named, not re-read).
- CGLE: no reference values for the defect- and phase-turbulence regimes (Shraiman et al. 1992; Chaté 1994) were collected.
- FitzHugh–Nagumo reaction–diffusion: no verified pulse speed or spiral parameters (Barkley's model) were collected.
- Swift–Hohenberg with quadratic or quintic terms (hexagons, snaking) has no reference values here.

## 4. Fluids and standard benchmarks

### Takeaway
The linear-stability numbers are exact to the digits quoted (Rayleigh–Bénard 1707.762 at k = 3.117 rigid–rigid; 27π⁴/4 = 657.511 free–free; Taylor–Couette 1707.76 in the thin gap; Kolmogorov flow √2), and the benchmark cases have tabulated reference data (Ghia et al.'s centreline velocities; Taylor–Green at Re = 1600 by a 512³ pseudo-spectral run). Ghia's table is known through transcriptions that contain errors, and later spectral solutions supersede it at Re = 1000.

### Cited Findings

**Rayleigh–Bénard convection**, Ra = gβ(T_b − T_u)L³/(να)

- Free–free: Ra_c = 27π⁴/4 ≈ 657.51 (Rayleigh 1916); rigid–free: ≈ 1100.65; a critical value 1708 quoted for the rigid case (Koschmieder 1993) [V] — [Wikipedia, Rayleigh–Bénard convection](https://en.wikipedia.org/wiki/Rayleigh%E2%80%93B%C3%A9nard_convection)
- Rigid–rigid: the conductive state first becomes unstable at wavenumber k_c = 3.117 and R_c = 1707.762 [V] — [Simulation of Rayleigh–Bénard convection using the lattice Boltzmann method, arXiv comp-gas/9612001](https://arxiv.org/pdf/comp-gas/9612001)
- Ra_c ≈ 1707.76 (two rigid walls), ≈ 1100.65 (rigid below, stress-free above), 27π⁴/4 ≈ 657.511 (two stress-free); free–free marginal curve Ra(k) = (π² + k²)³/k² [V] — [Nektar++ tutorial, Rayleigh–Bénard convection](https://doc.nektar.info/tutorials/latest/incns/rbc/rbcch3.html)
- Rigid–free critical wavenumber 2.682 (Chandrasekhar 1961, ch. II) [M] — [Wikipedia, Rayleigh–Bénard convection](https://en.wikipedia.org/wiki/Rayleigh%E2%80%93B%C3%A9nard_convection)

**Taylor–Couette flow**, μ = Ω₂/Ω₁, η = R₁/R₂

- Thin-gap Taylor number Ta = 4Ω₁²R₁⁴/ν² · (1 − μ)(1 − μ/η²)/(1 − η²)²; Ta_c ≈ 1708 for 0 ≤ μ ≤ 1; as μ → 1, Ta_c = 1707.76[1 − 0.00761((1 − μ)/(1 + μ))²]; Rayleigh's inviscid criterion: stable iff μ > η² (Taylor 1923; Chandrasekhar; Drazin and Reid) [V] — [Wikipedia, Taylor–Couette flow](https://en.wikipedia.org/wiki/Taylor%E2%80%93Couette_flow)

**Kolmogorov flow** (2D Navier–Stokes forced by sin ny)

- Meshalkin and Sinai (1961): on a domain of aspect L_x/L_y = 1/α the laminar flow is unstable for α < 1, with R_c → √2 as α → 0 (in Iudovich's normalisation); globally stable for α ≥ 1 (Iudovich 1965; Marchioro 1987); n = 4 on [0, 2π]² is the usual choice for α = 1 (Platt et al. 1991; Chandler and Kerswell 2013) [V] — [Kolmogorov flow: linear stability and energy transfers, arXiv 2007.06513](https://arxiv.org/pdf/2007.06513)

**Lid-driven cavity** (Ghia, Ghia and Shin 1982, J. Comput. Phys. 48, 387)

- u-velocity on the vertical line through the cavity's centre, as transcribed in a public gist attributing it to Ghia et al. [V, secondary transcription] — [gist ivan-pi, Lid-driven cavity flow benchmark u-velocity values](https://gist.github.com/ivan-pi/3e9326d18a366ffe6a8e5bfda6353219):

| y | Re = 100 | Re = 400 | Re = 1000 |
|---|---|---|---|
| 1.0000 | 1.00000 | 1.00000 | 1.00000 |
| 0.9766 | 0.84123 | 0.75837 | 0.65928 |
| 0.9688 | 0.78871 | 0.68439 | 0.57492 |
| 0.9609 | 0.73722 | 0.61756 | 0.51117 |
| 0.9531 | 0.68717 | 0.55892 | 0.46604 |
| 0.8516 | 0.23151 | 0.29093 | 0.33304 |
| 0.7344 | 0.00332 | 0.16256 | 0.18719 |
| 0.6172 | −0.13641 | 0.02135 | 0.05702 |
| 0.5000 | −0.20581 | −0.11477 | −0.06080 |
| 0.4531 | −0.21090 | −0.17119 | −0.10648 |
| 0.2813 | −0.15662 | −0.32726 | −0.27805 |
| 0.1719 | −0.10150 | −0.24299 | −0.38289 |
| 0.1016 | −0.06434 | −0.14612 | −0.29730 |
| 0.0703 | −0.04775 | −0.10338 | −0.22220 |
| 0.0625 | −0.04192 | −0.09266 | −0.20196 |
| 0.0547 | −0.03717 | −0.08186 | −0.18109 |
| 0.0000 | 0.00000 | 0.00000 | 0.00000 |

- A search summary of the same gist reported the Re = 1000 entry at y = 0.4531 as −0.86636 where the page read gives −0.10648; the two readings conflict, and the transcription must be checked against the paper's Table I [V, conflict] — [gist ivan-pi](https://gist.github.com/ivan-pi/3e9326d18a366ffe6a8e5bfda6353219)
- Secondary sources give the reference minimum of the stream function at Re = 1000 as about −0.11793 [V, secondary] — [K-Dense lid-driven cavity benchmark page](https://www.k-dense.ai/use-cases/lid-driven-cavity-cfd-benchmark)
- Ghia's primary vortex: ψ = −0.103423 at (0.6172, 0.7344) for Re = 100, −0.113909 at (0.5547, 0.6055) for Re = 400, −0.117929 at (0.5313, 0.5625) for Re = 1000 (their Table III) [M] — [Ghia, Ghia and Shin 1982](https://doi.org/10.1016/0021-9991(82)90058-4)
- The Chebyshev solution of Botella and Peyret (1998) at Re = 1000 is the more accurate reference: |ψ| = 0.1189366 at the primary vortex, vorticity 2.067753 there [M] — [Botella and Peyret 1998, Computers and Fluids 27, 421](https://doi.org/10.1016/S0045-7930(98)00002-4)

**Taylor–Green vortex, Re = 1600**

- Reference: a dealiased pseudo-spectral direct simulation on 512³ (all Fourier modes to the 256th harmonic), three-step low-storage Runge–Kutta, Δt = 10⁻³; it is case C3.5 of the International Workshop on High-Order CFD Methods; van Rees, Leonard, Pullin and Koumoutsakos, J. Comput. Phys. 230 (2011) 2794–2805 [V] — [HiOCFD workshop, case C3.5](https://cfd.ku.edu/hiocfd/case_c3.5.pdf)
- The dissipation peak is placed at t ≈ 8.95 [V] — [Accuracy, stability and performance comparison between spectral difference and flux reconstruction schemes, arXiv 2009.06072](https://arxiv.org/pdf/2009.06072)

**Orszag–Tang vortex**

- Original incompressible MHD problem: Orszag and Tang, J. Fluid Mech. 90 (1979) 129. The usual compressible version: [0, 1]² periodic, γ = 5/3, ρ = 25/(36π), p = 5/(12π), v = (−sin 2πy, sin 2πx), B = B₀(−sin 2πy, sin 4πx) with B₀ = 1/√(4π), compared at t = 0.5 [M] — [Orszag and Tang 1979](https://doi.org/10.1017/S002211207900210X)

**2D Navier–Stokes in vorticity form, shallow water, quasi-geostrophic, point vortices**

- ω_t + u·∇ω = ν∇²ω, ∇²ψ = −ω; exact decaying Taylor–Green solution u = cos x sin y e^{−2νt}, v = −sin x cos y e^{−2νt}; Lamb–Oseen vortex u_θ = (Γ/2πr)(1 − e^{−r²/4νt}) [M] — [Wikipedia, Taylor–Green vortex](https://en.wikipedia.org/wiki/Taylor%E2%80%93Green_vortex)
- Shallow water on the sphere: the seven test cases of Williamson et al. (1992) and the barotropic-instability case of Galewsky et al. (2004) are the standard suite; linear waves: gravity c = √(gH), Poincaré ω² = f² + gH k² [M] — [Williamson et al. 1992, J. Comput. Phys. 102, 211](https://doi.org/10.1016/S0021-9991(05)80016-6)
- Eady problem: fastest growth 0.31 (f/N) dU/dz at k L_d = 1.61, short-wave cut-off at k L_d = 2.399 [M] — [Eady 1949, Tellus 1, 33](https://doi.org/10.3402/tellusa.v1i3.8507)
- Point vortices: H = −(1/4π) Σ_{i<j} Γ_iΓ_j ln r_ij²; two vortices turn at Ω = (Γ₁ + Γ₂)/(2πd²); three are integrable and four generally chaotic; a ring of N equal vortices is stable for N < 7, marginal at 7 [M] — [Aref 1983, Annu. Rev. Fluid Mech. 15, 345](https://doi.org/10.1146/annurev.fl.15.010183.002021)

### Inferences
- [D] Free–free Rayleigh–Bénard: minimising (π² + k²)³/k² gives k_c = π/√2 = 2.2214 and Ra_c = 27π⁴/4 = 657.511; an implementation can reproduce the whole marginal curve in closed form before attempting the rigid cases, which need the root of a transcendental equation.
- The Taylor–Couette thin-gap number 1707.76 is the Rayleigh–Bénard rigid–rigid number: the two linear problems coincide in that limit, so one eigenvalue solver checks both templates.
- Critical values of fluid problems depend on the nondimensionalisation as much as on the physics: Taylor number against Reynolds number of the inner cylinder; Kolmogorov flow's √2 against the values in other scalings (one source's 5π√6/3 ≈ 12.83 for a different normalisation); Ghia's lid moving in +x on a unit square with y upward.
- The physis repository's existing checks (the onset of convection, two-layer quasi-geostrophic flow, Kuramoto's networks; its decision 0022) overlap with this section; the numbers above are the reference side of those checks.

### Gaps
- Chandrasekhar's tables (rigid–free wavenumber 2.682, Taylor–Couette critical wavenumber ≈ 3.12 and his T_c = 3390 for μ = 0 in his own definition) were not read.
- Ghia et al.'s original tables were not read; the v-velocity table and the vortex data are missing or [M]. The peak dissipation of the Taylor–Green reference (commonly quoted near 0.0129 at t ≈ 9) was not verified.
- Taylor–Couette at finite gap (for example Re_c ≈ 68.19 at η = 0.5) was not verified.
- Phillips's two-layer critical shear is convention-dependent (which deformation radius) and no verified statement was collected; Vallis's chapter 6 is the place to read it.
- Chandler and Kerswell's Reynolds numbers (40, 60, 80, 100) and their Re definition were not verified.
- Orszag–Tang initial data are [M]; codes differ in whether 4π is absorbed in B.

## 5. Networks and lattice systems

### Takeaway
The lattice systems are checked by exact combinatorial facts more than by measured exponents: periods and populations in the Game of Life, the Abelian property and the exact height probabilities of the sandpile, the critical connectivity of random Boolean networks, the identity of the Frenkel–Kontorova transition with Greene's K_c. Nothing in this section was verified on a page read in this session; all of it is [M].

### Cited Findings

- **Coupled map lattices** (Kaneko): x_{n+1}(i) = (1 − ε) f(x_n(i)) + (ε/2)[f(x_n(i − 1)) + f(x_n(i + 1))], f(x) = 1 − a x²; phases: frozen random patterns, pattern selection, defect turbulence, spatio-temporal intermittency, fully developed chaos [M] — [Kaneko 1989, Physica D 34, 1](https://doi.org/10.1016/0167-2789(89)90227-3)
- **Elementary cellular automata**: 256 rules, 88 inequivalent under reflection and complement; Wolfram's four classes (fixed, periodic, chaotic, complex); Rule 30 class III, Rule 90 additive (Sierpinski), Rule 184 traffic, Rule 110 class IV [M] — [Wolfram 1984, Physica D 10, 1](https://doi.org/10.1016/0167-2789(84)90245-8)
- Rule 110 is computationally universal [M] — [Cook 2004, Complex Systems 15, 1](https://www.complex-systems.com/abstracts/v15_i01_a01/)
- **Game of Life** (B3/S23): blinker period 2; glider period 4 with displacement (1, 1); Gosper's gun period 30; the R-pentomino settles at generation 1103 with 116 cells; the rule is Turing complete [M] — [Wikipedia, Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life)
- **Random Boolean networks** (Kauffman's N–K model): the annealed approximation gives the critical line 2p(1 − p)K = 1, so K_c = 2 at p = 1/2 [M] — [Derrida and Pomeau 1986, Europhys. Lett. 1, 45](https://doi.org/10.1209/0295-5075/1/2/001)
- **Fermi–Pasta–Ulam–Tsingou**: H = Σ ½p_i² + ½(q_{i+1} − q_i)² + (α/3)(q_{i+1} − q_i)³ + (β/4)(q_{i+1} − q_i)⁴, fixed ends; the original run had N = 32, α = 1/4, the energy in the lowest mode, and returned to it within a few per cent after about 157 periods of that mode; normal-mode frequencies ω_k = 2 sin(kπ/(2(N + 1))) [M] — [Fermi, Pasta and Ulam 1955, Los Alamos report LA-1940](https://doi.org/10.2172/4376203)
- **Frenkel–Kontorova**: H = Σ ½p_n² + ½(x_{n+1} − x_n − a)² + (K/(2π)²)(1 − cos 2πx_n); its equilibria are orbits of the standard map, and the Aubry transition of the golden-mean ground state is at K_c = 0.971635… [M] — [Braun and Kivshar 1998, Phys. Rep. 306, 1](https://doi.org/10.1016/S0370-1573(98)00029-5)
- **Sandpile** (Bak, Tang and Wiesenfeld): a site with z ≥ 4 topples, losing 4 and giving 1 to each neighbour; the model is Abelian and the number of recurrent configurations is det Δ (Dhar 1990) [M] — [Dhar 1990, Phys. Rev. Lett. 64, 1613](https://doi.org/10.1103/PhysRevLett.64.1613)
- Exact probability of the lowest height on the infinite square lattice: 2/π² − 4/π³ ≈ 0.0736 [M] — [Majumdar and Dhar 1991, J. Phys. A 24, L357](https://doi.org/10.1088/0305-4470/24/7/008)

### Inferences
- The exact checks available are: Game of Life census values; ECA rule tables and the 88 equivalence classes; the sandpile's order independence (toppling in any order gives the same stable configuration) and det Δ; the FPUT lattice's harmonic frequencies and energy conservation; the Derrida curve of a Boolean network against 2p(1 − p)K.
- The Frenkel–Kontorova model and the standard map share one threshold, so a library holding both should check them against the same constant.
- The globally coupled map lattice gives one clean formula worth a check: full synchronisation is stable when the coupling exceeds 1 − e^{−λ} (1/2 for the logistic map at r = 4); this is recalled, not sourced here.

### Gaps
- Every finding in this section is from memory; none was read in this session. The FPUT recurrence time in particular is quoted variously (in periods of the lowest mode or in integration cycles) and needs the report itself.
- The sandpile's avalanche exponents are disputed (simple finite-size scaling against multifractal scaling; BTW against Manna universality) and no value is offered.
- No reference values were collected for coupled map lattices beyond the phase names.

## 6. Existing curated collections

### Takeaway
The collections are of two kinds: catalogues of low-dimensional systems with parameters (dysts, PredefinedDynamicalSystems.jl, Sprott's tables) and datasets or generators of PDE trajectories for machine learning (PDEBench, PDEArena, The Well, APEBench). Only Sprott's tables are curated reference values; dysts' numbers are automatic estimates, DynamicalSystems.jl's module states that it has no tests, and the PDE collections hold trajectories, not reference invariants.

### Cited Findings

| Collection | Contents | Format | Licence | Reference values |
|---|---|---|---|---|
| dysts (Gilpin) | README: 222 continuous-time systems and 10 discrete maps (the maps "being expanded"); precomputed series on HuggingFace | JSON (`chaotic_attractors.json`, `discrete_maps.json`) plus Python classes | Apache-2.0 (code); none stated for the HuggingFace data | Estimated per system |
| PredefinedDynamicalSystems.jl | 67 constructor functions | Julia functions returning a `DynamicalSystem`; some with Jacobians | MIT (Expat), © 2017 George Datseris | None |
| Sprott's tables | 8 systems on the web page; 62 cases in Appendix A of the 2003 book | Web page; printed book | Not openly licensed (book) | Yes, curated |
| PDEBench | 1D advection, Burgers, reaction–diffusion, diffusion–sorption; 2D diffusion–reaction, Darcy flow, compressible Navier–Stokes, incompressible inhomogeneous Navier–Stokes, shallow water (radial dam break) | NumPy arrays merged to HDF5; DaRUS | Code MIT; data CC BY 4.0 | None |
| PDEArena | 2D Navier–Stokes (two sets), 2D shallow water, 3D Maxwell, 1D Kuramoto–Sivashinsky | Git LFS repositories on HuggingFace | Code MIT; data not stated | None |
| The Well | 16 datasets, 15 TB | Self-documenting HDF5, fp32 | Code BSD-3-Clause; data not stated on the pages read | None |
| APEBench | 46 PDEs in 1D, 2D, 3D on periodic domains | Generated procedurally (Exponax, JAX) | MIT | None |

- dysts: per system it stores name, parameters, initial conditions (taken on the attractor after the autocorrelation became stationary), `dt`, `period`, citation; Lyapunov spectra are averages over runs of 1,000 periods with standard errors from batch means; systems with discontinuous right-hand sides use finite perturbations, those without an analytic Jacobian central differences; cite Gilpin, NeurIPS 2021 (arXiv 2110.05266) and Phys. Rev. Research 2023 (arXiv 2303.08011) [V] — [dysts repository](https://github.com/williamgilpin/dysts)
- dysts' JSON fields: `bifurcation_parameter`, `citation`, `correlation_dimension`, `description`, `dt`, `embedding_dimension`, `hamiltonian`, `initial_conditions`, `kaplan_yorke_dimension`, `lyapunov_spectrum_estimated`, `maximum_lyapunov_estimated`, `mean`, `multiscale_entropy`, `nonautonomous`, `parameters`, `period`, `pesin_entropy`, `std`, `unbounded_indices`, `doi`; its family of Sprott's flows runs SprottA to SprottS with SprottJerk, SprottMore and SprottTorus [V] — [dysts, chaotic_attractors.json](https://raw.githubusercontent.com/williamgilpin/dysts/master/dysts/data/chaotic_attractors.json)
- dysts' `discrete_maps.json` lists 26 maps (Logistic r = 3.9, Tent μ = 0.95, Henon 1.4/0.3, Chirikov k = 0.971635, Ikeda u = 0.9, Baker, Circle ω = 0.333, k = 5.7, Zaslavskii, Tinkerbell, Gingerbreadman and others) with parameters and, for most, no citation and no Lyapunov or dimension fields [V] — [dysts, discrete_maps.json](https://raw.githubusercontent.com/williamgilpin/dysts/master/dysts/data/discrete_maps.json)
- PredefinedDynamicalSystems.jl: the README says the module "does not have any actual tests", is "not recommended … for anything else besides on-the-spot demonstrative examples", and is not guaranteed stable across versions [V] — [PredefinedDynamicalSystems.jl repository](https://github.com/JuliaDynamics/PredefinedDynamicalSystems.jl)
- Its 67 systems: antidots, arnoldcat, betatransformationmap, chua, coupled_roessler, coupledstandardmaps, daisyworld, double_pendulum, duffing, fitzhugh_nagumo, forced_pendulum, gissinger, grebogi_map, guckenheimer_holmes, halvorsen, henon, henonheiles, hindmarshrose, hindmarshrose_two_coupled, hodgkinhuxley, hyper_bao, hyper_cai, hyper_jha, hyper_lorenz, hyper_lu, hyper_pang, hyper_qi, hyper_roessler, hyper_wang, hyper_xu, ikedamap, kuramoto, logistic, lorenz, lorenz84, lorenz96, lorenz_bounded, lorenzdl, lotkavolterra, magnetic_pendulum, manneville_simple, more_chaos_example, morris_lecar, multispecies_competition, nld_coupled_logistic_maps, nosehoover, pomeau_manneville, qbh, riddled_basins, rikitake, roessler, rulkovmap, sakarya, shinriki, sprott_dissipative_conservative, standardmap, stommel_thermohaline, stuartlandau_oscillator, swinging_atwood, tentmap, thomas_cyclical, towel, tristable_predator_prey, ueda, ulam, vanderpol, zaslavskiimap [V] — [PredefinedDynamicalSystems.jl documentation](https://juliadynamics.github.io/PredefinedDynamicalSystems.jl/dev/)
- Licence: MIT "Expat", copyright 2017 George Datseris [V] — [PredefinedDynamicalSystems.jl LICENSE](https://raw.githubusercontent.com/JuliaDynamics/PredefinedDynamicalSystems.jl/main/LICENSE.md)
- Sprott's page tabulates eight systems with equations, parameters, exponents, D_KY and correlation dimension, gives no initial conditions, warns that the last digit is "only a best estimate", and points to 62 cases in Appendix A of *Chaos and Time-Series Analysis* (Oxford, 2003) [V] — [Sprott, Common chaotic systems](https://sprott.physics.wisc.edu/chaos/comchaos.htm)
- PDEBench: code MIT "except where otherwise stated"; paper Takamoto et al., NeurIPS 2022 Datasets and Benchmarks (arXiv 2210.07182); dataset DOI 10.18419/darus-2986 [V] — [PDEBench repository](https://github.com/pdebench/PDEBench)
- PDEBench data: CC BY 4.0, version 8.0, 336 files, first published 2022-07-26, files added in October and November 2023 [V] — [DaRUS, PDEBench datasets](https://darus.uni-stuttgart.de/dataset.xhtml?persistentId=doi:10.18419/darus-2986)
- PDEArena: MIT; Navier–Stokes and shallow water from Gupta and Brandstetter (arXiv 2209.15616), Maxwell from Brandstetter et al. (arXiv 2209.04934) [V] — [PDEArena repository](https://github.com/pdearena/pdearena)
- PDEArena generators and hosting: Navier–Stokes by ΦFlow (`pdearena/NavierStokes-2D`, `…-2D-conditoned`), shallow water by SpeedyWeather.jl (`pdearena/ShallowWater-2D`), Maxwell by a Python 3D FDTD simulator (`pdearena/Maxwell-3D`), Kuramoto–Sivashinsky by LPSDA (`pdearena/Kuramoto-Sivashinsky-1D`) [V] — [PDEArena data download page](https://pdearena.github.io/pdearena/datadownload/)
- The Well: code BSD-3-Clause; about 15 TB, single datasets from 6.9 GB to 5.1 TB; `viscoelastic_instability` deprecated in favour of `viscoelastic_instability_v2`; Ohana et al., NeurIPS 2024 (vol. 37, pp. 44989–45037) [V] — [The Well repository](https://github.com/PolymathicAI/the_well)
- The Well's datasets (resolution, trajectories): acoustic_scattering (256², 8,000), active_matter (256², 360), convective_envelope_rsg (256 × 128 × 256, 29), euler_multi_quadrants (512², 10,000), gray_scott_reaction_diffusion (128², 1,200), helmholtz_staircase (1024 × 256, 512), MHD (64³ and 256³, 100), planetswe (256 × 512, 120), post_neutron_star_merger (192 × 128 × 66, 8), rayleigh_benard (512 × 128, 1,750), rayleigh_taylor_instability (128³, 45), shear_flow (256 × 512, 1,120), supernova_explosion (64³ and 128³, 1,000), turbulence_gravity_cooling (64³, 2,700), turbulent_radiative_layer (2D 128 × 384 and 3D 128 × 128 × 256, 90 each), viscoelastic_instability (512², 260); arrays of shape (n_traj, n_steps, coords…) in fp32 HDF5 [V] — [Ohana et al., The Well, arXiv 2412.00568](https://arxiv.org/html/2412.00568v2)
- APEBench: MIT; JAX; scenarios are callable dataclasses; data generated by the pseudo-spectral ETDRK solvers of Exponax; a difficulty-based and a normalised-coefficients interface; Koehler et al., NeurIPS 2024 (arXiv 2411.00180) [V] — [APEBench repository](https://github.com/tum-pbs/apebench)
- APEBench's abstract: 46 distinct PDEs; a "unique identifier for PDE dynamics" tied to the stability criteria of classical numerical methods; rollout metrics [V] — [Koehler et al., arXiv 2411.00180](https://arxiv.org/abs/2411.00180)

### Inferences
- Reliability, in order: Sprott's tables (curated, cross-checked by a second algorithm on his own page, pass the divergence check to all digits shown); the primary papers; then dysts (automatic estimates at its own parameters; Lorenz β rounded to 2.667; its Lorenz 96 entry is four variables at F = 20 with a description of the other Lorenz model; correlation dimensions visibly low; a citation string with an encoding fault); DynamicalSystems.jl's module gives definitions only.
- dysts' README count (10 maps) and its JSON (26 maps) disagree, so counts quoted from it should come from the file at a pinned commit.
- For a library of candidate models to match against data, dysts is the largest machine-readable list of low-dimensional flows under a permissive licence (Apache-2.0), and its JSON schema (parameters, initial condition on the attractor, `dt`, `period`, citation) is a usable model for a template's metadata; the reference values should be recomputed and pinned to primary sources.
- The PDE collections are complementary as data sources but supply no invariants; APEBench's procedural generation (equations and coefficients, no files) is the closest in spirit to shipping templates. PDEBench's data licence (CC BY 4.0) permits redistribution with attribution; the others' data licences must be read per dataset before any redistribution.
- Scholarpedia remains the best peer-reviewed secondary source for definitions, but a library cannot depend on it being reachable: on 2026-11-12 its certificate had expired.

### Gaps
- The data licences of The Well (HuggingFace returned 401) and of PDEArena's datasets were not found on the pages read.
- Sprott's *Elegant Chaos* (2010) and his 1994 list of nineteen simple flows (Phys. Rev. E 50, R647) were not read; their contents are known here only through dysts' SprottA–SprottS entries.
- Scholarpedia's entries, licence and reliability could not be assessed in this session.
- The number of systems in dysts' continuous-time file was not counted directly (the README's 222 is taken as stated).
- Other collections were not surveyed: ChaosBook's examples, the Johns Hopkins Turbulence Databases, Exponax's own list, ODE benchmark suites for symbolic regression.

## 7. Disputed and convention-dependent reference values

### Takeaway
Almost every disagreement between quoted values comes from one of five conventions: the parameter set, the base of the logarithm, the form of the equations, the nondimensionalisation, and what is held fixed (initial condition, mean, boundary condition). True disputes are few: correlation dimensions, the sandpile's exponents, and accuracy of old benchmark tables.

### Cited Findings

- **Base of the logarithm and parameters.** Wolf et al.'s widely copied table is in bits and uses σ = 16, r = 45.92, b = 4 for Lorenz; Sprott's values are in base e at σ = 10, r = 28, b = 8/3 [V] — [Sprott, Lyapunov Exponent Spectrum Software](https://sprott.physics.wisc.edu/chaos/lespec.htm)
- **Rössler.** Two parameter sets are current, (0.2, 0.2, 5.7) and (0.1, 0.1, 14) [V] — [Wikipedia, Rössler attractor](https://en.wikipedia.org/wiki/R%C3%B6ssler_attractor)
- At (0.2, 0.2, 5.7) two independent sources agree: 0.0714, 0, −5.3943 (Sprott) and 0.07120, ≈0, −5.39373 (dysts) [V] — [Sprott, Common chaotic systems](https://sprott.physics.wisc.edu/chaos/comchaos.htm); [dysts, chaotic_attractors.json](https://raw.githubusercontent.com/williamgilpin/dysts/master/dysts/data/chaotic_attractors.json)
- **Lorenz, fourth digit.** 0.9056 and 0.90563 (Sprott, two methods) against 0.90504 (dysts, with β = 2.667) [V] — [Sprott, Lyapunov Exponent Spectrum Software](https://sprott.physics.wisc.edu/chaos/lespec.htm); [dysts, chaotic_attractors.json](https://raw.githubusercontent.com/williamgilpin/dysts/master/dysts/data/chaotic_attractors.json)
- **Dimensions.** For Hénon: correlation dimension 1.21 ± 0.01 (Grassberger–Procaccia), 1.220 ± 0.036 (Sprott), box-counting 1.261 ± 0.003, D_KY 1.25827; these are different dimensions, not conflicting measurements. For Lorenz: correlation 2.05 ± 0.01 and 2.068 ± 0.086, D_KY 2.06215 [V] — [Wikipedia, Hénon map](https://en.wikipedia.org/wiki/H%C3%A9non_map); [Sprott, Common chaotic systems](https://sprott.physics.wisc.edu/chaos/comchaos.htm)
- **Kuramoto–Sivashinsky.** With periodic boundaries and the mean left free there are two zero exponents; fixing the mean removes one; odd-periodic boundaries have none, and the dimension differs by an additive constant (−0.160 against −2.106) at the same slope 0.226 [V] — [Edson et al., arXiv 1902.09651](https://arxiv.org/abs/1902.09651)
- **Kuramoto model.** K_c = 2/(π g(0)) presumes the coupling written K/N and a unimodal symmetric g; the Lorentzian's width is written Δ or γ, and with a phase lag λ the threshold is 2Δ/cos λ [V] — [Model reduction for the Kuramoto–Sakaguchi model, arXiv 2004.06344](https://arxiv.org/pdf/2004.06344)
- **Kolmogorov flow.** R_c = √2 holds in Iudovich's normalisation in the long-domain limit; another scaling gives 5π√6/3 ≈ 12.83 for a related expression [V] — [Kolmogorov flow: linear stability and energy transfers, arXiv 2007.06513](https://arxiv.org/pdf/2007.06513)
- **Rayleigh–Bénard.** "1708" is quoted without its boundary conditions; the three cases are 1707.76, 1100.65 and 657.51 [V] — [Wikipedia, Rayleigh–Bénard convection](https://en.wikipedia.org/wiki/Rayleigh%E2%80%93B%C3%A9nard_convection); [Nektar++ tutorial](https://doc.nektar.info/tutorials/latest/incns/rbc/rbcch3.html)
- **Lorenz 96.** The primary paper gives the doubling time (0.42 units, with a time unit of 5 days), the number of positive exponents and the dimension for J = 40, not λ₁ itself; dysts' entry of the same name is a different configuration (4 variables, F = 20) [V] — [Lorenz and Emanuel 1998](https://texmex.mit.edu/ftp/pub/emanuel/PAPERS/lorenz98.pdf); [dysts, chaotic_attractors.json](https://raw.githubusercontent.com/williamgilpin/dysts/master/dysts/data/chaotic_attractors.json)
- **Ghia's table.** Transcriptions in circulation disagree with one another in at least one entry (see §4) [V] — [gist ivan-pi](https://gist.github.com/ivan-pi/3e9326d18a366ffe6a8e5bfda6353219)

### Inferences
- Conventions a template must fix and store with each reference value:
  1. the equations in full, as written in the cited source, with the meaning of each parameter (Hénon's form; Chua's f; KS integral or derivative; CGLE's signs; Gray–Scott's k);
  2. the parameter values as exact rationals where the source has them (b = 8/3, not 2.667; m₀ = −8/7);
  3. the unit of the exponents (base e; per iteration or per time unit; the time unit itself, as in Lorenz 96's 5 days);
  4. the phase-space convention (angles mod 2π or mod 1; a non-autonomous system counted with or without its phase as a variable — Ueda's zero exponent is the phase's);
  5. domain, boundary conditions and constraints for PDEs (L or L/2π; mean fixed or free);
  6. the nondimensionalisation of fluid thresholds (Ra, Ta against Re, the forcing scale of Kolmogorov flow);
  7. for mixed phase spaces, the initial condition or the energy (standard map, Hénon–Heiles);
  8. the kind of dimension (Kaplan–Yorke, correlation, box-counting, Hausdorff) and the estimator;
  9. a tolerance: Sprott's exponents are good to about the last digit shown; correlation dimensions to a few per cent; exact values to rounding.
- Each reference value should carry, beside its source, a class: exact (closed form), rigorous (computer-assisted proof, as Tucker's), converged numerical (several independent agreements), or estimated (single computation). The checks in §§1–2 that Σλ equals the mean divergence turn several estimated values into self-verifying ones.
- Where independent sources agree after conventions are matched (Hénon; Lorenz; Rössler at (0.2, 0.2, 5.7); KS at L = 22), the value can be a fixture with a tolerance of a few units in the last digit; where only one automatic estimate exists (most of dysts), it should be recomputed, following the repository's rule that fixtures come from running reference implementations with the script kept.

### Gaps
- No systematic comparison of dysts' spectra with Sprott's Appendix A across all shared systems was made; only Lorenz and Rössler were compared.
- High-precision values (for example a rigorous enclosure of the Lorenz exponent, or Viswanath's computation) were not collected, so the true fifth digit of 0.9056 is not settled here.
- Whether the Ghia transcription conflict lies in the gist or in the search summary of it was not resolved.
- The sandpile exponents and the FPUT recurrence figure remain unsourced disputes (see §5).
