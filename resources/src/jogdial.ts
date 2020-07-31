import { EventEmitter } from "events";
import { ClampIntRange, ClampIntRangeNeg, AddOrRemoveClass, IntValMax } from "./helpers";

export declare interface JogDial {
  on(event: "start", listener: () => void): this;
  on(event: "update", listener: (val: number) => void): this;
  on(event: "end", listener: (val: number) => void): this;
}

export class JogDial extends EventEmitter {
  value = 0;
  visualValue = 0;
  halves?: NodeListOf<HTMLElement>;
  activeTouch: number = NaN;
  previousTouchY = 0;

  constructor(private el: HTMLElement, private exponent: number, private minStep: number) {
    super();

    this.halves = el.querySelectorAll(".half");

    el.addEventListener("mousedown", this.HandleMouseDown.bind(this));
    el.addEventListener("touchstart", this.HandleTouchStart.bind(this));

    el.addEventListener("touchend", this.HandleTouchEnd);
    el.addEventListener("touchmove", this.HandleTouchMove);
  }

  private Start() {
    this.value = 0;
    this.emit("start");
  }

  private End() {
    this.visualValue %= this.halves![0].getBoundingClientRect().height;

    this.emit("end", this.value);
    this.value = 0;
    this.UpdateInterface();
  }

  private Update(relY: number) {
    this.visualValue += relY;
    relY *= window.devicePixelRatio;

    const v = Math.max(Math.round(Math.pow(Math.abs(relY), this.exponent)), this.minStep);

    if (relY < 0) {
      this.value += v;
    } else {
      this.value -= v;
    }

    this.value = ClampIntRangeNeg(this.value);

    this.UpdateInterface();
    this.emit("update", this.value);
  }

  private HandleMouseDown = (e: MouseEvent) => {
    this.el.requestPointerLock();

    this.Start();

    window.addEventListener("mouseup", this.HandleMouseUp);
    window.addEventListener("mousemove", this.HandleMouseMove);
    console.log("Mouse down");
  };

  private HandleMouseUp = (e: MouseEvent) => {
    document.exitPointerLock();

    this.End();

    console.log("Mouse up");
    window.removeEventListener("mouseup", this.HandleMouseUp);
    window.removeEventListener("mousemove", this.HandleMouseMove);
  };

  private HandleMouseMove = (e: MouseEvent) => {
    this.Update(e.movementY);
  };

  private HandleTouchStart = (e: TouchEvent) => {
    e.preventDefault();

    if (!Number.isNaN(this.activeTouch)) {
      return;
    }

    this.Start();

    this.activeTouch = e.targetTouches[0].identifier;
    this.previousTouchY = e.targetTouches[0].pageY;
  };

  private HandleTouchEnd = (e: TouchEvent) => {
    const arr = Array.from(e.changedTouches);

    if (!arr.find((t) => this.activeTouch === t.identifier)) {
      return;
    }

    this.End();

    this.activeTouch = NaN;
    e.preventDefault();
  };

  private HandleTouchMove = (e: TouchEvent) => {
    const arr = Array.from(e.changedTouches);

    const index = arr.findIndex((t) => this.activeTouch === t.identifier);
    if (index === -1) {
      return;
    }

    const touch = e.changedTouches.item(index)!;
    this.Update(touch.pageY - this.previousTouchY);
    this.previousTouchY = touch.pageY;

    e.preventDefault();
  };

  private UpdateInterface() {
    const bgPos = `0 ${this.visualValue}px`
    this.halves![0].style.backgroundPosition = bgPos;
    this.halves![1].style.backgroundPosition = bgPos;

    const max = Math.abs(this.value) === IntValMax;
    AddOrRemoveClass(this.el, 'max', max);
  }
}
