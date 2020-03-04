import path from "path";
import { NativeSound } from "..";

function delay(ms: number) {
  return new Promise( resolve => setTimeout(resolve, ms) );
}

describe('Sound Test', () => {
  it ('Play sound', async () => {
    let res = new NativeSound(path.join(__dirname, "test.mp3"));
    await delay(3000);
    expect(res).not.toEqual(false);
  })
});
