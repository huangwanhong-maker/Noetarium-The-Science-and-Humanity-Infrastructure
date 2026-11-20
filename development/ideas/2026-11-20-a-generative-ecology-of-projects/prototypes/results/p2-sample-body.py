import re


class CodeError(ValueError):
    pass


MAX_DEPTH = 5

_HEAD = re.compile('([A-Za-z]{2})-([0-9]{4})')
_LEVEL = re.compile('([1-9][0-9]{0,3})([a-z]?)')


def _check_code(code):
    if not isinstance(code, Code):
        raise CodeError('expected a Code')
    return code


class Code:
    __slots__ = ('country', 'repository', 'levels')

    def __init__(self, country, repository, levels=()):
        if not isinstance(country, str) or len(country) != 2 or not all('A' <= c <= 'Z' for c in country):
            raise CodeError('country must be two uppercase letters')
        if isinstance(repository, bool) or not isinstance(repository, int) or not 1 <= repository <= 9999:
            raise CodeError('repository must be an integer 1..9999')
        try:
            levels = tuple(levels)
        except TypeError:
            raise CodeError('levels must be iterable')
        if len(levels) > MAX_DEPTH:
            raise CodeError('too many levels')
        checked = []
        for lv in levels:
            if not isinstance(lv, tuple) or len(lv) != 2:
                raise CodeError('each level must be a (number, letter) pair')
            n, l = lv
            if isinstance(n, bool) or not isinstance(n, int) or not 1 <= n <= 9999:
                raise CodeError('level number must be 1..9999')
            if not isinstance(l, str) or not (l == '' or (len(l) == 1 and 'a' <= l <= 'z')):
                raise CodeError('level letter must be empty or a lowercase letter')
            checked.append((n, l))
        object.__setattr__(self, 'country', country)
        object.__setattr__(self, 'repository', repository)
        object.__setattr__(self, 'levels', tuple(checked))

    def __setattr__(self, name, value):
        raise AttributeError('Code is immutable')

    def _key(self):
        return (self.country, self.repository, self.levels)

    def __eq__(self, other):
        if not isinstance(other, Code):
            return NotImplemented
        return self._key() == other._key()

    def __ne__(self, other):
        if not isinstance(other, Code):
            return NotImplemented
        return self._key() != other._key()

    def __hash__(self):
        return hash(self._key())

    def __lt__(self, other):
        if not isinstance(other, Code):
            return NotImplemented
        return self._key() < other._key()

    def __le__(self, other):
        if not isinstance(other, Code):
            return NotImplemented
        return self._key() <= other._key()

    def __gt__(self, other):
        if not isinstance(other, Code):
            return NotImplemented
        return self._key() > other._key()

    def __ge__(self, other):
        if not isinstance(other, Code):
            return NotImplemented
        return self._key() >= other._key()

    def __repr__(self):
        return 'Code(%r)' % show(self)


def parse(text):
    if not isinstance(text, str):
        raise CodeError('code must be a str')
    parts = text.strip().split('/')
    m = _HEAD.fullmatch(parts[0])
    if not m:
        raise CodeError('bad head in code: %r' % text)
    repo = int(m.group(2))
    if repo == 0:
        raise CodeError('repository 0000 is not allowed')
    country = m.group(1).upper()
    if len(parts) - 1 > MAX_DEPTH:
        raise CodeError('more than %d levels' % MAX_DEPTH)
    levels = []
    for p in parts[1:]:
        m2 = _LEVEL.fullmatch(p)
        if not m2:
            raise CodeError('bad level %r' % p)
        levels.append((int(m2.group(1)), m2.group(2)))
    return Code(country, repo, levels)


def show(code):
    _check_code(code)
    s = '%s-%04d' % (code.country, code.repository)
    for n, l in code.levels:
        s += '/%d%s' % (n, l)
    return s


def depth(code):
    _check_code(code)
    return len(code.levels)


def parent(code):
    _check_code(code)
    if not code.levels:
        return None
    return Code(code.country, code.repository, code.levels[:-1])


def is_ancestor(a, b):
    _check_code(a)
    _check_code(b)
    if a.country != b.country or a.repository != b.repository:
        return False
    if len(a.levels) >= len(b.levels):
        return False
    return b.levels[:len(a.levels)] == a.levels


def common(a, b):
    _check_code(a)
    _check_code(b)
    if a.country != b.country or a.repository != b.repository:
        return None
    shared = []
    for x, y in zip(a.levels, b.levels):
        if x != y:
            break
        shared.append(x)
    return Code(a.country, a.repository, tuple(shared))


def next_sibling(code):
    _check_code(code)
    if not code.levels:
        raise CodeError('a repository has no sibling')
    n, l = code.levels[-1]
    if l == '':
        if n == 9999:
            raise CodeError('no sibling after 9999')
        new = (n + 1, '')
    else:
        if l == 'z':
            raise CodeError('no sibling after z')
        new = (n, chr(ord(l) + 1))
    return Code(code.country, code.repository, code.levels[:-1] + (new,))


def children(code, count):
    _check_code(code)
    if len(code.levels) >= MAX_DEPTH:
        raise CodeError('maximum depth reached')
    if isinstance(count, bool) or not isinstance(count, int) or not 0 <= count <= 9999:
        raise CodeError('count must be an integer 0..9999')
    return [Code(code.country, code.repository, code.levels + ((i, ''),)) for i in range(1, count + 1)]
