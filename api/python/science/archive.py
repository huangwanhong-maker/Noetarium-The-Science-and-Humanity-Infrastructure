"""Archives, for scripts in Python and the works' servers (ADR 0120): arrangement, custody, access, a sealed journal, BagIt packages
and the codebooks of datasets, asked of the engine a work has built.

The computations are archeion's C++, composed in ``api/cpp/science/archive.hpp``, which documents every operation:

    from science import archive
    archive.ask("arrange", units=[{"id": "F", "level": "fonds", "code": "F1", "title": "Papers"}])

The engine is a program built by a work: the Archive Studio builds ``data/cache/engines/archive-studio/archive/engine``;
``NOETARIUM_ARCHIVE_ENGINE`` names another. Each call is a process of its own.

A journal is written here, in Python, by archeion's own rule, so that a server can keep one as it works and the engine verify it:
each entry's seal is the SHA-256 of the seal before it, a zero byte, and its fields each followed by the unit separator (0x1F)
(``foundations/archeion/fixity/include/archeion/fixity/journal.hpp``).
"""

from __future__ import annotations

import hashlib

from .engines import Engine, EngineError, clean  # noqa: F401

ENGINE = Engine("archive-studio/archive", env="NOETARIUM_ARCHIVE_ENGINE")
FIELDS = ("when", "who", "what", "about", "note")
GENESIS = hashlib.sha256(b"").hexdigest()


def ask(op: str, timeout: float = 120.0, **request) -> dict:
    """One request to the archives engine: its answer, or EngineError with the engine's reason."""
    return ENGINE.ask(op, timeout=timeout, **request)


def path(p) -> str:
    """A file's path as the engine reads it (in WSL's form when it was built there): for a package's payload and a bag's files."""
    return ENGINE.path(p)


def seal(previous: str, entry: dict) -> str:
    """The seal of an entry after the one sealed `previous` (archeion::fixity::detail::seal_of)."""
    text = ""
    for f in FIELDS:
        v = str(entry.get(f, ""))
        if "\x1f" in v:
            raise ValueError("a journal's field may not hold the unit separator")
        text += v + "\x1f"
    return hashlib.sha256(previous.encode("utf-8") + b"\x00" + text.encode("utf-8")).hexdigest()


def sealed(previous: str, **entry) -> dict:
    """An entry with its seal."""
    e = {f: str(entry.get(f, "")) for f in FIELDS}
    e["seal"] = seal(previous, e)
    return e
