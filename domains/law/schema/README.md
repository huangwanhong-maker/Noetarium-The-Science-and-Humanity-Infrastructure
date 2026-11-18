# The Schema of Facts

[facts.cat](facts.cat) states the domain's taxonomy of facts as categories, in the language of the foundation kategoria, which answers each statement with a verdict. A statement marked `# expect fails` is meant to fail: the script holds both what is so and what is not.

| Part | States |
|---|---|
| Registers | the three registers as a chain ending in the ontological |
| Kinds | the kinds of fact as an order ending in Fact; `InRegister` carries it into the registers |
| Standing, Reached, Forget | standing as a free category that is not thin; its thin quotient; the functor between them, full and not faithful |
| Roles | evidentiary, intermediate, ultimate |
| Elements | custom as the product of a practice and its acceptance |
| Record, Sample, MindAsTrace, AcrossProceedings | a record of facts as a set-valued functor; a small record in which one holding is the trace of two facts; two records refused |

[theories.cat](theories.cat) states a theory of the case the same way (ADR 0127): the registers; the layers of a theory (a holding, an item read from it, an account it bears on, what happened), ending in what happened and lying in the registers in order; a theory's record (events of kinds in accounts, parts played in roles, items read from holdings of a modality, stances taken in accounts, bindings), thin, with no arrow by which an item names an event; two accounts of one fire on it; an account in which someone is the actor of a fault, and one kept as though it were a trace, refused. Its checks are `theories/tests/check_schema.cpp`, which also verify accounts written by the domain as functors on it.

The checks (`facts/tests/check_schema.cpp`) run the script, hold the C++'s order of kinds to the schema's pair by pair, and verify records written by `facts/schema.hpp` as functors. The diagrams and kategoria's answers are in [figures](figures/README.md).
