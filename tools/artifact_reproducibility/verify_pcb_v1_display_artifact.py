from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import sys


REPOSITORY = Path(__file__).resolve().parents[2]
DEFAULT_MANIFEST = (
    REPOSITORY
    / "tests"
    / "build"
    / "pcb-v1-minimal-display-smoke-test-artifact-manifest.json"
)
APP_BASENAME = "pcb_v1_minimal_display_smoke_test"
PREDECESSOR_ROLE = "pcb-v1-minimal-display-smoke-test"
SUCCESSOR_ROLE = "pcb-v1-display-backlight-validation"
SOURCE_INPUTS = (
    "firmware/CMakeLists.txt",
    "firmware/sdkconfig.defaults",
    "firmware/main/CMakeLists.txt",
    "firmware/main/idf_component.yml",
    "firmware/main/main.c",
    "firmware/main/pcb_v1_display.c",
    "firmware/main/pcb_v1_display.h",
    "firmware/main/pcb_v1_display_test_pattern.c",
    "firmware/main/pcb_v1_display_test_pattern.h",
)


class VerificationError(RuntimeError):
    pass


def require(condition: bool, message: str) -> None:
    if not condition:
        raise VerificationError(message)


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest().upper()


def source_inventory() -> dict[str, object]:
    files: list[dict[str, object]] = []
    for relative in SOURCE_INPUTS:
        path = REPOSITORY / relative
        require(path.is_file(), f"source input is missing: {relative}")
        files.append(
            {
                "path": relative,
                "bytes": path.stat().st_size,
                "sha256": sha256(path),
            }
        )

    canonical = "".join(
        f"{item['path']}\0{item['sha256']}\n" for item in files
    ).encode("utf-8")
    return {
        "algorithm": "sha256(path + NUL + uppercase-file-sha256 + LF)",
        "aggregate_sha256": hashlib.sha256(canonical).hexdigest().upper(),
        "files": files,
    }


def git_output(*arguments: str) -> str:
    result = subprocess.run(
        ["git", *arguments],
        cwd=REPOSITORY,
        check=True,
        capture_output=True,
        text=True,
    )
    return result.stdout.strip()


def git_is_ancestor(ancestor: str, descendant: str) -> bool:
    result = subprocess.run(
        ["git", "merge-base", "--is-ancestor", ancestor, descendant],
        cwd=REPOSITORY,
        check=False,
        capture_output=True,
        text=True,
    )
    return result.returncode == 0


def load_manifest(path: Path) -> dict[str, object]:
    require(path.is_file(), f"manifest is missing: {path}")
    return json.loads(path.read_text(encoding="utf-8"))


def verify_manifest_only(manifest: dict[str, object]) -> dict[str, object]:
    require(manifest.get("schema_version") == 2, "manifest schema must be 2")
    role = manifest.get("artifact_role")
    require(
        role in (PREDECESSOR_ROLE, SUCCESSOR_ROLE),
        "unsupported artifact role",
    )

    source = manifest["source"]
    current_head = git_output("rev-parse", "HEAD")
    if role == PREDECESSOR_ROLE:
        require(source["head"] == current_head, "HEAD mismatch")
        require(
            source["app_version"]
            == git_output("describe", "--always", "--tags", "--dirty"),
            "app version mismatch",
        )
    else:
        require(
            git_is_ancestor(source["head"], current_head),
            "recorded build baseline is not an ancestor of HEAD",
        )
    actual_inputs = source_inventory()
    require(source["source_inputs"] == actual_inputs, "source-input evidence is stale")

    defaults = (REPOSITORY / "firmware" / "sdkconfig.defaults").read_text(
        encoding="utf-8"
    )
    require(
        "CONFIG_APP_REPRODUCIBLE_BUILD=y" in defaults,
        "reproducible-build configuration is missing",
    )

    reproducibility = manifest["reproducibility"]
    require(
        reproducibility["status"] == "PASS_BYTE_IDENTICAL",
        "reproducibility is not recorded as passing",
    )
    require(
        reproducibility["resolved_config"]["app_reproducible_build"] is True,
        "resolved reproducible-build flag is not recorded",
    )
    require(
        reproducibility["resolved_config"]["app_compile_time_date"] is False,
        "compile-time date must be absent",
    )

    builds = reproducibility["clean_builds"]
    require(len(builds) == 2, "exactly two clean builds must be recorded")
    require(builds[0]["build_dir"] != builds[1]["build_dir"], "build dirs must differ")
    require(builds[0]["sdkconfig"] != builds[1]["sdkconfig"], "sdkconfigs must differ")
    for output in (
        "app_bin",
        "app_elf",
        "app_map",
        "bootloader_bin",
        "partition_table_bin",
        "resolved_sdkconfig",
    ):
        require(
            builds[0]["outputs"][output] == builds[1]["outputs"][output],
            f"recorded {output} identities differ",
        )

    canonical = manifest["app_artifact"]
    require("path" not in canonical, "manifest must not depend on an ignored BIN path")
    require(
        canonical["canonical_filename"] == f"{APP_BASENAME}.bin",
        "canonical filename mismatch",
    )
    require(
        canonical["sha256"] == builds[0]["outputs"]["app_bin"]["sha256"],
        "canonical hash differs from clean builds",
    )
    require(
        canonical["file_length_bytes"]
        == builds[0]["outputs"]["app_bin"]["bytes"],
        "canonical size differs from clean builds",
    )

    safety = manifest["safety"]
    require(safety["visual"] == "UNVERIFIED", "visual status changed")
    require(safety["device-execution"] == "NOT_AUTHORIZED", "device execution changed")
    if role == PREDECESSOR_ROLE:
        require(safety["backlight"] == "hard-disabled", "backlight policy changed")
        require(safety["usage"] == "not-for-visual-validation", "usage changed")
        require(not safety["gpio44_high_path"], "GPIO44-high path recorded")
        require(not safety["ledc"], "LEDC recorded")
        require(not safety["nonzero_backlight_duty"], "non-zero duty recorded")
    else:
        require(
            safety["backlight"] == "low-fixed-test-only",
            "successor backlight policy mismatch",
        )
        require(
            safety["usage"] == "visual-validation-candidate",
            "successor usage mismatch",
        )
        require(
            safety["gpio44_high_path"] == "controlled-pwm-only",
            "GPIO44 high path must be controlled PWM only",
        )
        require(safety["ledc"] is True, "LEDC contract is missing")
        require(
            safety["nonzero_backlight_duty"] == 10,
            "successor duty must be exactly 10",
        )
        contract = manifest["backlight_contract"]
        require(contract["gpio"] == 44, "backlight GPIO mismatch")
        require(contract["polarity"] == "active-high", "backlight polarity mismatch")
        require(contract["speed_mode"] == "LEDC_LOW_SPEED_MODE", "speed mode mismatch")
        require(contract["clock"] == "LEDC_USE_APB_CLK", "clock mismatch")
        require(contract["frequency_hz"] == 2000, "frequency mismatch")
        require(contract["resolution_bits"] == 10, "resolution mismatch")
        require(contract["timer"] == 0, "timer mismatch")
        require(contract["channel"] == 0, "channel mismatch")
        require(contract["initial_duty"] == 0, "initial duty mismatch")
        require(contract["validation_duty"] == 10, "validation duty mismatch")
        require(contract["max_duty"] == 1023, "maximum duty mismatch")
        require(contract["hpoint"] == 0, "h-point mismatch")
        require(contract["output_invert"] is False, "output inversion recorded")
        require(contract["fade"] is False, "fade path recorded")
        require(contract["dynamic_brightness"] is False, "dynamic brightness recorded")
        require(
            manifest["predecessor_artifact"]["sha256"]
            == "4E66B7EDCD38B5225B00E1DB790CA7DD9C842E765961E8929228105F9C10A05D",
            "predecessor artifact identity mismatch",
        )
        require(
            canonical["sha256"] != manifest["predecessor_artifact"]["sha256"],
            "successor artifact matches predecessor hash",
        )

    authorization = manifest["authorization"]
    require(not authorization["contains_device_command"], "device command recorded")
    require(not authorization["creates_device_authorization"], "authorization created")

    return {
        "status": "PASS",
        "source_input_sha256": actual_inputs["aggregate_sha256"],
        "canonical_bytes": canonical["file_length_bytes"],
        "canonical_sha256": canonical["sha256"],
    }


def ascii_field(data: bytes, offset: int, length: int) -> str:
    return data[offset : offset + length].split(b"\0", 1)[0].decode("ascii")


def build_identity(build_dir: Path) -> dict[str, object]:
    build_dir = build_dir.resolve()
    require(build_dir.is_dir(), f"build directory is missing: {build_dir}")
    description_path = build_dir / "project_description.json"
    config_path = build_dir / "config" / "sdkconfig.h"
    require(description_path.is_file(), f"project description missing: {build_dir}")
    require(config_path.is_file(), f"resolved sdkconfig missing: {build_dir}")

    description = json.loads(description_path.read_text(encoding="utf-8"))
    require(description["target"] == "esp32s3", "build target is not esp32s3")
    require(
        description["project_name"] == APP_BASENAME,
        "build project name mismatch",
    )
    resolved_config = config_path.read_text(encoding="utf-8")
    require(
        "#define CONFIG_APP_REPRODUCIBLE_BUILD 1" in resolved_config,
        "resolved build is not reproducible",
    )
    require(
        "#define CONFIG_APP_COMPILE_TIME_DATE 1" not in resolved_config,
        "resolved build embeds compile time/date",
    )

    outputs: dict[str, dict[str, object]] = {}
    for key, relative in (
        ("app_bin", f"{APP_BASENAME}.bin"),
        ("app_elf", f"{APP_BASENAME}.elf"),
        ("app_map", f"{APP_BASENAME}.map"),
        ("bootloader_bin", "bootloader/bootloader.bin"),
        ("partition_table_bin", "partition_table/partition-table.bin"),
        ("resolved_sdkconfig", "config/sdkconfig.h"),
    ):
        path = build_dir / relative
        require(path.is_file(), f"build output is missing: {path}")
        outputs[key] = {"bytes": path.stat().st_size, "sha256": sha256(path)}

    image = (build_dir / f"{APP_BASENAME}.bin").read_bytes()
    require(image[0] == 0xE9, "app image magic mismatch")
    descriptor = {
        "version": ascii_field(image, 0x30, 32),
        "project": ascii_field(image, 0x50, 32),
        "time": ascii_field(image, 0x70, 16),
        "date": ascii_field(image, 0x80, 16),
        "idf": ascii_field(image, 0x90, 32),
    }
    require(descriptor["time"] == "", "app descriptor time is not empty")
    require(descriptor["date"] == "", "app descriptor date is not empty")

    return {
        "build_dir": str(build_dir),
        "outputs": outputs,
        "descriptor": descriptor,
    }


def verify_build_pair(
    manifest: dict[str, object], build_a: Path, build_b: Path
) -> dict[str, object]:
    static_result = verify_manifest_only(manifest)
    require(build_a.resolve() != build_b.resolve(), "build directories must differ")
    identity_a = build_identity(build_a)
    identity_b = build_identity(build_b)

    for output in (
        "app_bin",
        "app_elf",
        "app_map",
        "bootloader_bin",
        "partition_table_bin",
        "resolved_sdkconfig",
    ):
        require(
            identity_a["outputs"][output] == identity_b["outputs"][output],
            f"fresh build {output} identities differ",
        )

    canonical = manifest["app_artifact"]
    require(
        identity_a["outputs"]["app_bin"]["sha256"] == canonical["sha256"],
        "fresh BIN hash differs from canonical manifest",
    )
    require(
        identity_a["outputs"]["app_bin"]["bytes"]
        == canonical["file_length_bytes"],
        "fresh BIN size differs from canonical manifest",
    )
    require(identity_a["descriptor"] == identity_b["descriptor"], "descriptors differ")
    require(
        identity_a["descriptor"] == canonical["app_descriptor"],
        "fresh app descriptor differs from manifest",
    )

    return {
        **static_result,
        "build_a": identity_a,
        "build_b": identity_b,
        "fresh_pair": "PASS_BYTE_IDENTICAL",
    }


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Verify the host-only PCB V1.0 display artifact contract."
    )
    parser.add_argument("--manifest", type=Path, default=DEFAULT_MANIFEST)
    parser.add_argument("--manifest-only", action="store_true")
    parser.add_argument("--build-a", type=Path)
    parser.add_argument("--build-b", type=Path)
    arguments = parser.parse_args()

    try:
        manifest = load_manifest(arguments.manifest.resolve())
        if arguments.manifest_only:
            require(
                arguments.build_a is None and arguments.build_b is None,
                "--manifest-only cannot be combined with build directories",
            )
            result = verify_manifest_only(manifest)
        else:
            require(
                arguments.build_a is not None and arguments.build_b is not None,
                "both --build-a and --build-b are required",
            )
            result = verify_build_pair(manifest, arguments.build_a, arguments.build_b)
    except (OSError, KeyError, ValueError, subprocess.CalledProcessError, VerificationError) as error:
        print(f"ARTIFACT_REPRODUCIBILITY FAIL: {error}", file=sys.stderr)
        return 1

    print(json.dumps(result, indent=2, sort_keys=True))
    print("ARTIFACT_REPRODUCIBILITY PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
