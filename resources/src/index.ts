import { Sliders } from "./sliders";
import { Fixtures } from "./fixtures";
import { Grid } from "./grid";

class MainClass {
  sliders: Sliders = new Sliders();
  fixtures: Fixtures = new Fixtures();
  grid: Grid = new Grid();

  constructor() {
  }

  Init() {
    this.sliders.Init();
    this.fixtures.Init();
    this.grid.Init();

    this.grid.Resize({x: 5, y: 5}); 

    document.body.classList.remove('hidden');
  }
}

const main = new MainClass();

window.addEventListener('load', () => {
  main.Init();
});