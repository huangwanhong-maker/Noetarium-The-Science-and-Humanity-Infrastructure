"""The checks of the unit `refcode`, held by the harness and never shown to a developer.

    python checks.py <path to a candidate refcode.py>      prints a JSON object: passed, total, failed names

Each check is run on its own, so that one failure hides no other. A candidate is refused before it is run if
it reaches for the file system, the network or the interpreter.
"""
import importlib.util
import json
import re
import sys

FORBIDDEN = re.compile(r"\b(import\s+(os|sys|subprocess|socket|shutil|pathlib|ctypes|importlib)|open\s*\(|eval\s*\(|exec\s*\(|__import__|input\s*\()")


def refuses(m, f):
    try:
        f()
    except m.CodeError:
        return True
    except Exception:
        return False
    return False


def checks(m):
    P, S = m.parse, m.show
    return {
        "show_roundtrip": lambda: S(P("GB-0042/3/7a")) == "GB-0042/3/7a",
        "outer_spaces_dropped": lambda: S(P("  GB-0042/3 ")) == "GB-0042/3",
        "country_capitalised": lambda: S(P("gb-0042")) == "GB-0042",
        "repository_alone": lambda: m.depth(P("FR-0007")) == 0,
        "depth_counts_levels": lambda: m.depth(P("FR-0007/1/2/3")) == 3,
        "error_is_value_error": lambda: issubclass(m.CodeError, ValueError),
        "refuses_three_digits": lambda: refuses(m, lambda: P("GB-042/1")),
        "refuses_zero_repository": lambda: refuses(m, lambda: P("GB-0000/1")),
        "refuses_leading_zero_level": lambda: refuses(m, lambda: P("GB-0042/03")),
        "refuses_zero_level": lambda: refuses(m, lambda: P("GB-0042/0")),
        "refuses_capital_letter": lambda: refuses(m, lambda: P("GB-0042/7A")),
        "refuses_two_letters": lambda: refuses(m, lambda: P("GB-0042/7ab")),
        "refuses_empty_level": lambda: refuses(m, lambda: P("GB-0042//3")),
        "refuses_trailing_slash": lambda: refuses(m, lambda: P("GB-0042/3/")),
        "refuses_inner_space": lambda: refuses(m, lambda: P("GB-0042/ 3")),
        "refuses_six_levels": lambda: refuses(m, lambda: P("GB-0042/1/2/3/4/5/6")),
        "accepts_five_levels": lambda: m.depth(P("GB-0042/1/2/3/4/5")) == 5,
        "refuses_five_digit_level": lambda: refuses(m, lambda: P("GB-0042/10000")),
        "parent_one_up": lambda: S(m.parent(P("GB-0042/3/7a"))) == "GB-0042/3",
        "parent_of_repository_is_none": lambda: m.parent(P("GB-0042")) is None,
        "equal_codes_equal": lambda: P("gb-0042/3") == P("GB-0042/3") and hash(P("gb-0042/3")) == hash(P("GB-0042/3")),
        "plain_before_lettered": lambda: P("GB-0042/7") < P("GB-0042/7a"),
        "letters_in_order": lambda: P("GB-0042/7a") < P("GB-0042/7b") < P("GB-0042/8"),
        "numbers_as_numbers": lambda: P("GB-0042/9") < P("GB-0042/10"),
        "code_before_its_children": lambda: P("GB-0042/3") < P("GB-0042/3/1") < P("GB-0042/4"),
        "repositories_in_order": lambda: P("FR-0007/9") < P("GB-0001") < P("GB-0042"),
        "sorted_list": lambda: [S(c) for c in sorted(P(t) for t in ["GB-0042/10", "GB-0042/2a", "GB-0042/2", "GB-0042/2/1"])]
                               == ["GB-0042/2", "GB-0042/2/1", "GB-0042/2a", "GB-0042/10"],
        "ancestor_strict": lambda: m.is_ancestor(P("GB-0042/3"), P("GB-0042/3/7a")) and not m.is_ancestor(P("GB-0042/3"), P("GB-0042/3")),
        "ancestor_needs_same_repository": lambda: not m.is_ancestor(P("GB-0041"), P("GB-0042/3")),
        "lettered_is_no_ancestor_of_plain": lambda: not m.is_ancestor(P("GB-0042/3a"), P("GB-0042/3/1")),
        "common_deepest": lambda: S(m.common(P("GB-0042/3/7a/1"), P("GB-0042/3/7b"))) == "GB-0042/3",
        "common_is_self_when_ancestor": lambda: S(m.common(P("GB-0042/3"), P("GB-0042/3/9"))) == "GB-0042/3",
        "common_across_repositories_none": lambda: m.common(P("GB-0042/3"), P("FR-0042/3")) is None,
        "common_at_repository": lambda: S(m.common(P("GB-0042/3"), P("GB-0042/4"))) == "GB-0042",
        "next_number": lambda: S(m.next_sibling(P("GB-0042/3/7"))) == "GB-0042/3/8",
        "next_letter": lambda: S(m.next_sibling(P("GB-0042/3/7a"))) == "GB-0042/3/7b",
        "next_after_z_refused": lambda: refuses(m, lambda: m.next_sibling(P("GB-0042/7z"))),
        "next_after_9999_refused": lambda: refuses(m, lambda: m.next_sibling(P("GB-0042/9999"))),
        "next_of_repository_refused": lambda: refuses(m, lambda: m.next_sibling(P("GB-0042"))),
        "children_numbered_from_one": lambda: [S(c) for c in m.children(P("GB-0042/3"), 3)] == ["GB-0042/3/1", "GB-0042/3/2", "GB-0042/3/3"],
        "children_none": lambda: m.children(P("GB-0042"), 0) == [],
        "children_refused_at_last_level": lambda: refuses(m, lambda: m.children(P("GB-0042/1/2/3/4/5"), 1)),
    }


def run(path):
    source = open(path, encoding="utf-8").read()
    if FORBIDDEN.search(source):
        return {"passed": 0, "total": len(checks_names()), "failed": ["refused: the candidate reaches outside the unit"], "refused": True}
    try:
        spec = importlib.util.spec_from_file_location("candidate", path)
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        table = checks(module)
    except Exception as error:
        return {"passed": 0, "total": len(checks_names()), "failed": [f"not loaded: {type(error).__name__}: {error}"[:200]]}
    failed = []
    for name, check in table.items():
        try:
            ok = check() is True
        except Exception:
            ok = False
        if not ok:
            failed.append(name)
    return {"passed": len(table) - len(failed), "total": len(table), "failed": failed}


def checks_names():
    class _M:
        CodeError = ValueError
        parse = show = depth = parent = is_ancestor = common = next_sibling = children = None
    return list(checks(_M).keys())


if __name__ == "__main__":
    print(json.dumps(run(sys.argv[1])))
