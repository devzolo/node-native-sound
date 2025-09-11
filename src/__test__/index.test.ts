import assert from "node:assert";
import path from "node:path";
import { describe, it } from "node:test";

import { NativeSound } from "../index.ts";

function delay(ms: number): Promise<void> {
	return new Promise((resolve) => setTimeout(resolve, ms));
}

describe("Sound Test", () => {
	it("Play sound", async () => {
		const res = new NativeSound(path.join(import.meta.dirname, "test.mp3"));
		await delay(3000);
		assert.notEqual(res, false);
	});
});
