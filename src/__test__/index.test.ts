import path from 'path';

// @ts-ignore
import { NativeSound } from '..';

function delay(ms: number): Promise<void> {
  return new Promise(resolve => setTimeout(resolve, ms));
}

describe('Sound Test', () => {
  it('Play sound', async () => {
    const res = new NativeSound(path.join(__dirname, 'test.mp3'));
    await delay(3000);
    expect(res).not.toEqual(false);
  });
});
