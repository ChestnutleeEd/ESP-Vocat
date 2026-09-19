from __future__ import annotations

import hashlib
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import unittest


REPOSITORY = Path(__file__).resolve().parents[2]
FIRMWARE = REPOSITORY / "firmware"
MAIN = FIRMWARE / "main"
DISPLAY_C = MAIN / "pcb_v1_display.c"
DISPLAY_H = MAIN / "pcb_v1_display.h"
PATTERN_C = MAIN / "pcb_v1_display_test_pattern.c"
PATTERN_H = MAIN / "pcb_v1_display_test_pattern.h"
MANIFEST = MAIN / "idf_component.yml"
HARNESS = Path(__file__).with_name("pcb_v1_display_host_harness.c")
FAKES = Path(__file__).with_name("fakes")
FAKE_API = FAKES / "pcb_v1_display_host_fakes.h"
ARTIFACT_MANIFEST = (
    REPOSITORY
    / "tests"
    / "build"
    / "pcb-v1-display-backlight-validation-artifact-manifest.json"
)
ARTIFACT_VERIFIER = (
    REPOSITORY
    / "tools"
    / "artifact_reproducibility"
    / "verify_pcb_v1_display_artifact.py"
)

EXPECTED_STATES = [
    "BOOT_MARKER",
    "BACKLIGHT_FORCED_OFF",
    "QSPI_BUS_INIT",
    "PANEL_IO_CREATE",
    "PANEL_RESET",
    "PANEL_INIT",
    "DISPLAY_ON",
    "TEST_PATTERN_DRAW",
    "BACKLIGHT_PWM_PREPARE",
    "BACKLIGHT_LOW_ENABLE",
    "READY",
    "FAIL_SAFE",
]
EXPECTED_SEQUENCE = EXPECTED_STATES[:-1]
EXPECTED_TABLE_HASH = (
    "e8a1f2ea307b51be59d3daa201bb444b5f5ddc2d8da2931fda6e91579c4522be"
)


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def extract_braced_body(source: str, declaration: str) -> str:
    declaration_index = source.index(declaration)
    opening = source.index("{", declaration_index)
    depth = 0
    for index in range(opening, len(source)):
        if source[index] == "{":
            depth += 1
        elif source[index] == "}":
            depth -= 1
            if depth == 0:
                return source[opening + 1 : index]
    raise AssertionError(f"unterminated initializer for {declaration}")


def top_level_initializer_count(body: str) -> int:
    depth = 0
    count = 0
    for character in body:
        if character == "{":
            if depth == 0:
                count += 1
            depth += 1
        elif character == "}":
            depth -= 1
    if depth != 0:
        raise AssertionError("initializer braces are unbalanced")
    return count


def visual_studio_environment() -> dict[str, str]:
    candidates = [
        Path(
            r"C:\Program Files (x86)\Microsoft Visual Studio"
            r"\Installer\vswhere.exe"
        ),
        Path(r"C:\Program Files\Microsoft Visual Studio\Installer\vswhere.exe"),
    ]
    vswhere = next((candidate for candidate in candidates if candidate.exists()), None)
    if vswhere is None:
        raise unittest.SkipTest("Visual Studio discovery tool is unavailable")

    result = subprocess.run(
        [
            str(vswhere),
            "-latest",
            "-products",
            "*",
            "-requires",
            "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
            "-property",
            "installationPath",
        ],
        check=True,
        capture_output=True,
        text=True,
    )
    installation = Path(result.stdout.strip())
    vcvars = installation / "VC" / "Auxiliary" / "Build" / "vcvars64.bat"
    if not vcvars.exists():
        raise unittest.SkipTest("Visual C++ x64 environment is unavailable")

    command = f'call "{vcvars}" >nul && set'
    environment_result = subprocess.run(
        command,
        shell=True,
        check=True,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
    )
    environment = os.environ.copy()
    for line in environment_result.stdout.splitlines():
        if "=" in line:
            key, value = line.split("=", 1)
            environment[key] = value
    return environment


class DisplaySourceAuditTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.display = read(DISPLAY_C)
        cls.header = read(DISPLAY_H)
        cls.pattern = read(PATTERN_C)
        cls.pattern_header = read(PATTERN_H)
        cls.main = read(MAIN / "main.c")
        cls.cmake = read(MAIN / "CMakeLists.txt")
        cls.manifest = read(MANIFEST)
        cls.harness = read(HARNESS)
        cls.fake_api = read(FAKE_API)
        cls.table_body = extract_braced_body(
            cls.display,
            "static const st77916_lcd_init_cmd_t vendor_specific_init_yysj[]",
        )

    def test_exact_dependency_pin(self) -> None:
        self.assertRegex(
            self.manifest,
            r'(?m)^\s*espressif/esp_lcd_st77916:\s*"==1\.0\.1"\s*$',
        )
        self.assertNotIn("^1.0.1", self.manifest)

    def test_vendor_table_command_count(self) -> None:
        self.assertEqual(top_level_initializer_count(self.table_body), 184)
        self.assertIn("==\n                   184", self.display)

    def test_vendor_table_legacy_token_count(self) -> None:
        self.assertEqual(len(re.findall(r"\{\s*0x", self.table_body)), 365)

    def test_vendor_table_normalized_hash(self) -> None:
        normalized = re.sub(r"\s+", "", self.table_body).encode("utf-8")
        self.assertEqual(hashlib.sha256(normalized).hexdigest(), EXPECTED_TABLE_HASH)

    def test_vendor_table_final_sequence(self) -> None:
        normalized = re.sub(r"\s+", "", self.table_body)
        self.assertTrue(
            normalized.endswith(
                "{0x21,(uint8_t[]){},0,0},"
                "{0x11,(uint8_t[]){},0,0},"
                "{0x00,(uint8_t[]){},0,120},"
            )
        )

    def test_state_enum_is_exact(self) -> None:
        enum_body = extract_braced_body(self.header, "typedef enum")
        actual = re.findall(r"PCB_V1_DISPLAY_STATE_([A-Z0-9_]+)", enum_body)
        self.assertEqual(actual, EXPECTED_STATES)

    def test_reachable_state_sequence_is_exact(self) -> None:
        sequence = extract_braced_body(
            self.display,
            "static const pcb_v1_display_state_t s_state_sequence[]",
        )
        actual = re.findall(r"PCB_V1_DISPLAY_STATE_([A-Z0-9_]+)", sequence)
        self.assertEqual(actual, EXPECTED_SEQUENCE)

    def test_exact_backlight_markers(self) -> None:
        self.assertEqual(
            self.display.count(
                'puts("DISPLAY_BACKLIGHT_CONFIG gpio=44 polarity=ACTIVE_HIGH "'
            ),
            1,
        )
        self.assertEqual(
            self.display.count(
                'puts("DISPLAY_BACKLIGHT_ENABLED raw_duty=10 max_duty=1023 "'
            ),
            1,
        )
        self.assertEqual(
            self.display.count(
                'puts("LCD_SM_READY visual=UNVERIFIED '
                'backlight=LOW_FIXED_TEST_ONLY")'
            ),
            1,
        )

    def test_enter_and_failure_marker_cardinality(self) -> None:
        self.assertEqual(self.display.count('"LCD_SM_ENTER %s\\n"'), 1)
        self.assertEqual(self.display.count('"LCD_SM_FAIL state=%s err=%s\\n"'), 1)

    def test_fault_injection_is_fake_api_level_not_state_short_circuit(self) -> None:
        self.assertNotIn("fault_injecting_executor", self.harness)
        self.assertNotIn("g_injected_failure_state", self.harness)
        self.assertIn("static bool should_inject(void)", self.harness)
        self.assertIn("run_state_sequence(execute_state)", self.harness)
        self.assertIn("g_fault.injected_calls == 1", self.harness)
        self.assertIn("g_fault.configured_calls >= 1", self.harness)

    def test_fault_point_inventory_is_stable_and_complete(self) -> None:
        enum_body = extract_braced_body(self.fake_api, "typedef enum")
        identifiers = re.findall(r"\b(FP_[A-Z0-9_]+)\b", enum_body)
        self.assertEqual(identifiers[0], "FP_NONE")
        self.assertEqual(identifiers[-1], "FP_COUNT")
        actual_points = identifiers[1:-1]
        self.assertEqual(len(actual_points), 51)
        self.assertEqual(len(set(actual_points)), 51)
        self.assertIn("failable_count == 46", self.harness)
        self.assertIn("cases=46 passed=46 uncovered=0", self.harness)

    def test_gpio44_only_has_low_writes(self) -> None:
        calls = re.findall(
            r"gpio_set_level\s*\(\s*LCD_GPIO_BACKLIGHT\s*,\s*([^)]+)\)",
            self.display,
        )
        self.assertGreaterEqual(len(calls), 1)
        self.assertEqual({call.strip() for call in calls}, {"0"})

    def test_exact_fixed_ledc_contract(self) -> None:
        expected = (
            "LCD_BACKLIGHT_PWM_FREQUENCY_HZ = 2000",
            "LCD_BACKLIGHT_INITIAL_DUTY = 0",
            "LCD_BACKLIGHT_VALIDATION_DUTY = 10",
            "LCD_BACKLIGHT_MAX_DUTY = 1023",
            "LCD_BACKLIGHT_HPOINT = 0",
            ".speed_mode = LEDC_LOW_SPEED_MODE",
            ".duty_resolution = LEDC_TIMER_10_BIT",
            ".timer_num = LEDC_TIMER_0",
            ".channel = LEDC_CHANNEL_0",
            ".clk_cfg = LEDC_USE_APB_CLK",
            ".output_invert = 0",
            "esp_driver_ledc",
        )
        combined = "\n".join([self.display, self.cmake])
        for required in expected:
            self.assertIn(required, combined)
        self.assertEqual(self.display.count("ledc_timer_config("), 1)
        self.assertEqual(self.display.count("ledc_channel_config("), 1)
        self.assertEqual(self.display.count("ledc_get_duty("), 2)
        self.assertEqual(self.display.count("ledc_stop("), 1)
        self.assertNotIn("LEDC_HIGH_SPEED_MODE", combined)
        self.assertNotIn("output_invert = 1", combined)

    def test_backlight_duty_is_fixed_and_non_dynamic(self) -> None:
        combined = "\n".join([self.display, self.pattern, self.main, self.cmake])
        self.assertEqual(
            self.display.count("LCD_BACKLIGHT_VALIDATION_DUTY"),
            3,
        )
        self.assertNotIn("ledc_fade", combined)
        self.assertNotIn("brightness", combined.lower())
        self.assertNotIn("nvs_", combined.lower())
        self.assertNotIn("ledc_set_duty(", combined)

    def test_ledc_preparation_occurs_after_pattern_completion(self) -> None:
        pattern_free = self.display.index("free(context->strip)")
        timer_call = self.display.index("ledc_timer_config(")
        zero_update = self.display.index(
            "ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE"
        )
        enable_function = self.display.index("static esp_err_t enable_low_backlight")
        enable_update = self.display.index(
            "ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE", enable_function
        )
        self.assertLess(pattern_free, timer_call)
        self.assertLess(timer_call, zero_update)
        self.assertLess(zero_update, enable_update)

    def test_exact_gpio_allowlist_and_no_v12_gpio(self) -> None:
        gpio_literals = {
            int(value)
            for value in re.findall(r"\bGPIO_NUM_(\d+)\b", self.display)
        }
        self.assertEqual(gpio_literals, {3, 11, 12, 13, 14, 18, 44, 46})
        self.assertTrue({9, 45, 47}.isdisjoint(gpio_literals))

    def test_qspi_configuration_is_bounded(self) -> None:
        self.assertIn("SPI2_HOST", self.display)
        self.assertIn("LCD_PIXEL_CLOCK_HZ = 40000000", self.display)
        self.assertNotIn("80000000", self.display)
        self.assertIn("LCD_TRANSFER_QUEUE_DEPTH = 1", self.display)
        self.assertIn("PCB_V1_LCD_STRIP_BYTES", self.display)

    def test_single_internal_dma_strip(self) -> None:
        self.assertEqual(self.display.count("heap_caps_malloc("), 1)
        self.assertIn("MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL", self.display)
        self.assertNotIn("MALLOC_CAP_SPIRAM", self.display)
        self.assertIn("PCB_V1_LCD_STRIP_BYTES == 57600", self.display)

    def test_five_half_open_windows(self) -> None:
        self.assertIn(
            "static const uint16_t strip_boundaries[] = "
            "{0, 80, 160, 240, 320, 360};",
            self.display,
        )
        self.assertIn("LCD_TRANSFER_QUEUE_DEPTH = 1", self.display)
        self.assertRegex(
            self.display,
            r"esp_lcd_panel_draw_bitmap\(context->panel,\s*"
            r"0,\s*y_start,\s*PCB_V1_LCD_WIDTH,\s*y_end,\s*context->strip\)",
        )

    def test_transfer_completion_before_reuse(self) -> None:
        draw_index = self.display.index("esp_lcd_panel_draw_bitmap")
        wait_index = self.display.index("xSemaphoreTake", draw_index)
        loop_end = self.display.index("free(context->strip)", wait_index)
        self.assertLess(draw_index, wait_index)
        self.assertLess(wait_index, loop_end)
        self.assertIn("context->transfer_in_flight", self.display)

    def test_timeout_retains_in_flight_buffer(self) -> None:
        cleanup = self.display[
            self.display.index("static esp_err_t cleanup_failed_run") :
            self.display.index("static esp_err_t execute_state")
        ]
        self.assertRegex(
            cleanup,
            r"(?s)if \(context->transfer_in_flight\) \{.*?"
            r"return result;\s*\}",
        )

    def test_pattern_geometry_and_text(self) -> None:
        self.assertIn('"ESP-VoCat LCD TEST"', self.pattern)
        self.assertIn("x == 0 || y == 0", self.pattern)
        self.assertIn("y < 90", self.pattern)
        self.assertIn("y < 180", self.pattern)
        self.assertIn("y < 270", self.pattern)

    def test_pattern_writes_rgb565_msb_first(self) -> None:
        self.assertIn("buffer[offset++] = (uint8_t)(color >> 8);", self.pattern)
        self.assertIn("buffer[offset++] = (uint8_t)(color & 0xFF);", self.pattern)

    def test_no_forbidden_services_or_product_features(self) -> None:
        combined = "\n".join([self.display, self.pattern, self.main, self.cmake])
        for forbidden in (
            "lvgl",
            "lv_init",
            "touch",
            "wifi",
            "bluetooth",
            "motor",
            "speaker",
            "microphone",
            "filesystem",
            "nvs_",
            "sdmmc",
        ):
            self.assertNotIn(forbidden, combined.lower())

    def test_no_executable_device_command(self) -> None:
        combined = "\n".join(
            [self.display, self.pattern, self.main, self.cmake, self.manifest]
        ).lower()
        for forbidden in (
            "idf.py flash",
            "flash monitor",
            "write_flash",
            "erase_flash",
            "--port",
            "espefuse",
        ):
            self.assertNotIn(forbidden, combined)

    def test_main_calls_once_then_uses_nonbusy_delay(self) -> None:
        self.assertEqual(self.main.count("pcb_v1_display_run_once()"), 1)
        self.assertIn("vTaskDelay(pdMS_TO_TICKS(1000))", self.main)
        self.assertIn("visual=UNVERIFIED", self.main)
        self.assertIn("visual-validation-candidate", self.main)
        self.assertIn("backlight=LOW_FIXED_TEST_ONLY", self.main)
        self.assertIn("device-execution=NOT_AUTHORIZED", self.main)


class DisplayCompiledHostHarnessTests(unittest.TestCase):
    def test_compiled_fake_backend_and_fault_injection_harness(self) -> None:
        environment = visual_studio_environment()
        path_values = [
            value
            for key, value in environment.items()
            if key.casefold() == "path"
        ]
        compiler_path = path_values[-1] if path_values else None
        compiler = shutil.which("cl.exe", path=compiler_path)
        if compiler is None:
            raise unittest.SkipTest("Visual C++ compiler is unavailable")
        output_directory = FIRMWARE / "build" / "host_tests"
        output_directory.mkdir(parents=True, exist_ok=True)
        executable = output_directory / "pcb_v1_display_host_harness.exe"
        object_file = output_directory / "pcb_v1_display_host_harness.obj"

        compile_result = subprocess.run(
            [
                compiler,
                "/nologo",
                "/std:c17",
                "/W4",
                "/WX",
                "/DPCB_V1_DISPLAY_HOST_TABLE_STUB",
                "/DPCB_V1_DISPLAY_HOST_FAULT_INJECTION",
                f"/I{FAKES}",
                f"/I{MAIN}",
                str(HARNESS),
                f"/Fo:{object_file}",
                f"/Fe:{executable}",
            ],
            cwd=REPOSITORY,
            env=environment,
            capture_output=True,
            text=True,
        )
        self.assertEqual(
            compile_result.returncode,
            0,
            msg=compile_result.stdout + compile_result.stderr,
        )

        run_result = subprocess.run(
            [str(executable)],
            cwd=REPOSITORY,
            capture_output=True,
            text=True,
        )
        self.assertEqual(
            run_result.returncode,
            0,
            msg=run_result.stdout + run_result.stderr,
        )
        self.assertRegex(run_result.stdout, r"HOST_C_TEST PASS assertions=\d+")
        self.assertIn(
            "FAULT_MATRIX_C_TEST PASS "
            "points=51 failable=46 cases=46 passed=46 uncovered=0",
            run_result.stdout,
        )
        self.assertIn(
            "CLEANUP_C_TEST PASS "
            "order_cases=5 fault_cases=10 priority_cases=1",
            run_result.stdout,
        )
        self.assertIn(
            "DISPLAY_BACKLIGHT_CONFIG gpio=44 polarity=ACTIVE_HIGH "
            "frequency_hz=2000 resolution_bits=10 initial_duty=0",
            run_result.stdout,
        )
        self.assertIn(
            "LCD_SM_READY visual=UNVERIFIED "
            "backlight=LOW_FIXED_TEST_ONLY",
            run_result.stdout,
        )
        self.assertIn(
            "LCD_SM_FAIL state=TEST_PATTERN_DRAW err=ESP_ERR_TIMEOUT",
            run_result.stdout,
        )


class DisplayArtifactManifestTests(unittest.TestCase):
    def test_manifest_matches_clean_source_reproducibility_contract(self) -> None:
        result = subprocess.run(
            [
                sys.executable,
                str(ARTIFACT_VERIFIER),
                "--manifest",
                str(ARTIFACT_MANIFEST),
                "--manifest-only",
            ],
            cwd=REPOSITORY,
            capture_output=True,
            text=True,
        )
        self.assertEqual(result.returncode, 0, msg=result.stdout + result.stderr)
        self.assertIn("ARTIFACT_REPRODUCIBILITY PASS", result.stdout)

    def test_reproducible_build_is_part_of_source_configuration(self) -> None:
        defaults = read(FIRMWARE / "sdkconfig.defaults")
        self.assertIn("CONFIG_APP_REPRODUCIBLE_BUILD=y", defaults)
        self.assertNotIn("CONFIG_APP_COMPILE_TIME_DATE=y", defaults)


if __name__ == "__main__":
    unittest.main()
