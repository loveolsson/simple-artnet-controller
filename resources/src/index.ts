import { Sliders } from "./sliders";
import { Fixtures } from "./fixtures";
import { Grid } from "./grid";
import { State } from "./state";
import { FixtureCells } from "./fixturecell";
import { FixtureBucket } from "./bucket";
import { JogDial } from "./jogdial";
import { GetElementOrThrow, IntValMax } from "./helpers";

class MainClass {
  state: State = new State();
  fixtureCells: FixtureCells = new FixtureCells();
  grid: Grid = new Grid();
  sliders: Sliders = new Sliders();
  bucket: FixtureBucket = new FixtureBucket();
  fixtures: Fixtures = new Fixtures(
    this.grid,
    this.state,
    this.fixtureCells,
    this.bucket
  );

  constructor() {}

  Init() {
    this.state.Init();
    this.grid.Init();
    this.sliders.Init();
    this.bucket.Init();
    this.fixtures.Init();

    document.onselectstart = () => {
      return false;
    };

    document.body.classList.remove("hidden");

    const dialEl = GetElementOrThrow(".dial.dimmer")
    const dialVal = GetElementOrThrow('.value', dialEl);
    const dial = new JogDial(dialEl, 2.7, 50);

    const setVal = (v: number) => {
      const drawVal = Math.round(v / IntValMax * 100);
      dialVal.innerText = `${drawVal}%`;
    }

    dial.on('start', setVal.bind(this, 0));
    dial.on('update', setVal);
    dial.on('end', () => {dialVal.innerText = '';});
  }
}

const main = new MainClass();

window.addEventListener("load", () => {
  main.Init();
});
