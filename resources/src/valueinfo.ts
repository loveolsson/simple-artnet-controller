export interface ValueInfoOption {
  name: string;
  value: string;
}

export interface ValueInfo {
  numInt?: number;
  minInt?: number;
  maxInt?: number;
  stpInt?: number;
  numFloat?: number;
  minFloat?: number;
  maxFloat?: number;
  stpFloat?: number;
  str?: string;
  opt?: ValueInfoOption[];
}

export interface ValueInfoListItem {
  name: string;
  info: ValueInfo;
}

export interface Value {
  numInt?: number;
  numFloat?: number;
  str?: number;
}

export interface ValueSetInfo {
  name: string;
  value: Value;
}

export function UpdateValueElement(
    elem: HTMLInputElement|null, valueInfo: ValueInfo) {
  if (!elem) {
    return;
  }

  if (valueInfo.numInt !== undefined && Number.isFinite(valueInfo.numInt)) {
    if (valueInfo.minInt !== undefined && Number.isFinite(valueInfo.minInt)) {
      if (elem.getAttribute('min') != valueInfo.minInt.toString()) {
        elem.setAttribute('min', valueInfo.minInt.toString());
      }
    }

    if (valueInfo.maxInt !== undefined && Number.isFinite(valueInfo.maxInt)) {
      if (elem.getAttribute('max') != valueInfo.maxInt.toString()) {
        elem.setAttribute('max', valueInfo.maxInt.toString());
      }
    }
    if (valueInfo.stpInt !== undefined && Number.isFinite(valueInfo.stpInt)) {
      if (elem.getAttribute('step') != valueInfo.stpInt.toString()) {
        elem.setAttribute('step', valueInfo.stpInt.toString());
      }
    }

    if (parseInt(elem.value, 10) != valueInfo.numInt) {
      elem.value = valueInfo.numInt.toString();
    }

  } else if (typeof valueInfo.str === 'string') {
    if (Array.isArray(valueInfo.opt) &&
        elem.tagName.toLowerCase() === 'select') {
      const optStr = JSON.stringify(valueInfo.opt);
      if (optStr !== elem.getAttribute('data-storedOpt')) {
        console.log('Updating select options');

        while (elem.firstChild) {
          elem.removeChild(elem.firstChild);
        }

        valueInfo.opt.forEach(o => {
          console.log(o);
          elem.append(new Option(o.name, o.value));
        });
        elem.setAttribute('data-storedOpt', optStr);
      }
    }

    if (elem.value !== valueInfo.str) {
      elem.value = valueInfo.str;
    }
  }
}