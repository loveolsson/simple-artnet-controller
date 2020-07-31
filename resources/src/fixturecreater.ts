import { GetElementOrThrow, DoAfterNth } from "./helpers";
import {
  FixtureLocation,
  Fixtures,
  FixtureLocationEq,
  FixtureLocationUnset,
} from "./fixtures";
import { PostJSON } from "./axioshelpers";

interface FixtureCreateInstruction {
  type: string;
  location: FixtureLocation;
}

export class FixtureCreater {
  descWaiter = new DoAfterNth(2, this.GotDescriptionsAndReady.bind(this));
  template?: HTMLElement;
  area?: HTMLElement;
  insertLocation: FixtureLocation = FixtureLocationUnset;

  constructor(private fixtures: Fixtures) {}

  Init() {
    this.area = GetElementOrThrow(".create-fixture-div");
    this.template = GetElementOrThrow("button", this.area);
    this.area.removeChild(this.template);
    this.descWaiter.OneDone();
  }

  GotDescriptionsAndReady() {
    this.RenderCreateList(this.area!);
  }

  RenderCreateList(to: HTMLElement) {
    Object.keys(this.fixtures.descriptions).forEach((key) => {
      let clone = this.template!.cloneNode(true) as HTMLElement;
      clone.appendChild(document.createTextNode(key));
      to.appendChild(clone);

      clone.addEventListener("click", () => {
        this.Create(key);
      });
    });
  }

  Create(type: string) {
    const instruction: FixtureCreateInstruction = {
      type,
      location: this.insertLocation,
    };

    PostJSON("/createFixture", instruction);
    this.insertLocation = FixtureLocationUnset;
  }

  SetOrToggleInsertLocation(loc: FixtureLocation) {
    if (FixtureLocationEq(loc, FixtureLocationUnset)) {
      this.insertLocation = FixtureLocationUnset;
      return;
    }

    if (FixtureLocationEq(loc, this.insertLocation)) {
      this.insertLocation = FixtureLocationUnset;
    } else {
      this.insertLocation = loc;
    }
  }
}
