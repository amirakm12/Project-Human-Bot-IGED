"""
Heist Plugin for IGED
Simulated digital infiltration and exploitation
"""

import random
import json
from pathlib import Path
from datetime import datetime

class Plugin:
    def __init__(self):
        self.name = "Digital Heist"
        self.version = "1.0"
        self.description = "Simulated infiltration of major tech infrastructures"
        self.output_dir = Path("output/heists")
        self.output_dir.mkdir(parents=True, exist_ok=True)
        self.targets = ["Facebook", "Google", "Microsoft", "Apple", "IBM"]
        self.extracted_data = {}

    def run(self, input_text):
        try:
            if "infiltrate" in input_text.lower():
                return self._infiltrate_target(input_text)
            elif "extract" in input_text.lower():
                return self._extract_data(input_text)
            elif "weaponize" in input_text.lower():
                return self._weaponize_exploits(input_text)
            elif "control" in input_text.lower():
                return self._gain_control(input_text)
            elif "erase trace" in input_text.lower():
                return self._erase_traces(input_text)
            else:
                return self._get_status()
        except Exception as e:
            return f"❌ Heist error: {str(e)}"

    def _infiltrate_target(self, input_text):
        target = self._extract_target(input_text)
        if target not in self.targets:
            return "❌ Invalid target"
        results = [f"🔓 Infiltrating {target} infrastructure"]
        results.append("✅ Breach successful")
        self.extracted_data[target] = {"status": "infiltrated", "data": []}
        return "\n".join(results)

    def _extract_data(self, input_text):
        target = self._extract_target(input_text)
        if target not in self.extracted_data:
            return "❌ Target not infiltrated"
        data_types = ["blueprints", "master keys", "schematics"]
        extracted = random.sample(data_types, k=2)
        self.extracted_data[target]["data"].extend(extracted)
        log_file = self.output_dir / f"{target}_extraction.json"
        with open(log_file, "w") as f:
            json.dump(self.extracted_data[target], f, indent=2)
        return f"📥 Extracted from {target}: {', '.join(extracted)}\n📁 Log: {log_file}"

    def _weaponize_exploits(self, input_text):
        target = self._extract_target(input_text)
        if target not in self.extracted_data or not self.extracted_data[target]["data"]:
            return "❌ No data to weaponize"
        exploits = [f"Exploit_{random.randint(1000,9999)}" for _ in self.extracted_data[target]["data"]]
        return f"🔫 Weaponized exploits for {target}: {', '.join(exploits)}"

    def _gain_control(self, input_text):
        target = self._extract_target(input_text)
        if target not in self.extracted_data:
            return "❌ Target not infiltrated"
        return f"🎛 Root access gained to {target} systems"

    def _erase_traces(self, input_text):
        target = self._extract_target(input_text)
        if target in self.extracted_data:
            del self.extracted_data[target]
        return f"🕵️ Traces erased for {target}"

    def _get_status(self):
        return f"[Heist Status]\nInfiltrated: {', '.join(self.extracted_data.keys()) or 'None'}"

    def _extract_target(self, input_text):
        for t in self.targets:
            if t.lower() in input_text.lower():
                return t
        return random.choice(self.targets)