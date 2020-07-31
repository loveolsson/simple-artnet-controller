import { IconDefinition } from "@fortawesome/free-solid-svg-icons";
import { CreateStyleTag } from "./helpers";

function CreateSVGElement(n: string, v?: { [index: string]: string }) {
  const e = document.createElementNS(
    "http://www.w3.org/2000/svg",
    n
  ) as SVGElement;
  for (var p in v) e.setAttributeNS(null, p, v[p]);
  return e;
}

export function GetSVG(img: IconDefinition, fill = "black") {
  const svg = CreateSVGElement("svg", {
    viewBox: `0 0 ${img.icon[0]} ${img.icon[1]}`,
  });
  svg.appendChild(
    CreateSVGElement("path", { d: img.icon[4].toString(), fill })
  );
  return (
    'url("data:image/svg+xml;base64,' +
    window.btoa(new XMLSerializer().serializeToString(svg)) +
    '")'
  );
}

export function SetSVGBackground(
  elem: HTMLElement,
  img: IconDefinition,
  fill = "black"
) {
  const s = elem.style;
  s.backgroundImage = GetSVG(img, fill);
  s.backgroundSize = "contain";
  s.backgroundRepeat = "no-repeat";
  s.backgroundPosition = "center";
}

export function CreateSVGStyle(
  query: string,
  img: IconDefinition,
  fill = "black"
) {
  return CreateStyleTag(`${query} {
    background-image: ${GetSVG(img, fill)};
    background-size: contain;
    background-repeat: no-repeat;
    background-position: center;
  }`);
}
