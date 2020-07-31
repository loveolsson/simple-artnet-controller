import { EventEmitter } from "events";
import axios, { AxiosResponse, CancelToken, CancelTokenSource } from "axios";

export declare interface Subscriber<T> {
  on(event: "updated", listener: (data: T) => void): this;
}

export class Subscriber<T> extends EventEmitter {
  latestUpdate = "0";
  cancelToken?: CancelTokenSource;

  constructor(private url: string, private readonly interval: number) {
    super();
  }

  Start() {
    this.Update();
  }

  Reset(url?: string) {
    if (url) {
      this.url = url;
    }

    this.latestUpdate = "0";
    this.cancelToken?.cancel();
  }

  async Update() {
    try {
      this.cancelToken = axios.CancelToken.source();
      const res = await axios.get<T>(this.url + "/" + this.latestUpdate, {
        cancelToken: this.cancelToken.token,
      });
      this.latestUpdate = res.headers["simpleartnettime"];
      this.emit("updated", res.data);

      setTimeout(this.Update.bind(this), this.interval);
    } catch (e) {
      if (axios.isCancel(e)) {
        setTimeout(this.Update.bind(this), 0);
      } else if (e.response && (e.response as AxiosResponse).status === 304) {
        setTimeout(this.Update.bind(this), this.interval);
      } else {
        setTimeout(this.Update.bind(this), 5000);
      }
    }
  }
}
