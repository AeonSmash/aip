#!/usr/bin/env node
import { mkdirSync, readFileSync, readdirSync, writeFileSync } from "node:fs";
import { dirname, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { mapEnvelope } from "./map.js";
import { validateEnvelope } from "./validate.js";

const __dirname = dirname(fileURLToPath(import.meta.url));
const sdkRoot = resolve(__dirname, "..");
const repoRoot = resolve(sdkRoot, "../..");

function usage(): never {
  console.error(`AIP CLI 0.1

Usage:
  aip validate <envelope.json>
  aip map --destination <name> <envelope.json>
  aip exchange write-inbox <envelope.json>
  aip exchange list

Destinations:
  unreal          examples/unreal-local-mapping.json
  unreal-fps      mappings/unreal-fps.json
  decentraland    examples/decentraland-local-mapping.json
`);
  process.exit(1);
}

function loadJson(path: string): unknown {
  return JSON.parse(readFileSync(path, "utf8"));
}

function destinationPath(name: string): string {
  switch (name) {
    case "unreal":
      return resolve(repoRoot, "examples/unreal-local-mapping.json");
    case "unreal-fps":
      return resolve(repoRoot, "mappings/unreal-fps.json");
    case "decentraland":
      return resolve(repoRoot, "examples/decentraland-local-mapping.json");
    default:
      throw new Error(`Unknown destination "${name}". Use unreal | unreal-fps | decentraland.`);
  }
}

function schemaPath(): string {
  return resolve(repoRoot, "schemas/aip-envelope-0.1.schema.json");
}

function main(argv: string[]): void {
  const [cmd, ...rest] = argv;
  if (!cmd) usage();

  if (cmd === "validate") {
    const file = rest[0];
    if (!file) usage();
    const result = validateEnvelope(loadJson(resolve(file)), { schemaPath: schemaPath() });
    if (!result.ok) {
      console.error("INVALID");
      for (const err of result.errors) console.error(`- ${err}`);
      process.exit(2);
    }
    console.log("VALID");
    console.log(JSON.stringify(result.envelope, null, 2));
    return;
  }

  if (cmd === "map") {
    let destination = "";
    const files: string[] = [];
    for (let i = 0; i < rest.length; i++) {
      if (rest[i] === "--destination") {
        destination = rest[++i] ?? "";
      } else if (rest[i]) {
        files.push(rest[i]!);
      }
    }
    const file = files[0];
    if (!destination || !file) usage();

    const validated = validateEnvelope(loadJson(resolve(file)), { schemaPath: schemaPath() });
    if (!validated.ok) {
      console.error("INVALID envelope — refuse to map");
      for (const err of validated.errors) console.error(`- ${err}`);
      process.exit(2);
    }

    const mapped = mapEnvelope(validated.envelope, loadJson(destinationPath(destination)));
    console.log(JSON.stringify(mapped, null, 2));
    return;
  }

  if (cmd === "exchange") {
    const sub = rest[0];
    if (sub === "write-inbox") {
      const file = rest[1];
      if (!file) usage();
      const validated = validateEnvelope(loadJson(resolve(file)), { schemaPath: schemaPath() });
      if (!validated.ok) {
        console.error("INVALID — not writing inbox");
        for (const err of validated.errors) console.error(`- ${err}`);
        process.exit(2);
      }
      const id = String(validated.envelope.id).replace(/[^a-zA-Z0-9._-]/g, "_");
      const inbox = resolve(repoRoot, "exchange/inbox");
      mkdirSync(inbox, { recursive: true });
      const dest = resolve(inbox, `${id}.aip.json`);
      writeFileSync(dest, `${JSON.stringify(validated.envelope, null, 2)}\n`, "utf8");
      console.log(`wrote ${dest}`);
      return;
    }

    if (sub === "list") {
      for (const dir of ["inbox", "outbox"] as const) {
        const path = resolve(repoRoot, "exchange", dir);
        mkdirSync(path, { recursive: true });
        const files = readdirSync(path).filter((f) => f.endsWith(".aip.json"));
        console.log(`${dir}/ (${files.length})`);
        for (const f of files) console.log(`  ${f}`);
      }
      return;
    }
  }

  usage();
}

main(process.argv.slice(2));
