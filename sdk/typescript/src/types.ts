export type AipKind = "object" | "event" | "identity";

export interface AipSource {
  world: string;
  app: string;
  scene?: string;
  issuedAt?: string;
}

export interface AipProvenance {
  origin?: string;
  discoveredBy?: string;
  history?: string[];
}

export interface AipPowerContext {
  scale: "relative";
  tier: "low" | "mid" | "high" | "exceptional";
  basis?: string;
}

export interface AipEnvelope {
  aip: "0.1";
  kind: AipKind;
  id: string;
  type: string;
  label: string;
  source: AipSource;
  provenance?: AipProvenance;
  context?: {
    power?: AipPowerContext;
  };
  capabilities?: string[];
  rights?: {
    owner?: string;
    transferable?: boolean;
    displayable?: boolean;
  };
  representation?: {
    glb?: string;
  };
}

export interface DestinationMappingDoc {
  destination: string;
  app?: string;
  envelopeId?: string;
  interpretation: {
    principle?: string;
    note?: string;
    localType: string;
    localActor?: string;
    localEntity?: string;
    slot?: string;
    upgrade?: string;
    damageMultiplier?: number;
    acceptedCapabilities?: string[];
    ignoredCapabilities?: string[];
  };
  rules?: Array<{
    match: {
      type?: string;
      powerTier?: string[];
      sourceWorld?: string;
    };
    interpretation: DestinationMappingDoc["interpretation"];
  }>;
}

export interface MappedInterpretation {
  destination: string;
  app?: string;
  envelopeId: string;
  principle: string;
  localType: string;
  localActor?: string;
  localEntity?: string;
  slot?: string;
  upgrade?: string;
  damageMultiplier?: number;
  acceptedCapabilities: string[];
  ignoredCapabilities: string[];
  note?: string;
  source: {
    world: string;
    app: string;
    scene?: string;
    label: string;
    type: string;
    powerTier?: string;
  };
}

export type ValidateResult =
  | { ok: true; envelope: AipEnvelope }
  | { ok: false; errors: string[] };
