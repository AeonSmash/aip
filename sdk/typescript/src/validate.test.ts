import assert from "node:assert/strict";
import { readFileSync } from "node:fs";
import { dirname, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { describe, it } from "node:test";
import { mapEnvelope } from "./map.js";
import { validateEnvelope } from "./validate.js";

const here = dirname(fileURLToPath(import.meta.url));
const repoRoot = resolve(here, "../../..");
const schemaPath = resolve(repoRoot, "schemas/aip-envelope-0.1.schema.json");

function load(rel: string): unknown {
  return JSON.parse(readFileSync(resolve(repoRoot, rel), "utf8"));
}

describe("validateEnvelope", () => {
  it("accepts emberblade.aip.json", () => {
    const result = validateEnvelope(load("examples/emberblade.aip.json"), { schemaPath });
    assert.equal(result.ok, true);
    if (result.ok) {
      assert.equal(result.envelope.label, "Emberblade");
      assert.equal(result.envelope.context?.power?.tier, "high");
    }
  });

  it("accepts main-breaker.aip.json", () => {
    const result = validateEnvelope(load("examples/main-breaker.aip.json"), { schemaPath });
    assert.equal(result.ok, true);
    if (result.ok) {
      assert.equal(result.envelope.kind, "event");
      assert.equal(result.envelope.type, "signal.breaker");
      assert.equal(result.envelope.source.world, "web");
    }
  });

  it("rejects a mutated envelope with a readable reason", () => {
    const bad = load("examples/emberblade.aip.json") as Record<string, unknown>;
    bad.aip = "9.9";
    delete bad.source;
    const result = validateEnvelope(bad, { schemaPath });
    assert.equal(result.ok, false);
    if (!result.ok) {
      assert.ok(result.errors.length >= 1);
      const blob = result.errors.join("\n");
      assert.match(blob, /aip|source|required/i);
    }
  });
});

describe("mapEnvelope", () => {
  it("maps Emberblade for classic unreal destination", () => {
    const env = validateEnvelope(load("examples/emberblade.aip.json"), { schemaPath });
    assert.equal(env.ok, true);
    if (!env.ok) return;
    const mapped = mapEnvelope(env.envelope, load("examples/unreal-local-mapping.json"));
    assert.equal(mapped.localType, "cosmetic.emblem");
    assert.deepEqual(mapped.acceptedCapabilities, ["display"]);
    assert.ok(mapped.ignoredCapabilities.includes("equip"));
  });

  it("maps Main Breaker for unreal-fps LinkBeam unlock", () => {
    const env = validateEnvelope(load("examples/main-breaker.aip.json"), { schemaPath });
    assert.equal(env.ok, true);
    if (!env.ok) return;
    const mapped = mapEnvelope(env.envelope, load("mappings/unreal-fps.json"));
    assert.equal(mapped.localType, "weapon.linkbeam");
    assert.equal(mapped.upgrade, "unlock-linkbeam");
    assert.equal(mapped.damageMultiplier, 1.0);
    assert.ok(mapped.ignoredCapabilities.includes("equip"));
  });

  it("maps Emberblade for unreal-fps as notice only", () => {
    const env = validateEnvelope(load("examples/emberblade.aip.json"), { schemaPath });
    assert.equal(env.ok, true);
    if (!env.ok) return;
    const mapped = mapEnvelope(env.envelope, load("mappings/unreal-fps.json"));
    assert.equal(mapped.localType, "weapon.notice");
    assert.equal(mapped.upgrade, "none");
    assert.ok(mapped.ignoredCapabilities.includes("equip"));
  });

  it("maps an Unreal-origin achievement for decentraland", () => {
    const sigil = {
      aip: "0.1",
      kind: "event",
      id: "aip:unreal-reference:sigil:01",
      type: "achievement.sigil",
      label: "Arena Sigil",
      source: { world: "unreal", app: "aip-unreal-reference" },
      capabilities: ["display", "quest-flag", "equip"],
    };
    const env = validateEnvelope(sigil, { schemaPath });
    assert.equal(env.ok, true);
    if (!env.ok) return;
    const mapped = mapEnvelope(env.envelope, load("examples/decentraland-local-mapping.json"));
    assert.equal(mapped.localType, "collectible.museum");
    assert.ok(mapped.acceptedCapabilities.includes("display"));
  });
});
