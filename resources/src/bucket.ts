import { GetElementOrThrow } from "./helpers";
import { MappedFixtureConf } from "./fixtures";

export class FixtureBucket {
  template?: HTMLElement;
  area?: HTMLElement;

  constructor() {}

  Init() {
    this.area = GetElementOrThrow(".fixture-bucket-div");
    this.template = GetElementOrThrow(".element", this.area);
    this.area.removeChild(this.template);
  }

  Resize(l: number, fn: (el: HTMLElement, index: number) => void) {
    const e = this.area!.querySelectorAll<HTMLElement>(".element");
    for (let i = l; i < e.length; ++i) {
      this.area!.removeChild(e[i]);
    }

    for (let i = e.length; i < l; ++i) {
      const ne = this.template!.cloneNode(true) as HTMLElement;
      this.area!.appendChild(ne);
      fn(ne, i);
    }
  }

  ForEach(fn: (el: HTMLElement, index: number) => void) {
    {
      const e = this.area!.querySelectorAll<HTMLElement>(".element");
      e.forEach(fn);
    }
  }
}
