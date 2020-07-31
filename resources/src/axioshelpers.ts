import axios from "axios";

export async function PostJSON(url: string, payload: unknown) {
  return axios.post(url, JSON.stringify(payload), {
    headers: { "Content-Type": "application/json" },
  });
}
