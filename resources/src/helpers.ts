export function GetElementOrThrow(query: string, from: HTMLElement = document.body) : HTMLElement {
  const elem = from.querySelector(query);
  
  if (!elem) {
    throw new Error(`Could not find element '${query}'`);
  }

  return elem as HTMLElement;
}

export function GetParentOrThrow(from: HTMLElement) : HTMLElement {
  const elem = from.parentNode;
  
  if (!elem) {
    throw new Error(`Could not find parent for ${from.nodeName}`);
  }

  return elem as HTMLElement;
}

export class DoAfterNth {
  constructor(private num: number, private fn: ()=>void) {}

  Done() {
    if (--this.num === 0) {
      this.fn();
    }
  }
}