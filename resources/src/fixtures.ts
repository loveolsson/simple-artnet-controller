import axios, { AxiosResponse } from "axios";
import { GetElementOrThrow, AddOrRemoveClass, DoAfterNth } from "./helpers";
import { Grid } from "./grid";
import { Subscriber } from "./subscriber";
import { PostJSON } from "./axioshelpers";
import { State, SidebarState } from "./state";
import { FixtureCreater } from "./fixturecreater";
import { FixtureCells } from "./fixturecell";
import { FixtureBucket } from "./bucket";

export enum FixtureProvides {
  Dimmer = "dimmer",
  Temperature = "temperature",
  Color = "color",
  Position = "position",
}

export enum FixtureParameters {
  Zero = 0,
  One,
  Full,
  Dimmer,
  Red,
  Green,
  Blue,
  WW,
  CW,
  Count,
}

export enum FixtureColorMatrix {
  None = 0,
  RGB,
  RGBA,
  RGBW,
  DimRGB,
  DimRGBA,
  DimRGBW,
  TempApprox,
  DimTempApprox,
}

export interface FixtureDescription {
  provides: FixtureProvides[];
  channels: FixtureParameters[];
  color_matrix?: FixtureColorMatrix;
}

interface FixtureDescriptions {
  [index: string]: FixtureDescription;
}

export interface FixtureLocation {
  x: number;
  y: number;
}

export const FixtureLocationUnset: FixtureLocation = { x: -1, y: -1 };

export function FixtureLocationEq(a: FixtureLocation, b: FixtureLocation) {
  return a.x === b.x && a.y === b.y;
}

export interface FixtureConf {
  channel: number;
  location: FixtureLocation;
  name: string;
  universe: number;
  type: string;
}

interface FixtureConfs {
  [index: string]: FixtureConf;
}

export interface MappedFixtureConf {
  key: string;
  conf: FixtureConf;
  desc?: FixtureDescription;
}

export class Fixtures {
  fixtureCreater = new FixtureCreater(this);
  fixtureSubscriber = new Subscriber<FixtureConfs>("/fixtures", 50);
  descriptions: FixtureDescriptions = {};
  selected: string[] = [];
  waiter = new DoAfterNth(2, () => this.fixtureSubscriber.Start());
  lastFixtureState = SidebarState.Dimmer;

  constructor(
    private grid: Grid,
    private state: State,
    private fixtureCells: FixtureCells,
    private bucket: FixtureBucket
  ) {
    this.LoadDescriptions();
    this.fixtureCells.on("drop", this.HandleFixtureMoveDrop.bind(this));
    this.fixtureCells.on("click", this.HandleFixtureClick.bind(this));
  }

  Init() {
    this.fixtureSubscriber.on("updated", this.UpdateFixturesInGrid.bind(this));
    this.fixtureCreater.Init();

    this.state.on("stateChange", (state) => {
      this.UpdateVisual();

      if (
        state >= SidebarState.Dimmer &&
        state < SidebarState.LastFixtureState
      ) {
        this.lastFixtureState = state;
      }
    });

    this.grid.Resize({ x: 20, y: 20 }, (el, loc) =>
      this.fixtureCells.InitGridCell(el, loc)
    );

    this.fixtureCells.InitDragTarget(this.state.sidebar!, FixtureLocationUnset);
    this.waiter.OneDone();
  }

  async LoadDescriptions() {
    const res = await axios.get<FixtureDescriptions>("/fixtureDescriptions/0");
    this.descriptions = res.data;
    this.fixtureCreater.descWaiter.OneDone();
    this.waiter.OneDone();
  }

  UpdateFixturesInGrid(conf: FixtureConfs) {
    const lookup: MappedFixtureConf[] = Object.keys(conf).map((key) => {
      return {
        key,
        conf: conf[key],
        desc: this.descriptions[conf[key].type],
      };
    });

    this.grid.ForEach((el, loc) => {
      const find = lookup.find((c) => FixtureLocationEq(c.conf.location, loc));
      this.fixtureCells.SetCellContents(el, find?.key, find?.conf, find?.desc);
    });

    const inBucket = lookup.filter((l) =>
      FixtureLocationEq(l.conf.location, FixtureLocationUnset)
    );

    const nameSort = <T>(a: T, b: T) => {
      return a === b ? 0 : a < b ? -1 : 1;
    };

    inBucket.sort((a, b) => {
      const t = nameSort(a.conf.type, b.conf.type);
      if (t != 0) {
        return t;
      }

      return nameSort(a.conf.name, b.conf.name);
    });

    this.bucket.Resize(inBucket.length, (e, i) =>
      this.fixtureCells.InitDraggable(e, FixtureLocationUnset)
    );

    this.bucket.ForEach((e, i) => {
      const d = inBucket[i];
      this.fixtureCells.SetCellContents(e, d.key, d.conf, d.desc);
    });

    this.UpdateVisual();
  }

  HandleFixtureMoveDrop(
    sourceId: string,
    targetId: string | null,
    sourceLoc: FixtureLocation,
    targetLoc: FixtureLocation
  ) {
    const url = "/setFixtureLocation";

    PostJSON(url, {
      id: sourceId,
      location: targetLoc,
    });

    if (targetId) {
      PostJSON(url, {
        id: targetId,
        location: sourceLoc,
      });
    }
  }

  ToggleSelected(id: string) {
    const pos = this.selected.indexOf(id);

    if (pos !== -1) {
      this.selected.splice(pos, 1);
    } else {
      this.selected.push(id);
    }

    this.state.SetSidebarState(this.lastFixtureState);
  }

  OpenInsertDialog(loc: FixtureLocation) {
    this.fixtureCreater.SetOrToggleInsertLocation(loc);
    this.state.SetSidebarState(SidebarState.CreateFixture);
  }

  HandleFixtureClick(elem: HTMLElement, loc: FixtureLocation) {
    const id = elem.getAttribute("data-fixtureid");
    if (id) {
      this.ToggleSelected(id);
    } else {
      this.OpenInsertDialog(loc);
    }
  }

  UpdateFixtureVisual(el: HTMLElement, loc: FixtureLocation) {
    const id = el.getAttribute("data-fixtureid");

    const isSelected: boolean = id ? this.selected.indexOf(id) !== -1 : false;
    AddOrRemoveClass(el, "selected", isSelected);

    const isCreateTarget = FixtureLocationEq(
      loc,
      this.fixtureCreater.insertLocation
    );
    AddOrRemoveClass(el, "insert-target", isCreateTarget);
  }

  UpdateVisual() {
    this.grid.ForEach(this.UpdateFixtureVisual.bind(this));
  }
}
