import {
  FixtureLocation,
  FixtureConf,
  FixtureDescription,
  FixtureProvides,
} from "./fixtures";
import { EventEmitter } from "events";
import { AddOrRemoveClass, GetElementOrThrow } from "./helpers";

export declare interface FixtureCells {
  on(
    event: "drop",
    listener: (
      sourceId: string,
      targetId: string | null,
      sourceLoc: FixtureLocation,
      targetLoc: FixtureLocation
    ) => void
  ): this;
  on(
    event: "click",
    listener: (elem: HTMLElement, loc: FixtureLocation) => void
  ): this;
}

export class FixtureCells extends EventEmitter {
  constructor() {
    super();
  }

  InitDraggable(elem: HTMLElement, loc: FixtureLocation) {
    elem.addEventListener("dragstart", (e: DragEvent) => {
      if (!elem.classList.contains("occupied")) {
        e.preventDefault();
        return;
      }

      elem.classList.add("dragged");
      e.dataTransfer!.dropEffect = "move";
      e.dataTransfer!.setData(
        "sourceFixtureId",
        elem.getAttribute("data-fixtureid") || ""
      );
      e.dataTransfer!.setData("sourceLocation", JSON.stringify(loc));
    });

    elem.addEventListener("dragend", (e: DragEvent) => {
      e.preventDefault();

      elem.classList.remove("dragged");
    });
  }

  InitDragTarget(elem: HTMLElement, loc: FixtureLocation) {
    elem.addEventListener("dragover", (e: DragEvent) => {
      if (!e.dataTransfer) {
        return false;
      }

      elem.classList.add("drag-target");
      e.preventDefault();
      e.dataTransfer.dropEffect = "move";
    });

    elem.addEventListener("dragleave", (e: DragEvent) => {
      elem.classList.remove("drag-target");
    });

    elem.addEventListener("drop", (e: DragEvent) => {
      elem.classList.remove("drag-target");
      if (!e.dataTransfer) {
        return false;
      }

      const sourceLocationStr = e.dataTransfer.getData("sourceLocation");
      const sourceId = e.dataTransfer.getData("sourceFixtureId");

      if (!sourceLocationStr || !sourceId) {
        return false;
      }

      const sourceLocation: FixtureLocation = JSON.parse(sourceLocationStr);

      const targetId = elem.getAttribute("data-fixtureid");
      this.emit("drop", sourceId, targetId, sourceLocation, loc);
    });
  }

  InitGridCell(elem: HTMLElement, loc: FixtureLocation) {
    this.InitDraggable(elem, loc);
    this.InitDragTarget(elem, loc);

    elem.addEventListener("click", (e: MouseEvent) => {
      this.emit("click", elem, loc);
    });
  }

  private GetDMXString(data: FixtureConf) {
    if (data.universe != -1 && data.channel != -1) {
      return `${data.universe}-${data.channel}`;
    } else {
      return "x";
    }
  }

  SetCellContents(
    el: HTMLElement,
    id?: string,
    data?: FixtureConf,
    desc?: FixtureDescription
  ) {
    AddOrRemoveClass(el, "occupied", data !== undefined);
    const nameElem = GetElementOrThrow(".name", el);
    const typeElem = GetElementOrThrow(".type", el);
    const dmxElem = GetElementOrThrow(".dmx", el);

    if (data && id) {
      nameElem.innerText = data.name;
      typeElem.innerText = data.type;
      dmxElem.innerText = this.GetDMXString(data);
      el.setAttribute("data-fixtureid", id);

      const lumElement = GetElementOrThrow(".icon.luma", el);
      const colorElement = GetElementOrThrow(".icon.color", el);
      const posElement = GetElementOrThrow(".icon.pos", el);
      AddOrRemoveClass(
        lumElement,
        "active",
        desc?.provides.indexOf(FixtureProvides.Dimmer) !== -1
      );
      AddOrRemoveClass(
        colorElement,
        "active",
        desc?.provides.indexOf(FixtureProvides.Color) !== -1
      );
      AddOrRemoveClass(
        posElement,
        "active",
        desc?.provides.indexOf(FixtureProvides.Position) !== -1
      );
    } else {
      nameElem.innerText = "";
      typeElem.innerText = "";
      dmxElem.innerText = "";
      el.removeAttribute("data-fixtureid");
    }
  }
}
