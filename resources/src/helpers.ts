export function GetElementOrThrow(
  query: string,
  from: HTMLElement = document.body
): HTMLElement {
  const elem = from.querySelector(query);

  if (!elem) {
    throw new Error(`Could not find element '${query}'`);
  }

  return elem as HTMLElement;
}

export function GetParentOrThrow(from: HTMLElement): HTMLElement {
  const elem = from.parentNode;

  if (!elem) {
    throw new Error(`Could not find parent for ${from.nodeName}`);
  }

  return elem as HTMLElement;
}

export class DoAfterNth {
  constructor(private num: number, private fn: () => void) {}

  OneDone() {
    if (--this.num === 0) {
      this.fn();
    }
  }
}

export function AddOrRemoveClass(elem: HTMLElement, cl: string, b: boolean) {
  if (b) {
    elem.classList.add(cl);
  } else {
    elem.classList.remove(cl);
  }
}

export function CreateStyleTag(d: string) {
  const e = document.createElement("style");
  e.innerHTML = d;
  document.head.appendChild(e);
  return e;
}

export const IntValMax = 65535;

export function ClampIntRange(val: number) {
  return Math.min(Math.max(val, 0), IntValMax);
}

export function ClampIntRangeNeg(val: number) {
  return Math.min(Math.max(val, -IntValMax), IntValMax);
}

export function IntValToPercent(val: number) {
  return ClampIntRange(val) / IntValMax * 100;
}

export function IntValToPercentNeg(val: number) {
  return ClampIntRangeNeg(val) / IntValMax * 100;
}