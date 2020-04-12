import axios, {CancelTokenSource} from 'axios'
import {URLParam} from './urlparam';
import {UpdateValueElement, ValueInfoListItem} from './valueinfo';

class MainClass {
  cancelToken: CancelTokenSource = axios.CancelToken.source();
  readonly someRange: HTMLInputElement|null;

  constructor(private i: number) {
    this.someRange = document.querySelector('#someRange');
    if (this.someRange) {
      this.someRange.addEventListener('change', this.SetSomeRange.bind(this));
    }

    this.RegularUpdate();
  }

  private SetSomeRange() {
    console.log('Set data');
    const payload = {
      data: (this.someRange ? this.someRange.value : 0).toString()
    };
    this.PostJson(`/${this.i}/settings`, payload);
  }

  private async PostJson(url: string, payload: any) {
    this.cancelToken.cancel();
    this.cancelToken = axios.CancelToken.source();

    try {
      const {data} = await axios.post(
          url, JSON.stringify(payload),
          {headers: {'Content-Type': 'application/json'}});
      console.log('Successfull set', data);
    } catch (e) {
      console.error('Set value failed', e);
    }
  }

  private ParseUpdateData(items: ValueInfoListItem[]) {
    console.error('parse', items);

    items.forEach(item => {
      UpdateValueElement(this.someRange, item.info);
    });
  };

  private async RegularUpdate() {
    try {
      const {data} = await axios.get(
          `/${this.i}/settings`, {cancelToken: this.cancelToken.token});
      if (!data || typeof data !== 'object') {
        throw new Error('Bad data');
      }

      this.ParseUpdateData(data);
      setTimeout(this.RegularUpdate.bind(this), 2000);
    } catch (e) {
      setTimeout(this.RegularUpdate.bind(this), 5000);
    }
  }
}

window.addEventListener('load', () => {
  const cameraIndex = parseInt(URLParam('index') || '0');
  if (Number.isFinite(cameraIndex)) {
    console.log(`Page loaded for camera ${cameraIndex}`);
    new MainClass(cameraIndex);
  } else {
    console.error('Bad camera index');
  }
});