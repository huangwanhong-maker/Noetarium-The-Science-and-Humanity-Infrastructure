# Time and Relations: What Is Computed, and Why It Holds

*Status: draft (2026-11-13). The sociology domain's first note ([ADR 0104](../../../development/decisions/0104-sociology-founded-time-and-relations.md)). Activities: research, learning.*

Two things organise everyday life beyond what is produced and bought: how a day's time is divided, and the ties people live among. This note states what the domain computes of each, on tyche's `compositions` and `networks`, and proves the properties its checks hold.

## 1. Concepts

### 1.1 A Day Is a Composition

**Definition 1.1.** A *day* in $D$ parts is a point $x = (x_1, \dots, x_D)$ of the simplex $\mathcal S^D_{1440} = \{x \in \mathbb R^D_{>0} : \sum_i x_i = 1440\}$, with $x_i$ the minutes of part $i$. A *classification* is a map from activity codes to parts; it is declared, never the domain's (HETUS's codes, a survey's own, or Ås's four kinds of time: necessary, contracted, committed and free).

The simplex carries Aitchison's geometry (Aitchison 1986): perturbation $x \oplus y = \mathcal C(x_1 y_1, \dots, x_D y_D)$, powering $a \odot x = \mathcal C(x_1^a, \dots, x_D^a)$, with $\mathcal C$ the closure to the total, and the inner product of the centred log-ratios
$$\mathrm{clr}(x)_i = \ln x_i - \frac1D \sum_j \ln x_j, \qquad \langle x, y \rangle_a = \langle \mathrm{clr}(x), \mathrm{clr}(y) \rangle .$$
The Aitchison distance is $d_a(x, y) = \lVert \mathrm{clr}(x) - \mathrm{clr}(y) \rVert$.

*Rationale.* Minutes cannot be compared one at a time, since they are bound by the day: an hour more of work is an hour less of something else. Ratios are what a day's division is; $d_a$ is invariant to the unit (minutes or shares) and to the subcomposition's closure, and an hour moved between two short parts counts for more than an hour moved between two long ones.

### 1.2 Counted, Presumed

A survey counts minutes in slots (HETUS: ten minutes). A part a diary did not record holds a *rounded* zero, replaced multiplicatively by $0.65$ of a slot (Martín-Fernández et al. 2003); the others keep their ratios. Minutes the diary did not assign are spread in proportion, which is the closure. Both are declared in every answer.

## 2. The Day

### 2.1 A Group's Day and Its Spread

**Definition 2.1.** For days $x^{(1)}, \dots, x^{(n)}$ the *centre* is $g = \mathcal C(\bar g_1, \dots, \bar g_D)$, $\bar g_i = (\prod_k x^{(k)}_i)^{1/n}$; the *total variance* is $\frac{1}{n - \delta} \sum_k d_a(x^{(k)}, g)^2$ with $\delta$ the declared ddof.

The centre is the Fréchet mean of Aitchison's metric. It is not the arithmetic mean that surveys publish; the domain reports both.

### 2.2 Two Days Contrasted

**Proposition 2.2.** For two days $x, y$, the difference $x \ominus y = \mathcal C(x_1/y_1, \dots, x_D/y_D)$ satisfies:

1. $\sum_i \mathrm{clr}(x \ominus y)_i = 0$;
2. $\lVert \mathrm{clr}(x \ominus y) \rVert = d_a(x, y)$;
3. $(x \ominus y) \oplus y = x$.

*Proof.* (1) holds of every clr vector. (2): $\mathrm{clr}$ is an isomorphism of $(\mathcal S^D, \oplus)$ onto the hyperplane $\{\sum z_i = 0\}$, so $\mathrm{clr}(x \ominus y) = \mathrm{clr}(x) - \mathrm{clr}(y)$. (3): $\mathcal C$ is idempotent under perturbation, and $(x_i / y_i) y_i = x_i$. ∎

`check_sociology_time` holds all three.

### 2.3 Who Carries a Part

With groups $g$ of $n_g$ people giving $m_g$ minutes a day to a part, group $g$ carries the share $s_g = n_g m_g / \sum_h n_h m_h$ of the part's minutes, against its share $w_g = n_g / \sum_h n_h$ of the people. The ratio $s_g / w_g$ exceeds one when the group carries more than its number.

### 2.4 The Balance of Two Kinds of Time

**Definition 2.3** (Egozcue and Pawlowsky-Glahn 2005). For disjoint sets of parts $P$ ($r$ parts) and $M$ ($s$ parts), the balance is
$$b_{P|M}(x) = \sqrt{\frac{rs}{r + s}} \ln \frac{g(x_P)}{g(x_M)},$$
with $g$ the geometric mean. It is an orthonormal coordinate of $\mathcal S^D$; zero holds the two kinds in the same geometric proportion. The balance of unpaid against paid work is the domain's first use.

### 2.5 Places Compared

**Proposition 2.4.** Let $X$ be $n$ places' days and $G = -\tfrac12 J \Delta J$ Gower's centred matrix of their squared Aitchison distances $\Delta$, $J = I - \tfrac1n \mathbf 1 \mathbf 1^\top$. Then $G$ is positive semi-definite, its principal coordinates reproduce every distance exactly, and the sum of its eigenvalues is $(n - 1)$ times the total variance with ddof one.

*Proof.* $d_a$ is the Euclidean distance between the rows of $Z = \mathrm{clr}(X)$, so $G = (JZ)(JZ)^\top$ (Gower 1966), which is positive semi-definite, and its principal coordinates are an isometric embedding. $\operatorname{tr} G = \sum_k \lVert z_k - \bar z \rVert^2 = \sum_k d_a(x^{(k)}, g)^2$, since $\mathrm{clr}(g) = \bar z$, and this is $(n - 1)$ times the total variance with $\delta = 1$. ∎

### 2.6 Time Moved Between Activities

An outcome $y$ is regressed on the isometric log-ratio coordinates $z = \mathrm{ilr}(x) = V^\top \mathrm{clr}(x)$ and covariates. Moving $\mu$ minutes to part $i$ from part $j$ of a reference day $x^0$ gives $x^1 = x^0 + \mu(e_i - e_j)$, and the predicted change is $\hat\gamma^\top (\mathrm{ilr}(x^1) - \mathrm{ilr}(x^0))$, a linear combination of the coefficients with its exact standard error (Dumuid et al. 2019). The table of every $(i, j)$ is the isotemporal substitution table. A move that would leave a part without time is not evaluated.

**Proposition 2.5.** If $y = c + b^\top \mathrm{clr}(x)$ exactly, with $\sum_i b_i = 0$, the fitted change of every move is $b^\top(\mathrm{clr}(x^1) - \mathrm{clr}(x^0))$.

*Proof.* $V$ is orthonormal on the clr hyperplane, so $b^\top \mathrm{clr}(x) = (V^\top b)^\top \mathrm{ilr}(x)$ and the model is linear in $z$; least squares on a design of full rank recovers $\gamma = V^\top b$ exactly, and the change follows. ∎

*Caveat.* The table is an association across people whose days differ. It moves no one's time; a causal reading needs a design (peira's).

## 3. Relations

### 3.1 The Whole

A network of $n$ people and $m$ ties is read by its density, transitivity $3T/\tau$ ($T$ triangles, $\tau$ connected triples), the mean of the local clustering coefficients (zero for people with fewer than two contacts, as networkx counts them), reciprocity $M/(M + A)$ of the directed dyads, the triad census (Batagelj and Mrvar's algorithm), components, isolates and Newman's degree assortativity.

### 3.2 Belonging

For groups $g$ with share $w_g$ of the people:

- the share $H_g$ of the ends of its members' ties that stay within it, and Coleman's inbreeding homophily $(H_g - w_g)/(1 - w_g)$;
- the E–I index $(E - I)/(E + I)$ of external and internal ties (Krackhardt and Stern 1988), against $(B - W)/(B + W)$, the index of the complete network on the same groups, which depends only on their sizes: with $n_g$ people in each, $W = \sum_g n_g(n_g - 1)$ and $B = \sum_g n_g(n - n_g)$;
- Newman's assortativity by group;
- the QAP correlation between the adjacency matrix and the matrix of shared group, tested by relabelling the people (Krackhardt 1988). Relabelling keeps the network's whole structure and breaks only its alignment with the groups, so the test asks whether groups and ties go together beyond what the network's shape gives.

### 3.3 Each Person's Ties

Burt's constraint $C_i = \sum_{j \in N(i)} (p_{ij} + \sum_{q \ne i, j} p_{iq} p_{qj})^2$ with $p_{ij}$ the proportion of $i$'s ties invested in $j$, and the effective size $\sum_j (1 - \sum_q p_{iq} m_{jq})$ (Burt 1992), as networkx computes them. The k-core is Batagelj and Zaveršnik's. The reach within two ties is $\lvert N(i) \cup N(N(i)) \setminus \{i\} \rvert$. The diversity of contacts' groups is Blau's $1 - \sum_g p_g^2$, and Hill's number of order two, $1 / \sum_g p_g^2$, the effective number of groups among one's contacts.

### 3.4 Closure Beyond Chance

Transitivity is compared with networks drawn with the same degrees by double-edge swaps (a conditional uniform graph test), with $p = (1 + \#\{t^* \ge t\})/(1 + B)$. The null keeps how many contacts each person has, so a high transitivity is closure beyond what popularity alone gives.

**Validation.** On Zachary's karate club (34 members, 78 ties), the module reproduces networkx 3.7's density, transitivity, average clustering, degree assortativity and assortativity by club to $10^{-12}$ or better, and every member's constraint, effective size, k-core, clustering and reach within two ties (`check_sociology_relations`, fixtures made by `tests/fixtures/make_relations_fixtures.py`).

## 4. What a Day in Europe Shows

On the harmonised time-use surveys of 2010 (Eurostat's `tus_00age`), eighteen countries' days of women and men in eight parts:

- the parts that differ most as ratios are household and family care and voluntary work; personal care and sleep differs least;
- the first principal coordinate holds about half the differences and follows household and family care ($r = 0.94$ with its logarithm); in every one of the eighteen countries women's and men's days lie on opposite sides of it;
- household and family care is what separates women's and men's days most in sixteen of the eighteen; the distance between them is greatest in Türkiye and least in Norway;
- the balance of unpaid against paid work is positive for women and negative for men in every country.

*Caveat.* These are compositions of the survey's means over everyone, not the centres of people's diaries, which are not open.

## 5. What Comes Next

- Days as sequences (optimal matching) and the week's rhythm, when diaries are read.
- Essential zeros, by mixtures or subcompositions by group.
- Networks in time, for kinesis; stochastic actor-oriented models.
- Institutions and stratification.

## 6. References

- Aitchison, J. (1986), *The Statistical Analysis of Compositional Data*, Chapman and Hall.
- Ås, D. (1978), Studies of time-use: problems and prospects, *Acta Sociologica* 21, 125–141.
- Batagelj, V. and Mrvar, A. (2001), A subquadratic triad census algorithm for large sparse networks with small maximum degree, *Social Networks* 23, 237–243.
- Batagelj, V. and Zaveršnik, M. (2003), An O(m) algorithm for cores decomposition of networks, arXiv:cs/0310049.
- Blau, P. M. (1977), *Inequality and Heterogeneity*, Free Press.
- Burt, R. S. (1992), *Structural Holes*, Harvard University Press.
- Coleman, J. S. (1958), Relational analysis: the study of social organizations with survey methods, *Human Organization* 17, 28–36.
- Dumuid, D. et al. (2019), The compositional isotemporal substitution model, *Statistical Methods in Medical Research* 28, 846–857.
- Egozcue, J. J. and Pawlowsky-Glahn, V. (2005), Groups of parts and their balances in compositional data analysis, *Mathematical Geology* 37, 795–828.
- Eurostat (2019), *Harmonised European Time Use Surveys (HETUS) 2018 Guidelines*, Publications Office of the European Union.
- Gower, J. C. (1966), Some distance properties of latent root and vector methods used in multivariate analysis, *Biometrika* 53, 325–338.
- Krackhardt, D. (1988), Predicting with networks: nonparametric multiple regression analysis of dyadic data, *Social Networks* 10, 359–381.
- Krackhardt, D. and Stern, R. N. (1988), Informal networks and organizational crises, *Social Psychology Quarterly* 51, 123–140.
- Martín-Fernández, J. A., Barceló-Vidal, C. and Pawlowsky-Glahn, V. (2003), Dealing with zeros and missing values in compositional data sets using nonparametric imputation, *Mathematical Geology* 35, 253–278.
- Zachary, W. W. (1977), An information flow model for conflict and fission in small groups, *Journal of Anthropological Research* 33, 452–473.
