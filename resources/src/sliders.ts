import axios, {AxiosResponse} from 'axios'
import { PostJSON } from './axioshelpers';
import { GetElementOrThrow, GetParentOrThrow } from './helpers';

interface FaderData {
  faders: Array<{
    value: number;
  }>;
}

export class Sliders {
  readonly sliders: HTMLInputElement[] = [];
  userInteract: boolean[] = [];
  activePost: boolean[] = [];
  postAgain: boolean[] = [];

  latestUpdate: string = '0';

  Init() {
    const sliderElement = GetElementOrThrow('.slider');
    const sliderParent = GetParentOrThrow(sliderElement);

    sliderParent.removeChild(sliderElement);
    for (let i = 0; i < 8; ++i) {
      const newElement = sliderElement.cloneNode(true) as HTMLElement;
      sliderParent.appendChild(newElement);

      const input = newElement.querySelector('input');
      if (!input) {
        throw new Error('Slider element has no input');
      }

      input.addEventListener('mousedown', this.MouseDown.bind(this, i));
      input.addEventListener('mouseup', this.MouseUp.bind(this, i));
      input.addEventListener('touchstart', this.MouseDown.bind(this, i));
      input.addEventListener('touchend', this.MouseUp.bind(this, i));
      input.addEventListener('input', this.Input.bind(this, i, false));
      input.addEventListener('change', this.Input.bind(this, i, true));

      this.sliders.push(input);
    }

    this.Update();
  }

  async Update() {
    try {
      const res: AxiosResponse<FaderData> = await axios.get('/faders?since='+ this.latestUpdate);
      this.latestUpdate = res.headers['simpleartnettime'];
      res.data.faders.forEach((f, i) => {
        if (!this.userInteract[i]) {
          this.sliders[i].value = f.value.toString();
        }
      });

      setTimeout(this.Update.bind(this), this.GetUpdateInterval(false));
    } catch (e) {
      if (e.response && (e.response as AxiosResponse).status === 304) {
        setTimeout(this.Update.bind(this), this.GetUpdateInterval(false));
      } else {
        setTimeout(this.Update.bind(this), this.GetUpdateInterval(true));
      }
    }
  }

  async SetFader(index: number) {
    if (this.activePost[index]) {
      this.postAgain[index] = true;
      return;
    }

    this.activePost[index] = true;
    try {
      await PostJSON('/faders', { index, value: parseInt(this.sliders[index].value) });
    } catch (e) {
    }
    this.activePost[index] = false;

    if (this.postAgain[index]) {
      setTimeout(this.SetFader.bind(this, index));
      this.postAgain[index] = false;
    }
  }

  MouseDown(index: number) {
    this.userInteract[index] = true;
  }

  MouseUp(index: number) {
    this.userInteract[index] = false;
    this.latestUpdate = '0';
  }

  Input(index: number, final: boolean) {
    this.SetFader(index);
  }

  GetUpdateInterval(error: boolean) {
    if (error) {
      return this.IsActive() ? 2000 : 5000;
    } else {
      return this.IsActive() ? 250 : 5000;
    }
  }

  IsActive() {
    return !document.body.classList.contains("sliders");
  }
}