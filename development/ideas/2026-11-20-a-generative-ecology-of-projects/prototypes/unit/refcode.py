"""Reference codes of a small register, such as GB-0042/3/7a.

A code names a repository (two letters of a country, a hyphen, four digits) and, beneath it, up to five
levels, each a number from 1 to 9999 that may carry one lower-case letter. The unit is deliberately
particular: its rules are conventions of this register and cannot be guessed from the name alone.
"""
import re
from functools import total_ordering

MAX_DEPTH = 5
_HEAD = re.compile(r"^([A-Za-z]{2})-([0-9]{4})$")
_LEVEL = re.compile(r"^([1-9][0-9]{0,3})([a-z]?)$")


class CodeError(ValueError):
    """A text that is not a reference code, or an operation that a code does not admit."""


@total_ordering
class Code:
    def __init__(self, country, repository, levels=()):
        self.country, self.repository, self.levels = country, repository, tuple(levels)

    def _key(self):
        return (self.country, self.repository, self.levels)

    def __eq__(self, other):
        return isinstance(other, Code) and self._key() == other._key()

    def __lt__(self, other):
        # Level by level: the number first, then the letter, no letter before "a".
        # A code sorts before every code beneath it, since a tuple sorts before its extensions.
        return self._key() < other._key()

    def __hash__(self):
        return hash(self._key())

    def __repr__(self):
        return f"Code({show(self)!r})"


def parse(text):
    """Reads a code. Outer spaces are dropped and the country is put in capitals; nothing else is repaired."""
    if not isinstance(text, str):
        raise CodeError("a code is given as text")
    parts = text.strip().split("/")
    head = _HEAD.match(parts[0])
    if not head:
        raise CodeError("a code begins with two letters, a hyphen and four digits")
    if head.group(2) == "0000":
        raise CodeError("no repository is numbered 0000")
    if len(parts) - 1 > MAX_DEPTH:
        raise CodeError(f"a code has at most {MAX_DEPTH} levels")
    levels = []
    for part in parts[1:]:
        level = _LEVEL.match(part)
        if not level:
            raise CodeError(f"not a level: {part!r}")
        levels.append((int(level.group(1)), level.group(2)))
    return Code(head.group(1).upper(), head.group(2), levels)


def show(code):
    """The canonical text of a code."""
    return "/".join([f"{code.country}-{code.repository}"] + [f"{n}{letter}" for n, letter in code.levels])


def depth(code):
    """The number of levels beneath the repository."""
    return len(code.levels)


def parent(code):
    """The code one level up, or None for a repository."""
    if not code.levels:
        return None
    return Code(code.country, code.repository, code.levels[:-1])


def is_ancestor(a, b):
    """Whether a stands strictly above b in one repository."""
    return (a.country, a.repository) == (b.country, b.repository) and len(a.levels) < len(b.levels) \
        and b.levels[:len(a.levels)] == a.levels


def common(a, b):
    """The deepest code that is each one's ancestor or itself, or None across repositories."""
    if (a.country, a.repository) != (b.country, b.repository):
        return None
    shared = []
    for x, y in zip(a.levels, b.levels):
        if x != y:
            break
        shared.append(x)
    return Code(a.country, a.repository, shared)


def next_sibling(code):
    """The next code at the same level: 7 gives 8, and 7a gives 7b. The last of either kind has none."""
    if not code.levels:
        raise CodeError("a repository has no sibling")
    number, letter = code.levels[-1]
    if letter:
        if letter == "z":
            raise CodeError("no letter follows z")
        last = (number, chr(ord(letter) + 1))
    else:
        if number == 9999:
            raise CodeError("no number follows 9999")
        last = (number + 1, "")
    return Code(code.country, code.repository, code.levels[:-1] + (last,))


def children(code, count):
    """The first `count` codes beneath a code, numbered from 1."""
    if len(code.levels) >= MAX_DEPTH:
        raise CodeError("a code at the last level has no children")
    if count < 0 or count > 9999:
        raise CodeError("a level holds from 0 to 9999 children")
    return [Code(code.country, code.repository, code.levels + ((n, ""),)) for n in range(1, count + 1)]
