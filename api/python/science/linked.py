"""Linked data, for scripts in Python and the works' servers (ADR 0118): documents of RDF given as text, asked of the engine a work has
built.

The computations are symploke's C++ (RDF's terms and documents, a store, SPARQL's core, entailment, SHACL, isomorphism), composed in
``api/cpp/science/linked.hpp``, which documents every operation. A script states what it asks and asks; it does not compute these again.

    from science import linked
    doc = linked.document(text, "turtle")
    linked.ask("query", documents=[doc], query="SELECT ?s WHERE { ?s a ?c }")

The engine is a program built by a work: the Archive Studio builds ``data/cache/engines/archive-studio/linked/engine``;
``NOETARIUM_LINKED_ENGINE`` names another. Each call is a process of its own, so a request that fails harms no one.
"""

from __future__ import annotations

from .engines import Engine, EngineError, clean  # noqa: F401

ENGINE = Engine("archive-studio/linked", env="NOETARIUM_LINKED_ENGINE")
FORMATS = {".ttl": "turtle", ".trig": "trig", ".nt": "ntriples", ".nq": "nquads", ".jsonld": "jsonld"}


def ask(op: str, timeout: float = 120.0, **request) -> dict:
    """One request to the linked-data engine: its answer, or EngineError with the engine's reason."""
    return ENGINE.ask(op, timeout=timeout, **request)


def document(text: str, format: str = "turtle", base: str | None = None, graph: str | None = None, contexts: dict | None = None) -> dict:
    """A document as the engine reads it; a JSON-LD document's remote contexts are given in ``contexts`` (IRI: JSON text)."""
    d = {"text": text, "format": format}
    if contexts:
        d["contexts"] = contexts
    if base:
        d["base"] = base
    if graph:
        d["graph"] = graph
    return d


def iri(value: str) -> dict:
    return {"kind": "iri", "value": value}


def literal(value: str, datatype: str | None = None, language: str | None = None) -> dict:
    t = {"kind": "literal", "value": value, "datatype": datatype or "http://www.w3.org/2001/XMLSchema#string"}
    if language:
        t["language"] = language
        t["datatype"] = "http://www.w3.org/1999/02/22-rdf-syntax-ns#langString"
    return t


def blank(label: str) -> dict:
    return {"kind": "blank", "value": label}
