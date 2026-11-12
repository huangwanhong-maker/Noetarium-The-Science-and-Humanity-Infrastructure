// What one may write: the language of scripts, for the page's guide. The grammar itself is kategoria's
// (foundations/kategoria/script/include/kategoria/script/language.hpp); GET /api/grammar gives it in BNF.

const pre = (text) => `<pre>${text.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;')}</pre>`;

export const GUIDE = `
<p>A script is a sequence of statements, one to a line; a statement with a block in braces runs to the brace that closes it. Inside a block, items are separated by new lines or by <code>;</code>. A <code>#</code> begins a remark, which the sheet shows in its place. A name is letters, digits, <code>_</code> and <code>'</code>, or anything between double quotes.</p>
<p>Composition is written <code>g . f</code> (g after f; also ∘) or <code>f >> g</code> (f then g). The tensor is <code>*</code> (also ⊗). An arrow is <code>-></code> (also →), a transformation <code>=></code> (also ⇒), an identity <code>id A</code>.</p>

<h3>Categories</h3>
<p>By generators and equations. The engine completes the equations into rewriting rules; where that ends and the arrows are finitely many, the category is built whole.</p>
${pre(`category Sq {
  objects A B C D
  f : A -> B
  g : B -> D
  h : A -> C
  k : C -> D
  g . f = k . h
}`)}
<p>From a family, or made from others.</p>
${pre(`category C = chain 4          discrete 3 · subsets 3 · divisors 12 · finset 2 · simplex 2 · mult 6
category G = symmetric 3      cyclic 4 · dihedral 4 · quaternion
category I = arrow            iso · pair · point
category P = product C D      sum C D · op C · skeleton C · arrows C · functors C D
category S = slice C A        coslice C A · comma F G · elements X · subobjects X · algebras L · kleisli L`)}

<h3>Functors and natural transformations</h3>
<p>A functor is given on as many objects and generators as fix it; the rest follows. It is refused where it breaks an equation, and left open where more than one functor fits.</p>
${pre(`functor F : Sq -> I { A -> 0; B -> 0; C -> 1; D -> 1; g -> "0≤1"; h -> "0≤1" }
functor G = identity C        compose G F · constant C D X · diagonal C · first P · second P · projection E
functor R = right adjoint F   left adjoint F
transformation t : F => G { A -> u; B -> v }`)}

<h3>Set-valued functors</h3>
<p>A set for each object and a function for each generator: data on a schema.</p>
${pre(`sets X : S {
  Employee = {ann, bo}
  Department = {maths}
  works : ann -> maths, bo -> maths
}
sets H = hom C A              constant C 2 · omega C
sets L = lan X K              ran X K · pullback Y K`)}

<h3>Theories, models and diagrams</h3>
<p>A symmetric monoidal theory has types, generators with wires in and out (<code>I</code> for none), and laws. A model gives each type a dimension and each generator a function (among sets, by its values in order) or a matrix (rows separated by <code>|</code>).</p>
${pre(`theory Monoid {
  type x
  m : x x -> x
  e : I -> x
  law associativity : (m * x) >> m = (x * m) >> m
  law unit : (e * x) >> m = x
}
model And : Monoid in sets { x = 2; m = 0 0 0 1; e = 1 }
model Line : Monoid in sums { x = 1; m = [1 1]; e = [] }     in sets · relations · matrices · sums, then: mod 2 · boolean
diagram d = (m * x) >> m in Monoid                            swap(x, y) is the symmetry`)}

<h3>Claims</h3>
<p>Each is answered <em>holds</em> with its evidence, <em>fails</em> with the place where it fails, or <em>not decided</em> with the reason.</p>
${pre(`check g . f = k . h in Sq          equal paths, with the derivation; or equal arrows of a family
check F                            whatever F names is what it was declared as
check F is faithful                full · essentially surjective · an equivalence
check f is mono in C               epi · iso
check C is complete                cocomplete · cartesian closed · thin · a groupoid · discrete · connected · skeletal
check L -| R                       an adjunction, with its unit and counit
check C equivalent D               check C isomorphic D (categories, functors, set-valued functors)
check yoneda X A                   check classifier X
check d = d2 in Monoid             equal as diagrams: by the axioms of symmetric monoidal categories alone
check d = d2 in And                equal in a model`)}

<h3>Questions</h3>
${pre(`find product A B in C              coproduct · equalizer f g · coequalizer f g · pullback f g · pushout f g
find terminal in C                 initial · exponential A B (B to the power A)
find limit F                       colimit F, of a functor read as a diagram
find functors C -> D               find transformations F => G
find generators C                  rules C · table C
find sections X                    classes X · subfunctors X`)}
`;
