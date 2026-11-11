// Elements made in a line: h for HTML, s for SVG. Attributes whose value is null, undefined or false
// are left out; `on…` attributes are listeners; children may be nodes, strings or lists of them.

const SVG = 'http://www.w3.org/2000/svg';

function fill(node, attrs, kids) {
  for (const [k, v] of Object.entries(attrs || {})) {
    if (v === null || v === undefined || v === false) continue;
    if (k.startsWith('on') && typeof v === 'function') node.addEventListener(k.slice(2), v);
    else node.setAttribute(k, v === true ? '' : v);
  }
  for (const kid of kids.flat(Infinity)) if (kid !== null && kid !== undefined && kid !== false) node.append(kid instanceof Node ? kid : document.createTextNode(String(kid)));
  return node;
}

export const h = (tag, attrs, ...kids) => fill(document.createElement(tag), attrs, kids);
export const s = (tag, attrs, ...kids) => fill(document.createElementNS(SVG, tag), attrs, kids);
export const clear = (node) => { while (node.firstChild) node.removeChild(node.firstChild); return node; };
