import { PostJSON } from "./axioshelpers";
import { GetElementOrThrow, GetParentOrThrow, CreateStyleTag } from "./helpers";
import { Subscriber } from "./subscriber";
import { create as createSlider, noUiSlider } from "nouislider";
import { readFileSync } from "fs";

interface FaderData {
  faders: Array<{
    value: number;
  }>;
}

export class Sliders {
  readonly rangeables: noUiSlider[] = [];
  userInteract: boolean[] = [];
  activePost: boolean[] = [];
  postAgain: boolean[] = [];
  faderSubscriber?: Subscriber<FaderData>;

  Init() {
    CreateStyleTag(
      readFileSync(
        __dirname + "/../node_modules/nouislider/distribute/nouislider.min.css",
        "utf-8"
      )
    );

    const sliderElement = GetElementOrThrow(".slider");
    const sliderParent = GetParentOrThrow(sliderElement);
    sliderParent.removeChild(sliderElement);

    for (let i = 0; i < 8; ++i) {
      const newElement = sliderElement.cloneNode(true) as HTMLElement;
      sliderParent.appendChild(newElement);

      const slider = createSlider(newElement, {
        start: 0,
        range: {
          min: 0,
          max: 65535,
        },
        orientation: "vertical",
        direction: "rtl",
        tooltips: false,
      });

      slider.on("start", this.FaderInteractionStart.bind(this, i));
      slider.on("end", this.FaderInteractionEnd.bind(this, i));
      slider.on("slide", this.FaderValueChanged.bind(this, i, true));

      this.rangeables.push(slider);
    }

    this.faderSubscriber = new Subscriber<FaderData>("/faders", 50);
    this.faderSubscriber.on("updated", this.UpdateFaders.bind(this));
    this.faderSubscriber.Start();
  }

  async UpdateFaders(data: FaderData) {
    data.faders.forEach((f, i) => {
      if (!this.userInteract[i]) {
        this.rangeables[i].set(f.value);
      }
    });
  }

  async SetFader(index: number) {
    if (this.activePost[index]) {
      this.postAgain[index] = true;
      return;
    }

    this.activePost[index] = true;
    try {
      await PostJSON("/faders", {
        index,
        value: parseInt(this.rangeables[index].get() as string),
      });
    } catch (e) {}
    this.activePost[index] = false;

    if (this.postAgain[index]) {
      setTimeout(this.SetFader.bind(this, index));
      this.postAgain[index] = false;
    }
  }

  FaderInteractionStart(index: number) {
    this.userInteract[index] = true;
  }

  FaderInteractionEnd(index: number) {
    this.SetFader(index);
    this.userInteract[index] = false;
    this.faderSubscriber?.Reset();
  }

  FaderValueChanged(index: number) {
    this.SetFader(index);
  }
}
