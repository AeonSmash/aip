import type { AipEnvelope, DestinationMappingDoc, MappedInterpretation } from "./types.js";

function capabilitiesOf(envelope: AipEnvelope): string[] {
  return envelope.capabilities ?? [];
}

function splitCapabilities(
  offered: string[],
  accepted: string[] | undefined,
  ignored: string[] | undefined,
): { acceptedCapabilities: string[]; ignoredCapabilities: string[] } {
  if (accepted || ignored) {
    const acceptedSet = new Set(accepted ?? []);
    const ignoredSet = new Set(ignored ?? []);
    return {
      acceptedCapabilities: offered.filter((c) => acceptedSet.has(c)),
      ignoredCapabilities: offered.filter((c) => ignoredSet.has(c) || !acceptedSet.has(c)),
    };
  }
  return { acceptedCapabilities: [], ignoredCapabilities: [...offered] };
}

function ruleMatches(
  envelope: AipEnvelope,
  rule: NonNullable<DestinationMappingDoc["rules"]>[number],
): boolean {
  const { match } = rule;
  if (match.type && match.type !== envelope.type) return false;
  if (match.sourceWorld && match.sourceWorld !== envelope.source.world) return false;
  if (match.powerTier?.length) {
    const tier = envelope.context?.power?.tier;
    if (!tier || !match.powerTier.includes(tier)) return false;
  }
  return true;
}

/**
 * Apply a destination-owned mapping document to a validated envelope.
 * Prefer `rules[]` when present; otherwise use the document's top-level interpretation
 * (compatible with the public 0.1 example mapping files).
 */
export function mapEnvelope(
  envelope: AipEnvelope,
  mappingDoc: unknown,
): MappedInterpretation {
  const doc = mappingDoc as DestinationMappingDoc;
  if (!doc || typeof doc !== "object" || !doc.destination) {
    throw new Error("Mapping document must include a destination field.");
  }

  let interpretation = doc.interpretation;
  if (doc.rules?.length) {
    const hit = doc.rules.find((rule) => ruleMatches(envelope, rule));
    if (!hit) {
      throw new Error(
        `No mapping rule matched envelope type=${envelope.type} world=${envelope.source.world} tier=${envelope.context?.power?.tier ?? "none"}`,
      );
    }
    interpretation = hit.interpretation;
  }

  if (!interpretation?.localType) {
    throw new Error("Mapping interpretation is missing localType.");
  }

  const offered = capabilitiesOf(envelope);
  const caps = splitCapabilities(
    offered,
    interpretation.acceptedCapabilities,
    interpretation.ignoredCapabilities,
  );

  return {
    destination: doc.destination,
    app: doc.app,
    envelopeId: envelope.id,
    principle: interpretation.principle ?? "destination-sovereignty",
    localType: interpretation.localType,
    localActor: interpretation.localActor,
    localEntity: interpretation.localEntity,
    slot: interpretation.slot,
    upgrade: interpretation.upgrade,
    damageMultiplier: interpretation.damageMultiplier,
    acceptedCapabilities: caps.acceptedCapabilities,
    ignoredCapabilities: caps.ignoredCapabilities,
    note: interpretation.note,
    source: {
      world: envelope.source.world,
      app: envelope.source.app,
      scene: envelope.source.scene,
      label: envelope.label,
      type: envelope.type,
      powerTier: envelope.context?.power?.tier,
    },
  };
}
