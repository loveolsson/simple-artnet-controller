import { GetElementOrThrow, GetParentOrThrow } from "./helpers";
import { FixtureLocation } from "./fixtures";

export class Grid {
  table?: HTMLElement;
  templateCell?: HTMLElement;
  currentSize: FixtureLocation = { x: 0, y: 0 };

  Init() {
    this.table = GetElementOrThrow(".fixture-grid-container table tbody");
    this.templateCell = GetElementOrThrow("td", this.table);

    const tr = GetParentOrThrow(this.templateCell);
    tr.removeChild(this.templateCell);
  }

  GetElement(loc: FixtureLocation): HTMLElement | undefined {
    const tr = this.table?.querySelector(`tr:nth-of-type(${loc.y + 1})`);
    return tr?.querySelector(`td:nth-of-type(${loc.x + 1})`) as HTMLElement;
  }

  Resize(
    size: FixtureLocation,
    fn: (el: HTMLElement, location: FixtureLocation) => void
  ) {
    this.currentSize = size;
    while (this.table!.querySelectorAll("tr").length > size.y) {
      const children = this.table!.querySelectorAll("tr");
      this.table!.removeChild(children[children.length - 1]);
    }

    while (this.table!.querySelectorAll("tr").length < size.y) {
      const row = document.createElement("tr");
      this.table!.append(row);
    }

    this.table!.querySelectorAll("tr").forEach((row, y) =>
      this.FillRow(row as HTMLElement, y, fn)
    );
  }

  FillRow(
    row: HTMLElement,
    y: number,
    fn: (el: HTMLElement, location: FixtureLocation) => void
  ) {
    let x: number;
    while ((x = row.querySelectorAll("td").length) < this.currentSize.x) {
      const newNode = this.templateCell!.cloneNode(true) as HTMLElement;
      row.append(newNode);
      fn(newNode, { x, y });
    }

    while (row.querySelectorAll("td").length > this.currentSize.x) {
      const children = row.querySelectorAll("td");
      row.removeChild(children[children.length - 1]);
    }
  }

  ForEach(fn: (el: HTMLElement, location: FixtureLocation) => void) {
    this.table!.querySelectorAll("tr").forEach(
      (row: HTMLElement, y: number) => {
        row
          .querySelectorAll("td")
          .forEach((ts: HTMLElement, x: number) => fn(ts, { x, y }));
      }
    );
  }
}
