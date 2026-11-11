"""A new column: computed from the others by a formula."""

import ast

import numpy as np

FUNCTIONS = {"log": np.log, "log10": np.log10, "exp": np.exp, "sqrt": np.sqrt, "abs": np.abs, "round": np.round, "least": np.minimum, "greatest": np.maximum,
             "where": np.where, "isnan": np.isnan, "floor": np.floor, "ceil": np.ceil}
ALLOWED = (ast.Expression, ast.BinOp, ast.UnaryOp, ast.Compare, ast.BoolOp, ast.Call, ast.Name, ast.Constant, ast.Load, ast.Add, ast.Sub, ast.Mult, ast.Div, ast.Pow, ast.Mod, ast.FloorDiv,
           ast.USub, ast.UAdd, ast.Lt, ast.LtE, ast.Gt, ast.GtE, ast.Eq, ast.NotEq, ast.BitAnd, ast.BitOr, ast.Invert)


def evaluate(expression: str, names: dict):
    """An arithmetic formula of columns, quantities and a few functions; nothing else of Python is read."""
    tree = ast.parse(expression.strip(), mode="eval")
    for node in ast.walk(tree):
        if not isinstance(node, ALLOWED):
            raise ValueError(f"a formula is arithmetic of columns, numbers and {', '.join(FUNCTIONS)}: not '{type(node).__name__}'")
        if isinstance(node, ast.Constant) and not isinstance(node.value, (int, float)):
            raise ValueError("a formula's constants are numbers")
        if isinstance(node, ast.Call) and not (isinstance(node.func, ast.Name) and node.func.id in FUNCTIONS and not node.keywords):
            raise ValueError(f"a formula may call {', '.join(FUNCTIONS)}")
        if isinstance(node, ast.Name) and node.id not in names and node.id not in FUNCTIONS:
            raise LookupError(f"the formula names '{node.id}', which is neither a column of numbers nor a quantity (there are: {', '.join(names)})")
    with np.errstate(all="ignore"):
        return eval(compile(tree, "<formula>", "eval"), {"__builtins__": {}}, {**FUNCTIONS, **names})  # noqa: S307 (the tree was checked node by node)


def create(api):
    class Compute(api.BoardAnalysis):
        id = "compute"
        title = "A new column"
        description = ("A table with one more column, computed row by row from a formula of its columns of numbers, the project's quantities and numbers: "
                       "sales / hectares, log(people), where(dwellings > 0, cafes / dwellings, 0). What cannot be computed for a row is left empty.")
        params = [api.param("table", "Table", "", kind="text", role="table"),
                  api.param("column", "The new column", "value", kind="text"),
                  api.param("formula", "Is", "", kind="text", help="arithmetic of columns and quantities by name; log, exp, sqrt, abs, round, least, greatest, where"),
                  api.param("name", "Call the table", "computed", kind="text")]

        def run(self, board, params, context):
            t = board.table(params["table"])
            if not str(params.get("formula") or "").strip():
                raise ValueError("say what the new column is: a formula")
            names = {q.name: q.mid for q in board.quantities.values()}
            names.update({c: t.column(c) for c in t.numeric() if c.isidentifier()})
            value = np.broadcast_to(np.asarray(evaluate(params["formula"], names), dtype=float), (t.rows,))
            column = str(params.get("column") or "value")
            columns = {c: t.raw(c) for c in t.columns}
            columns[column] = [float(v) if np.isfinite(v) else None for v in value]
            roles = dict(getattr(t, "roles", None) or {})
            places = getattr(t, "places", None)
            kind = "areas" if roles.get("cell") and places else "points" if roles.get("lat") and roles.get("lon") else None
            return [api.table(params.get("name") or "computed", columns, title=f"{t.title or t.name}, with {column} = {params['formula'].strip()}", roles={**roles, "amount": column}, places=places,
                              show={"kind": "layer", "layer": kind, "colour": column, **({"size": column} if kind == "points" else {})} if kind else None)]
    return Compute()
