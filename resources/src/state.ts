import {
  GetElementOrThrow,
  AddOrRemoveClass,
  GetParentOrThrow,
} from "./helpers";
import { EventEmitter } from "events";
import {
  IconDefinition,
  faPlus,
  faPalette,
  faArchive,
  faNetworkWired,
  faSun,
  faArrowsAlt,
  faLightbulb,
} from "@fortawesome/free-solid-svg-icons";
import { SetSVGBackground, CreateSVGStyle } from "./svg";

export enum SidebarState {
  None,
  Dimmer,
  Color,
  Position,
  LastFixtureState,
  CreateFixture,
  Patch,
  Achive,
}

interface ButtonInfo {
  elem: HTMLElement;
  state: SidebarState;
}

export declare interface State {
  on(event: "stateChange", listener: (data: SidebarState) => void): this;
}

export class State extends EventEmitter {
  currentSidebarState = SidebarState.None;
  sidebar?: HTMLElement;
  buttons: ButtonInfo[] = [];

  Init() {
    this.sidebar = GetElementOrThrow(".sidebar");
    const buttonTemplate = GetElementOrThrow(".buttons > button");
    const buttonParent = GetParentOrThrow(buttonTemplate);
    buttonParent.removeChild(buttonTemplate);

    CreateSVGStyle(".bg-sun", faLightbulb, "black");
    CreateSVGStyle(".bg-palette", faPalette, "black");
    CreateSVGStyle(".bg-arrows", faArrowsAlt, "black");
    CreateSVGStyle(".bg-plus", faPlus, "black");
    CreateSVGStyle(".bg-networkwired", faNetworkWired, "black");
    CreateSVGStyle(".bg-archive", faArchive, "black");
    CreateSVGStyle(".bg-plus-green", faPlus, "limegreen");

    const clone = (icon: string, state: SidebarState) => {
      const elem = buttonTemplate.cloneNode(true) as HTMLElement;
      const ic = GetElementOrThrow(".icon", elem);
      ic.classList.add(icon);
      elem.addEventListener("click", () => {
        this.SetSidebarState(state);
      });
      elem.setAttribute("data-state", state.toString());
      this.buttons.push({ elem, state });
      buttonParent.append(elem);
    };

    clone("bg-sun", SidebarState.Dimmer);
    clone("bg-palette", SidebarState.Color);
    clone("bg-arrows", SidebarState.Position);
    clone("bg-plus", SidebarState.CreateFixture);
    clone("bg-networkwired", SidebarState.Patch);
    clone("bg-archive", SidebarState.Achive);

    this.UpdateNavigationButtons();
  }

  UpdateNavigationButtons() {
    this.buttons.forEach((info) => {
      if (this.currentSidebarState === info.state) {
        info.elem.classList.add("btn-primary");
        info.elem.classList.remove("btn-outline-primary");
      } else {
        info.elem.classList.add("btn-outline-primary");
        info.elem.classList.remove("btn-primary");
      }
    });
  }

  SetSidebarState(state: SidebarState) {
    this.currentSidebarState = state;
    AddOrRemoveClass(
      document.body,
      "create-fixture",
      state === SidebarState.CreateFixture
    );
    AddOrRemoveClass(
      document.body,
      "fixture-dimmer",
      state === SidebarState.Dimmer
    );
    AddOrRemoveClass(
      document.body,
      "fixture-bucket",
      state === SidebarState.Achive
    );

    this.emit("stateChange", state);
    this.UpdateNavigationButtons();
  }
}
