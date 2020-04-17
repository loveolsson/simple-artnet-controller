import { GetElementOrThrow, GetParentOrThrow } from "./helpers";
import { FixtureLocation } from "./fixtures";

export class Grid {
  table?: HTMLElement;
  templateCell?: HTMLElement;
  currentSize: FixtureLocation = {x: 0, y: 0};

  Init() {
    this.table = GetElementOrThrow('.fixture-grid-container table tbody');
    this.templateCell = GetElementOrThrow('td', this.table);

    const tr = GetParentOrThrow(this.templateCell);
    tr.removeChild(this.templateCell);
  }

  GetElement(loc: FixtureLocation) : HTMLElement | undefined {
    const tr = this.table?.querySelector(`tr:nth-of-type(${loc.y + 1})`);
    return tr?.querySelector(`td:nth-of-type(${loc.x + 1})`) as HTMLElement;
  }

  Resize(size: FixtureLocation) {
    this.currentSize = size;
    while (this.table!.querySelectorAll('tr').length > size.y) {
      const children = this.table!.querySelectorAll('tr');
      this.table?.removeChild(children[children.length - 1]);
    }

    while (this.table!.querySelectorAll('tr').length < size.y) {
      const row = document.createElement('tr');
      this.table?.append(row);
    }

    this.table!.querySelectorAll('tr').forEach((row, y) => this.FillRow(row as HTMLElement, y))
  }

  IsValidDraggable(elem: HTMLElement) {
    if (!document.body.classList.contains('dragging')) {
      return false;
    }

    if (!elem.getAttribute('data-fixtureid')) {
      return false;
    }

    return true;
  }

  InitDraggable(elem: HTMLElement, x: number, y: number) {
    elem.addEventListener('dragstart', (e: DragEvent) => { 
      if (!this.IsValidDraggable(elem)) {
        //e.preventDefault();
        //return;  
      }

      elem.classList.add('dragged'); 
      e.dataTransfer!.dropEffect = "move";
      e.dataTransfer!.setData("location", JSON.stringify([x, y]));
    });

    elem.addEventListener('dragend', (e: DragEvent) => { 
      elem.classList.remove('dragged'); 
    });

    elem.addEventListener('click', (e: MouseEvent) => { 
      elem.classList.toggle('selected'); 
    });
  }

  InitDragTarget(elem: HTMLElement, x: number, y: number) {
    elem.addEventListener('dragover', (e: DragEvent) => {  
      elem.classList.add('drag-target'); 
      e.preventDefault();
      e.dataTransfer!.dropEffect = "move";
    });

    elem.addEventListener('dragleave', (e: DragEvent) => { 
      elem.classList.remove('drag-target');
    });

    elem.addEventListener('drop', (e: DragEvent) => { 
      elem.classList.remove('drag-target');
      const data = JSON.parse(e.dataTransfer!.getData("location"));

      console.log(`Move from ${data[0]},${data[1]} to ${x},${y}`); 
    });
  }

  FillRow(row: HTMLElement, y: number) {
    let x:number;
    while((x = row.querySelectorAll('td').length) < this.currentSize.x) {
      const newNode = this.templateCell!.cloneNode(true) as HTMLElement;
      row.append(newNode);
      newNode.innerText = `${x}, ${y}`;
      this.InitDraggable(newNode, x, y);
      this.InitDragTarget(newNode, x, y);
    }

    while(row.querySelectorAll('td').length > this.currentSize.x) {
      const children = row.querySelectorAll('td');
      row.removeChild(children[children.length - 1]);
    }
  }
}