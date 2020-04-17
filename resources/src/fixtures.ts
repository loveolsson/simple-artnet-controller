import axios, { AxiosResponse } from "axios";
import { GetElementOrThrow, GetParentOrThrow, DoAfterNth } from "./helpers";

export enum FixtureProvides {
  Dimmer = 'dimmer',
  Temperature = 'temperature',
  Color = 'color',
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

interface FixtureDescription {
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

export class Fixtures {
  descWaiter = new DoAfterNth(2, this.GotDescriptionsAndReady.bind(this));
  fixtureCreateTemplate?: HTMLElement;
  fixtureCreateArea?: HTMLElement;
  descriptions: FixtureDescriptions = {};

  constructor() {
    this.LoadDescriptions();
  }

  Init() {
    this.fixtureCreateTemplate = GetElementOrThrow('.create-fixture');
    this.fixtureCreateArea = GetParentOrThrow(this.fixtureCreateTemplate);
    this.fixtureCreateArea.removeChild(this.fixtureCreateTemplate);
    this.descWaiter.Done();
  }

  async LoadDescriptions() {
    const res = await axios.get<FixtureDescriptions>('/fixtureDescriptions');
    this.descriptions = res.data;
    this.descWaiter.Done();
  }

  GotDescriptionsAndReady() {
    this.RenderCreateList(this.fixtureCreateArea!, {x: -1, y: -1});
  }

  RenderCreateList(to: HTMLElement, location: FixtureLocation) {
    to.innerHTML = '';
    
    Object.keys(this.descriptions).forEach(key => {
      let clone = this.fixtureCreateTemplate!.cloneNode(true) as HTMLElement;
      clone.innerText = key;
      to.appendChild(clone);

      clone.addEventListener('click', () => {
        console.log("Create fixture", key);
      });
    });
  }
}