import { readFileSync } from "node:fs";
import { createRequire } from "node:module";
import type { AipEnvelope, ValidateResult } from "./types.js";

export interface ValidateOptions {
  /** Absolute path to schemas/aip-envelope-0.1.schema.json */
  schemaPath: string;
}

type ValidateFunction = ((data: unknown) => boolean) & {
  errors: Array<{
    instancePath: string;
    message?: string;
    params?: { allowedValues?: string[] };
  }> | null;
};

const require = createRequire(import.meta.url);
const Ajv2020 = require("ajv/dist/2020.js") as new (opts?: object) => {
  compile: (schema: object) => ValidateFunction;
};
const addFormats = require("ajv-formats") as (ajv: unknown) => void;

const cache = new Map<string, ValidateFunction>();

function getValidator(schemaPath: string): ValidateFunction {
  const hit = cache.get(schemaPath);
  if (hit) return hit;
  const ajv = new Ajv2020({ allErrors: true, strict: false });
  addFormats(ajv);
  const schema = JSON.parse(readFileSync(schemaPath, "utf8")) as object;
  const validate = ajv.compile(schema);
  cache.set(schemaPath, validate);
  return validate;
}

/**
 * Validate an unknown JSON value against AIP envelope 0.1.
 * Returns a readable error list on failure (never throws for schema misses).
 */
export function validateEnvelope(data: unknown, options: ValidateOptions): ValidateResult {
  const validate = getValidator(options.schemaPath);
  if (validate(data)) {
    return { ok: true, envelope: data as AipEnvelope };
  }

  const errors: string[] = [];
  for (const err of validate.errors ?? []) {
    const path = err.instancePath || "/";
    const msg = err.message ?? "failed validation";
    const allowed = err.params?.allowedValues;
    const extra = allowed ? ` (allowed: ${allowed.join(", ")})` : "";
    errors.push(`${path}: ${msg}${extra}`);
  }

  if (errors.length === 0) {
    errors.push("Envelope failed schema validation.");
  }

  return { ok: false, errors };
}
